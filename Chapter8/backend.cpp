#include "backend.h"
#include <QDebug>
#include <QDir> // 用于处理文件路径
#include <opencv2/imgcodecs.hpp> // 用于图像读写
#include "imageprovider.h" // 在cpp中包含完整的头文件
#include <vector> // C++标准库，用于存储轮廓

Backend::Backend(ImageProvider *provider, QObject *parent)
    : QObject(parent),
    m_imageProvider(provider) // 在构造函数中保存指针
{
    // --- 1. 加载ONNX模型 ---
    QString modelPath = QDir::currentPath() + "/../../best.onnx";
    try {
        m_net = cv::dnn::readNetFromONNX(modelPath.toStdString());
        if (m_net.empty()) {
            qWarning() << "Failed to load ONNX model!";
        } else {
            qDebug() << "ONNX model loaded successfully.";
            // 设置计算后端。CPU是默认选项，但可以显式指定
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }
    } catch (const cv::Exception& e) {
        qWarning() << "Error loading model:" << e.what();
    }

    // --- 2. 加载类别名称 ---
    // 这个顺序必须与训练时的.yaml文件严格一致！
    m_classNames = {"neck_defect", "thread_defect", "head_defect"};
}

// 辅助函数：将cv::Mat转换为QImage
static QImage matToQImage(const cv::Mat &mat)
{
    // 检查Mat是否为空
    if (mat.empty()) {
        return QImage();
    }

    // 根据Mat的类型进行转换
    if (mat.type() == CV_8UC3) {
        // 对于彩色图像 (BGR顺序)
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    } else if (mat.type() == CV_8UC1) {
        // 对于灰度图像
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    }

    // 其他格式可以按需添加
    return QImage();
}

void Backend::startScan()
{
    qDebug() << "C++: Loading image with OpenCV...";
    emit statusMessageChanged("正在从数据集加载图像...");

    // QDir::currentPath() 获取的是构建目录，需要向上两级到项目根目录
    // 我们从MVTec数据集中挑选一张带瑕疵的图片作为示例
    QString imagePath = QDir::currentPath() + "/../../dataset/screw/test/scratch_head/000.png";

    cv::Mat sourceMat = cv::imread(imagePath.toStdString());
    if (sourceMat.empty()) {return;}

    emit statusMessageChanged("图像加载成功，开始预处理...");

    // --- 5. 调用推理函数 ---
    cv::Mat resultMat = runInference(sourceMat);

    // 在UI上直观展示处理结果
    QImage imageQ = matToQImage(resultMat);
    if (imageQ.isNull()){ /* ... 错误处理 ... */ return; }

    // 更新ImageProvider中的图像
    m_imageProvider->updateImage(imageQ);

    // 发射信号，只告诉QML需要刷新的图像ID
    emit imageReady("screw_processed");
    emit statusMessageChanged("AI推理完成");
}

cv::Mat Backend::runInference(const cv::Mat &inputImage)
{
    if (m_net.empty()) {
        qDebug() << "Network not loaded.";
        return inputImage;
    }

    // --- 3. 图像预处理 ---
    // YOLOv8需要一个640x640的方形输入
    const int inputWidth = 640;
    const int inputHeight = 640;
    cv::Mat blob;
    // 将图像转换为blob格式：调整尺寸、归一化(像素值/255)、通道重排(BGR->RGB)
    cv::dnn::blobFromImage(inputImage, blob, 1./255., cv::Size(inputWidth, inputHeight), cv::Scalar(), true, false);

    // --- 执行推理 ---
    m_net.setInput(blob);
    std::vector<cv::Mat> outputs;
    m_net.forward(outputs, m_net.getUnconnectedOutLayersNames());

    cv::Mat output_buffer = outputs[0]; // [1, num_classes + 4, 8400]
    output_buffer = output_buffer.reshape(1, {output_buffer.size[1], output_buffer.size[2]}); // [num_classes + 4, 8400]
    cv::transpose(output_buffer, output_buffer); // [8400, num_classes + 4]

    // --- 后处理 ---
    float conf_threshold = 0.5f; // 置信度阈值
    float nms_threshold = 0.4f;  // NMS阈值

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    float x_factor = (float)inputImage.cols / 640.f;
    float y_factor = (float)inputImage.rows / 640.f;

    for (int i = 0; i < output_buffer.rows; i++) {
        cv::Mat row = output_buffer.row(i);
        cv::Mat scores = row.colRange(4, output_buffer.cols);
        double confidence;
        cv::Point class_id_point;
        cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &class_id_point);

        if (confidence > conf_threshold) {
            confidences.push_back(confidence);
            class_ids.push_back(class_id_point.x);

            float cx = row.at<float>(0,0);
            float cy = row.at<float>(0,1);
            float w = row.at<float>(0,2);
            float h = row.at<float>(0,3);

            int left = (int)((cx - 0.5 * w) * x_factor);
            int top = (int)((cy - 0.5 * h) * y_factor);
            int width = (int)(w * x_factor);
            int height = (int)(h * y_factor);

            boxes.push_back(cv::Rect(left, top, width, height));
        }
    }

    // --- 非极大值抑制 (NMS) ---
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);

    // --- 结果可视化 ---
    cv::Mat resultImage = inputImage.clone();
    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        int class_id = class_ids[idx];

        // 绘制边界框
        cv::rectangle(resultImage, box, cv::Scalar(0, 255, 0), 2);

        // 绘制标签
        std::string label = cv::format("%s: %.2f", m_classNames[class_id].c_str(), confidences[idx]);
        cv::putText(resultImage, label, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    }

    // 暂时结果图像
    return resultImage;
}




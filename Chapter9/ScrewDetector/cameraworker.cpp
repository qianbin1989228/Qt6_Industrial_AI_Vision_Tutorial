#include "cameraworker.h"
#include <QCameraDevice>
#include <QMediaDevices>
#include <QDebug>
#include <QDir>
#include <opencv2/imgproc.hpp>

// 辅助函数：将QVideoFrame转换为cv::Mat，通过QImage作为中介
static cv::Mat videoFrameToMat(const QVideoFrame &frame) {
    if (!frame.isValid()) {
        return cv::Mat();
    }

    // 1. 将QVideoFrame转换为QImage
    // QImage的构造函数已经为我们处理了复杂的内存映射和格式问题
    QImage image = frame.toImage();

    if (image.isNull()) {
        return cv::Mat();
    }

    // 2. 将QImage转换为cv::Mat
    // 确保图像是3通道或4通道的
    if (image.format() != QImage::Format_RGB888 && image.format() != QImage::Format_ARGB32) {
        image = image.convertToFormat(QImage::Format_RGB888);
    }

    // 根据QImage的格式创建对应的cv::Mat
    cv::Mat mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());

    // 返回一个深拷贝，确保数据独立和线程安全
    return mat.clone();
}



CameraWorker::CameraWorker(QObject *parent) : QObject(parent)
{
    // --- 加载ONNX模型 ---
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
    m_classNames = {"neck_defect", "thread_defect", "head_defect"};
}

CameraWorker::~CameraWorker()
{
    stopCamera();
}

void CameraWorker::startCamera()
{
    if (m_camera && m_camera->isActive()) {
        return; // 如果已在运行，则不执行任何操作
    }

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        emit statusMessageChanged("错误: 未找到可用的摄像头");
        return;
    }

    m_camera = new QCamera(cameras.first(), this);
    m_captureSession = new QMediaCaptureSession(this);
    m_videoSink = new QVideoSink(this);

    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoSink(m_videoSink);

    // 关键连接：当VideoSink接收到新的一帧时，触发我们的处理槽函数
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &CameraWorker::processFrame);

    m_camera->start();
    emit statusMessageChanged("摄像头已启动");
}

void CameraWorker::stopCamera()
{
    if (m_camera) {
        m_camera->stop();
        // 释放资源
        delete m_camera; m_camera = nullptr;
        delete m_captureSession; m_captureSession = nullptr;
        delete m_videoSink; m_videoSink = nullptr;
        emit statusMessageChanged("摄像头已停止");
    }
}

void CameraWorker::processFrame(const QVideoFrame &frame)
{
    if (!frame.isValid()) return;

    // 1. 格式转换
    cv::Mat mat = videoFrameToMat(frame);
    if (mat.empty()) return;

    // QVideoFrame通常是BGRA格式，我们需要BGR
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);

    // 2. AI推理 (复用上一章的runInference函数)
    cv::Mat resultMat = runInference(mat);

    // 3. 将结果转换为QImage并发送信号
    QImage resultImage(resultMat.data, resultMat.cols, resultMat.rows, resultMat.step, QImage::Format_RGB888);
    emit newFrameReady(resultImage.copy()); // 使用copy确保线程安全
}

cv::Mat CameraWorker::runInference(const cv::Mat &inputImage)
{
    // 这个函数的全部内容与上一章Backend中的runInference完全相同
    // 包括预处理、前向传播、后处理和绘制结果
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

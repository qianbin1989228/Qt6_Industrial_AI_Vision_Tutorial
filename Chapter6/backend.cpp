#include "backend.h"
#include <QDebug>
#include <QDir> // 用于处理文件路径
#include <opencv2/imgcodecs.hpp> // 用于图像读写
#include "imageprovider.h" // 在cpp中包含完整的头文件
#include <vector> // C++标准库，用于存储轮廓

Backend::Backend(ImageProvider *provider, QObject *parent)
    : QObject(parent),
    m_imageProvider(provider) // 在构造函数中保存指针
{}

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

    // 将BGR彩色图像转换为单通道的灰度图像
    cv::Mat grayMat;
    cv::cvtColor(sourceMat, grayMat, cv::COLOR_BGR2GRAY);

    // 将灰度图像转换为只有黑白两种颜色的二值图像
    // 此处使用OTSU方法自动寻找最佳阈值
    cv::Mat binaryMat;
    cv::threshold(grayMat, binaryMat, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    emit statusMessageChanged("预处理完成，开始寻找轮廓...");

    // --- 3. 轮廓发现 ---
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryMat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 假设最大轮廓就是我们的螺丝
    if (contours.empty()) {
        emit statusMessageChanged("错误：未在图像中找到任何轮廓！");
        return;
    }

    // 寻找面积最大的轮廓
    double maxArea = 0;
    int maxAreaIdx = -1;
    for (int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            maxAreaIdx = i;
        }
    }

    if (maxAreaIdx == -1) {
        // ... 错误处理
        emit statusMessageChanged("错误：未在图像中找到任何轮廓！");
        return;
    }

    // 计算最大轮廓的最小外接矩形
    cv::RotatedRect rotatedRect = cv::minAreaRect(contours[maxAreaIdx]);

    // 获取矩形的尺寸。注意：width和height不一定是物理的长和宽
    cv::Size2f rectSize = rotatedRect.size;
    float width = std::min(rectSize.width, rectSize.height);
    float length = std::max(rectSize.width, rectSize.height);

    qDebug() << "Measured dimensions (pixels): Length =" << length << ", Width =" << width;
    QString resultMessage = QString("测量结果: 长度= %1 px, 宽度= %2 px").arg(length, 0, 'f', 2).arg(width, 0, 'f', 2);
    // 为了直观展示，我们在原始彩色图上把轮廓和矩形画出来
    // 获取矩形的四个顶点
    cv::Point2f vertices[4];
    rotatedRect.points(vertices);
    // 将轮廓和矩形画在sourceMat上
    cv::drawContours(sourceMat, contours, maxAreaIdx, cv::Scalar(0, 255, 0), 2); // 绿色轮廓
    for (int i = 0; i < 4; i++) {
        cv::line(sourceMat, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), 2); // 红色矩形
    }

    // 在UI上直观展示处理结果
    QImage imageQ = matToQImage(sourceMat);
    if (imageQ.isNull()){ /* ... 错误处理 ... */ return; }

    // 更新ImageProvider中的图像
    m_imageProvider->updateImage(imageQ);

    // 发射信号，只告诉QML需要刷新的图像ID
    emit imageReady("screw_processed");
    emit statusMessageChanged(resultMessage);
}




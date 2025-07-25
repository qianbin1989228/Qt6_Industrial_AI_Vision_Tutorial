#include "backend.h"
#include <QDebug>
#include <QDir> // 用于处理文件路径
#include <opencv2/imgcodecs.hpp> // 用于图像读写
#include "imageprovider.h" // 在cpp中包含完整的头文件

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

    // 1. 构建数据集图片的绝对路径
    // QDir::currentPath() 获取的是构建目录，需要向上两级到项目根目录
    // 我们从MVTec数据集中挑选一张带瑕疵的图片作为示例
    QString imagePath = QDir::currentPath() + "/../../dataset/screw/test/scratch_head/000.png";

    // 2. 使用OpenCV从文件系统加载图像
    // 注意：imread需要一个标准C++字符串
    cv::Mat imageMat = cv::imread(imagePath.toStdString());

    if (imageMat.empty()) {
        qDebug() << "Error: Could not load image from path:" << imagePath;
        emit statusMessageChanged("错误：无法从数据集加载图像！请检查路径。");
        return;
    }

    // 3. 将cv::Mat转换为QImage
    QImage imageQ = matToQImage(imageMat);
    if (imageQ.isNull()){
        emit statusMessageChanged("错误：图像格式转换失败！");
        return;
    }

    // 4. 更新ImageProvider中的图像
    m_imageProvider->updateImage(imageQ);

    // 5. 发射信号，只告诉QML需要刷新的图像ID
    emit imageReady("screw_sample");
    emit statusMessageChanged("图像显示成功！");
}




#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>

#include <opencv2/dnn.hpp> // 包含OpenCV DNN模块头文件

class ImageProvider; // 使用前向声明

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class Backend : public QObject
{
    Q_OBJECT // 必须添加，以支持信号槽和QML交互
public:
    // 构造函数需要接收ImageProvider的指针
    explicit Backend(ImageProvider *provider, QObject *parent = nullptr);

    // 使用 Q_INVOKABLE 宏，使这个普通的C++成员函数可以被QML调用
    Q_INVOKABLE void startScan();

signals:
    // 图像完成信号，传递一个图像ID
    void imageReady(const QString &imageId);
    // 定义一个信号，用于从C++向QML传递状态更新信息
    void statusMessageChanged(const QString &message);

private:
    ImageProvider *m_imageProvider;
    cv::Mat runInference(const cv::Mat &inputImage); // 添加一个私有方法用于AI推理
    cv::dnn::Net m_net; // 添加一个Net对象，用于表示神经网络
    std::vector<std::string> m_classNames; // 用于存储类别名称
};

#endif // BACKEND_H

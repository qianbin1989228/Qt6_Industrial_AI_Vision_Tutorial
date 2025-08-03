#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>

#include <opencv2/dnn.hpp> // 包含OpenCV DNN模块头文件

#include <QThread> // 包含QThread头文件
#include "cameraworker.h" //包含定义摄像头工作类

class ImageProvider; // 使用前向声明
class CameraWorker; // 前向声明

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class Backend : public QObject
{
    Q_OBJECT // 必须添加，以支持信号槽和QML交互
public:
    // 构造函数需要接收ImageProvider的指针
    explicit Backend(ImageProvider *provider, QObject *parent = nullptr);
    ~Backend();

    Q_INVOKABLE void startCamera();
    Q_INVOKABLE void stopCamera();

signals:
    // 图像完成信号，传递一个图像ID
    void imageReady(const QString &imageId);
    // 定义一个信号，用于从C++向QML传递状态更新信息
    void statusMessageChanged(const QString &message);

    // 内部信号，用于安全地与工作线程通信
    void startCameraRequested();
    void stopCameraRequested();

public slots:
    // 槽，用于接收来自工作线程的图像
    void handleNewFrame(const QImage &frame);

private:
    ImageProvider *m_imageProvider;
    QThread *m_cameraThread;
    CameraWorker *m_cameraWorker;
};

#endif // BACKEND_H

#include "backend.h"
#include "imageprovider.h" // 在cpp中包含完整的头文件

Backend::Backend(ImageProvider *provider, QObject *parent)
    : QObject(parent),
    m_imageProvider(provider) // 在构造函数中保存指针
{
    m_cameraThread = new QThread(this);
    m_cameraWorker = new CameraWorker();

    // 关键一步：将worker对象移动到新线程
    m_cameraWorker->moveToThread(m_cameraThread);

    // --- 设置线程间的通信 ---
    // UI线程 -> 工作线程
    connect(this, &Backend::startCameraRequested, m_cameraWorker, &CameraWorker::startCamera);
    connect(this, &Backend::stopCameraRequested, m_cameraWorker, &CameraWorker::stopCamera);

    // 工作线程 -> UI线程
    connect(m_cameraWorker, &CameraWorker::newFrameReady, this, &Backend::handleNewFrame);
    connect(m_cameraWorker, &CameraWorker::statusMessageChanged, this, &Backend::statusMessageChanged);

    // 确保线程退出时，worker对象能被安全删除
    connect(m_cameraThread, &QThread::finished, m_cameraWorker, &QObject::deleteLater);

    m_cameraThread->start(); // 启动线程的事件循环
}

Backend::~Backend()
{
    // 退出线程
    m_cameraThread->quit();
}

void Backend::startCamera() { emit startCameraRequested(); }
void Backend::stopCamera() { emit stopCameraRequested(); }

void Backend::handleNewFrame(const QImage &frame)
{
    m_imageProvider->updateImage(frame);
    // 发射带有时间戳的信号，强制QML刷新
    emit imageReady("live_frame?" + QString::number(QDateTime::currentMSecsSinceEpoch()));
}


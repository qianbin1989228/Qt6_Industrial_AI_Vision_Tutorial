#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QVideoFrame>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>


class QCamera;
class QMediaCaptureSession;
class QVideoSink;

class CameraWorker : public QObject
{
    Q_OBJECT
public:
    explicit CameraWorker(QObject *parent = nullptr);
    ~CameraWorker();

signals:
    // 信号：一帧新的、处理完毕的图像已准备好
    void newFrameReady(const QImage &frame);
    // 信号：用于向UI传递状态或错误信息
    void statusMessageChanged(const QString &message);

public slots:
    // 槽：启动摄像头
    void startCamera();
    // 槽：停止摄像头
    void stopCamera();

private slots:
    // 槽：当摄像头捕获到新的一帧时被调用
    void processFrame(const QVideoFrame &frame);

private:
    cv::Mat runInference(const cv::Mat &inputImage);

    QCamera *m_camera = nullptr;
    QMediaCaptureSession *m_captureSession = nullptr;
    QVideoSink *m_videoSink = nullptr;

    cv::dnn::Net m_net;
    std::vector<std::string> m_classNames;
};

#endif // CAMERAWORKER_H

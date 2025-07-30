#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>

// 继承自 QQuickImageProvider
class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();

    // QML引擎会调用这个纯虚函数来请求图片
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    // 一个公共方法，用于从C++的Backend更新图片
    void updateImage(const QImage &image);

private:
    // 用于存储当前要显示的图像
    QImage m_image;
};

#endif // IMAGEPROVIDER_H

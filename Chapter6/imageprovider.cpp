#include "imageprovider.h"

ImageProvider::ImageProvider()
    // 构造函数中必须指定Provider的类型
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);

    if (size) {
        *size = m_image.size();
    }
    return m_image;
}

void ImageProvider::updateImage(const QImage &image)
{
    if (m_image != image) {
        m_image = image;
    }
}

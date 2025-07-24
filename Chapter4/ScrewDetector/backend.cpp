#include "backend.h"
#include <QDebug>
#include <QTimer> // 用于模拟耗时操作

Backend::Backend(QObject *parent) : QObject(parent)
{
}

void Backend::startScan()
{
    qDebug() << "C++: startScan() method called from QML.";
    emit statusMessageChanged("正在准备扫描设备...");

    // 使用QTimer::singleShot模拟一个2秒后的异步操作
    QTimer::singleShot(2000, this, [this]() {
        qDebug() << "C++: Simulated scan finished.";
        // 任务完成后，再次发射信号更新状态
        emit statusMessageChanged("扫描完成！");
    });
}

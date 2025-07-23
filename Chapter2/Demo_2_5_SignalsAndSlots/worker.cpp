#include "worker.h"
#include <QDebug>
#include <QThread>

Worker::Worker(QObject *parent) : QObject(parent) {}
void Worker::doWork()
{
    qDebug() << "[工作者] 开始执行耗时任务...";
    QThread::sleep(2); // 模拟耗时2秒
    QString result = "所有螺丝检测完毕";
    qDebug() << "[工作者] 任务完成，准备发射信号...";
    emit workFinished(result); // 使用emit关键字发射信号
}

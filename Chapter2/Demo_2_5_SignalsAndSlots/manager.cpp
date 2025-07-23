#include "manager.h"
#include <QDebug>

Manager::Manager(QObject *parent) : QObject(parent) {}

void Manager::onWorkFinished(const QString &result)
{
    qDebug() << "[管理者] 收到信号，槽函数被调用！";
    qDebug() << "  > 结果:" << result;
}

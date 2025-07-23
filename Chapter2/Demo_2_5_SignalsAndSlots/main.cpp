#include <QCoreApplication>
#include "worker.h"
#include "manager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Worker worker;
    Manager manager;

    // --- 信号与槽的连接 ---
    QObject::connect(&worker, &Worker::workFinished,
                     &manager, &Manager::onWorkFinished);

    qDebug() << "[主程序] 命令工作者开始工作。";
    worker.doWork(); // 调用此函数将最终导致信号被发射

    return a.exec();
}

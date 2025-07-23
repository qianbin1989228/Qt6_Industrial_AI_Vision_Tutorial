#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <QString>

class Worker : public QObject
{
    Q_OBJECT // 必须添加此宏以支持信号与槽
public:
    explicit Worker(QObject *parent = nullptr);
    void doWork(); // 模拟执行一个任务
signals:
    // 信号只需要声明，无需实现。在signals关键字下声明
    void workFinished(const QString &result);
};
#endif // WORKER_H

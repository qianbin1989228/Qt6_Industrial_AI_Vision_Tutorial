#ifndef MANAGER_H
#define MANAGER_H
#include <QObject>
#include <QString>

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
public slots:
    // 槽函数声明在 public slots: 区域
    void onWorkFinished(const QString &result);
};
#endif // MANAGER_H

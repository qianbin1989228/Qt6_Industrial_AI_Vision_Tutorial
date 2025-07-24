#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>

class Backend : public QObject
{
    Q_OBJECT // 必须添加，以支持信号槽和QML交互
public:
    explicit Backend(QObject *parent = nullptr);

    // 使用 Q_INVOKABLE 宏，使这个普通的C++成员函数可以被QML调用
    Q_INVOKABLE void startScan();

signals:
    // 定义一个信号，用于从C++向QML传递状态更新信息
    void statusMessageChanged(const QString &message);
};

#endif // BACKEND_H

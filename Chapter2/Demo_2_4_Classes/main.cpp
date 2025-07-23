#include <QCoreApplication>
#include <QDebug>
#include "simplecounter.h" // 包含我们自己定义的类的头文件

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "创建两个计数器: 合格品计数器和次品计数器";

    // 从SimpleCounter类创建两个具体的对象
    SimpleCounter qualifiedCounter;
    SimpleCounter defectiveCounter;

    // 模拟生产过程
    qDebug() << "生产了3个合格品...";
    qualifiedCounter.increment();
    qualifiedCounter.increment();
    qualifiedCounter.increment();

    qDebug() << "生产了1个次品...";
    defectiveCounter.increment();

    // 通过公有方法获取内部的计数值并打印
    qDebug() << "当前合格品数量:" << qualifiedCounter.getCount();
    qDebug() << "当前次品数量:" << defectiveCounter.getCount();

    return a.exec();
}

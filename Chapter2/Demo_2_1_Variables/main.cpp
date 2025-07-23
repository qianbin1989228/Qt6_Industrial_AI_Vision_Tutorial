#include <QCoreApplication>
#include <QDebug>       // 包含Qt的调试输出头文件
#include <QString>      // 包含Qt的字符串类头文件

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // --- 变量定义与初始化 ---
    int screwCount = 100;
    double diameter = 5.02;
    bool isQualified = true;
    QString batchNumber = "SN20250715-B";

    // --- 使用qDebug()进行控制台输出 ---
    qDebug() << "开始进行产品信息检查...";
    qDebug() << "螺丝数量:" << screwCount << "个";
    qDebug() << "产品直径:" << diameter << "mm";
    qDebug() << "质量状态:" << isQualified;
    qDebug() << "生产批号:" << batchNumber;
    qDebug() << "检查完毕。";

    return a.exec();
}

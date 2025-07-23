#include <QCoreApplication>
#include <QDebug>
#include <QVector> // 包含QVector容器的头文件

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 创建一个QVector容器，用于存放一批待检测的螺丝直径
    QVector<double> diameterBatch;
    diameterBatch.append(5.01); // 使用append()方法向容器末尾添加元素
    diameterBatch.append(4.98);
    diameterBatch.append(5.09); // 这个不合格
    diameterBatch.append(4.92); // 这个也不合格
    diameterBatch.append(5.04);

    qDebug() << "流水线来了一批产品，共" << diameterBatch.size() << "个，开始质检...";
    int qualifiedCount = 0;

    // 使用for循环遍历QVector中的每一个元素
    for (int i = 0; i < diameterBatch.size(); ++i) {
        double currentDiameter = diameterBatch[i]; // 使用索引[i]访问容器中的元素
        bool isCurrentQualified = (currentDiameter >= 4.95 && currentDiameter <= 5.05);

        if (isCurrentQualified) {
            qDebug() << "第" << i + 1 << "个螺丝，直径" << currentDiameter << "mm, 合格。";
            qualifiedCount++;
        } else {
            qDebug() << "第" << i + 1 << "个螺丝，直径" << currentDiameter << "mm, 不合格！";
        }
    }

    qDebug() << "质检完成。合格数量:" << qualifiedCount;
    return a.exec();
}

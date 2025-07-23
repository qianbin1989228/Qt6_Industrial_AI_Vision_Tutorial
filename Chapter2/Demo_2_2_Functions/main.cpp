#include <QCoreApplication>
#include <QDebug>

// --- 函数定义 ---
// 这个函数接收一个double类型的参数(parameter)，没有返回值(void)
// 它的作用是封装了完整的尺寸检查逻辑
void checkScrewDiameter(double diameterToTest)
{
    qDebug() << "--- 开始检查直径:" << diameterToTest << "mm ---";
    const double standardDiameter = 5.0;
    const double tolerance = 0.05;

    if (diameterToTest > standardDiameter + tolerance) {
        qDebug() << "判定结果: 不合格，直径过大。";
    } else if (diameterToTest < standardDiameter - tolerance) {
        qDebug() << "判定结果: 不合格，直径过小。";
    } else {
        qDebug() << "判定结果: 合格，尺寸在公差范围内。";
    }
    qDebug() << "--- 检查结束 ---" << Qt::endl; // Qt::endl显式地换行
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // --- 函数调用 ---
    // 通过函数名和传递相应的参数来执行函数中的代码
    checkScrewDiameter(5.08); // 调用函数，检查一个过大的螺丝
    checkScrewDiameter(4.93); // 调用函数，检查一个过小的螺丝
    checkScrewDiameter(5.01); // 调用函数，检查一个合格的螺丝

    return a.exec();
}

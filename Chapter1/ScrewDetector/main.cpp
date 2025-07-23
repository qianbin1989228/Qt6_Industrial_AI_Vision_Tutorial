#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon> // 添加图标类头文件引用

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 在创建QGuiApplication对象后，设置窗口图标
    app.setWindowIcon(QIcon(":/icons/appicon.png"));

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("ScrewDetector", "Main");

    return app.exec();
}

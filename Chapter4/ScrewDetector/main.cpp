#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon> // 添加图标类头文件引用
#include <QQmlContext>   // 1. 包含上下文头文件
#include "backend.h"    // 2. 包含我们自己的Backend头文件

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 在创建QGuiApplication对象后，设置窗口图标
    app.setWindowIcon(QIcon(":/icons/appicon.png"));

    QQmlApplicationEngine engine;

    // 3. 创建Backend的实例
    Backend backend;

    // 4. 将C++对象注册为QML的上下文属性
    //    第一个参数是QML中使用的名字，我们将其命名为"backend"
    //    第二个参数是C++对象的地址
    engine.rootContext()->setContextProperty("backend", &backend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("ScrewDetector", "Main");

    return app.exec();
}

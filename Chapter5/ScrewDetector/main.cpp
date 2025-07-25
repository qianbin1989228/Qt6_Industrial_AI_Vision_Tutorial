#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon> // 添加图标类头文件引用
#include <QQmlContext>   // 1. 包含上下文头文件
#include "backend.h"    // 2. 包含我们自己的Backend头文件
#include "imageprovider.h" // 1. 包含ImageProvider头文件

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 在创建QGuiApplication对象后，设置窗口图标
    app.setWindowIcon(QIcon(":/icons/appicon.png"));

    QQmlApplicationEngine engine;

    // 2. 实例化ImageProvider
    ImageProvider *imageProvider = new ImageProvider();

    // 3. 将Provider注册到QML引擎，并给它一个URL方案名，例如"liveImage"
    engine.addImageProvider(QLatin1String("liveImage"), imageProvider);

    // 4. 创建Backend实例时，将provider的指针传给它
    Backend backend(imageProvider);
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

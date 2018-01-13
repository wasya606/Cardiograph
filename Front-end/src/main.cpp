#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "appcontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    AppController appController;
    Q_UNUSED(appController);

    return app.exec();
}

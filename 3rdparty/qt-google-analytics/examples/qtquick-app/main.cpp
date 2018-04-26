#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QtQml>

#include "ganalytics.h"

int main(int argc, char* argv[])
{
    QApplication::setApplicationName("QtQuick-App");
    QApplication::setApplicationVersion("0.1");

    QApplication app(argc, argv);

    qmlRegisterType<GAnalytics>("analytics", 0, 1, "Tracker");

    QQmlApplicationEngine engine(QUrl("qrc:/qml/MainWindow.qml"));

    return app.exec();
}

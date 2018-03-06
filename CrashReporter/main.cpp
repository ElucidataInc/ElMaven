#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");

    MainWindow w;

    w.restartApplicationPath = QString(argv[1]);
    w.bucketName = QString(argv[2]);
    w.accessKey = QString(argv[3]);
    w.secretKey = QString(argv[4]);
    w.logInformation = QString(argv[5]);
    w.windowState = QString(argv[6]).toInt();

    w.show();
    w.onStart();

    return a.exec();
}

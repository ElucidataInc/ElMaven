#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");


    // argv[1] contains the session id of el-maven
    MainWindow w(nullptr,QString(argv[1]));

    w.show();

    return a.exec();
}

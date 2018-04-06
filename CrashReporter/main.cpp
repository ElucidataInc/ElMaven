#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");

    MainWindow w;

    w.show();

    return a.exec();
}

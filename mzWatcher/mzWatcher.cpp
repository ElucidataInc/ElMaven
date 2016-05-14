#include<QtGui>
#include<QDebug>
#include<QHttp>
#include "mainWindow.h"

int main( int argc, char *argv[] ) {
    QApplication app(argc, argv);
    MainWindow* mw = new MainWindow(0);
    mw->show();
    app.exec();
    return 1;
}

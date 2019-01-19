#include "mainwindow.h"
#include <QApplication>

#include "file_uploader.h"

#include <QFile>
#include <iostream>


#define _STR(X) #X
#define STR(X) _STR(X)


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "dump path: " << argv[1];
    qDebug() << "app name " << STR(APPNAME);
    qDebug() << "version: " << STR(APPVERSION);

    FileUploader uploader((QString(argv[1])));
    MainWindow w(nullptr, &uploader);
    w.show();

    return a.exec();
}

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
    QString endpoint;

    QFile f(":/endpoint");
    if(f.exists() && f.open(QIODevice::ReadOnly)) {
        while(!f.atEnd()) {
            endpoint = f.readLine().trimmed();
        }
    }
    else {
        qDebug() << "could not find the file containing the sentry endpoint";
        exit(0);
    }
    if(argc < 2) {
        qDebug() << "dump path should be provided to crash reporter";
        exit(0);
    }

    FileUploader uploader((QString(argv[1])),endpoint);
    MainWindow w(nullptr, &uploader);
    w.show();

    return a.exec();
}

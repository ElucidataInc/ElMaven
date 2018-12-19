#include "mainwindow.h"
#include <QApplication>

#include "file_uploader.h"

#include <QFile>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");


     std::cerr << "dump path from CR : " << argv[1] << std::endl;
    // argv[1] contains the path of dump files and logs

    FileUploader uploader((QString(argv[1])));
    MainWindow w(nullptr, &uploader);
    w.show();

    return a.exec();
}

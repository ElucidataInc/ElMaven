#include "mainwindow.h"
#include <QApplication>
#include "file_uploader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");


    // argv[1] contains the path of dump files and logs
    FileUploader uploader((QString(argv[1])));
    MainWindow w;

    w.show();

    return a.exec();
}

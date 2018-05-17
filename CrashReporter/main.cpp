#include "mainwindow.h"
#include <QApplication>
#ifdef Q_OS_WIN
#include "file_uploader.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");


    // argv[1] contains the path of dump files and logs
#ifdef Q_OS_WIN
    FileUploader uploader((QString(argv[1])));
    MainWindow w(nullptr, &uploader);
#endif

#ifdef Q_OS_UNIX
    MainWindow w(nullptr, QString(argv[1]));
#endif

    w.show();

    return a.exec();
}

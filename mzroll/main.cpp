#include "stable.h"
#include "globals.h"
#include "mainwindow.h"
#include "database.h"
#include "mzfileio.h"
#include <QtConcurrentMap>


Database DB;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/images/splash.png","PNG",Qt::ColorOnly);
    QSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();
    app.processEvents();

    MainWindow* mainWindow = new MainWindow();

    QStringList filelist;
    for (int i = 1; i < argc; ++i) {
        QString filename(argv[i]);

        if (filename.endsWith(".mzroll",Qt::CaseInsensitive) ) {
            mainWindow->projectDockWidget->loadProject(filename);
        }

        if (filename.endsWith("mzxml",Qt::CaseInsensitive) ||
                filename.endsWith("mzdata",Qt::CaseInsensitive) ||
                filename.endsWith("mzdata.xml",Qt::CaseInsensitive) ||
                filename.endsWith("cdf",Qt::CaseInsensitive) ||
                filename.endsWith("netcdf",Qt::CaseInsensitive) ||
                filename.endsWith("nc",Qt::CaseInsensitive) ||
                filename.endsWith("mzcsv",Qt::CaseInsensitive))  {
            filelist << filename;
            splash.showMessage("Loading " + filename, Qt::AlignLeft, Qt::white );
        }
    }

    splash.finish(mainWindow);
    mainWindow->show();

    if ( filelist.size() > 0 ) {
        mzFileIO* fileLoader  = new mzFileIO(mainWindow);
        fileLoader->setMainWindow(mainWindow);
        fileLoader->loadSamples(filelist);
        fileLoader->start();
    }

    int rv = app.exec();
    return rv;
}

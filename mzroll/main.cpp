/** @mainpage Project: Maven opensource 
*   @par MAVEN is an open source cross platform metabolomics data analyser.
*   The aim of this software packages is to reduce complexity of metabolomics 
*   analysis by developing a highly intitute interface for exploring and validating 
*   metabolomics data. The program features multi-file chromatographic aligner, 
*   peak-feature detector, isotope and adduct calculator, formula predictor, pathway
*   visualizer, and isotopic flux animator. Data from both triple quadropole and full
*   spectrum instruments is supported.
*
*   @par The project will not focus on developing or adding new features to the versions 
*   775 of MAVEN that is being currently in use by scientists at Agios. This version has 
*   numerous implementations of many features which enhance the utility of MAVEN.
*   Specifically, it allows for processing of many more samples (~100) for entire KEGG database 
*   (~1000 metabolites) for full scan high resolution data from QExactive
*/
/**
* @file main.cpp
*/





#include "stable.h"
#include "globals.h"
#include "mainwindow.h"
#include "database.h"
#include "mzfileio.h"
#include <QtConcurrentMap>



Database DB;
void customMessageHandler(QtMsgType type, const char *msg);

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QPixmap pixmap(":/images/splash.png","PNG",Qt::ColorOnly);
    QSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();
    //Added while merging with Maven776 - Kiran
    splash.showMessage(QString("Ver:")+ QString::number(MAVEN_VERSION), Qt::AlignBottom, Qt::black);
    app.processEvents();

    MainWindow* mainWindow = new MainWindow();
    qInstallMsgHandler(customMessageHandler);

    //updated while merging with Maven776 - Kiran
    for (int i = 1; i < argc; ++i)
        mainWindow->fileLoader->addFileToQueue(QString(argv[i]));

    splash.finish(mainWindow);
    mainWindow->show();
    mainWindow->fileLoader->start();
    int rv = app.exec();
    return rv;


}


void customMessageHandler(QtMsgType type, const char *msg)
{
        switch (type) {
        	case QtDebugMsg:
				cerr << "Debug: " << msg << endl;
                break;
        	case QtWarningMsg:
				cerr << "Warning: " << msg << endl;
                break;
        	case QtCriticalMsg:
				cerr << "Critical: " << msg << endl;
                break;
        	case QtFatalMsg:
                cerr << "Fetal: " << msg << endl;
				break;
                //abort();
        }
        //QFile outFile("debuglog.txt");
        //outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        //QTextStream ts(&outFile);
        //ts << txt << endl;
}


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
#define _STR(X) #X
#define STR(X) _STR(X)
#include "stable.h"
#include "globals.h"
#include "mainwindow.h"
#include "database.h"
#include "mzfileio.h"
#include "libplog/Log.h"
#include "libplog/Appenders/CustomAppender.h"
#include "controller.h"

#include <list>



Database DB;
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
plog::MyAppender<plog::TxtFormatter> myAppender; // Create our custom appender. 

int main(int argc, char *argv[])
{
    plog::init(plog::verbose, &myAppender); // Initialize the logger with our appender.

    QApplication app(argc, argv);
    QPixmap pixmap(":/images/splash.png","PNG",Qt::ColorOnly);
    QSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();
    //Added while merging with Maven776 - Kiran
    splash.showMessage(QString("Ver:") + STR(EL_MAVEN_VERSION), Qt::AlignBottom, Qt::black);
    app.processEvents();


    Controller contrl;
    qInstallMessageHandler(customMessageHandler);

    //updated while merging with Maven776 - Kiran
    for (int i = 1; i < argc; ++i)
        contrl.getMainWindow()->fileLoader->addFileToQueue(QString(argv[i]));

    splash.finish(contrl.getMainWindow());
    contrl.getMainWindow()->show();
    contrl.getMainWindow()->fileLoader->start();
    int rv = app.exec();
    return rv;


}


void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
        switch (type) {
        	case QtDebugMsg:
				cerr << "Debug: " << msg.toStdString() << endl;
                break;
        	case QtWarningMsg:
				cerr << "Warning: " << msg.toStdString() << endl;
                break;
        	case QtCriticalMsg:
				cerr << "Critical: " << msg.toStdString() << endl;
                break;
        	case QtFatalMsg:
                cerr << "Fatal: " << msg.toStdString() << endl;
				break;
                //abort();
        }
        //QFile outFile("debuglog.txt");
        //outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        //QTextStream ts(&outFile);
        //ts << txt << endl;
}


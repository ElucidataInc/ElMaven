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
#include "base64.h"
#include "Compound.h"
#include "mainwindow.h"
#include "database.h"
#include "mzfileio.h"
#include "controller.h"
#include "elmavenlogger.h"

#ifdef __OSX_AVAILABLE
#ifndef DEBUG
#include "common/sentry.h"
#endif
#endif

#ifdef Q_OS_MAC
#include <QDateTime>
#endif

#ifdef Q_OS_WIN
#ifndef DEBUG
#include "elmavexceptionhandler.h"
#endif
#endif

#include <QDir>
#include <list>

// initialize the global metabolite database
Database DB;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void initializeLogger()
{
    QDir dir;
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
                   qApp->organizationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + qApp->sessionId() \
                   + QDir::separator() ;

    #ifdef Q_OS_MAC
        // session id is not available on mac os. therefore we use QDatetime to create a unique path for
        // every session
        path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
            qApp->organizationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss") \
            + QDir::separator() ;
    #endif

    // Logs  won't be saved anywhere if this mkpath returns False;
    dir.mkpath(path);
    ElMavenLogger::init(path.toStdString());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");
    qApp->setApplicationVersion(STR(EL_MAVEN_VERSION));
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef __OSX_AVAILABLE
#ifndef DEBUG
    string sentryDsnEncoded(STR(SENTRY_DSN_BASE64));
    string sentryDsn = base64::decodeString(sentryDsnEncoded.c_str(),
                                            sentryDsnEncoded.size());
    // remove newline from the end appended by decoder
    sentryDsn = sentryDsn.substr(0, sentryDsn.size() - 1);
    if (!sentryDsn.empty()) {
        cerr << "Starting crash handling service…" << endl;
        sentry_options_t *options = sentry_options_new();
        sentry_options_set_dsn(options, sentryDsn.c_str());

        // path to bundled crash handler
        auto handlerPath = QCoreApplication::applicationDirPath()
                           + QDir::separator()
                           + "crashpad_handler";
        sentry_options_set_handler_path(options,
                                        handlerPath.toStdString().c_str());

        // path to dump files and other run specific information
        auto dbPath = QStandardPaths::writableLocation(
                          QStandardPaths::GenericConfigLocation)
                      + QDir::separator()
                      + "ElMaven";
        sentry_options_set_database_path(options,
                                         dbPath.toStdString().c_str());

        sentry_init(options);

        sentry_set_extra("Application",
                         sentry_value_new_string("El-MAVEN"));
        sentry_set_extra("Version",
                         sentry_value_new_string(STR(EL_MAVEN_VERSION)));
    } else {
        cerr << "DSN missing, build cannot report crashes." << endl;
    }
#endif
#endif

#ifdef Q_OS_WIN
#ifndef DEBUG
    elmavexceptionhandler::init();
#endif
#endif

    QPixmap pixmap(":/images/splash.png","PNG",Qt::ColorOnly);
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();


    Controller contrl;
    qInstallMessageHandler(customMessageHandler);

    for (int i = 1; i < argc; ++i)
        contrl.getMainWindow()->fileLoader->addFileToQueue(QString(argv[i]));

    splash.finish(contrl.getMainWindow());
    contrl.getMainWindow()->show();
    contrl.getMainWindow()->fileLoader->start();
    int rv = app.exec();

#ifdef __OSX_AVAILABLE
#ifndef DEBUG
    sentry_shutdown();
#endif
#endif

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


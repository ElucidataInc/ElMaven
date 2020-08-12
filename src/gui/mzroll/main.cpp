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
#include "ligandwidget.h"
#include "mainwindow.h"
#include "database.h"
#include "mzfileio.h"
#include "controller.h"
#include "elmavenlogger.h"
#include "phantomstyle.h"

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

enum ThemeType : int {
    ElMavenLight = 0,
    ElMavenDark = 1,
};

QPalette namedColorSchemePalette(ThemeType x) {
    struct ThemeColors {
        QColor window;
        QColor text;
        QColor disabledText;
        QColor brightText;
        QColor highlight;
        QColor highlightedText;
        QColor base;
        QColor alternateBase;
        QColor shadow;
        QColor button;
        QColor disabledButton;
        QColor tooltip;
        QColor tooltipText;
    };

    auto themeColorsToPalette = [](const ThemeColors& x) -> QPalette {
        QPalette pal;
        pal.setColor(QPalette::Window, x.window);
        pal.setColor(QPalette::WindowText, x.text);
        pal.setColor(QPalette::Text, x.text);
        pal.setColor(QPalette::ButtonText, x.text);
        if (x.brightText.isValid())
            pal.setColor(QPalette::BrightText, x.brightText);
        pal.setColor(QPalette::Disabled, QPalette::WindowText, x.disabledText);
        pal.setColor(QPalette::Disabled, QPalette::Text, x.disabledText);
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, x.disabledText);
        pal.setColor(QPalette::Base, x.base);
        pal.setColor(QPalette::AlternateBase, x.alternateBase);
        if (x.shadow.isValid())
            pal.setColor(QPalette::Shadow, x.shadow);
        pal.setColor(QPalette::Button, x.button);
        pal.setColor(QPalette::Highlight, x.highlight);
        pal.setColor(QPalette::HighlightedText, x.highlightedText);
        if (x.disabledButton.isValid())
            pal.setColor(QPalette::Disabled, QPalette::Button, x.disabledButton);
        pal.setColor(QPalette::ToolTipBase, x.tooltip);
        pal.setColor(QPalette::ToolTipText, x.tooltipText);
        // Used as the shadow text color on disabled menu items
        pal.setColor(QPalette::Disabled, QPalette::Light, Qt::transparent);
        return pal;
    };

    ThemeColors c;
    switch (x) {
    case ElMavenLight: {
        QColor base(0xffffff);
        QColor highlight(0xe3daff);
        QColor bright(0xffffff);
        QColor lessBright(0xf4f4f4);
        QColor button(0xfafafa);
        QColor text(0x141414);
        QColor disabledText(0x9a9a9a);
        c.window = bright;
        c.highlight = highlight;
        c.highlightedText = text;
        c.base = base;
        c.alternateBase = lessBright;
        c.button = button;
        c.text = text;
        c.disabledText = disabledText;
        c.tooltip = Qt::black;
        c.tooltipText = Qt::white;
        break;
    }
    case ElMavenDark: {
        // TODO: maybe someday :)
        break;
    }
    }
    return themeColorsToPalette(c);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qApp->setOrganizationName("ElucidataInc");
    qApp->setApplicationName("El-Maven");
    qApp->setApplicationVersion(STR(EL_MAVEN_VERSION));
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    // set application-wide style
    qApp->setStyle(new PhantomStyle);
    qApp->setPalette(namedColorSchemePalette(ElMavenLight));

    // set application-wide font
    QFontDatabase::addApplicationFont(
        ":/fonts/SourceSansPro/SourceSansPro-Regular.ttf");
    QFontDatabase::addApplicationFont(
        ":/fonts/SourceSansPro/SourceSansPro-Bold.ttf");
    QFontDatabase::addApplicationFont(
        ":/fonts/SourceSansPro/SourceSansPro-Light.ttf");
    QFontDatabase::addApplicationFont(
        ":/fonts/SourceSansPro/SourceSansPro-Italic.ttf");
    QFont interfaceFont("Source Sans Pro");
    interfaceFont.setPixelSize(14);
    qApp->setFont(interfaceFont);

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

    Controller controller;
    qInstallMessageHandler(customMessageHandler);

    for (int i = 1; i < argc; ++i) {
        controller.getMainWindow()->fileLoader->addFileToQueue(
            QString(argv[i]));
    }

    controller.getMainWindow()->show();
    splash.finish(controller.getMainWindow());
    controller.getMainWindow()->ligandWidget->updateIsotopesAndAdducts();
    controller.getMainWindow()->fileLoader->start();
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


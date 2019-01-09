#include "ElmavCrashHandler.h"

#include <QApplication>
#include <QStandardPaths>
#include <QProcess>

#ifdef Q_OS_MAC
#include <QDateTime>
#endif


#ifdef Q_OS_LINUX
#include <client/linux/handler/exception_handler.h>
#endif


#ifdef Q_OS_WIN
#include <client/windows/handler/exception_handler.h>
#define CRASH_REPORTER_WIN "CrashReporter.exe"
#endif

#ifdef Q_OS_MAC
#include <client/mac/handler/exception_handler.h>
#endif

#define CRASH_REPORTER_LINUX "CrashReporter"

static google_breakpad::ExceptionHandler* eh=0;

#ifdef Q_OS_LINUX
static google_breakpad::MinidumpDescriptor* md=0;

static bool startCrashReporter(const google_breakpad::MinidumpDescriptor& descriptor,void* context, bool succeeded)
{
    std::cerr << "path of dump file: " << descriptor.directory().c_str() << std::endl;
    QProcess* cReporter = new QProcess(nullptr);
    
    cReporter->startDetached(qApp->applicationDirPath() + QDir::separator() + CRASH_REPORTER_LINUX, \
    QStringList() << QString(descriptor.directory().c_str()));

    return succeeded;
}
#endif



#ifdef Q_OS_WIN
static bool startCrashReporter(const wchar_t* dump_path,const wchar_t* id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* ass, bool succeeded)
{

    std::cerr << "starting crash reporter" << std::endl;
    QProcess* cReporter = new QProcess(nullptr);
    cReporter->setProgram(qApp->applicationDirPath() + QDir::separator() + CRASH_REPORTER_WIN);
    cReporter->setArguments(QStringList() << QString::fromWCharArray(dump_path));
    cReporter->start();
    return true;
}
#endif


#ifdef Q_OS_MAC
static bool startCrashReporter(const char* dump_dir,const char* id, void* context, bool succeeded)
{

    std::cerr << "starting crash reporter" << std::endl;
    std::cerr << "dump path : " << eh->dump_path() << std::endl;
    std::cerr <<  "dump id : " << id << std::endl;

    QString crashReporterPath;
    QString binFolder = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." \
    + QDir::separator() + ".." + QDir::separator();

    crashReporterPath = binFolder + "CrashReporter.app" + QDir::separator() + "Contents" + QDir::separator() + "MacOS" + QDir::separator() + "CrashReporter";

    QProcess* cReporter = new QProcess(nullptr);

//    cReporter->setProgram(crashReporterPath);
//    cReporter->setArguments(QStringList() << QString(eh->dump_path().c_str()));
    qDebug() << "arguments : " << cReporter->arguments();
    cReporter->startDetached(crashReporterPath,QStringList() << QString(eh->dump_path().c_str()));
    return true;
}
#endif

ElmavCrashHandler::ElmavCrashHandler()
{
    // set up break pad
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
    dir.mkpath(path);

    std::cerr << "path of breakpad : " << path.toStdString() << std::endl;
    std::cerr << " path of crash reporter : " << qApp->applicationDirPath().toStdString() << std::endl;

    #ifdef Q_OS_LINUX
        md = new google_breakpad::MinidumpDescriptor(path.toStdString());
        eh = new google_breakpad::ExceptionHandler(*md, 0, startCrashReporter,0, true, -1);
    #endif

    #ifdef Q_OS_WIN
        eh = new google_breakpad::ExceptionHandler(path.toStdWString(), 0, startCrashReporter, 0, google_breakpad::ExceptionHandler::HANDLER_ALL);
    #endif

    #ifdef Q_OS_MAC
        eh = new google_breakpad::ExceptionHandler(path.toStdString(), 0, startCrashReporter, 0, true, nullptr);
    #endif

}

ElmavCrashHandler::~ElmavCrashHandler()
{

    #ifdef Q_OS_LINUX
        delete md;
    #endif

    delete eh;
}


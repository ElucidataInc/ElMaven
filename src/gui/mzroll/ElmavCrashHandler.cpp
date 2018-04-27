#include "ElmavCrashHandler.h"

#include <QApplication>
#include <QStandardPaths>

#ifdef linux
#include <client/linux/handler/exception_handler.h>
#endif

static google_breakpad::ExceptionHandler* eh=0;
static google_breakpad::MinidumpDescriptor* md=0;

static bool startCrashReporter(const google_breakpad::MinidumpDescriptor& descriptor,void* context, bool succeeded)
{
    std::cerr << "creating crash dump " << descriptor.path();

    // start the crash reporter
    return succeeded;
}

ElmavCrashHandler::ElmavCrashHandler()
{
    // set up break pad
    QDir dir;
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
                   qApp->organizationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + "crash_dumps" \
                   + QDir::separator() ;
    dir.mkpath(path);

    md = new google_breakpad::MinidumpDescriptor(path.toStdString());
    eh = new google_breakpad::ExceptionHandler(*md, 0, startCrashReporter,0, true, -1);

}

ElmavCrashHandler::~ElmavCrashHandler()
{
    delete md;
    delete eh;
}


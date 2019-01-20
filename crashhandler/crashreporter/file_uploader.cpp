#include "file_uploader.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QApplication>

#ifdef Q_OS_MAC
#include <QProcess>
#endif

#ifdef Q_OS_WIN
#include <common/windows/http_upload.h>
#endif

#include <iostream>

#ifdef Q_OS_LINUX
#include <common/linux/http_upload.h>
#endif

#define _STR(X) #X
#define STR(X) _STR(X)


FileUploader::FileUploader(const QString& dPath, const QString& endpoint):
    _dumpPath(dPath),
    _endpoint(endpoint)
{
}


FileUploader::~FileUploader()
{}

bool FileUploader::uploadMinidump()
{

    qDebug() << "dump path: " << _dumpPath;
    qDebug() << "app name " << STR(APPNAME);
    qDebug() << "version: " << STR(APPVERSION);
#ifdef Q_OS_WIN
    typedef std::basic_string<wchar_t> wstring;
    wstring endpoint = _endpoint.toStdWString();
    std::map<wstring, wstring> files;
    std::map<wstring, wstring> parameters;
    files[L"upload_file_minidump"] = _dumpPath.toStdWString();
    parameters[L"Application"] = QString(STR(APPNAME)).toStdWString();
    parameters[L"Version"] = QString(STR(APPVERSION)).toStdWString();
    qDebug() << "uploading file: " << _dumpPath;

    bool uploaded = google_breakpad::HTTPUpload::SendRequest(
                endpoint,
                parameters,
                files,
                /* timeout */ nullptr,
                /* response body */ nullptr,
                /* response code */ nullptr
                );
    qDebug() << "uploaded : " << uploaded;
    emit uploadDone();
#endif

#ifdef Q_OS_MAC
    qDebug() << "uploading ...";
    QString binFolder = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." \
    + QDir::separator() + ".." + QDir::separator();

    QString upPath = binFolder  + "sup.sh";

    uProcess = new QProcess;
    uProcess->setProgram(QStandardPaths::findExecutable("bash"));
    connect(uProcess, SIGNAL(finished(int)), this, SLOT(processFinished(int)));

    QStringList args;
    args << upPath;
    args << dmpFilePath;

    uProcess->setArguments(args);
    uProcess->start();
#endif


#if defined(Q_OS_X11) || defined (Q_OS_LINUX)
    std::cerr << "uploading ...." << std::endl;
    std::cerr << dmpFilePath.toStdString() << std::endl;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> files;
    files["upload_file_minidump"] = dmpFilePath.toStdString();
    google_breakpad::HTTPUpload::SendRequest("https://sentry.io/api/294375/minidump?sentry_key=5428a76c424142128a3ff1c04e5e342e", \
                                             parameters,files,
                                             "","","",
                                             nullptr,nullptr, nullptr);
    emit uploadDone();

#endif
}

#ifdef Q_OS_MAC
void FileUploader::processFinished(int exitCode)
{
    qDebug() << " exit status : " << uProcess->exitStatus();
    emit uploadDone();
}
#endif


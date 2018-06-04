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

FileUploader::FileUploader(const QString& dPath): dumpPath(dPath)
{

    if(!dumpPath.isEmpty())
        preProcess();

}

FileUploader::~FileUploader()
{}

bool FileUploader::uploadMinidump()
{
#ifdef Q_OS_WIN
    typedef std::basic_string<wchar_t> wstring;
    std::map<wstring, wstring> files;
    std::map<wstring, wstring> parameters;
    parameters[L"Logs"] =  QString(additionalData).toStdWString();
    files[L"upload_file_minidump"] = dmpFilePath.toStdWString();
    qDebug() << "uploading file: " << dmpFilePath;

    google_breakpad::HTTPUpload::SendRequest(
                L"https://sentry.io/api/294375/minidump?sentry_key=5428a76c424142128a3ff1c04e5e342e",
                parameters,
                files,
                /* timeout */ nullptr,
                /* response body */ nullptr,
                /* response code */ nullptr
                );
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
    args << QString(additionalData);

    uProcess->setArguments(args);
    uProcess->start();
#endif


#if defined(Q_OS_X11) || defined (Q_OS_LINUX)
    std::cerr << "uploading ...." << std::endl;
    std::cerr << dmpFilePath.toStdString() << std::endl;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> files;
    files["upload_file_minidump"] = dmpFilePath.toStdString();
    parameters["logs"] = QString(additionalData).toStdString();
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

void FileUploader::preProcess()
{

    QDir dir(dumpPath);
    if(dir.exists()) {
        dir.setFilter(QDir::Files);

        // find dmp files in the dumpPath
        dir.setNameFilters(QStringList() << "*.dmp");
        for(const QString& fPath: dir.entryList()) {
            // pick the first file. ideally there should only be  one dmp file per session
            dmpFilePath = dir.filePath(fPath);
            break;
        }

        // find the logs and read it's contents
        dir.setNameFilters(QStringList() << "*logs*");
        QFile fptr;
        for(const QString& fPath: dir.entryList()) {

            fptr.setFileName(dir.filePath(fPath));
            if(fptr.open(QIODevice::ReadOnly)) {
                additionalData += fptr.readAll();
            }
            fptr.close();
        }
    }


    else
        std::cerr << "dir does not exist " << std::endl;
}

#include "file_uploader.h"
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN
#include <common/windows/http_upload.h>
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
    typedef std::basic_string<wchar_t> wstring;
    std::map<wstring, wstring> files;
    std::map<wstring, wstring> parameters;
    files[L"upload_file_minidump"] = dmpFilePath.toStdWString();
    qDebug() << "uploading file: " << dmpFilePath;

    return google_breakpad::HTTPUpload::SendRequest(
                L"https://sentry.io/api/294375/minidump?sentry_key=5428a76c424142128a3ff1c04e5e342e",
                parameters,
                files,
                /* timeout */ nullptr,
                /* response body */ nullptr,
                /* response code */ nullptr
                );
}

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
            fptr.setFileName(fPath);
            if(fptr.open(QIODevice::ReadOnly)) {
                additionalData += fptr.readAll();
            }
            fptr.close();
        }

    }
    else
        qDebug() << "dir does not exist ";

}

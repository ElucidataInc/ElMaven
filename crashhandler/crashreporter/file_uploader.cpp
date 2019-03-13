#include "file_uploader.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QApplication>

#ifdef Q_OS_MAC
#include "macuploader/interface.h"
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


FileUploader::FileUploader(const QString& dPath, const QString& dir, const QString& endpoint):
    _dumpPath(dPath),
    _dumpDir(dir),
    _endpoint(endpoint)
#if defined(Q_OS_MAC)
    ,_iuploader(nullptr)
#endif
{
}


FileUploader::~FileUploader()
{
    QDir dir(_dumpDir);
    if(dir.exists()) {

        if(dir.removeRecursively())
            qDebug() << "removed " << _dumpDir;
        else
            qDebug() << "failed to remove " << _dumpDir;
    }
    else
        qDebug() << "dir : " << _dumpDir << "does not exist";
}

void FileUploader::uploadMinidump()
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
#endif

#ifdef Q_OS_MAC
    _iuploader = new UploaderInterface;
    _iuploader->init(_endpoint.toStdString().c_str(),
                     _dumpPath.toStdString().c_str(),
                     STR(APPNAME),
                     STR(APPVERSION));
    _iuploader->upload();
#endif


#if defined(Q_OS_X11) || defined (Q_OS_LINUX)
    std::cerr << "uploading ...." << std::endl;
    std::cerr << _dumpPath.toStdString() << std::endl;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> files;
    files["upload_file_minidump"] = _dumpPath.toStdString();
    parameters["Application"] = QString(STR(APPNAME)).toStdString();
    parameters["Version"] = QString(STR(APPVERSION)).toStdString();
    google_breakpad::HTTPUpload::SendRequest(
                _endpoint.toStdString(),
                parameters,
                files,
                "",
                "",
                "",
                nullptr,
                nullptr,
                nullptr
                );

#endif

    emit uploadDone();
}


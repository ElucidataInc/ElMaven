#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "downloadmanager.h"
#include "logger.h"
#include "pollyCLI/pollyintegration.h"

DownloadManager::DownloadManager():
    _manager(new QNetworkAccessManager),
    _reply(nullptr),
    err(false)
{
}

Logger* DownloadManager::_log()
{
    QString parentFolder = "ElMaven";
    QString logFile = QString::fromStdString(Logger::constant_time()
                                             + "_download_manager.log");
    QString fpath = QStandardPaths::writableLocation(
                        QStandardPaths::GenericConfigLocation)
                    + QDir::separator()
                    + parentFolder
                    + QDir::separator()
                    + logFile;
    static Logger* log = new Logger(fpath.toStdString());
    return log;
}

void DownloadManager::setRequest(const QString &url, void *requester, bool async)
{
    _url = url;
    //reset the data for every url
    _data = "";
    PollyIntegration* _requester = reinterpret_cast<PollyIntegration*>(requester);
    connect(this, SIGNAL(downloaded()), _requester, SLOT(requestSuccess()));
    connect(this, SIGNAL(failed()), _requester, SLOT(requestFailed()));
    download(async);
}

void DownloadManager::download(bool async)
{
    QNetworkRequest request(_url);
    _reply = _manager->get(request);

    if(!async) {
        // synchronous request
        QEventLoop loop;
        connect(_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        connect(_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                &loop, &QEventLoop::quit);
        loop.exec();
        if(_reply->error() == QNetworkReply::NoError) {
            _data += _reply->readAll();
            err = false;
        } else {
            _log()->error() << "Failed to download requested file\n"
                          << "\tError code: "
                          << QString(_reply->error()).toStdString()
                          << "\n"
                          << "\tError: "
                          << _reply->errorString().toStdString()
                          << std::flush;
            err = true;

        }
    } else {
        connect(_reply, &QNetworkReply::readyRead, this, &DownloadManager::dataAvailable);
        connect(_reply, &QNetworkReply::finished, this, &DownloadManager::finished);
        connect(_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                this, &DownloadManager::error);
    }
}

QByteArray DownloadManager::getData()
{
    return _data;
}


void DownloadManager::dataAvailable()
{
    _log()->debug() << "Data available" << std::flush;
    while(_reply->bytesAvailable()) {
        _data += _reply->readLine();
    }
}

void DownloadManager::finished()
{
    if(err) {
        _log()->debug() << "Failed to download requested file" << std::flush;
        _log()->error() << QString(_reply->errorString()).toStdString()
                      << std::flush;
         emit failed();
    } else {
        // read any data that might be in buffer
        _data += _reply->readAll();
        _log()->debug() << "File download successful"
                      << std::flush;
        emit downloaded();
    }
}


void DownloadManager::error(QNetworkReply::NetworkError error)
{
    _log()->debug() << "Failed to download requested file" << std::flush;
    _log()->debug() << "\tError code: "
                  << QString(error).toStdString()
                  << std::flush;
    _log()->debug() << "\tError string: "
                  << _reply->errorString().toStdString()
                  << std::flush;
    err = true;
}

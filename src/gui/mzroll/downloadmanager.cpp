#include "downloadmanager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include  <QNetworkReply>
#include <QDebug>
#include "pollyintegration.h"

DownloadManager::DownloadManager():
    _manager(new QNetworkAccessManager),
    _reply(nullptr),
    err(false)
{
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
            qDebug() << "sync request complete";
        } else {
            qDebug() << "error code : " << _reply->error();
            qDebug() << "error : " << _reply->errorString();
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
    qDebug() << "data available";
    while(_reply->bytesAvailable()) {
        _data += _reply->readLine();
    }
}

void DownloadManager::finished()
{
    if(err) {
        qDebug() << _reply->errorString();
         emit failed();
    } else {
        // read any data that might be in buffer
        _data += _reply->readAll();
        qDebug() << "success";
        emit downloaded();
    }
}


void DownloadManager::error(QNetworkReply::NetworkError error)
{
    qDebug() << "error code :" << error;
    qDebug() << "error string: "<< _reply->errorString();
    err = true;
}

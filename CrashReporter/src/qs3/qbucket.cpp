#include "qs3/qs3.h"
#include "qs3/qbucket.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWriteLocker>
#include <QNetworkProxy>
#include <QEventLoop>
#include <QUrl>
#include <QDebug>

namespace QS3 {

Bucket::Bucket(const QString& bucketName, const S3* s3, const QString& accessKey, const QString& secretKey, QObject *parent) :
    QObject(parent), _bucketName(bucketName), _s3(s3), _accessKey(accessKey), _secretKey(secretKey)
{
    this->_manager = new QNetworkAccessManager();

    // https://gist.github.com/gokmen/2558692
    this->connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* qnr, QList<QSslError> errors) {
        qDebug() << "sslErrorHandler: " << qnr->url().toString();
        // show list of all ssl errors
        foreach (QSslError err, errors) {
            qDebug() << "ssl error: " << err.errorString() << err;
        }
        //std::cerr << qnr->sslConfiguration().peerCertificate().toText();
        qnr->ignoreSslErrors(errors);
    });

    auto proxyUrl = s3->proxy();
    if (!proxyUrl.isEmpty()) {
        QUrl url(proxyUrl);
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, url.host(), url.port());
        this->_manager->setProxy(proxy);
    }

    connect(this->_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(_receiveFinished(QNetworkReply*)));
}

Bucket::~Bucket()
{
    delete this->_manager;
}

void Bucket::upload(const QString &name, const QByteArray &data)
{
    QWriteLocker locker(&this->_lock);

    auto task = QSharedPointer<NetworkTask>(new NetworkTask(name));
    auto host = this->_s3->hostName();
    QUrl url(QString("http://%1.%2/%3").arg(this->_bucketName, host, name));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(data.size()));
    request.setRawHeader("Host", QString("%1.%2").arg(this->_bucketName, host).toLocal8Bit());
    this->_s3->signV2(request, this->_accessKey, this->_secretKey, "PUT", QString("/%1/%2").arg(this->_bucketName, name));

    QNetworkReply* reply = this->_manager->put(request, data);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_receiveProgress(qint64,qint64)));
    connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* reply, QList<QSslError> errors) {
        qDebug() << "ssl errors";
        foreach(auto error, errors) {
            qDebug() << error.errorString();
        }
        reply->ignoreSslErrors(errors);
    });

    this->_tasks[reply] = task;
}

void Bucket::download(const QString &name)
{
    QWriteLocker locker(&this->_lock);

    auto task = QSharedPointer<NetworkTask>(new NetworkTask(name, false));
    auto host = this->_s3->hostName();
    QUrl url(QString("http://%1.%2/%3").arg(this->_bucketName, host, name));
    QNetworkRequest request(url);
    request.setRawHeader("Host", QString("%1.%2").arg(this->_bucketName, host).toLocal8Bit());
    this->_s3->signV2(request, this->_accessKey, this->_secretKey, "GET", QString("/%1/%2").arg(this->_bucketName, name));

    QNetworkReply* reply = this->_manager->get(request);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_receiveProgress(qint64,qint64)));
    connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* reply, QList<QSslError> errors) {
        qDebug() << "ssl errors";
        foreach(auto error, errors) {
            qDebug() << error.errorString();
        }
        reply->ignoreSslErrors(errors);
    });

    this->_tasks[reply] = task;
}

int Bucket::downloadSync(const QString &name, QByteArray &result)
{
    QSharedPointer<NetworkTask> task;
    {
        QWriteLocker locker(&this->_lock);

        task = QSharedPointer<NetworkTask>(new NetworkTask(name, false));
        auto host = this->_s3->hostName();
        QUrl url(QString("http://%1.%2/%3").arg(this->_bucketName, host, name));
        QNetworkRequest request(url);
        request.setRawHeader("Host", QString("%1.%2").arg(this->_bucketName, host).toLocal8Bit());
        this->_s3->signV2(request, this->_accessKey, this->_secretKey, "GET", QString("/%1/%2").arg(this->_bucketName, name));

        task->loop = new QEventLoop();
        QNetworkReply* reply = this->_manager->get(request);
        this->_tasks[reply] = task;

        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_receiveProgress(qint64,qint64)));
        connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* reply, QList<QSslError> errors) {
            qDebug() << "ssl errors";
            foreach(auto error, errors) {
                qDebug() << error.errorString();
            }
            reply->ignoreSslErrors(errors);
        });
    }

    task->loop->exec();

    {
        QWriteLocker locker(&this->_lock);
        result.swap(task->downloadedContent);
        return task->httpStatusCode;
    }
}

void Bucket::_receiveFinished(QNetworkReply *reply)
{
    QWriteLocker locker(&this->_lock);
    auto task = this->_tasks[reply];
    task->finished = true;
    reply->deleteLater();

    task->httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (task->send) {
        emit uploaded(task->key, task->httpStatusCode);
    } else {
        QByteArray receivedData = reply->readAll();
        emit downloaded(task->key, receivedData, task->httpStatusCode);
        task->downloadedContent.swap(receivedData);
    }

    if (task->loop) {
        task->loop->quit();
        task->loop->deleteLater();
        task->loop = nullptr;
    }
    bool isAllFinished = true;
    for (auto task : this->_tasks.values()) {
        if (!task->finished) {
            isAllFinished = false;
            break;
        }
    }
    if (isAllFinished) {
        this->_tasks.clear();
        emit finished();
    }
}

void Bucket::_receiveProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QWriteLocker locker(&this->_lock);
    auto reply = qobject_cast<QNetworkReply*>(this->sender());
    auto task = this->_tasks[reply];
    if (task) {
        emit progress(task->key, bytesSent, bytesTotal);
    }
}

}

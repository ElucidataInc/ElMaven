#ifndef QBUCKET_H
#define QBUCKET_H

#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QReadWriteLock>
#include <QSharedPointer>

class QNetworkAccessManager;
class QNetworkReply;
class QEventLoop;

namespace QS3 {

class S3;

struct NetworkTask {
    NetworkTask(const QString& filename, bool sendFlag=true) : finished(false), key(filename), send(sendFlag), loop(nullptr), httpStatusCode(0) {}
    bool finished;
    QString key;
    bool send;
    QEventLoop* loop;
    QByteArray downloadedContent;
    int httpStatusCode;
};

class Bucket : public QObject
{
    Q_OBJECT
public:
    explicit Bucket(const QString& bucketName, const S3* s3, const QString& accessKey, const QString& secretKey, QObject *parent = 0);
    ~Bucket();
    void upload(const QString& name, const QByteArray& data);
    void download(const QString& name);
    int downloadSync(const QString& name, QByteArray& result);

signals:
    void finished();
    void progress(const QString& key, qint64 sent, qint64 total);

    void uploaded(const QString& key, int httpStatus);
    void downloaded(const QString& key, const QByteArray& data, int httpStatus);

private slots:
    void _receiveFinished(QNetworkReply* reply);
    void _receiveProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    QReadWriteLock _lock;

    QString _bucketName;
    const S3* _s3;
    QString _accessKey;
    QString _secretKey;
    QNetworkAccessManager* _manager;
    QMap<QNetworkReply*, QSharedPointer<NetworkTask> > _tasks;
};

}

#endif // QBUCKET_H

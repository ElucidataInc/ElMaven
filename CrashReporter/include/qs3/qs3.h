#ifndef QS3_H
#define QS3_H

#include <QObject>
#include "qbucket.h"
#include <QNetworkRequest>

namespace QS3 {

class S3 : public QObject
{
    Q_OBJECT
public:
    explicit S3(const QString& hostName, QObject *parent = 0);
    explicit S3(const QString& hostName, const QString& proxy, QObject *parent = 0);

    QString hostName() const;
    QString proxy() const;

    void signV2(QNetworkRequest& request,
                const QString& accessKey,
                const QString& secretKey,
                const QString& method,
                const QString& pathString) const;

    QString signatureV2(const QString& accessKey,
                        const QString& secretKey,
                        const QString& method,
                        const QString& dateString,
                        const QString& pathString,
                        const QString& contentType = "",
                        const QString& md5 = "") const;

    QS3::Bucket* bucket(const QString& bucketName, const QString& accessKey, const QString& secretKey) const;

signals:

public slots:

protected:
    QString _hmacSha1(QByteArray key, QByteArray baseString) const;

    QString _hostName;
    QString _proxy;
};

}

#endif // QS3_H

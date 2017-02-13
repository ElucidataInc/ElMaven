#include "qs3/qs3.h"
#include <QDateTime>
#include <QStringList>
#include <QCryptographicHash>
#include <QDebug>

namespace QS3 {

S3::S3(const QString& hostName, QObject *parent) :
    QObject(parent), _hostName(hostName)
{
}

S3::S3(const QString& hostName, const QString& proxy, QObject *parent) :
    QObject(parent), _hostName(hostName), _proxy(proxy)
{
}

QString S3::hostName() const
{
    return _hostName;
}

QString S3::proxy() const
{
    return _proxy;
}

void S3::signV2(QNetworkRequest &request, const QString &accessKey, const QString &secretKey, const QString &method, const QString &pathString) const
{
    auto date = QDateTime::currentDateTimeUtc();
    QString dateString = date.toString("ddd, dd MMM yyyy hh:mm:ss +0000");
    request.setRawHeader("Date", dateString.toLocal8Bit());
    QString signature = this->signatureV2(accessKey, secretKey, method, dateString, pathString);
    //qDebug() << signature;
    request.setRawHeader("Authorization", signature.toLocal8Bit());
}

QString S3::signatureV2(const QString &accessKey, const QString &secretKey, const QString &method, const QString &dateString, const QString &pathString, const QString &contentType, const QString &md5) const
{
    QStringList inputs;
    inputs << method << md5 << contentType << dateString << pathString;

    QString templateString = "AWS %1:%2";

    return templateString.arg(accessKey, this->_hmacSha1(secretKey.toLocal8Bit(), inputs.join("\n").toLocal8Bit()));
}

QS3::Bucket* S3::bucket(const QString &bucketName, const QString &accessKey, const QString &secretKey) const
{
    return new QS3::Bucket(bucketName, this, accessKey, secretKey);
}

// http://qt-project.org/wiki/HMAC-SHA1
QString S3::_hmacSha1(QByteArray key, QByteArray baseString) const
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toBase64();
}

}

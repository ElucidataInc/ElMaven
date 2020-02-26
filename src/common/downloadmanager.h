#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H
#include <QObject>
#include <QUrl>
#include <QNetworkReply>

class Logger;
class QNetworkAccessManager;
class QNetworkRequest;
class QUrl;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager();
    void setRequest(const QString& url, void* requester, bool async=true);
    QByteArray getData();
    bool err;
 //TOOD: Use this to deduce the request type instead of "bool async"
//    enum class RequestType {
//        ASYNC,
//        SYNC
//    };


private:
    void download(bool async=true);
    Logger* _log();

private slots:
    void dataAvailable();
    void finished();
    void error(QNetworkReply::NetworkError error);

Q_SIGNALS:
    void failed();
    void downloaded();

private:
    QNetworkAccessManager* _manager;
    QNetworkReply* _reply;
    QUrl _url;
    QByteArray _data;
};

#endif // DOWNLOADMANAGER_H

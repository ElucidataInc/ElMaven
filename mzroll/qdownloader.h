#ifndef QDOWNLOADER_H
#define QDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QStringList>

class QDownloader : public QObject
{
    Q_OBJECT
public:
    explicit QDownloader(QObject *parent = 0);
    virtual ~QDownloader();
    void setFile(QString fileURL, QString destFolder, QString destFile);
    void getPage(QString fileURL);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    QByteArray pagedata;

signals:
    void downloadResult(QString);

private slots:
    void onDownloadProgress(qint64,qint64);

    void endDataDownload(QNetworkReply*);
    void writeDataToByteArray();

    void endFileWrite(QNetworkReply*);
    void writeDataToFile();

    void fileWriteFinished();


};

#endif // QDOWNLOADER_H

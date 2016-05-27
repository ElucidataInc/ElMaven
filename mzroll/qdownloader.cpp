#include "qdownloader.h"

QDownloader::QDownloader(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager;
}

QDownloader::~QDownloader()
{
    manager->deleteLater();
}

void QDownloader::getPage(QString URL)
{

    QNetworkRequest request;
    request.setUrl(QUrl(URL));
    reply = manager->get(request);

    qDebug() << "Downloading " + URL;
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(onDownloadProgress(qint64,qint64)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(endDataDownload(QNetworkReply*)));
    connect(reply,SIGNAL(readyRead()),this,SLOT(writeDataToByteArray()));
}


void QDownloader::setFile(QString fileURL, QString destFolder, QString destFile)
{
    QString filePath = fileURL;
    QString saveFilePath;
    QStringList filePathList = filePath.split('/');
    QString fileName = destFile;
    if (destFile.isEmpty()) {
        fileName = filePathList.at(filePathList.count() - 1);
    }
    saveFilePath = QString(destFolder + "/" + fileName );

    QNetworkRequest request;
    request.setUrl(QUrl(fileURL));
    reply = manager->get(request);

    file = new QFile;
    file->setFileName(saveFilePath);
    file->open(QIODevice::WriteOnly);

    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(onDownloadProgress(qint64,qint64)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(endFileWrite(QNetworkReply*)));
    connect(reply,SIGNAL(readyRead()),this,SLOT(writeDataToFile()));
    connect(reply,SIGNAL(finished()),this,SLOT(fileWriteFinished()));
}

void QDownloader::onDownloadProgress(qint64 bytesRead,qint64 bytesTotal)
{
    qDebug(QString::number(bytesRead).toLatin1() +" - "+ QString::number(bytesTotal).toLatin1());
}

void QDownloader::endFileWrite(QNetworkReply * reply)
{
    switch(reply->error())
    {
        case QNetworkReply::NoError:
        {
            emit downloadResult(file->fileName() );
        }break;
        default:{
            emit downloadResult("Download Failed");
            qDebug(reply->errorString().toLatin1());
        };
    }

    if(file->isOpen())
    {
        file->close();
        file->deleteLater();
    }
}


void QDownloader::endDataDownload(QNetworkReply * reply)
{
    switch(reply->error())
    {
        case QNetworkReply::NoError:
        {
            QString pageText(pagedata.constData());
            emit downloadResult(pageText);
        } break;
        default:{
            emit downloadResult("Download Failed");
            qDebug(reply->errorString().toLatin1());
            cerr << "DOWNLOAD ERROR: " << reply->errorString().toStdString() << endl;
        };
    }
}

void QDownloader::writeDataToFile()
{
    file->write(reply->readAll());
}

void QDownloader::writeDataToByteArray()
{
    pagedata = reply->readAll();
}

void QDownloader::fileWriteFinished()
{
    if(file->isOpen())
    {
        file->close();
        file->deleteLater();
    }
}

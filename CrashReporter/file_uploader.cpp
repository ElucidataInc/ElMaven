#include "file_uploader.h"
#include <QDir>
#include <QDebug>

FileUploader::FileUploader(const QString& dPath): dumpPath(dPath)
{

    if(!dumpPath.isEmpty())
        preProcess();

}

void FileUploader::preProcess()
{
    QDir dir(dumpPath);
    if(dir.exists()) {
        dir.setFilter(QDir::Files);

        // find dmp files in the dumpPath
        dir.setNameFilters(QStringList() << "*.dmp");
        for(const QString& fPath: dir.entryList()) {
            // pick the first file. ideally there should only be  one dmp file per session
            dmpFilePath = dir.filePath(fPath);
            break;
        }

        // find the logs and read it's contents
        dir.setNameFilters(QStringList() << "*logs*");
        QFile fptr;
        for(const QString& fPath: dir.entryList()) {
            fptr.setFileName(fPath);
            if(fptr.open(QIODevice::ReadOnly)) {
                additionalData += fptr.readAll();
            }
            fptr.close();
        }

    }
    else
        qDebug() << "dir does not exist ";

}

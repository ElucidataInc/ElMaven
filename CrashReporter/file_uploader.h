#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H
#include <QString>
#include <QObject>

class FileUploader: public QObject
{

    Q_OBJECT
    public:
        FileUploader(const QString& dPath);
        ~FileUploader();

    public slots:
        bool uploadMinidump();

    private:
        /**
         * @brief prepare all the files and contents that have to be uploaded. i.e dmp file and logs
         *
         */
        void preProcess();
        QString dumpPath;
        QString dmpFilePath;
        QByteArray additionalData;
};
#endif

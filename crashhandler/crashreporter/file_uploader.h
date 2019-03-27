#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H
#include <QString>
#include <QObject>


#ifdef Q_OS_MAC
class UploaderInterface;
#endif

class FileUploader: public QObject
{

    Q_OBJECT
    public:
        FileUploader(const QString& dPath, const QString& dir, const QString& endpoint);
        ~FileUploader();

    public slots:
        void uploadMinidump();

    private:
        /**
         * @brief prepare all the files and contents that have to be uploaded. i.e dmp file and logs
         */
        QString _dumpPath;
        QString _dumpDir;
        QString _endpoint;

    Q_SIGNALS:
        void uploadDone();

#ifdef Q_OS_MAC
    private:
        UploaderInterface* _iuploader;
#endif
};
#endif

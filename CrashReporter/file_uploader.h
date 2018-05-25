#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H
#include <QString>
#include <QObject>


#ifdef Q_OS_MAC
class QProcess;
#endif

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
         */
        void preProcess();
        QString dumpPath;
        QString dmpFilePath;
        QByteArray additionalData;

    #if defined(Q_OS_MAC) | defined(Q_OS_WIN)
    Q_SIGNALS:
        void uploadDone();
    #endif

    #ifdef Q_OS_MAC
    public slots:
        void processFinished(int exitCode);
    private:
        QProcess* uProcess;
    #endif
};
#endif

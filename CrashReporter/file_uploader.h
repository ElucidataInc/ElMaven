#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H
#include <QString>

class FileUploader{

    public:
        FileUploader(const QString& dPath);

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

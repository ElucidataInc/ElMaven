#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

//#include <QThread>
#include <QProcess>
#include <common/downloadmanager.h>

class AutoUpdate: public QObject
//        public QThread
{
    Q_OBJECT
public:
    AutoUpdate(DownloadManager* dlManager);
    ~AutoUpdate();
    void start();
//    void setDownloadManager(DownloadManager* manager);

    enum class UpdateState {
        NotStarted,
        Updating,
        Success,
        Failure
    };
    const char* newVersion;
    bool updateAvailable;

private:
    void installUpdate();
    void checkForUpdate();
    void parseOutput();
protected:

private slots:
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus status);
    void processError(QProcess::ProcessError err);
    void readOutput();
    void readError();

Q_SIGNALS:
    void statusChanged();
    void success();
    void failure();


private:
//    UpdateState _state;
    QProcess* _proc;
    QByteArray _output;
    QByteArray _error;
    DownloadManager _dlManager;

};

#endif
// request download manager to download the update
// use maintenance tool to update


// start the maintenance tool
// install the update
// wait for process to complete
// inform user that new update has been installed

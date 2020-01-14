#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QProcess>
#include <common/downloadmanager.h>

class AutoUpdate: public QObject
{
    Q_OBJECT
public:
    AutoUpdate();
    ~AutoUpdate();

    void checkForUpdate();
    const char* newVersion;
    enum State {
        NotStarted,
        CheckingForUpdates,
        Updating
    };

private:
    void parseOutput();

private slots:
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus status);
    void processError(QProcess::ProcessError err);
    void readOutput();
    void readError();

public slots:
    void update();

Q_SIGNALS:
    void statusChanged();
    void updateAvailable();
    void failure();

private:
    State _state;
    QProcess* _proc;
    QByteArray _output;
    QByteArray _error;

};

#endif
// request download manager to download the update
// use maintenance tool to update


// start the maintenance tool
// install the update
// wait for process to complete
// inform user that new update has been installed

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QtGui>
#include<QtSql>
#include<QHttp>
#include<QFileSystemWatcher>
#include<QDebug>
#include "ui_mzWatcherGui.h"

class BackgroundThread : public QThread
{
    Q_OBJECT

public:
    BackgroundThread(QWidget*) {
        _stopped=false;
    }
    void setSystemCommand(QString cmd) {
        command = cmd;
    }
    void killProcess() {
        _stopped=true;
    }

signals:
    void statusChanged(QString);

protected:
    void run(void) {
        QProcess converter;
        converter.start(command);

        _stopped=false;

        emit statusChanged("Runing " + command);

        if (!converter.waitForStarted()) {
            emit statusChanged("Command Failed!");
            converter.kill();
            return;
        }

        while (!converter.waitForFinished()) {
            emit statusChanged("Running conversion");
            if (_stopped) {
                converter.terminate();;
                emit statusChanged("Job killed");
                _stopped=false;
                return;
            }
            sleep(1);
        };

        emit statusChanged("Converstion done!");
    }

private:
    bool _stopped;
    QString command;

};

class MainWindow:public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent);

public slots:
    void updateFileList();
    void getFileList(const QString &fromDir);
    void processChangedFiles();
    void processFile(QString filename);
    void processSelectedFiles();
    void getFormValues();
    void monitor();
    void setStatus(QString status);
    void stop_startConversion();
    void startConversion();
    void stopConversion();
    void selectDestFolder();
    void selectSourceFolder();
    void clearTables();
    void updateButtonColors();

protected:
    void timerEvent(QTimerEvent *event);
    bool checkParameters();
    void closeEvent(QCloseEvent *event);

private:
    QWidget* centralWidget;
    Ui_mzWatcherGui* guiForm;
    QSet<QString>directoryList;
    QHash<QString,int>dbFiles;
    QHash<QString,int>fileList;


    QString extention;
    QString sourceFolder;
    QString destFolder;
    QString convertCommand;
    BackgroundThread* converter;

    int timerId;
    unsigned int maxDayDiff;

    QSqlDatabase DB;

    QSettings* settings;
    void readSettings();
    void writeSettings();


    //database functions
    void createTables();
    void insertFileInfo(QString);
    void showDataFilesTable();
    void markFileConverted(QString filename);
    void getDBFileList();

    QString createTempPath();

    void convertFile(QString filename);
    void makeBackupCopy(QString filename);


    //remote database connection
    int connectionId;
    QHttp http;

    void remoteLogMessage(QString infotype, QString filename, int fileSize, QString msgText);
    void readRemoteData(const QHttpResponseHeader &resp);

};



#endif

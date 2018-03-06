#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QDir>
#include <QtDebug>
#include <QProcess>
#include "include/qs3/qs3.h"
#include <QDateTime>
#include <QEventLoop>
#include <QStatusBar>
#include <QStatusBar>


class MainWindow : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString restartApplicationPath;
    QString accessKey;
    QString secretKey;
    QString bucketName;
    QString logInformation;
    int windowState;
    void onStart();

public slots:
    void readOutput();
    void readError();
    void finished(int exitCode);
    void started();
    void processError(QProcess::ProcessError perr);

private:
    Ui::MainWindow *ui;
    void startElMaven();
    void processLogs();
    void uploadLogs();

private Q_SLOTS:
    void uploadToS3Done();

    void on_restart_clicked();

    void on_cancel_clicked();

    void on_reportRestart_clicked();

private:
    QProcess* _process;
    QString _script;
    QString _logs;
    QStringList _logsPath;
};

#endif // MAINWINDOW_H

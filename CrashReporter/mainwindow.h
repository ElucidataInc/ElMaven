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

private:
    Ui::MainWindow *ui;
    void startElMaven();
private Q_SLOTS:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void uploadToS3Done();
};

#endif // MAINWINDOW_H

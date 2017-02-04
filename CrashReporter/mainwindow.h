#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QtDebug>
#include <QProcess>
#include "ui_mainwindow.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString restartApplicationPath;

private:
    Ui::MainWindow *ui;
    void startElMaven();
private Q_SLOTS:
    void on_restart_clicked();
};

#endif // MAINWINDOW_H

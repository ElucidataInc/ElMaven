#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->restartApplicationPath = "";
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::startElMaven()
 {
     QProcess * myProcess = new QProcess();
     myProcess->start(this->restartApplicationPath);
     QCoreApplication::quit();
 }

void MainWindow::on_pushButton_clicked()
{
    this->startElMaven();
}

void MainWindow::on_pushButton_3_clicked()
{
    QCoreApplication::quit();
}

void MainWindow::on_pushButton_2_clicked()
{
    QCoreApplication::quit();
}

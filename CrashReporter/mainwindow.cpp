#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->restartApplicationPath = "";
    connect(restart, SIGNAL(clicked()), this, SLOT(on_restart_clicked()));
    connect(send_report, SIGNAL(clicked()), this, SLOT(on_send_report_clicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(on_cancel_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_restart_clicked()
{
    this->startElMaven();
}

void MainWindow::startElMaven()
{
    QProcess * myProcess = new QProcess();
    myProcess->start(this->restartApplicationPath);
    QCoreApplication::quit();
}

void MainWindow::on_send_report_clicked()
{
    QCoreApplication::quit();
}

void MainWindow::on_cancel_clicked()
{
    QCoreApplication::quit();
}

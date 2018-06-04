#include "mainwindow.h"
#include <iostream>

#include "file_uploader.h"


#include <QStandardPaths>



MainWindow::MainWindow(QWidget *parent, FileUploader* fUploader) :
    QMainWindow(parent),
    uploader(fUploader),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    this->restartApplicationPath = "";

    connect(uploader, &FileUploader::uploadDone, this, &MainWindow::uploadFinished);

}




MainWindow::~MainWindow()
{

    delete ui;
}



void MainWindow::uploadFinished()
{
    qDebug() << "uploading finished";
    this->close();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closing ";
    QCoreApplication::quit();
}

void MainWindow::readOutput()
{
    qDebug() << "reading output " << _process->readAllStandardOutput();
}


void MainWindow::processError(QProcess::ProcessError perr)
{
    qDebug() << perr << " " << _process->errorString();
    QCoreApplication::quit();
}

void MainWindow::readError()
{
    qDebug() << "reading output " << _process->readAllStandardError();
}

void MainWindow::started()
{
    std::cerr << "process started" <<std::endl;
//TODO: update travis to new version of qt. Error occurred does not work with 5.2.1
}

void MainWindow::finished(int exitCode)
{
    // check the exit status and exit code.
    // if the node program failed to upload the logs, inform the user about it
    QCoreApplication::quit();

}


void MainWindow::uploadLogs()
{

    QStringList args;
    args.append(_script);

    _process->setArguments(args);

    if(_process->state() == QProcess::Running)
        _process->kill();

    _process->start();
}

void MainWindow::startElMaven()
{
    if (this->windowState == 1) {
        QProcess * myProcess = new QProcess();
        myProcess->start(this->restartApplicationPath);
    }

}


void MainWindow::onStart()
{

    this->show();
}


void MainWindow::on_cancel_clicked()
{
    this->close();
}

void MainWindow::on_reportRestart_clicked()
{

    uploader->uploadMinidump();

}

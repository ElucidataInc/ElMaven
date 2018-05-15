#include "mainwindow.h"
#include <iostream>
#ifdef Q_OS_WIN
#include "file_uploader.h"
#endif

#include <QStandardPaths>


#ifdef Q_OS_WIN
MainWindow::MainWindow(QWidget *parent, FileUploader* fUploader) :
    QMainWindow(parent),
    uploader(fUploader),
    ui(new Ui::MainWindow)
#endif

#if defined(Q_OS_UNIX)
MainWindow::MainWindow(QWidget *parent, const QString& path) :
    QMainWindow(parent),
    logsPath(path),
    ui(new Ui::MainWindow)
#endif
{
    ui->setupUi(this);
    // ui->label_4->setText( "<b>El-MAVEN</b> has encountered a problem and needs to close. We are sorry for the inconvenience.\n\n" \
    // "We have created an error0 report that you can send to us.\n\n<b>This report does not contain your input files or any other personally identifiable information</b>");
    this->restartApplicationPath = "";


     #if defined(Q_OS_LINUX)
        _script = qApp->applicationDirPath() + QDir::separator() + "report_issue.js";
    #endif

    #ifdef Q_OS_MAC
        _script = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." \
                + QDir::separator() + ".." + QDir::separator() + "report_issue.js";
    #endif


    #if defined(Q_OS_UNIX)
        QString nodePath = QStandardPaths::findExecutable("node");
        #ifdef Q_OS_LINUX

        if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
            nodePath = qApp->applicationDirPath() + QDir::separator() + "node";
        #endif

        #ifdef Q_OS_MAC
        QString binDir = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator();
        if(!QStandardPaths::findExecutable("node", QStringList() << binDir + "node_bin" + QDir::separator() ).isEmpty())
            nodePath = binDir + "node_bin" + QDir::separator() + "node";
        #endif
        //     set up the process
            _process  = new QProcess(this);
            _process->setProcessChannelMode(QProcess::SeparateChannels);
            _process->setProgram(nodePath);

            connect(_process, &QProcess::readyReadStandardOutput, this, &MainWindow::readOutput);
            connect(_process, &QProcess::readyReadStandardError, this, &MainWindow::readError);
            connect(_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::finished);
            connect(_process, &QProcess::started, this, &MainWindow::started);
//TODO: update travis to new version of qt. Error occurred does not work with 5.2.1
//            connect(_process, &QProcess::errorOccurred, this, &MainWindow::processError);
    #endif

}




MainWindow::~MainWindow()
{

    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
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
#ifdef Q_OS_UNIX
    uploadLogs();
#endif

#ifdef Q_OS_WIN
    uploader->uploadMinidump();
    this->close();
#endif

}

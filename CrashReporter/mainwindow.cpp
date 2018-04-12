#include "mainwindow.h"
#include <QStandardPaths>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->restartApplicationPath = "";


    QString basePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
                   qApp->organizationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + "logs" \
                   + QDir::separator();

    _logsPath << ( basePath + "elMavLogs") << (basePath + "elMavLogs.1") << (basePath + "elMavLogs.2");
    // won't work on mac;
     #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
        _script = qApp->applicationDirPath() + QDir::separator() + "report_issue.js";
    #endif

    #ifdef Q_OS_MAC
        _script = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." \
                + QDir::separator() + ".." + QDir::separator() + "report_issue.js";
    #endif


    QString nodePath = QStandardPaths::findExecutable("node");

    #ifdef Q_OS_WIN
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
        nodePath = qApp->applicationDirPath() + QDir::separator() + "node.exe";
    #endif

    #ifdef Q_OS_LINUX
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
          nodePath = qApp->applicationDirPath() + QDir::separator() + "node";
    #endif

    #ifdef Q_OS_MAC
      QString binDir = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator();
      if(!QStandardPaths::findExecutable("node", QStringList() << binDir + "node_bin" + QDir::separator() ).isEmpty())
        nodePath = binDir + "node_bin" + QDir::separator() + "node";
    #endif



    // set up the process
    _process  = new QProcess(this);
    _process->setProcessChannelMode(QProcess::SeparateChannels);
    _process->setProgram(nodePath);



    connect(_process, &QProcess::readyReadStandardOutput, this, &MainWindow::readOutput);
    connect(_process, &QProcess::readyReadStandardError, this, &MainWindow::readError);
    connect(_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::finished);
    connect(_process, &QProcess::started, this, &MainWindow::started);
    //TODO: update travis to new version of qt. Error occurred does not work with 5.2.1
    // connect(_process, &QProcess::errorOccurred, this, &MainWindow::processError);
}




MainWindow::~MainWindow()
{
    if(_process->state() != QProcess::NotRunning ) {
        _process->waitForFinished();
    }


    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    uploadLogs();
}

void MainWindow::readOutput()
{
}


void MainWindow::processError(QProcess::ProcessError perr)
{
    QCoreApplication::quit();
}

void MainWindow::readError()
{
}

void MainWindow::started()
{
//TODO: update travis to new version of qt. Error occurred does not work with 5.2.1
}

void MainWindow::finished(int exitCode)
{
    // check the exit status and exit code.
    // if the node program failed to upload the logs, inform the user about it
    QCoreApplication::quit();

}

void MainWindow::processLogs()
{
    for(const QString& path: _logsPath) {
        QFile fptr(path);

        if(fptr.open(QIODevice::ReadOnly)) {
            std::string log = std::string(fptr.readAll().data());
            std::string::size_type n;

            while(log.find(' ') != std::string::npos) {
               n = log.find(' ');
               log.replace(n, 1, "__");
            }

            while(log.find('\n') != std::string::npos) {
               n = log.find('\n');
               log.replace(n, 1, "###");
            }

            _logs += QString(log.c_str());
            fptr.close();
        }
    }
}

void MainWindow::uploadLogs()
{
    processLogs();

    QStringList args;
    args.append(_script);
    args.append(_logs);

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

void MainWindow::uploadToS3Done() {
    ui->plainTextEdit->clear();
    ui->statusBar->showMessage("Report is sent. Our team will get back to you as soon as possible.");
    this->startElMaven();
}



void MainWindow::onStart()
{
    if (this->windowState != 1) {
        ui->restart->setVisible( false );
        ui->label->setText("Description of feature/ bug");
        this->setWindowTitle ("Bug reporting and Feature request");
        ui->groupBox->setTitle("");
        ui->label_4->setText( "<b>Make El Maven better by giving us your feedback.</b>");
        ui->label_5->setText( "If you have a feature wish list or if you identified a bug please get in touch with us. A brief description below would help us a lot. We would love to hear from you." );
        ui->reportRestart->setText("Submit Feedback");
    }
    this->show();
}

void MainWindow::on_restart_clicked()
{

    uploadLogs();
}

void MainWindow::on_cancel_clicked()
{
    uploadLogs();
}

void MainWindow::on_reportRestart_clicked()
{
    uploadLogs();
}

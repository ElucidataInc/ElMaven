#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>


LoginForm::LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog) :
    QDialog(),
    _aboutPolly(NULL),
    ui(new Ui::LoginForm)
    
{
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    
    ui->setupUi(this);
    ui->label_3->setOpenExternalLinks(true);

    setWindowTitle("Polly");
    // pushButton_about_polly->setFlat(true);
    connect(ui->pushButton_about_polly, SIGNAL(clicked(bool)), SLOT(showAboutPolly()));
}

LoginForm::~LoginForm()
{
    delete ui;
    if (_aboutPolly) delete (_aboutPolly);
}

WorkerThread::WorkerThread()
{
    _pollyintegration = new PollyIntegration();   
    
};

void WorkerThread::run()
{
    QString status;
    if (!_pollyintegration->activeInternet())
        status = "error";
    else
        status = _pollyintegration->authenticateLogin(username, password);
    
    emit resultReady(status);
}

WorkerThread::~WorkerThread()
{
    if (_pollyintegration) delete (_pollyintegration);
};

void LoginForm::on_pushButton_clicked()
{   
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("Authenticating..");
    QCoreApplication::processEvents();
    ui->pushButton->setEnabled(false);
    QCoreApplication::processEvents();

    WorkerThread *workerThread = new WorkerThread();
    connect(workerThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->username= ui->lineEdit_username->text();
    workerThread->password = ui->lineEdit_password->text();
    workerThread->start();
}

void LoginForm::handleResults(QString status)
{
    if (status == "ok") {
        qDebug() << "Logged in, moving on now....";
        ui->login_label->setText("Fetching data from Polly..");
        QCoreApplication::processEvents();
        _pollyelmaveninterfacedialog->startupDataLoad();
        hide();
        _pollyelmaveninterfacedialog->show();
        
    } else if (status == "error") {
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Please check your internet");
        ui->pushButton->setEnabled(true);
    } else {
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Incorrect credentials");
        ui->pushButton->setEnabled(true);
    }
}

void LoginForm::cancel()
{
    qDebug() << "closing the log in form now..";
    close();
}

void LoginForm::showAboutPolly()
{
    _aboutPolly =new AboutPolly();
    _aboutPolly->setModal(true);
    _aboutPolly->show();
}

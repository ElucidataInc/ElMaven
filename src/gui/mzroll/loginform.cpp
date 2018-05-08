#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>


LoginForm::LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog) :
    QDialog(),
    ui(new Ui::LoginForm)
    
{
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    
    ui->setupUi(this);
    ui->login_label->setText("<a href=\"https://polly.elucidata.io/#/signup\">Register on Polly</a>");
    ui->login_label->setTextFormat(Qt::RichText);
    ui->login_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->login_label->setOpenExternalLinks(true); 
}

LoginForm::~LoginForm()
{
    delete ui;
}

WorkerThread::WorkerThread()
{
    _pollyintegration = new PollyIntegration();   
    
};

void WorkerThread::run(){
    QString status_inside = _pollyintegration->authenticate_login(username,password);
    emit resultReady(QStringList()<<status_inside<<username<<password);
}

WorkerThread::~WorkerThread()
{
    if (_pollyintegration) delete (_pollyintegration);
};

void LoginForm::on_pushButton_clicked()
{   
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("authenticating to polly..");
    QCoreApplication::processEvents();
    ui->pushButton->setEnabled(false);
    QCoreApplication::processEvents();

    WorkerThread *workerThread = new WorkerThread();
    connect(workerThread, SIGNAL(resultReady(QStringList)), this, SLOT(handleResults(QStringList)));
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->username= ui->lineEdit_username->text();
    workerThread->password = ui->lineEdit_password->text();
    workerThread->start();
}


void LoginForm::handleResults(QStringList results){
    QString status_inside=results.at(0);
    QString username=results.at(1);
    QString password=results.at(2);
    if (status_inside=="ok"){
        qDebug()<<"Logged in, moving on now....";
        ui->login_label->setText("getting data from polly..");
        QCoreApplication::processEvents();
        _pollyelmaveninterfacedialog->credentials = QStringList()<< username << password;
        _pollyelmaveninterfacedialog->startup_data_load();
        hide();
        
    }
    else if(status_inside=="error"){
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Please check internet connection");
        ui->pushButton->setEnabled(true);
    }
    else {
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Incorrect credentials");
        ui->pushButton->setEnabled(true);
    }
}

void LoginForm::cancel(){
    qDebug()<<"closing the log in form now..";
    close();
}

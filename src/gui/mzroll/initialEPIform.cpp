#include "initialEPIform.h"
#include "ui_initialEPIform.h"
#include <QMessageBox>


InitialEPIForm::InitialEPIForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog) :
    QDialog(),
    ui(new Ui::InitialEPIForm)
    
{
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    
    ui->setupUi(this);
    ui->initial_login_label->setStyleSheet("QLabel {color : green; }");
    ui->initial_login_label->setText("Welcome back "+_pollyelmaveninterfacedialog->credentials.at(0));
    
}

InitialEPIForm::~InitialEPIForm()
{
    delete ui;
}

initialWorkerThread::initialWorkerThread()
{
    _pollyintegration = new PollyIntegration();   
    
};

void initialWorkerThread::run(){
    QString status_inside = _pollyintegration->authenticate_login(username,password);
    emit resultReady(QStringList()<<status_inside<<username<<password);
}

initialWorkerThread::~initialWorkerThread()
{
    if (_pollyintegration) delete (_pollyintegration);
};

void InitialEPIForm::on_pushButton_inital_form_clicked()
{   
    ui->initial_form_status_label->setStyleSheet("QLabel {color : green; }");
    ui->initial_form_status_label->setText("Connecting to polly..Please wait");
    QCoreApplication::processEvents();
    ui->pushButton_inital_form->setEnabled(false);
    QCoreApplication::processEvents();

    initialWorkerThread *workerThread = new initialWorkerThread();
    connect(workerThread, SIGNAL(resultReady(QStringList)), this, SLOT(handleResults(QStringList)));
    connect(workerThread, &initialWorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->username= _pollyelmaveninterfacedialog->credentials.at(0);
    workerThread->password = _pollyelmaveninterfacedialog->credentials.at(1);
    workerThread->start();
}


void InitialEPIForm::handleResults(QStringList results){
    QString status_inside=results.at(0);
    QString username=results.at(1);
    QString password=results.at(2);
    if (status_inside=="ok"){
        qDebug()<<"Logged in, moving on now....";
        ui->initial_form_status_label->setText("getting data from polly..");
        QCoreApplication::processEvents();
        _pollyelmaveninterfacedialog->credentials = QStringList()<< username << password;
        _pollyelmaveninterfacedialog->startup_data_load();
        hide();
        
    }
    else if(status_inside=="error"){
        ui->initial_form_status_label->setStyleSheet("QLabel {color : red; }");
        ui->initial_form_status_label->setText("Please check internet connection");
        ui->pushButton_inital_form->setEnabled(true);
    }
    else {
        ui->initial_form_status_label->setStyleSheet("QLabel {color : red; }");
        ui->initial_form_status_label->setText("Incorrect credentials");
        ui->pushButton_inital_form->setEnabled(true);
    }
}

void InitialEPIForm::cancel(){
    qDebug()<<"closing the log in form now..";
    close();
}

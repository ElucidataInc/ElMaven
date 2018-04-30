#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>


LoginForm::LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog) :
    QDialog(),
    ui(new Ui::LoginForm)
    
{
    _pollyintegration = new PollyIntegration();
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    ui->setupUi(this);
    ui->login_label->setText("<a href=\"https://polly.elucidata.io/#/signup\">Register on Polly</a>");
    ui->login_label->setTextFormat(Qt::RichText);
    ui->login_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->login_label->setOpenExternalLinks(true); 
}

LoginForm::~LoginForm()
{
    qDebug()<<"inside ~LoginForm now....";
    delete ui;
}

void LoginForm::on_pushButton_clicked()
{
    QString status_inside;
    username = ui->lineEdit_username->text();
    password = ui->lineEdit_password->text();
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("Logging into polly..");
    status_inside = _pollyintegration->authenticate_login(username,password);
    if (status_inside=="ok"){
        hide();
        qDebug()<<"Logged in, moving on now....";
        _pollyelmaveninterfacedialog->credentials = QStringList()<< username << password;
        _pollyelmaveninterfacedialog->startup_data_load();
    }
    else if(status_inside=="error"){
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Please check internet connection");
    }
    else {
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Incorrect credentials");
    }
}

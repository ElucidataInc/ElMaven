#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>


LoginForm::LoginForm(MainWindow *parent,PollyIntegration* pollyintegration) :
    QDialog(parent),
    ui(new Ui::LoginForm)
{

    ui->setupUi(this);
    _mainwindow=parent;
    _pollyintegration = pollyintegration;
    
}

LoginForm::~LoginForm()
{
    qDebug()<<"inside ~LoginForm now....";
    delete ui;
}

void LoginForm::on_pushButton_clicked()
{
    int status_inside;
    username = ui->lineEdit_username->text();
    password = ui->lineEdit_password->text();
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("Logging into polly..");
    status_inside = _pollyintegration->authenticate_login(username,password);
    if (status_inside==1){
        hide();
        _mainwindow->check_polly_login->setText("connected");
        _mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
        _pollyintegration->get_project_name();
    }
    else{
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Incorrect credentials");
    }
}

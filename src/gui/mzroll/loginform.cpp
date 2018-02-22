#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>


LoginForm::LoginForm(MainWindow *parent,PollyIntegration* pollyintegration) :
    QWidget(0),
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
    qDebug()<<"username and password -  "<<username <<"   "<<password;
    status_inside = _pollyintegration->authenticate_login(username,password);
    if (status_inside==1){
        // w->hide();
        QMessageBox::information(this,"Login","username and password are correct..logged in");
        qDebug() << "logged in, you can now send data to polly....\n\n";
        hide();
        qDebug() << "popup deleted....\n\n";
        _mainwindow->check_polly_login->setText("connected");
        _mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
        // status=1;
        qDebug() << "status changed....\n\n";
        _pollyintegration->get_project_name();
        qDebug() << "logged in, data sent to polly....\n\n";
        
        
    }
    else{
        QMessageBox::information(this,"Login","username and password are not correct");
    }
    qDebug()<<"exiting on_pushButton_clicked now.."<<status_inside;
}

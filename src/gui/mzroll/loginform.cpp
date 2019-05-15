#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>
#include "controller.h"


LoginForm::LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog) :
    QDialog(),
    _aboutPolly(NULL),
    ui(new Ui::LoginForm)
    
{
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    _pollyintegration = _pollyelmaveninterfacedialog->getMainWindow()->getController()->iPolly;
    
    ui->setupUi(this);
    setWindowTitle("Sign in to Polly™");
    ui->lineEdit_username->setFocus();

    auto analytics = _pollyelmaveninterfacedialog->getMainWindow()
                                                 ->getAnalytics();
    connect(ui->registerLabel,
            &QLabel::linkActivated,
            [=](const QString &link) {
                QDesktopServices::openUrl(QUrl(link));
                analytics->hitEvent("PollyDialog", "Register");
            });
    connect(ui->aboutLabel,
            &QLabel::linkActivated,
            [=] {
                showAboutPolly();
                analytics->hitEvent("PollyDialog", "AboutPolly");
            });
}

LoginForm::~LoginForm()
{
    delete ui;
    if (_aboutPolly) delete (_aboutPolly);
}

QString LoginForm::login(QString username, QString password)
{
    QString status;
    if (!_pollyintegration->activeInternet())
        status = "error";
    else
        status = _pollyintegration->authenticateLogin(username, password);
    
    return status;
}

void LoginForm::on_pushButton_clicked()
{   
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("Authenticating…");
    QCoreApplication::processEvents();
    ui->pushButton->setEnabled(false);
    QCoreApplication::processEvents();

    QString status = login(ui->lineEdit_username->text(), ui->lineEdit_password->text());
    handleResults(status);
}

void LoginForm::handleResults(QString status)
{
    if (status == "ok") {
        qDebug() << "Logged in, moving on now…";
        ui->login_label->setText("Fetching user data…");
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
    qDebug() << "closing the log in form now…";
    close();
}

void LoginForm::showAboutPolly()
{
    _aboutPolly =new AboutPolly();
    _aboutPolly->setModal(true);
    _aboutPolly->show();
}

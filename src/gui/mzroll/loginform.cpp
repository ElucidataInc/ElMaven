#include <QMessageBox>

#include "aboutpolly.h"
#include "common/analytics.h"
#include "controller.h"
#include "loginform.h"
#include "mainwindow.h"
#include "pollyelmaveninterface.h"
#include "pollyintegration.h"
#include "ui_loginform.h"


LoginForm::LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog, bool showPollyApp) :
    QDialog(),
    _aboutPolly(NULL),
    ui(new Ui::LoginForm)
    
{
    _pollyelmaveninterfacedialog = pollyelmaveninterfacedialog;
    _pollyintegration = _pollyelmaveninterfacedialog->getMainWindow()->getController()->iPolly;

    _showEPI = showPollyApp;
    
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
    connect(_pollyintegration,
            &PollyIntegration::receivedEPIError,
            [=] {
                ui->login_label->clear();
                ui->pushButton->setEnabled(true);
            });
    connect(this, SIGNAL(loginSuccessful()), _pollyelmaveninterfacedialog, SLOT(emitLoginReady()));
    connect(this, SIGNAL(widgetClosed()), _pollyelmaveninterfacedialog, SLOT(loginFormClosed()));
}

LoginForm::~LoginForm()
{
    delete ui;
    if (_aboutPolly) delete (_aboutPolly);
}

void LoginForm::closeEvent(QCloseEvent * event)
{
    emit widgetClosed();
}

void LoginForm::login(QString username, QString password)
{
    ErrorStatus response = _pollyintegration->authenticateLogin(username, password);
    if (response == ErrorStatus::Success) {
        qDebug() << "Logged in, moving on now…";
        hide();
        if (_showEPI) {
            _pollyelmaveninterfacedialog->initialSetup();
            _pollyelmaveninterfacedialog->show();
        } else {
            emit loginSuccessful();
        }
    } else if (response == ErrorStatus::Failure) {
        QCoreApplication::processEvents();
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Incorrect credentials");
        ui->pushButton->setEnabled(true);
    } else {
        QCoreApplication::processEvents();
        ui->login_label->setStyleSheet("Qlabel {color : green; }");
        ui->login_label->clear();
        ui->pushButton->setEnabled(true);
    }
}

void LoginForm::on_pushButton_clicked()
{   
    ui->login_label->setStyleSheet("QLabel {color : green; }");
    ui->login_label->setText("Authenticating…");
    QCoreApplication::processEvents();
    ui->pushButton->setEnabled(false);
    QCoreApplication::processEvents();

    if (checkInternetConnectivity()) {
        login(ui->lineEdit_username->text(), ui->lineEdit_password->text());
    }
}

bool LoginForm::checkInternetConnectivity()
{
    ErrorStatus response = _pollyintegration->activeInternet();
    if (response == ErrorStatus::Success) {
        return true;
    } else if (response == ErrorStatus::Failure) {
        ui->login_label->setStyleSheet("QLabel {color : red; }");
        ui->login_label->setText("Please check your internet");
        ui->pushButton->setEnabled(true);
        return false;
    }

    return false;
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

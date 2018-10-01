#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "pollyintegration.h"
#include "pollyelmaveninterface.h"
#include "aboutpolly.h"

class PollyIntegration;
class AboutPolly;
class PollyElmavenInterfaceDialog;
namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog);
    ~LoginForm();
    PollyIntegration* _pollyintegration;
    PollyElmavenInterfaceDialog* _pollyelmaveninterfacedialog;
    /**
     * @brief pointer to Pollyinfo class..
    */
    AboutPolly* _aboutPolly;
    void cancel();
    

public slots:
    void handleResults(QString status);

private slots:
    /**
     * @brief This function tries to log in the user based on credentials provided by the user
     * @details this function performs the following taks in the given order -
     * 1. first of all, check the values of lineEdit_username and lineEdit_password
     * 2. call authenticateLogin function from PollyCLI library to log in the user
     * 3. if successfull login, hide the form and save these credentials to a file for persistent login in future..
     * 4. If failed, display incorrect credentials..
     */
    void on_pushButton_clicked();
    void showAboutPolly();

private:
    Ui::LoginForm *ui;
};

class WorkerThread : public QThread
{
    Q_OBJECT
    public:
        WorkerThread();
        ~WorkerThread();
        PollyIntegration* _pollyintegration;
        QString username;
        QString password;
        void run();
    signals:
        void resultReady(QString status);
};

#endif // LOGINFORM_H

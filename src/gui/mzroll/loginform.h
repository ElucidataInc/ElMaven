#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

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
    LoginForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog, bool showPollyApp);
    ~LoginForm();
    PollyIntegration* _pollyintegration;
    PollyElmavenInterfaceDialog* _pollyelmaveninterfacedialog;
    /**
     * @brief pointer to Pollyinfo class..
    */
    AboutPolly* _aboutPolly;
    void cancel();
    void closeEvent(QCloseEvent* event);

Q_SIGNALS:
    void loginSuccessful();
    void widgetClosed();

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
    void login(QString username, QString password);
    void showAboutPolly();
    bool checkInternetConnectivity();

private:
    Ui::LoginForm *ui;
    bool _showPollyApp;
};

#endif // LOGINFORM_H

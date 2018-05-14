#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "pollyintegration.h"
#include "pollyelmaveninterface.h"

class PollyIntegration;
class PollyElmavenInterfaceDialog;
namespace Ui {
class InitialEPIForm;
}

class InitialEPIForm : public QDialog
{
    Q_OBJECT

public:
    InitialEPIForm(PollyElmavenInterfaceDialog* pollyelmaveninterfacedialog);
    ~InitialEPIForm();
    PollyIntegration* _pollyintegration;
    PollyElmavenInterfaceDialog* _pollyelmaveninterfacedialog;
    
    void cancel();

public slots:
    void handleResults(QStringList results);

private slots:
    /**
     * @brief This function tries to log in the user based on credentials provided by the user
     * @details this function performs the following taks in the given order -
     * 1. first of all, check the values of lineEdit_username and lineEdit_password
     * 2. call authenticate_login function from PollyCLI library to log in the user
     * 3. if successfull login, hide the form and save these credentials to a file for persistent login in future..
     * 4. If failed, display incorrect credentials..
     */
    void on_pushButton_clicked();

private:
    Ui::InitialEPIForm *ui;
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
        void resultReady(QStringList result);
};

#endif // LOGINFORM_H

#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "pollyintegration.h"
#include "pollyelmaveninterface.h"

class PollyIntegration;
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
    QString username;
    QString password;
private slots:
    void on_pushButton_clicked();

private:
    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H

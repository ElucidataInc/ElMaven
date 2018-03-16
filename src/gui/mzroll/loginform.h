#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "pollyintegration.h"

class PollyIntegration;
namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    LoginForm(PollyIntegration* pollyintegration);
    ~LoginForm();
    PollyIntegration* _pollyintegration;
    QString username;
    QString password;
private slots:
    void on_pushButton_clicked();

private:
    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H

#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "mainwindow.h"
#include "pollyintegration.h"

class PollyIntegration;
namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(MainWindow *parent=nullptr);
    LoginForm(MainWindow *parent,PollyIntegration* pollyintegration);
    ~LoginForm();
    PollyIntegration* _pollyintegration;
    QString username;
    QString password;
    MainWindow* _mainwindow;
private slots:
    void on_pushButton_clicked();

private:
    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H

#ifndef AWSBUCKETCREDENTIALSDIALOG_H
#define AWSBUCKETCREDENTIALSDIALOG_H

#include "stable.h"
#include "ui_awsbucketcredentialsdialog.h"

class MainWindow;

namespace Ui {
class AwsBucketCredentialsDialog;
}

class AwsBucketCredentialsDialog : public QDialog, public Ui_AwsBucketCredentialsDialog
{
    Q_OBJECT

public:
    explicit AwsBucketCredentialsDialog(QWidget *parent = 0);
    ~AwsBucketCredentialsDialog();
    void setSettings(QSettings* settings) { this->settings = settings; }
    void setMainWindow(MainWindow* w) { this->mainwindow = w; }

public Q_SLOTS:
    void accept();

private:
    Ui::AwsBucketCredentialsDialog *ui;
    QSettings *settings;
    MainWindow *mainwindow;
};

#endif // AWSBUCKETCREDENTIALSDIALOG_H


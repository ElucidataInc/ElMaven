#ifndef POLLYWAITDIALOG_H
#define POLLYWAITDIALOG_H

#include <QDialog>
#include "ui_pollywaitdialog.h"

namespace Ui {
class PollyWaitDialog;
}

class PollyWaitDialog : public QDialog, public Ui_PollyWaitDialog
{
    Q_OBJECT

public:
    explicit PollyWaitDialog(QWidget *parent = 0);
    ~PollyWaitDialog();
    QMovie* movie;

    void keyPressEvent(QKeyEvent* e);
    void closeEvent(QCloseEvent* e);

private:
    Ui::PollyWaitDialog *ui;
};

#endif // POLLYWAITDIALOG_H

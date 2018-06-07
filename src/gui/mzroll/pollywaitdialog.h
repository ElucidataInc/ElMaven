#ifndef POLLYWAITDIALOG_H
#define POLLYWAITDIALOG_H

#include <QDialog>

namespace Ui {
class PollyWaitDialog;
}

class PollyWaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PollyWaitDialog(QWidget *parent = 0);
    ~PollyWaitDialog();
    QLabel* statusLabel;

private:
    Ui::PollyWaitDialog *ui;
};

#endif // POLLYWAITDIALOG_H

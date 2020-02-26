#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

class MainWindow;

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(MainWindow *parent = nullptr);
    ~InfoDialog();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::InfoDialog *ui;
    MainWindow* _mw;

    void _updateForm(bool buttonClicked = false);
};

#endif // INFODIALOG_H

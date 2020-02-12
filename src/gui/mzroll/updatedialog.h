#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

class MainWindow;

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(MainWindow* parent = nullptr);
    ~UpdateDialog();
    bool updateAllowed() const { return _updateAllowed; }

private:
    Ui::UpdateDialog *ui;
    MainWindow* _mw;
    bool _updateAllowed;

private slots:
    void _update();
    void _finish();
};

#endif // UPDATEDIALOG_H

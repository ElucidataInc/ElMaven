#ifndef ISOTOPEDIALOG_H
#define ISOTOPEDIALOG_H

#include <QDialog>

namespace Ui {
class IsotopeDialog;
}

class IsotopeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IsotopeDialog(QWidget *parent = 0);
    ~IsotopeDialog();

private:
    Ui::IsotopeDialog *ui;
};

#endif // ISOTOPEDIALOG_H

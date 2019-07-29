#ifndef ISOTOPEDIALOG_H
#define ISOTOPEDIALOG_H

#include "stable.h"
#include "ui_isotopedialog.h"

class MainWindow;

class IsotopeDialog : public QDialog, public Ui_IsotopeDialog
{
    Q_OBJECT

    public:
        explicit IsotopeDialog(MainWindow* parent = 0);
        ~IsotopeDialog();
    
    public Q_SLOTS:
        void show();

    private:
        MainWindow* _mw;
};

#endif // ISOTOPEDIALOG_H

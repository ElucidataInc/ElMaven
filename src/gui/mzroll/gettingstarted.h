#ifndef GETTINGSTARTED_H
#define GETTINGSTARTED_H

#include <QDialog>
#include "ui_gettingstarted.h"
#include <QString>

namespace Ui {
class GettingStarted;
}

class GettingStarted : public QDialog
{
    Q_OBJECT

public:
    explicit GettingStarted(QWidget *parent = 0);
    ~GettingStarted();
    bool showDialog();

private:
    Ui::GettingStarted *ui;
    
};

#endif // GETTINGSTARTED_H

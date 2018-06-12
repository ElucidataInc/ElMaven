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

public Q_SLOTS:
    void setFlag();

private:
    Ui::GettingStarted *ui;
    QString settingsPath1;
    QString settingsPath;
    
};

#endif // GETTINGSTARTED_H

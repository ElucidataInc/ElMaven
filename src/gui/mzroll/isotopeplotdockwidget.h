#ifndef ISOTOPEPLOTDOCKWIDGET_H
#define ISOTOPEPLOTDOCKWIDGET_H

#include "mainwindow.h"
#include <QDockWidget>

namespace Ui {
class IsotopePlotDockWidget;
}

class IsotopePlotDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit IsotopePlotDockWidget(MainWindow *mw = 0);
    ~IsotopePlotDockWidget();

private:
    Ui::IsotopePlotDockWidget *ui;
    MainWindow *_mw;
};

#endif // ISOTOPEPLOTDOCKWIDGET_H

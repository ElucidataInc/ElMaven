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

private Q_SLOTS:
    void updateC13Flag(bool setState);

private:
    Ui::IsotopePlotDockWidget *ui;
    MainWindow *_mw;
    void setToolBar();
    void recompute();
};

#endif // ISOTOPEPLOTDOCKWIDGET_H

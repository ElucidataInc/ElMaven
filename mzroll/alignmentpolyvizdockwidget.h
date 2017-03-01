#ifndef ALIGNMENTPOLYVIZDOCKWIDGET_H
#define ALIGNMENTPOLYVIZDOCKWIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include "ui_alignmentpolyvizdockwidget.h"

class MainWindow;

using namespace std;

namespace Ui {
class AlignmentPolyVizDockWidget;
}

class AlignmentPolyVizDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit AlignmentPolyVizDockWidget(QWidget *parent = 0);
    ~AlignmentPolyVizDockWidget();

private:
    Ui::AlignmentPolyVizDockWidget *ui;
    MainWindow* _mw;
};

#endif // ALIGNMENTPOLYVIZDOCKWIDGET_H
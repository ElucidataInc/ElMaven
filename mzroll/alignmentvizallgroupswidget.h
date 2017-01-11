#ifndef ALIGNMENTVIZALLGROUPSWIDGET_H
#define ALIGNMENTVIZALLGROUPSWIDGET_H

#include "stable.h"
#include "qcustomplot.h"
#include "mainwindow.h"
#include "PeakGroup.h"
#include "mzSample.h"
#include "Peak.h"

class MainWindow;
class PeakGroup;
class mzSample;
class Peak;

class AlignmentVizAllGroupsWidget : public QObject
{
    Q_OBJECT

public:
    AlignmentVizAllGroupsWidget(MainWindow* mw);

public Q_SLOTS:
    void plotGraph(QList<PeakGroup> allgroups);
    void setXAxis();
    void setYAxis();
    void setLegend();
    void selectionChanged();
    void makeAllGraphsVisible();

private:
    MainWindow* _mw;
};

#endif
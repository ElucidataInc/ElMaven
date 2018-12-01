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
class QDockWidget;

class AlignmentVizAllGroupsWidget : public QObject
{
    Q_OBJECT

public:
    AlignmentVizAllGroupsWidget(MainWindow* mw, QDockWidget* dockWidget);
    map<pair<float, float>, PeakGroup> pairPeakGroup;
    map<mzSample*, QVector<double> > retentionTime;
    map<mzSample*, QVector<double> > retentionTimeDeviation;
    QList<PeakGroup> saveGroups;

public Q_SLOTS:
    void replotGraph();
    void plotGraph(QList<PeakGroup> allgroups);
    void setXAxis();
    void setYAxis();
    void setLegend();
    void makeAllGraphsVisible();
    vector<float> generate_bezier(float x1, float y1, float x2, float y2, float x3, float y3);
    float quadratic_bezier_point(float t, float point_0, float point_1, float point_2);
    float mid_point(float a, float b);
    vector<float> populate_bezier(vector<float> xy_points);
    void displayGroup();

private:
    MainWindow* _mw;
    QDockWidget* _dockWidget;
};

#endif

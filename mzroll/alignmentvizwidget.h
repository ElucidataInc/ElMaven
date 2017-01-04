#ifndef ALIGNMENTVIZWIDGET_H
#define ALIGNMENTVIZWIDGET_H

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

class AlignmentVizWidget : public QObject
{
    Q_OBJECT
public:
    AlignmentVizWidget(MainWindow* mw);

public Q_SLOTS:
    void plotGraph(PeakGroup* group);
    vector<mzSample*> getSamplesFromGroup(PeakGroup* group);
    QVector<double> getRetentionTime(vector<mzSample*> samples, PeakGroup* group);
    void setXAxis(PeakGroup* group);
    QVector<double> setYAxis(vector<mzSample*> samples);

private:
    MainWindow* _mw;

};

#endif
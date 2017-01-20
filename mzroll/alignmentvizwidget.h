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

    void intialSetup();
    void setXAxis();
    void setYAxis();

    void refRtLine(PeakGroup*);
    double getRefRt(PeakGroup* group);

    PeakGroup* getNewGroup(PeakGroup*);
    bool checkGroupEquality(PeakGroup* grp1, PeakGroup* grp2);

    void plotIndividualGraph(PeakGroup* group, QColor color);

    vector<mzSample*> getSamplesFromGroup(PeakGroup* group);
    float getWidthOfBar(PeakGroup* group);
    double getRetentionTime(mzSample* sample, PeakGroup* group);

private:
    MainWindow* _mw;

};

#endif
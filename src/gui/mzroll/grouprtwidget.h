#ifndef GROUPRTWIDGET_H
#define GROUPRTWIDGET_H

#include "stable.h"
#include "qcustomplot.h"
#include "mainwindow.h"
#include "PeakGroup.h"
#include "mzSample.h"
#include "Peak.h"
#include <math.h>

class MainWindow;
class PeakGroup;
class mzSample;
class Peak;

class GroupRtWidget : public QObject
{
    Q_OBJECT
public:
    GroupRtWidget(MainWindow* mw);
    void updateGraph();
    QCPBars *bar;
    map<int, mzSample*> mapSample;
    map<int, pair<double, double> > mapXAxis;
    QCPItemText *textLabel;
    QCPItemText *sampleLabel; 
    QList<PeakGroup> currentGroups;

public Q_SLOTS:
    void plotGraph(PeakGroup*);

    void intialSetup();
    void setXAxis();
    void setYAxis();

    void refRtLine(PeakGroup);
    double getRefRt(PeakGroup);

    void drawMessageBox(PeakGroup, PeakGroup);
    float calculateRsquare(PeakGroup,PeakGroup);

    PeakGroup getNewGroup(PeakGroup);
    void setCurrentGroups(QList<PeakGroup> groups) {currentGroups = groups;}
    float checkGroupEquality(PeakGroup, PeakGroup);

    void plotIndividualGraph(PeakGroup, int alpha);

    vector<mzSample*> getSamplesFromGroup(PeakGroup);
    float getWidthOfBar(PeakGroup);
    double getRetentionTime(mzSample*, PeakGroup);

    void mouseQCPBar(QMouseEvent *event);    

private:
    MainWindow* _mw;
    PeakGroup* currentDisplayedGroup;

};

#endif

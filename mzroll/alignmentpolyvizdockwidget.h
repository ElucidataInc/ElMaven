#ifndef ALIGNMENTPOLYVIZDOCKWIDGET_H
#define ALIGNMENTPOLYVIZDOCKWIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include "ui_alignmentpolyvizdockwidget.h"

class MainWindow;
class mzSample;

using namespace std;

namespace Ui {
class AlignmentPolyVizDockWidget;
}

class AlignmentPolyVizDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit AlignmentPolyVizDockWidget(MainWindow *mw = 0);
    ~AlignmentPolyVizDockWidget();
    void setDegreeMap(map<mzSample*, int> sampleDegree) {
        degreeMap = sampleDegree;
    }
    void setCoefficientMap(map<mzSample*, vector<double> > sampleCoefficient) {
        coefficientMap = sampleCoefficient;
    }

    void intialSetup();
    void setXAxis();
    void setYAxis();

public Q_SLOTS:
    void plotGraph();
    void refresh();

private:
    Ui::AlignmentPolyVizDockWidget *ui;
    MainWindow* _mw;
    map<mzSample*, int> degreeMap;
    map<mzSample*, vector<double> > coefficientMap;

};

#endif // ALIGNMENTPOLYVIZDOCKWIDGET_H
#ifndef SAMPLERTWIDGET_H
#define SAMPLERTWIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include "ui_samplertwidget.h"

class MainWindow;
class mzSample;

using namespace std;

namespace Ui {
class SampleRtWidget;
}

class SampleRtWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SampleRtWidget(MainWindow *mw = 0);
    ~SampleRtWidget();
    void setDegreeMap(map<mzSample*, int> sampleDegree) {
        degreeMap = sampleDegree;
    }
    void setCoefficientMap(map<mzSample*, vector<double> > sampleCoefficient) {
        coefficientMap = sampleCoefficient;
    }

    void intialSetup();
    void setXAxis();
    void setYAxis();

    void prepareGraphDataLoessFit(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample);
    void prepareGraphDataPolyFit(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample);
    void prepareGraphDataObiWarp(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample);

public Q_SLOTS:
    void plotGraph();
    void plotIndividualGraph(mzSample* sample);
    void refresh();

private:
    Ui::SampleRtWidget *ui;
    MainWindow* _mw;
    map<mzSample*, int> degreeMap;
    map<mzSample*, vector<double> > coefficientMap;

};

#endif // SAMPLERTWIDGET_H

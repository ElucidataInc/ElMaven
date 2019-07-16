#include <qcustomplot.h>

#include "alignmentdialog.h"
#include "Compound.h"
#include "mainwindow.h"
#include "mzFit.h"
#include "mzSample.h"
#include "Peptide.hpp"
#include "projectdockwidget.h"
#include "samplertwidget.h"
#include "Scan.h"

SampleRtWidget::SampleRtWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::SampleRtWidget)
{
    this->_mw= mw;
    ui->setupUi(this);
    setObjectName("SampleRtWidget");
    setWindowTitle("Sample Deviation");

    setXAxis();
    setYAxis();
}

SampleRtWidget::~SampleRtWidget()
{
    delete ui;
}

void SampleRtWidget::plotGraph() {

    if(!isVisible()) return;

    intialSetup();

    // plot individual graphs here
    Q_FOREACH(mzSample* sample, _mw->getSamples()) {
        if(sample->isSelected) {
            plotIndividualGraph(sample);
        }
    }

    refresh();

}

void SampleRtWidget::intialSetup() {

    _mw->sampleRtVizPlot->clearPlottables();
    _mw->sampleRtVizPlot->clearGraphs();

}

void SampleRtWidget::setXAxis() {

    _mw->sampleRtVizPlot->xAxis->setTicks(true);
    _mw->sampleRtVizPlot->xAxis->setSubTicks(true);
    _mw->sampleRtVizPlot->xAxis->setVisible(true);
    _mw->sampleRtVizPlot->xAxis->setLabel("Retention Time");
}

void SampleRtWidget::setYAxis() {

    _mw->sampleRtVizPlot->yAxis->setTicks(true);
    _mw->sampleRtVizPlot->yAxis->setSubTicks(true);
    _mw->sampleRtVizPlot->yAxis->setVisible(true);
    _mw->sampleRtVizPlot->yAxis->setLabel("Retention Time Deviation");

}

void SampleRtWidget::prepareGraphDataPolyFit(QVector<double>&xAxis,
                                             QVector<double>&yAxis,
                                             mzSample* sample)
{
    vector<double> coefficients;
    double degree;

    if (degreeMap.empty()) return;
    if (coefficientMap.empty()) return;

    degree = degreeMap[sample];
    coefficients = coefficientMap[sample];
    double *coe = &coefficients[0];

    // just a sanity check to prevent SIGSEV
    if (!sample->scans.empty() && coe != NULL) {
        for (auto const scan : sample->scans) {
            xAxis.push_back(scan->rt);

            double y = 0;
            y = leasev(coe, degree, scan->rt);

            yAxis.push_back(y - scan->rt);
        }
    }
}

void SampleRtWidget::prepareGraphDataObiWarp(QVector<double>&xAxis,
                                             QVector<double>&yAxis,
                                             mzSample* sample)
{
    double rt, rtDiff;

    for (auto const scan : sample->scans) {
        rt = scan->originalRt;
        xAxis.push_back(rt);

        rtDiff = scan->originalRt - scan->rt;
        yAxis.push_back(rtDiff);
    }
}

void SampleRtWidget::plotIndividualGraph(mzSample* sample)
{

    QVector<double> xAxis;
    QVector<double> yAxis;

    int alignAlgo = _mw->alignmentDialog->alignAlgo->currentIndex();

    if(alignAlgo == 1)
        prepareGraphDataPolyFit(xAxis, yAxis, sample);

    if(alignAlgo == 0)
        prepareGraphDataObiWarp(xAxis, yAxis, sample);

    if(!xAxis.isEmpty() && !yAxis.isEmpty()){

        QColor color = _mw->projectDockWidget->storeSampleColors[sample];

        QPen pen;
        pen.setColor(color);

        _mw->sampleRtVizPlot->addGraph();
        _mw->sampleRtVizPlot->graph()->setPen(pen);
        _mw->sampleRtVizPlot->graph()->setLineStyle(QCPGraph::lsLine);

        _mw->sampleRtVizPlot->graph()->setData(xAxis, yAxis);

        _mw->sampleRtVizPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |  QCP::iSelectPlottables);
    }

}

void SampleRtWidget::refresh() {
    _mw->sampleRtVizPlot->rescaleAxes();
    _mw->sampleRtVizPlot->replot();
}

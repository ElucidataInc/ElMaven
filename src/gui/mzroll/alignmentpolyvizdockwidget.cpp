#include "alignmentpolyvizdockwidget.h"

AlignmentPolyVizDockWidget::AlignmentPolyVizDockWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::AlignmentPolyVizDockWidget)
{
    this->_mw= mw;
    ui->setupUi(this);
    setObjectName("AlignmentPolyVizDockWidget");
    setWindowTitle("AlignmentPolyVizWidget");

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer1);

    QToolButton *btnResetZoom = new QToolButton(toolBar);
    btnResetZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
    connect(btnResetZoom, SIGNAL(clicked()), SLOT(refresh()));
    toolBar->addWidget(btnResetZoom);

    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer2);

    QToolButton *btnHide = new QToolButton(toolBar);
    btnHide->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(btnHide, SIGNAL(clicked()),SLOT(hide()));
    toolBar->addWidget(btnHide);

    setTitleBarWidget(toolBar);
}

AlignmentPolyVizDockWidget::~AlignmentPolyVizDockWidget()
{
    delete ui;
}

void AlignmentPolyVizDockWidget::plotGraph() {

    intialSetup();

    // plot individual graphs here
    Q_FOREACH(mzSample* sample, _mw->getSamples()) {
        if(sample->isSelected) {
            plotIndividualGraph(sample);
        }
    }

    refresh();

}

void AlignmentPolyVizDockWidget::intialSetup() {

    _mw->alignmentPolyVizPlot->clearPlottables();
    _mw->alignmentPolyVizPlot->clearGraphs();
    setXAxis();
    setYAxis();

}

void AlignmentPolyVizDockWidget::setXAxis() {

    _mw->alignmentPolyVizPlot->xAxis->setTicks(true);
    _mw->alignmentPolyVizPlot->xAxis->setSubTicks(true);
    _mw->alignmentPolyVizPlot->xAxis->setVisible(true);
    _mw->alignmentPolyVizPlot->xAxis->setLabel("Retention Time");
}

void AlignmentPolyVizDockWidget::setYAxis() {

    _mw->alignmentPolyVizPlot->yAxis->setTicks(true);
    _mw->alignmentPolyVizPlot->yAxis->setSubTicks(true);
    _mw->alignmentPolyVizPlot->yAxis->setVisible(true);
    _mw->alignmentPolyVizPlot->yAxis->setLabel("Retention Time Deviation");

}

void AlignmentPolyVizDockWidget::prepareGraphDataPolyFit(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample)
{
        vector<double> coefficients;
        double degree;

        if (degreeMap.empty()) return;
        if (coefficientMap.empty()) return;

        degree = degreeMap[sample];
        coefficients = coefficientMap[sample];
        double *coe = &coefficients[0];

        /* just a sanity check to prevent SIGSEV
         */
        if(!sample->scans.empty() && coe != NULL){

            for(unsigned int i=0; i < sample->scans.size(); i++ ) {
                double rt = sample->scans[i]->rt;
                xAxis.push_back(rt);

                double y = 0;
                y = leasev(coe, degree, rt);

                yAxis.push_back(y - rt);
            }
        }
}

void AlignmentPolyVizDockWidget::prepareGraphDataLoessFit(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample)
{
    double rt, rtDiff;
    if(!sample->originalRetentionTimes.empty() && !sample->scans.empty()){

        for(unsigned int i=0; i < sample->scans.size(); i++ ) {

            rt = sample->originalRetentionTimes[i];
            xAxis.push_back(rt);

            rtDiff = sample->originalRetentionTimes[i] - sample->scans[i]->rt;
            yAxis.push_back(rtDiff);
        }
    }
}

void AlignmentPolyVizDockWidget::prepareGraphDataObiWarp(QVector<double>&xAxis, QVector<double>&yAxis, mzSample* sample)
{
    double rt, rtDiff;

    for(unsigned int i=0; i < sample->scans.size(); i++ ) {

        rt = sample->scans[i]->originalRt;
        xAxis.push_back(rt);

        rtDiff = sample->scans[i]->originalRt - sample->scans[i]->rt;
        yAxis.push_back(rtDiff);
    }
}

void AlignmentPolyVizDockWidget::plotIndividualGraph(mzSample* sample)
{

    QVector<double> xAxis;
    QVector<double> yAxis;

    int alignAlgo = _mw->alignmentDialog->alignAlgo->currentIndex();

    if(alignAlgo == 0)
        prepareGraphDataPolyFit(xAxis, yAxis, sample);

    if(alignAlgo == 1)
        prepareGraphDataLoessFit(xAxis, yAxis, sample);

    if(alignAlgo == 2)
        prepareGraphDataObiWarp(xAxis, yAxis, sample);

    if(!xAxis.isEmpty() && !yAxis.isEmpty()){

        QColor color = _mw->projectDockWidget->storeSampleColors[sample];

        QPen pen;
        pen.setColor(color);

        _mw->alignmentPolyVizPlot->addGraph();
        _mw->alignmentPolyVizPlot->graph()->setPen(pen);
        _mw->alignmentPolyVizPlot->graph()->setLineStyle(QCPGraph::lsLine);

        _mw->alignmentPolyVizPlot->graph()->setData(xAxis, yAxis);

        _mw->alignmentPolyVizPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |  QCP::iSelectPlottables);
    }

}

void AlignmentPolyVizDockWidget::refresh() {
    _mw->alignmentPolyVizPlot->rescaleAxes();
    _mw->alignmentPolyVizPlot->replot();
}

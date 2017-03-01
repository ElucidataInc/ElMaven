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

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

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
    for (unsigned int i = 0;i<_mw->samples.size();i++) {
        mzSample* sample = _mw->samples[i];
        plotIndividualGraph(sample);
    }

    refresh();

}

void AlignmentPolyVizDockWidget::intialSetup() {

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
    _mw->alignmentPolyVizPlot->yAxis->setLabel("Polynomial Model");

}

void AlignmentPolyVizDockWidget::plotIndividualGraph(mzSample* sample) {

    QVector<double> xAxis;
    QVector<double> yAxis;
    vector<double> coefficients;
    double degree;

    degree = degreeMap[sample];
    coefficients = coefficientMap[sample];

    for(unsigned int i=0; i < sample->scans.size(); i++ ) {
        double rt = sample->scans[i]->rt;
        xAxis.push_back(rt);

        double y = 0;
        for(int j=0; j <= degree; j++ ) {
            y += coefficients[j] * pow(rt, degree - j);
        }
        yAxis.push_back(y);
    }

    QColor color = _mw->projectDockWidget->storeSampleColors[sample];

    QPen pen;
    pen.setColor(color);

    _mw->alignmentPolyVizPlot->addGraph();
    _mw->alignmentPolyVizPlot->graph()->setPen(pen);
    _mw->alignmentPolyVizPlot->graph()->setLineStyle(QCPGraph::lsLine);

    _mw->alignmentPolyVizPlot->graph()->setData(xAxis, yAxis);

}

void AlignmentPolyVizDockWidget::refresh() {
    _mw->alignmentPolyVizPlot->rescaleAxes();
    _mw->alignmentPolyVizPlot->replot();
}
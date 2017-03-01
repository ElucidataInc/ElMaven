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

    _mw->alignmentPolyVizPlot->yAxis->setVisible(true);
    _mw->alignmentPolyVizPlot->yAxis->setLabel("Polynomial Model");

}

void AlignmentPolyVizDockWidget::refresh() {
    _mw->alignmentPolyVizPlot->replot();
}
#include "alignmentpolyvizdockwidget.h"

AlignmentPolyVizDockWidget::AlignmentPolyVizDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AlignmentPolyVizDockWidget)
{
    ui->setupUi(this);
}

AlignmentPolyVizDockWidget::~AlignmentPolyVizDockWidget()
{
    delete ui;
}
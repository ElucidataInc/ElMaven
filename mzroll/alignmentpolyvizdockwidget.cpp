#include "alignmentpolyvizdockwidget.h"

AlignmentPolyVizDockWidget::AlignmentPolyVizDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AlignmentPolyVizDockWidget)
{
    ui->setupUi(this);
    setObjectName("AlignmentPolyVizWidget");
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
#include "isotopeplotdockwidget.h"
#include "ui_isotopeplotdockwidget.h"

IsotopePlotDockWidget::IsotopePlotDockWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::IsotopePlotDockWidget)
{
    this->_mw = mw;
    ui->setupUi(this);
    setObjectName("IsotopePlotDockWidget");

    setToolBar();

}

IsotopePlotDockWidget::~IsotopePlotDockWidget()
{
    delete ui;
}

void IsotopePlotDockWidget::setToolBar()
{
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QLabel *title = new QLabel("Isotope Plot: ");
    toolBar->addWidget(title);

    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer1);

    QCheckBox *C13 = new QCheckBox("C13");
    C13->setChecked(true);
    toolBar->addWidget(C13);

    QWidget *spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer2);

    QCheckBox *N15 = new QCheckBox("N15");
    N15->setChecked(true);
    toolBar->addWidget(N15);

    QWidget *spacer3 = new QWidget();
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer3);

    QCheckBox *D2 = new QCheckBox("D2");
    D2->setChecked(true);
    toolBar->addWidget(D2);

    QWidget *spacer4 = new QWidget();
    spacer4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer4);

    QCheckBox *S34 = new QCheckBox("S34");
    S34->setChecked(true);
    toolBar->addWidget(S34);

    setTitleBarWidget(toolBar);
}
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
    N15->setChecked(false);
    toolBar->addWidget(N15);

    QWidget *spacer3 = new QWidget();
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer3);

    QCheckBox *D2 = new QCheckBox("D2");
    D2->setChecked(false);
    toolBar->addWidget(D2);

    QWidget *spacer4 = new QWidget();
    spacer4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer4);

    QCheckBox *S34 = new QCheckBox("S34");
    S34->setChecked(false);
    toolBar->addWidget(S34);

    setTitleBarWidget(toolBar);

    connect(C13, SIGNAL(toggled(bool)), this, SLOT(updateC13Flag(bool)));
    connect(N15, SIGNAL(toggled(bool)), this, SLOT(updateN15Flag(bool)));
    connect(D2, SIGNAL(toggled(bool)), this, SLOT(updateD2Flag(bool)));
    connect(S34, SIGNAL(toggled(bool)), this, SLOT(updateS34Flag(bool)));
}

void IsotopePlotDockWidget::updateC13Flag(bool setState)
{
    _mw->mavenParameters->C13Labeled_Barplot = setState;
    recompute();
}

void IsotopePlotDockWidget::updateN15Flag(bool setState)
{
    _mw->mavenParameters->N15Labeled_Barplot = setState;
    recompute();
}

void IsotopePlotDockWidget::updateD2Flag(bool setState)
{
    _mw->mavenParameters->D2Labeled_Barplot = setState;
    recompute();
}

void IsotopePlotDockWidget::updateS34Flag(bool setState)
{
    _mw->mavenParameters->S34Labeled_Barplot = setState;
    recompute();
}

void IsotopePlotDockWidget::recompute()
{
    if (_mw->getEicWidget()->isVisible()) {
        PeakGroup* group = _mw->getEicWidget()->getParameters()->getSelectedGroup();
        if (group)
        {
            group->childrenBarPlot.clear();
            _mw->isotopeWidget->updateIsotopicBarplot(group);
        }
    }
}
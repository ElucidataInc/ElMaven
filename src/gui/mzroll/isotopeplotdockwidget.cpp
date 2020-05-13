#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>

#include "eiclogic.h"
#include "eicwidget.h"
#include "isotopeplot.h"
#include "isotopeplotdockwidget.h"
#include "isotopeswidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "ui_isotopeplotdockwidget.h"

IsotopePlotDockWidget::IsotopePlotDockWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::IsotopePlotDockWidget)
{
    this->_mw = mw;
    ui->setupUi(this);
    setObjectName("IsotopePlotDockWidget");

    setToolBar();

    setWindowTitle("Isotope Plot: ");
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

    QLabel *title = new QLabel("Isotope Plot");
    title->setStyleSheet("QLabel { margin-left: 6px; }");
    toolBar->addWidget(title);

    toolBar->addSeparator();
    toolBar->addWidget(new QLabel("Labels: "));

    QWidget* spacer1 = new QWidget();
    toolBar->addWidget(spacer1);

    QCheckBox *C13 = new QCheckBox("C13");
    C13->setChecked(true);
    toolBar->addWidget(C13);

    QCheckBox *N15 = new QCheckBox("N15");
    N15->setChecked(false);
    toolBar->addWidget(N15);

    QCheckBox *D2 = new QCheckBox("D2");
    D2->setChecked(false);
    toolBar->addWidget(D2);

    QCheckBox *S34 = new QCheckBox("S34");
    S34->setChecked(false);
    toolBar->addWidget(S34);

    toolBar->addSeparator();
    QLabel *pool = new QLabel("Other: <");
    toolBar->addWidget(pool);

    QDoubleSpinBox *poolLabels = new QDoubleSpinBox();
    poolLabels->setRange(0.0, 10.0);
    poolLabels->setValue(1.0);
    poolLabels->setSingleStep(0.5);
    poolLabels->setToolTip("Show < x% as Other");
    poolLabels->setSuffix(" %");
    toolBar->addWidget(poolLabels);

    QWidget* spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    QToolButton *closeButton = new QToolButton(toolBar);
    closeButton->setIcon(this->style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    connect(closeButton,
            &QToolButton::clicked,
            this,
            &IsotopePlotDockWidget::hide);
    toolBar->addWidget(closeButton);

    setTitleBarWidget(toolBar);

    connect(C13, SIGNAL(toggled(bool)), this, SLOT(updateC13Flag(bool)));
    connect(N15, SIGNAL(toggled(bool)), this, SLOT(updateN15Flag(bool)));
    connect(D2, SIGNAL(toggled(bool)), this, SLOT(updateD2Flag(bool)));
    connect(S34, SIGNAL(toggled(bool)), this, SLOT(updateS34Flag(bool)));
    connect(poolLabels, SIGNAL(valueChanged(double)), this, SLOT(setPoolThreshold(double)));

    if (_mw->isotopePlot != nullptr) {
        connect(_mw->isotopePlot,
                &IsotopePlot::peakGroupSet,
                C13,
                &QCheckBox::setDisabled);
        connect(_mw->isotopePlot,
                &IsotopePlot::peakGroupSet,
                D2,
                &QCheckBox::setDisabled);
        connect(_mw->isotopePlot,
                &IsotopePlot::peakGroupSet,
                N15,
                &QCheckBox::setDisabled);
        connect(_mw->isotopePlot,
                &IsotopePlot::peakGroupSet,
                S34,
                &QCheckBox::setDisabled);
    }
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

void IsotopePlotDockWidget::setPoolThreshold(double poolThreshold)
{
    _mw->isotopePlot->setPoolThreshold(poolThreshold);
    recompute();
}

void IsotopePlotDockWidget::recompute()
{
    if (_mw->getEicWidget()->isVisible()) {
        PeakGroup* group = _mw->getEicWidget()->getParameters()->displayedGroup();
        if (group && group->tableName().empty()) {
            group->childrenBarPlot.clear();
            _mw->isotopeWidget->updateIsotopicBarplot(group);
        } else {
            _mw->isotopeWidget->updateIsotopicBarplot();
        }
    }
}

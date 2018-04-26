#include "isotopeplotdockwidget.h"
#include "ui_isotopeplotdockwidget.h"

IsotopePlotDockWidget::IsotopePlotDockWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::IsotopePlotDockWidget)
{
    this->_mw = mw;
    ui->setupUi(this);
    setObjectName("IsotopePlotDockWidget");
    setWindowTitle("Isotope Plot: ");
}

IsotopePlotDockWidget::~IsotopePlotDockWidget()
{
    delete ui;
}

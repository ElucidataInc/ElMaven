#include "analytics.h"
#include "isotopedialog.h"
#include "mainwindow.h"

IsotopeDialog::IsotopeDialog(MainWindow* parent) : QDialog(parent) {
    setupUi(this);
    setModal(false);
    setWindowTitle("Isotope Settings");

    _mw = parent;
}

IsotopeDialog::~IsotopeDialog()
{

}

void IsotopeDialog::show()
{
    if (_mw == NULL) return;

    _mw->getAnalytics()->hitScreenView("IsotopeDialog");
    QDialog::exec();
}

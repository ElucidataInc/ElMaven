#include "isotopedialog.h"
#include "ui_isotopedialog.h"

IsotopeDialog::IsotopeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IsotopeDialog)
{
    ui->setupUi(this);
}

IsotopeDialog::~IsotopeDialog()
{
    delete ui;
}

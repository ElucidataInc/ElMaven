#include "adductwidget.h"

AdductWidget::AdductWidget(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setModal(false);
    setWindowTitle("Adducts");
}

AdductWidget::~AdductWidget()
{

}

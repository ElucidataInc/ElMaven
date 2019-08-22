#include "adductwidget.h"
#include "database.h"

AdductWidget::AdductWidget(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setModal(false);
    setWindowTitle("Adducts");

    qRegisterMetaType<Adduct*>("Adduct*");
}

AdductWidget::~AdductWidget()
{

}

void AdductWidget::loadAdducts()
{
    adductList->clear();
    int counter = 0;
    for (auto adduct : DB.adductsDB) {
        QTreeWidgetItem* item = new QTreeWidgetItem(adductList);
        item->setCheckState(0, Qt::Unchecked);
        item->setData(0, Qt::UserRole, QVariant::fromValue(adduct));

        item->setText(0, QString(adduct->getName().c_str()));
        item->setText(1, QString::number(adduct->getNmol()));
        item->setText(2, QString::number(adduct->getCharge()));
        item->setText(3, QString::number(adduct->getMass()));
    }
}

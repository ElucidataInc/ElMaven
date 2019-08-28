#include "adductwidget.h"
#include "database.h"
#include "mainwindow.h"
#include "mavenparameters.h"

AdductWidget::AdductWidget(MainWindow* parent) :
    QDialog(parent)
{
    _mw = parent;
    setupUi(this);
    setModal(false);
    setWindowTitle("Adducts");

    qRegisterMetaType<Adduct*>("Adduct*");
}

void AdductWidget::loadAdducts()
{
    adductList->clear();
    for (auto adduct : DB.adductsDB) {
        QTreeWidgetItem* item = new QTreeWidgetItem(adductList);
        item->setCheckState(0, Qt::Unchecked);
        item->setData(0, Qt::UserRole, QVariant::fromValue(adduct));

        item->setText(0, QString(adduct->getName().c_str()));
        item->setText(1, QString::number(adduct->getNmol()));
        item->setText(2, QString::number(adduct->getCharge()));
        item->setText(3, QString::number(adduct->getMass()));
    }
    adductList->resizeColumnToContents(0);
}

vector<Adduct*> AdductWidget::getSelectedAdducts()
{
    vector<Adduct*> selectedAdducts;
    QTreeWidgetItemIterator it(adductList);
    while (*it) {
        if ((*it)->checkState(0) == Qt::Checked) {
            auto variant = (*it)->data(0, Qt::UserRole);
            selectedAdducts.push_back(variant.value<Adduct*>());
        }
        ++it;
    }
    return selectedAdducts;
}

void AdductWidget::hideEvent(QHideEvent* event)
{
    _mw->mavenParameters->setChosenAdductList(getSelectedAdducts());
    QDialog::hideEvent(event);
}

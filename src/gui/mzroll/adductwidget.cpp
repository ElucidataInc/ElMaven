#include "adductwidget.h"
#include "common/analytics.h"
#include "Compound.h"
#include "database.h"
#include "datastructures/adduct.h"
#include "globals.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzfileio.h"
#include "mzUtils.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "Scan.h"

AdductsDialogSettings::AdductsDialogSettings(AdductWidget* dialog) : id(dialog)
{
    settings.insert("searchAdducts", QVariant::fromValue(id->searchAdducts));
    settings.insert("filterAdductsAgainstParent", QVariant::fromValue(id->filterAdductsAgainstParent));
    settings.insert("adductSearchWindow", QVariant::fromValue(id->adductSearchWindow));
    settings.insert("adductPercentCorrelation", QVariant::fromValue(id->adductPercentCorrelation));
}

void AdductsDialogSettings::updateAdductsDialogSettings(string key,
                                                        string value)
{
    QString k(QString(key.c_str()));
    if (settings.find(k) != settings.end()
        && !value.empty()) {
        const QVariant& v = settings[k];
        // convert the val to proper type;
        if(QString(v.typeName()).contains("QDoubleSpinBox"))
            v.value<QDoubleSpinBox*>()->setValue(std::stod(value));

        if(QString(v.typeName()).contains("QGroupBox"))
            v.value<QGroupBox*>()->setChecked(std::stod(value));

        if(QString(v.typeName()).contains("QCheckBox"))
            v.value<QCheckBox*>()->setChecked(std::stod(value));

        if(QString(v.typeName()).contains("QSpinBox"))
            v.value<QSpinBox*>()->setValue(std::stod(value));

        emit id->settingsUpdated(k, v);
    }
}

AdductWidget::AdductWidget(MainWindow* parent) :
    QDialog(parent)
{
    _mw = parent;
    setupUi(this);
    setModal(false);
    setWindowTitle("Adduct settings");

    qRegisterMetaType<Adduct*>("Adduct*");
    _adductSettings = new AdductsDialogSettings(this);

    connect(resetToDefaults,
            &QPushButton::clicked,
            this,
            &AdductWidget::onReset);
    connect(this,
            &AdductWidget::settingsChanged,
            _adductSettings,
            &AdductsDialogSettings::updateAdductsDialogSettings);
    connect(this,
            &QDialog::rejected,
            this,
            &AdductWidget::triggerSettingsUpdate);

    connect(searchAdducts,
            &QCheckBox::toggled,
            [this](const bool checked)
            {
                QString state = checked? "On" : "Off";
                _mw->getAnalytics()->hitEvent("Peak Detection",
                                              "Adduct Detection Swtiched",
                                              state);
                _updateWindowState();
            });

    connect(adductList,
            &QTreeWidget::itemChanged,
            this,
            [this] (QTreeWidgetItem* item, int column) {
                QCoreApplication::processEvents();
                if (item == nullptr || column != 0)
                    return;
                auto checked = item->checkState(column);
                auto items = adductList->selectedItems();
                for (auto item : items) {
                    if (item != nullptr)
                        item->setCheckState(column, checked);
                }
            });
}

void AdductWidget::loadAdducts()
{
    adductList->clear();
    for (auto adduct : DB.adductsDB()) {
        auto * item = new NumericTreeWidgetItem(adductList, AdductType);
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

Adduct* AdductWidget::defaultAdduct()
{
    Adduct* adduct = nullptr;
    for (auto parentAdduct : _mw->mavenParameters->getDefaultAdductList()) {
        if (SIGN(parentAdduct->getCharge())
            == SIGN(_mw->mavenParameters->getCharge())) {
            adduct = parentAdduct;
        }
    }
    return adduct;
}

void AdductWidget::selectAdductsForCurrentPolarity()
{
    QTreeWidgetItemIterator it(adductList);
    while (*it) {
        auto variant = (*it)->data(0, Qt::UserRole);
        auto adduct = variant.value<Adduct*>();
        if (SIGN(adduct->getCharge()) == SIGN(_mw->getIonizationMode())
            && adduct->isParent()
            && !_mw->getVisibleSamples().empty()) {
            // the primary adduct for current polarity should not be allowed
            // to be unselected
            (*it)->setCheckState(0, Qt::Checked);
            (*it)->setDisabled(true);
        } else if (SIGN(adduct->getCharge()) != SIGN(_mw->getIonizationMode())
                   && adduct->isParent()
                   && !_mw->getVisibleSamples().empty()) {
            (*it)->setCheckState(0, Qt::Unchecked);
            (*it)->setDisabled(false);
        }

        ++it;
    }
    _mw->mavenParameters->setChosenAdductList(getSelectedAdducts());
}

void AdductWidget::triggerSettingsUpdate()
{
    emit updateSettings(_adductSettings);
}

void AdductWidget::onReset()
{
    emit resetSettings(_adductSettings->getSettings().keys());
}

void AdductWidget::show()
{
    if (_mw == nullptr) return;
    _mw->getAnalytics()->hitScreenView("AdductDialog");
    QDialog::exec();
}

void AdductWidget::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    selectAdductsForCurrentPolarity();
}

void AdductWidget::hideEvent(QHideEvent* event)
{
    _mw->mavenParameters->setChosenAdductList(getSelectedAdducts());
    QDialog::hideEvent(event);
}

void AdductWidget::_updateWindowState()
{
    if (searchAdducts->isChecked()) {
        filterAdductsAgainstParent->setEnabled(true);
        adductList->setEnabled(true);
    } else {
        filterAdductsAgainstParent->setChecked(false);
        filterAdductsAgainstParent->setEnabled(false);
        adductList->setEnabled(false);
    }
}

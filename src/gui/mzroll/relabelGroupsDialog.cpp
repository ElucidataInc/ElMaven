#include "relabelGroupsDialog.h"
#include "multiselectcombobox.h"

RelabelGroupsDialog::RelabelGroupsDialog(TableDockWidget* tabledockWidget):
    QDialog(tabledockWidget)
{
    setupUi(this);
    
    _tabledockWidget = tabledockWidget;
    //default values
    badGroupLimit = 0.2;
    maybeGoodGroupLimit = 0.7;
    
    _slider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, _tabledockWidget);
    gridLayout_2->addWidget(_slider);

    connect (_slider, SIGNAL(rangeChanged(int, int)), this, SLOT(updateCurationParameter(int, int)));
    connect (relabelButton, SIGNAL(clicked()), this, SLOT(handleRelabel()));
    QDialog::exec();
}

void RelabelGroupsDialog::handleRelabel()
{
    relabelButton->setEnabled(false);
    _tabledockWidget->relabelGroups(badGroupLimit, maybeGoodGroupLimit);
    _tabledockWidget->legend()->uncheckRelabel();
    QDialog::close();
}

void RelabelGroupsDialog::updateCurationParameter(int lowerRange, int upperRange)
{
    badGroupLimit = lowerRange/10.0;
    maybeGoodGroupLimit = upperRange/10.0;
}

void RelabelGroupsDialog::closeEvent(QCloseEvent* event)
{
    _tabledockWidget->legend()->uncheckRelabel();
    QDialog::close();   
}
#include <QVariant>

#include "relabelGroupsDialog.h"
#include "multiselectcombobox.h"
#include "mzUtils.h"

RelabelGroupsDialog::RelabelGroupsDialog(TableDockWidget* tabledockWidget):
    QDialog(tabledockWidget)
{
    setupUi(this);
    
    _tabledockWidget = tabledockWidget;
    //default values
    _badGroupLimit = 0.3;
    _maybeGoodGroupLimit = 0.6;
    
    _slider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, _tabledockWidget);
    gridLayout_2->addWidget(_slider);
    
    connect (_slider, SIGNAL(rangeChanged(int, int)), this, SLOT(updateCurationParameter(int, int)));
    connect (relabelButton, SIGNAL(clicked()), this, SLOT(handleRelabel()));
    
}

void RelabelGroupsDialog::showDialog()
{
    QDialog::exec();
}

void RelabelGroupsDialog::handleRelabel()
{
    relabelButton->setEnabled(false);
    _inRelabelingMode = true;
    _tabledockWidget->relabelGroups(_badGroupLimit, _maybeGoodGroupLimit);
    statusText->setText("Status");
    _tabledockWidget->legend()->uncheckRelabel();
    relabelButton->setEnabled(true);
    _inRelabelingMode = false;
    QDialog::close();
}

void RelabelGroupsDialog::setProgressBar(QString text, int progress, int totalSteps)
{
    statusText->setText(text);
    relabelProgressBar->setRange(0, totalSteps);
    relabelProgressBar->setValue(progress);
}

void RelabelGroupsDialog::updateCurationParameter(int lowerRange, int upperRange)
{
    _badGroupLimit = lowerRange/10.0;
    _maybeGoodGroupLimit = upperRange/10.0;

    QString noiseLabel= "Noise Range: 0.0 - ";
    noiseLabel += QString::fromStdString(mzUtils::float2string(_badGroupLimit, 1));
    noiseRange->setText(noiseLabel);

    QString signalLabel= "Signal Range: ";
    signalLabel += QString::fromStdString(mzUtils::float2string(_maybeGoodGroupLimit, 1));
    signalLabel += " - 1.0";
    signalRange->setText(signalLabel);
}

void RelabelGroupsDialog::closeEvent(QCloseEvent* event)
{
    if (_inRelabelingMode) {
        event->ignore();
        return;
    }
    _tabledockWidget->legend()->uncheckRelabel();
    QDialog::close();   
}
#include "common/analytics.h"
#include "isotopedialog.h"
#include "mainwindow.h"

IsotopeDialogSettings::IsotopeDialogSettings(IsotopeDialog* dialog):id(dialog)
{
    settings.insert("reportIsotopes", QVariant::fromValue(id->reportIsotopesOptions));
    settings.insert("D2LabelBPE", QVariant::fromValue(id->D2Labeled_BPE));
    settings.insert("C13LabelBPE", QVariant::fromValue(id->C13Labeled_BPE));
    settings.insert("N15LabelBPE", QVariant::fromValue(id->N15Labeled_BPE));
    settings.insert("S34LabelBPE", QVariant::fromValue(id->S34Labeled_BPE));
    
    settings.insert("minIsotopeParentCorrelation", QVariant::fromValue(id->minIsotopicCorrelation));
    settings.insert("maxIsotopeScanDiff", QVariant::fromValue(id->maxIsotopeScanDiff));
    settings.insert("abundanceThreshold", QVariant::fromValue(id->doubleSpinBoxAbThresh));
    settings.insert("maxNaturalAbundanceError", QVariant::fromValue(id->maxNaturalAbundanceErr));
    settings.insert("correctC13IsotopeAbundance", QVariant::fromValue(id->isotopeC13Correction));
}

void IsotopeDialogSettings::updateIsotopeDialogSettings(string key, string value)
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

IsotopeDialog::IsotopeDialog(MainWindow* parent) : QDialog(parent) {
    setupUi(this);
    setModal(false);
    setWindowTitle("Isotope Settings");

    _mw = parent;
    isotopeSettings = new IsotopeDialogSettings(this);

    connect(resetButton, &QPushButton::clicked, this, &IsotopeDialog::onReset);
    connect(this, &IsotopeDialog::settingsChanged,
            isotopeSettings, &IsotopeDialogSettings::updateIsotopeDialogSettings);
    connect(this, &QDialog::rejected, this, &IsotopeDialog::triggerSettingsUpdate);
    connect(reportIsotopesOptions,
            &QGroupBox::toggled,
            [this](const bool checked)
            {
                QString state = checked? "On" : "Off";
                this->_mw
                    ->getAnalytics()
                    ->hitEvent("Peak Detection",
                               "Isotope Detection Switched",
                               state);
                if (checked) {
                    if (!D2Labeled_BPE->isChecked()
                        && !C13Labeled_BPE->isChecked()
                        && !N15Labeled_BPE->isChecked()
                        && !S34Labeled_BPE->isChecked())
                        C13Labeled_BPE->setChecked(true);
                } else {
                    C13Labeled_BPE->setChecked(false);
                    D2Labeled_BPE->setChecked(false);
                    N15Labeled_BPE->setChecked(false);
                    S34Labeled_BPE->setChecked(false);
                }
            });

    auto checkIsotopeDetectionValid = [this](const bool checked) {
        if (!D2Labeled_BPE->isChecked()
            && !C13Labeled_BPE->isChecked()
            && !N15Labeled_BPE->isChecked()
            && !S34Labeled_BPE->isChecked()) {
            reportIsotopesOptions->setChecked(false);
        }
    };
    connect(D2Labeled_BPE, &QCheckBox::toggled, checkIsotopeDetectionValid);
    connect(C13Labeled_BPE, &QCheckBox::toggled, checkIsotopeDetectionValid);
    connect(N15Labeled_BPE, &QCheckBox::toggled, checkIsotopeDetectionValid);
    connect(S34Labeled_BPE, &QCheckBox::toggled, checkIsotopeDetectionValid);
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

void IsotopeDialog::triggerSettingsUpdate()
{
    // happens when users presses 'esc' key; 
    emit updateSettings(isotopeSettings);
}

void IsotopeDialog::onReset()
{
    emit resetSettings(isotopeSettings->getSettings().keys());
}

bool IsotopeDialog::isotopeDetectionEnabled()
{
    return reportIsotopesOptions->isChecked();
}

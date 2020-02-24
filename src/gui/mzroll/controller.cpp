#include <cstdlib>
#include <map>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QSpinBox>

#include "alignmentdialog.h"
#ifndef Q_OS_LINUX
#include "autoupdater.h"
#endif
#include <common/downloadmanager.h>
#include "controller.h"
#include "isotopedialog.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzfileio.h"
#include "peakdetectiondialog.h"
#include "pollyintegration.h"
#include "settingsform.h"

Controller::Controller()
{
    _dlManager = new DownloadManager;
    iPolly = new PollyIntegration(_dlManager);
    _mw = new MainWindow(this);
    updateUi();
    connect(_mw->isotopeDialog, &IsotopeDialog::updateSettings, this, &Controller::updateIsotopeDialogSettings);
    connect(_mw->isotopeDialog, &IsotopeDialog::settingsUpdated, this, &Controller::_updateSettingsForSave);
    connect(_mw->peakDetectionDialog, &PeakDetectionDialog::updateSettings, this, &Controller::updatePeakDetectionSettings);
    connect(_mw->peakDetectionDialog, &PeakDetectionDialog::settingsUpdated, this, &Controller::_updateSettingsForSave);
    connect(_mw->settingsForm, &SettingsForm::updateSettings, this, &Controller::updateOptionsDialogSettings);
    connect(_mw->settingsForm, &SettingsForm::settingsUpdated, this, &Controller::_updateSettingsForSave);
    connect(_mw->fileLoader, &mzFileIO::settingsLoaded, this, &Controller::_updateSettingsFromLoad);
    connect(_mw, &MainWindow::loadedSettings, this, &Controller::updateUi);
    connect(_mw->settingsForm, &SettingsForm::resetSettings, this, &Controller::resetMP);
    connect(_mw->peakDetectionDialog, &PeakDetectionDialog::resetSettings, this, &Controller::resetMP);
    connect(_mw->isotopeDialog, &IsotopeDialog::resetSettings, this, &Controller::resetMP);
    _mw->settingsForm->triggerSettingsUpdate();
    _mw->peakDetectionDialog->triggerSettingsUpdate();
    _mw->isotopeDialog->triggerSettingsUpdate();

#ifndef Q_OS_LINUX
    _updater = new AutoUpdater();
    connect(_updater,
            &AutoUpdater::updateAvailable,
            _mw,
            &MainWindow::promptUpdate);
    connect(_mw,
            &MainWindow::updateAllowed,
            _updater,
            &AutoUpdater::startMaintenanceTool);

    qDebug() << "Checking for updates…";
    _updater->start();
#endif
}

Controller::~Controller()
{
    delete _mw;
    delete iPolly;

}

void Controller::resetMP(QList<QString> keys)
{
    // we need to change the 'type' from QString to std::string since maven parameters
    // needs to independent of Qt
    QList<std::string> _keys;
    for(auto key: keys)
        _keys.append(key.toStdString());

    _mw->mavenParameters->reset(_keys.toStdList());
    updateUi();
}

template <typename T>
void Controller::_syncMpWithUi(T* dialogPtr)
{
    QMap<QString, QVariant>& settings = dialogPtr->getSettings();

    for(const QString& key: settings.keys()) {
        const QVariant& v = settings.value(key);

        /** NOTE: QVariant is storing different  types and hence we can't say which type it points to. Therefore we check the typename to know what type 
        it's containing and then convert to it. can't say if this can be considered as a HACK or not **/

        if(QString(v.typeName()).contains("QDoubleSpinBox")) {
            _updateMavenParameters(key, v.value<QDoubleSpinBox*>()->value());
        }

        if(QString(v.typeName()).contains("QGroupBox")) {
            _updateMavenParameters(key, v.value<QGroupBox*>()->isChecked());
        }

        if(QString(v.typeName()).contains("QCheckBox")) {
            _updateMavenParameters(key, v.value<QCheckBox*>()->isChecked());
        }

        if(QString(v.typeName()).contains("QSpinBox")) {
            _updateMavenParameters(key, v.value<QSpinBox*>()->value());
        }

        if(QString(v.typeName()).contains("QSlider")) {
            _updateMavenParameters(key, v.value<QSlider*>()->value());
        }

        if(QString(v.typeName()).contains("QComboBox")) {
            _updateMavenParameters(key, v.value<QComboBox*>()->currentIndex());
        }

        if(QString(v.typeName()).contains("QTabWidget")) {
            _updateMavenParameters(key, v.value<QTabWidget*>()->currentIndex());
        }

        if(QString(v.typeName()).contains("QLineEdit"))
            _updateMavenParameters(key, v.value<QLineEdit*>()->text());

        /*note: this updates massCutOffType of
         * - massCutoffMerge
         * - compoundMassCutoffWindow
         * - fragmentTolerance
         * @see PeakDetectionSettings::updatePeakSettings
         */

        if(QString(v.typeName()).contains("QString")) {
            _updateMavenParameters(key, *v.value<QString*>());
        }
    }
}

void Controller::updateOptionsDialogSettings(OptionsDialogSettings* od)
{
    _syncMpWithUi(od);

    auto settings = od->getSettings();
    for(const auto& k : settings.keys()) {
        const QVariant& v = settings.value(k);
        _updateSettingsForSave(k, v);
    }

}

void Controller::updatePeakDetectionSettings(PeakDetectionSettings* pd)
{
    _syncMpWithUi(pd);

    auto settings = pd->getSettings();
    for(const auto& k : settings.keys()) {
        const QVariant& v = settings.value(k);
        _updateSettingsForSave(k, v);
    }
}

void Controller::updateIsotopeDialogSettings(IsotopeDialogSettings* id)
{
    _syncMpWithUi(id);

    auto settings = id->getSettings();
    for (const auto& k : settings.keys()) {
        const QVariant& v = settings.value(k);
        _updateSettingsForSave(k, v);
    }
}

void Controller::updateUi()
{
    std::map<std::string, std::string>& mavenSettings = _mw->mavenParameters->getSettings();

    for(std::map<std::string, std::string>::iterator  it = mavenSettings.begin(); it != mavenSettings.end(); it++) {
        emit _mw->peakDetectionDialog->settingsChanged(it->first, it->second);
        emit _mw->settingsForm->settingsChanged(it->first, it->second);
        emit _mw->isotopeDialog->settingsChanged(it->first, it->second);
    }
}

void Controller::_updateMavenParameters(const QString& key,  const QVariant& value)
{
    /*TODO: can this be solved in a better way?.
     * In case of bool(if QVariant holds a bool val), "value.toByteArray().data()" retruns "false" for 0 and "true" for 1  but "false"  and "true" cant be converted 
     * to float(@see MavenParameters::setPeakDetectionSettings) and double(@see  PeakDetectionSettings::updatePeakSettings), hence we explicitly convert them 
     * to 0 and 1.
    */

    string data = value.toByteArray().data();

    if(value.type() == QVariant::Bool)
        data = std::to_string(value.toBool());

    _mw->mavenParameters->setIsotopeDialogSettings(key.toLocal8Bit().data(), data.c_str());
    _mw->mavenParameters->setPeakDetectionSettings(key.toLocal8Bit().data(),data.c_str());
    _mw->mavenParameters->setOptionsDialogSettings(key.toLocal8Bit().data(),data.c_str());
}

void Controller::_updateSettingsForSave(const QString& k, const QVariant& v)
{
    variant var;

    if (QString(v.typeName()).contains("QDoubleSpinBox"))
        var = v.value<QDoubleSpinBox*>()->value();

    if (QString(v.typeName()).contains("QSpinBox"))
        var = v.value<QSpinBox*>()->value();

    if (QString(v.typeName()).contains("QSlider"))
        var = v.value<QSlider*>()->value();

    if (QString(v.typeName()).contains("QGroupBox"))
        var = static_cast<int>(v.value<QGroupBox*>()->isChecked());

    if (QString(v.typeName()).contains("QCheckBox"))
        var = static_cast<int>(v.value<QCheckBox*>()->isChecked());

    if (QString(v.typeName()).contains("QComboBox"))
        var = v.value<QComboBox*>()->currentIndex();

    if (QString(v.typeName()).contains("QTabWidget"))
        var = v.value<QTabWidget*>()->currentIndex();

    if (QString(v.typeName()).contains("QLineEdit"))
        var = v.value<QLineEdit*>()->text().toStdString();

    // specifically for massCutoffType
    if (QString(v.typeName()).contains("QString"))
        var = v.value<QString*>()->toStdString();

    _mw->fileLoader->insertSettingForSave(k.toStdString(), var);
}

void Controller::_updateSettingsFromLoad(const map<string, variant>& settingsMap)
{
    for (const auto& it : settingsMap) {
        string key = it.first;
        variant var = it.second;
        string value = boost::apply_visitor(stringify(), var);
        _mw->mavenParameters->setOptionsDialogSettings(key.c_str(),
                                                       value.c_str());
        _mw->mavenParameters->setPeakDetectionSettings(key.c_str(),
                                                       value.c_str());
        _mw->mavenParameters->setIsotopeDialogSettings(key.c_str(),
                                                       value.c_str());

        // handle settings that need to be explicitly updated
        if (key == "uploadMultiprocessing")
            _mw->getSettings()->setValue(QString(key.c_str()), stoi(value));
        if (key == "mainWindowCharge")
            _mw->ionChargeBox->setValue(stoi(value));
        if (key == "mainWindowPeakQuantitation")
            _mw->quantType->setCurrentIndex(stoi(value));
        if (key == "mainWindowMassResolution")
            _mw->massCutoffWindowBox->setValue(stod(value));
        if (key == "mainWindowSelectedDbName")
            _mw->ligandWidget->setDatabase(QString(value.c_str()));
    }

    _mw->alignmentDialog->updateUiFromValues(settingsMap);
    updateUi();
    emit _mw->fileLoader->appSettingsUpdated();
}

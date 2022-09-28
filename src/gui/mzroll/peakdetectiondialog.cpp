#include <string>

#include <QVariant>

#include "alignmentdialog.h"
#include "common/analytics.h"
#include "common/mixpanel.h"
#include "backgroundopsthread.h"
#include "database.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "peakdetectiondialog.h"
#include "peakdetector.h"
#include "tabledockwidget.h"
#include "pollyelmaveninterface.h"
#include "mzUtils.h"
#include "pollyintegration.h"
#include "json.hpp"

using json = nlohmann::json;

PeakDetectionSettings::PeakDetectionSettings(PeakDetectionDialog* dialog):pd(dialog)
{
    // automated feature detection settings
    settings.insert("automatedDetection", QVariant::fromValue(pd->featureOptions));
    settings.insert("massDomainResolution", QVariant::fromValue(pd->ppmStep));
    settings.insert("timeDomainResolution", QVariant::fromValue(pd->rtStep));
    settings.insert("minMz", QVariant::fromValue(pd->mzMin));
    settings.insert("maxMz", QVariant::fromValue(pd->mzMax));
    settings.insert("minRt", QVariant::fromValue(pd->rtMin));
    settings.insert("maxRt", QVariant::fromValue(pd->rtMax));
    settings.insert("minIntensity", QVariant::fromValue(pd->minIntensity));
    settings.insert("maxIntensity", QVariant::fromValue(pd->maxIntensity));
    settings.insert("chargeMax", QVariant::fromValue(pd->chargeMax));
    settings.insert("chargeMin", QVariant::fromValue(pd->chargeMin));
    settings.insert("mustHaveFragmentation", QVariant::fromValue(pd->mustHaveMs2));
    settings.insert("identificationDatabase", QVariant::fromValue(pd->identificationDatabase));
    settings.insert("identificationMatchRt", QVariant::fromValue(pd->identificationMatchRt));
    settings.insert("identificationRtWindow", QVariant::fromValue(pd->identificationRtWindow));

    // db search settings
    settings.insert("databaseSearch", QVariant::fromValue(pd->dbSearch));
    settings.insert("compoundExtractionWindow", QVariant::fromValue(pd->compoundPPMWindow));
    settings.insert("matchRt", QVariant::fromValue(pd->matchRt));
    settings.insert("compoundRtWindow", QVariant::fromValue(pd->compoundRTWindow));
    settings.insert("limitGroupsPerCompound", QVariant::fromValue(pd->eicMaxGroups));

    //peakMl curation
    settings.insert("peakMlCuration", QVariant::fromValue(pd->peakMl));
    settings.insert("modelTypes", QVariant::fromValue(pd->modelTypes));

    // fragmentation settings
    settings.insert("matchFragmentation", QVariant::fromValue(pd->matchFragmentationOptions));
    settings.insert("minFragMatchScore", QVariant::fromValue(pd->minFragMatchScore));
    settings.insert("fragmentTolerance", QVariant::fromValue(pd->fragmentTolerance));
    settings.insert("minFragMatch", QVariant::fromValue(pd->minFragMatch));

    // group filtering settings
    settings.insert("peakQuantitation", QVariant::fromValue(pd->peakQuantitation));
    settings.insert("minGroupIntensity", QVariant::fromValue(pd->minGroupIntensity));
    settings.insert("intensityQuantile", QVariant::fromValue(pd->quantileIntensity));
    settings.insert("minGroupQuality", QVariant::fromValue(pd->doubleSpinBoxMinQuality));
    settings.insert("qualityQuantile", QVariant::fromValue(pd->quantileQuality));
    settings.insert("minSignalBlankRatio", QVariant::fromValue(pd->sigBlankRatio));
    settings.insert("signalBlankRatioQuantile", QVariant::fromValue(pd->quantileSignalBlankRatio));
    settings.insert("minSignalBaselineRatio", QVariant::fromValue(pd->sigBaselineRatio));
    settings.insert("signalBaselineRatioQuantile", QVariant::fromValue(pd->quantileSignalBaselineRatio));
    settings.insert("minPeakWidth", QVariant::fromValue(pd->minNoNoiseObs));
    settings.insert("peakWidthQuantile", QVariant::fromValue(pd->quantilePeakWidth));

    /* special case: there is no Ui element defined inside Peaks dialog that can be used
     * to change/access massCutOfftype. the only way to change massCutofftype is to change it from mainWindow(top right corner).
     * PeakDetectionDialog::masCutOffType is a variable that stores the value of MassCutOfftype defined in mainWindow
     * Better would be to have a ui element that allows to change/access massCutoff from peaks dialog
     */
    settings.insert("massCutoffType", QVariant::fromValue(&pd->massCutoffType));
}

void PeakDetectionSettings::updatePeakSettings(string key, string value)
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

        if(QString(v.typeName()).contains("QSlider"))
            v.value<QSlider*>()->setValue(std::stod(value));

        if(QString(v.typeName()).contains("QComboBox"))
            v.value<QComboBox*>()->setCurrentIndex(std::stoi(value));

        if(QString(v.typeName()).contains("QLineEdit"))
            v.value<QLineEdit*>()->setText(QString(value.c_str()));

        /* IMPORTANT
         * special case: only pd->massCutOfftype  and the places where it is used are updated here
         * there is no other Ui element that with  typeName as "QString".
         * Better solution is to have a Ui element in Peaks Dialog that can be used to
         * change/access massCutoff type
         */
        if(QString(v.typeName()).contains("QString")) {
            pd->massCutoffType = value.c_str();
            pd->getMainWindow()->massCutoffComboBox->setCurrentText(pd->massCutoffType);
        }

        emit pd->settingsUpdated(k, v);
    }
}

PeakDetectionDialog::PeakDetectionDialog(MainWindow* parent) :
        QDialog(parent)
{
        setupUi(this);

        settings = NULL;
        mainwindow = parent;

        setModal(false);
        peakupdater = NULL;

        _peakMlSet = false;

        massCutoffType = "ppm";
        peakSettings = new PeakDetectionSettings(this);

        peakupdater = new BackgroundOpsThread(this);
        if (mainwindow) peakupdater->setMainWindow(mainwindow);

        _inDetectionMode = false;
        
        auto tracker = parent->getUsageTracker();

        connect(resetButton, &QPushButton::clicked, this, &PeakDetectionDialog::onReset);
        connect(compoundDatabase, SIGNAL(currentTextChanged(QString)), SLOT(toggleFragmentation()));
        connect(identificationDatabase, SIGNAL(currentTextChanged(QString)), SLOT(toggleFragmentation()));
        connect(identificationDatabase,
                &QComboBox::currentTextChanged,
                [this] (QString text) {
                    if (!featureOptions->isChecked()
                        || identificationDatabase->currentIndex() == 0
                        || text == "") {
                        identificationRtWindow->setEnabled(false);
                        identificationMatchRt->setEnabled(false);
                    } else {
                        identificationMatchRt->setEnabled(true);
                        if (identificationMatchRt->isChecked())
                            identificationRtWindow->setEnabled(true);
                    }
                });

        connect(computeButton, SIGNAL(clicked(bool)), SLOT(findPeaks()));
        connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(matchRt,SIGNAL(clicked(bool)),compoundRTWindow,SLOT(setEnabled(bool)));
        connect(identificationMatchRt,
                &QCheckBox::toggled,
                identificationRtWindow,
                [this] (bool checked) {
                    if (checked && identificationMatchRt->isEnabled()) {
                        identificationRtWindow->setEnabled(true);
                    } else {
                        identificationRtWindow->setEnabled(false);
                    }
                });
        connect(matchFragmentationOptions,
                &QGroupBox::toggled,
                [this](const bool checked)
                {
                    QString state = checked? "On" : "Off";
                    this->mainwindow
                        ->getAnalytics()
                        ->hitEvent("Peak Detection",
                                   "Match Fragmentation Switched",
                                   state);
                    if (state == "On") {
                        this->mainwindow
                            ->getAnalytics()
                            ->hitEvent("PRM", "PRM Analysis");
                    }
                });
        connect(fragmentTolerance,
                SIGNAL(valueChanged(double)),
                mainwindow->massCalcWidget->fragPpm,
                SLOT(setValue(double)));

        _slider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, this);
        verticalLayout_3->addWidget(_slider);
        connect(peakMl, &QGroupBox::toggled,
                [this, tracker](const bool checked)
                {
                    if(checked){
                        getLoginForPeakMl();
                    }
                    else{
                        _peakMlSet = false;
                        mainwindow->mavenParameters->classifyUsingPeakMl = false;
                        modelTypes->setEnabled(false);
                    }
                    QMap<QString, QVariant> eventDetails;
                    eventDetails["Clicked button"] = "PeakML";
                    tracker->trackEvent("Peak detection dialog", eventDetails);
                });
        connect (_slider, SIGNAL(rangeChanged(int, int)), this, SLOT(updateCurationParameter(int, int)));
        connect(quantileIntensity,SIGNAL(valueChanged(int)),this, SLOT(showIntensityQuantileStatus(int)));
        connect(quantileQuality, SIGNAL(valueChanged(int)), this, SLOT(showQualityQuantileStatus(int)));
        connect(quantileSignalBaselineRatio, SIGNAL(valueChanged(int)), this, SLOT(showBaselineQuantileStatus(int)));
        connect(quantileSignalBlankRatio, SIGNAL(valueChanged(int)), this, SLOT(showBlankQuantileStatus(int)));
        connect(quantilePeakWidth,
                &QSlider::valueChanged,
                this,
                &PeakDetectionDialog::showPeakWidthQuantileStatus);

        connect(this, &QDialog::rejected, this, &PeakDetectionDialog::triggerSettingsUpdate);

        label_20->setVisible(false);
        chargeMin->setVisible(false);
        chargeMax->setVisible(false);



        connect(dbSearch, SIGNAL(toggled(bool)), SLOT(dbSearchClicked()));
        featureOptions->setChecked(false);
        connect(featureOptions, SIGNAL(toggled(bool)), SLOT(featureOptionsClicked()));

        connect(this, &PeakDetectionDialog::settingsChanged, peakSettings, &PeakDetectionSettings::updatePeakSettings);
        connect(peakQuantitation,
                &QComboBox::currentTextChanged,
                [&](QString type) {
                    mainwindow->setUserQuantType(type);
                });

        void (QDoubleSpinBox::* doubleChanged)(double) =
            &QDoubleSpinBox::valueChanged;
        connect(compoundPPMWindow,
                doubleChanged,
                [=] (double value) {
                    mainwindow->massCutoffWindowBox->setValue(value);
                });
}

void PeakDetectionDialog::getLoginForPeakMl()
{
    auto fileLocation = QStandardPaths::writableLocation(
                        QStandardPaths::GenericConfigLocation)
                        + QDir::separator();

    auto cookieFile = fileLocation + "El-MAVEN_cookie.json";
    QFile file(cookieFile);
    if (file.exists()) {
        loginSuccessful();
    }
    else {
        // Remove cred file and refreshTokenFile to maintain
        // consistency.
        auto credFile = fileLocation + "cred_file";
        QFile file (credFile);
        file.remove();
        QFile refreshTokenFile (credFile + "_refreshToken");
        refreshTokenFile.remove();
        mainwindow->pollyElmavenInterfaceDialog->loginForPeakMl();
    }     
}

void PeakDetectionDialog::handleAuthorization(QMap<QString, int> modelDetails, QString status) {
    if (status != "OK") {
        unsuccessfulLogin();
        auto htmlText = QString("<p><b>Something went wrong. Kindly check for your authentication</b></p>");
            htmlText += "<p>Please contact tech support at elmaven@elucidata.io if the problem persists.</p>";
        mainwindow->showWarning(htmlText);
    } else {
        QMapIterator<QString, int> iterator(modelDetails);
        while (iterator.hasNext()) {
            iterator.next();
            string modelName = iterator.key().toStdString();
            mainwindow->mavenParameters->availablePeakMLModels.insert({modelName, iterator.value()});
            modelTypes->addItem(iterator.key());
        }
    }
}

bool PeakDetectionDialog::_checkForCohortFile()
{
    auto samples = mainwindow->samples;
    
    for (auto sample : samples) {
        if (sample->getSetName() == "") 
            return false;
    }
    return true;
}

void PeakDetectionDialog::loginSuccessful()
{ 
    bool cohortUploaded = _checkForCohortFile();
    if (!cohortUploaded) {
        QString warningMessage = QString("<p><b>Cohorts must be defined to be able to run Polly-PeakML.</b></p>");
        warningMessage += "<p>Please define cohorts for the samples and try again later.</p>";

        mainwindow->showWarning(warningMessage);
        unsuccessfulLogin();
        return;
    }
    _peakMlSet = true;
    peakMl->setChecked(true);
    mainwindow->mavenParameters->classifyUsingPeakMl = true;
    modelTypes->setEnabled(true);
    mainwindow->pollyElmavenInterfaceDialog->getModelsForPeakML();
}

void PeakDetectionDialog::unsuccessfulLogin()
{   
    _peakMlSet = false;
    peakMl->setChecked(false);
    modelTypes->setEnabled(false);
    if(mainwindow)
        mainwindow->mavenParameters->classifyUsingPeakMl = false;
}

void PeakDetectionDialog::onReset()
{
    emit resetSettings(peakSettings->getSettings().keys());
}

void PeakDetectionDialog::setMassCutoffType(QString type)
{
    massCutoffType = type;
    auto suffix = QString(" %1").arg(type);
    ppmStep->setSuffix(suffix);
    compoundPPMWindow->setSuffix(suffix);
    emit updateSettings(peakSettings);
}

void PeakDetectionDialog::updateCurationParameter(int lowerRange, int upperRange)
{
    float noiseLimit = lowerRange/10.0;
    float maybeGoodLimit = upperRange/10.0;
    
    QString noiseLabel= "Noise Range: 0.0 - ";
    noiseLabel += QString::fromStdString(mzUtils::float2string(noiseLimit, 1));
    noiseRange->setText(noiseLabel);

    QString signalLabel= "Signal Range: ";
    signalLabel += QString::fromStdString(mzUtils::float2string(maybeGoodLimit, 1));
    signalLabel += " - 1.0";
    signalRange->setText(signalLabel);

}

void PeakDetectionDialog::setQuantType(QString type)
{
    peakQuantitation->setCurrentText(type);
}

void PeakDetectionDialog::closeEvent(QCloseEvent* event)
{
    if (_inDetectionMode) {
        event->ignore();
        return;
    }

    // update maven peak settings on close-event ('close' or 'cancel' button)
    emit updateSettings(peakSettings);
    QDialog::closeEvent(event);
}

void PeakDetectionDialog::keyPressEvent(QKeyEvent *event)
{
    if (_inDetectionMode) {
        event->ignore();
        return;
    }
    QDialog::keyPressEvent(event);
}

void PeakDetectionDialog::dbSearchClicked()
{   
    if (dbSearch->isChecked()) {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(0);
        featureOptions->setChecked(false);
        toggleFragmentation();
    } else {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(1);
        featureOptions->setChecked(true);
    }
    toggleFragmentation();
}

bool PeakDetectionDialog::databaseSearchEnabled()
{
    return dbSearch->isChecked();
}

void PeakDetectionDialog::triggerSettingsUpdate()
{
    // happens when users presses 'esc' key; 
    emit updateSettings(peakSettings);
}

void PeakDetectionDialog::featureOptionsClicked()
{
    if (featureOptions->isChecked()) {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(1);
        dbSearch->setChecked(false);
        toggleFragmentation();
    } else {
        dbSearch->setChecked(true);
    }
    toggleFragmentation();
}

PeakDetectionDialog::~PeakDetectionDialog() {
    if (peakupdater) delete (peakupdater);
}
/**
 * PeakDetectionDialog::cancel Stoping the peak detection process
 */
void PeakDetectionDialog::cancel()
{
    if (peakupdater && peakupdater->isRunning()) {
        setProgressBar("Cancelling…", 0, 0);
        peakupdater->completeStop();
        setProgressBar("Cancelled", 0, 1);
    }
    setDetectionMode(false);
}

void PeakDetectionDialog::initPeakDetectionDialogWindow(
    FeatureDetectionType type) {
    displayAppropriatePeakDetectionDialog(type);
    inputInitialValuesPeakDetectionDialog();
}
/**
 * PeakDetectionDialog::setFeatureDetection This function is being called when
 * the database search button or the feature detection button is cliecked
 * from the main window
 * @param type this is an enum which can take the following values
 * FullSpectrum, CompoundDB, QQQ
 */
void PeakDetectionDialog::displayAppropriatePeakDetectionDialog(
    FeatureDetectionType type) {
    _featureDetectionType = type;

    if (_featureDetectionType == QQQ) {
        dbSearch->hide();
        featureOptions->hide();
    } else if (_featureDetectionType == FullSpectrum) {
        dbSearch->hide();
        featureOptions->show();
    } else if (_featureDetectionType == CompoundDB) {
        dbSearch->show();
        featureOptions->hide();
    }

    tabwidget->setCurrentIndex(
        0);
    adjustSize();
}

void PeakDetectionDialog::show() {

    if (mainwindow == NULL) return;

    peakMl->setChecked(false);
    _peakMlSet = false;
    mainwindow->mavenParameters->classifyUsingPeakMl = false;
    modelTypes->setEnabled(false);

    mainwindow->getAnalytics()->hitScreenView("PeakDetectionDialog");
    // delete(peakupdater);
    peakupdater = new BackgroundOpsThread(this);
    if (mainwindow) peakupdater->setMainWindow(mainwindow);

    connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
               SLOT(setProgressBar(QString, int,int)));
    connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
               mainwindow->alignmentDialog, SLOT(setProgressBar(QString, int,int)));

    // peakupdater->useMainWindowLabelOptions = false;

    inputInitialValuesPeakDetectionDialog();
    toggleFragmentation();
}

/**
 * PeakDetectionDialog::show This function is being called when the
 * database search button or the feature detection button is cliecked
 * from the main window
 */
void PeakDetectionDialog::inputInitialValuesPeakDetectionDialog() {
    // TODO: Why only this two variables are updated in the windows that is
    // selected by the user
    if (mainwindow != NULL) {
        // Peak Scoring and Filtering
        showQualityQuantileStatus(quantileQuality->value());
        showBaselineQuantileStatus(quantileSignalBaselineRatio->value());
        showBlankQuantileStatus(quantileSignalBlankRatio->value());
        showIntensityQuantileStatus(quantileIntensity->value());
        showPeakWidthQuantileStatus(quantilePeakWidth->value());

        refreshCompoundDatabases();

        /**
         * Getting the database present and updating in the dropdown of the
         * peak detection windows
         */
        map<string, int>::iterator itr;
        map<string, int> dbnames = DB.getDatabaseNames();

        // Storing this value so that we can set it back after multiple DB name
        // changes. The enabled/disabled state will still be managed later once.
        bool fragmentationWasEnabled = matchFragmentationOptions->isChecked();

        // Clearing so that old value is not appended with the new values
        compoundDatabase->clear();
        for (itr = dbnames.begin(); itr != dbnames.end(); itr++) {
            string db = (*itr).first;
            if (!db.empty()) compoundDatabase->addItem(QString(db.c_str()));
        }
        matchFragmentationOptions->setChecked(fragmentationWasEnabled);
        modelTypes->clear();
        // selecting the compound database that is selected by the user in the
        // ligand widget
        QString selectedDB = mainwindow->ligandWidget->getDatabaseName();
        compoundDatabase->setCurrentIndex(
            compoundDatabase->findText(selectedDB));

        //match fragmentation only enabled during targeted detection with NIST library
        toggleFragmentation();

        if (!dbSearch->isChecked() || !matchRt->isChecked()) {
            compoundRTWindow->setEnabled(false);
        } else {
            compoundRTWindow->setEnabled(true);
        }

        if (!featureOptions->isChecked()
            || !identificationMatchRt->isEnabled()
            || !identificationMatchRt->isChecked()) {
            identificationRtWindow->setEnabled(false);
        } else {
            identificationRtWindow->setEnabled(true);
        }

        QDialog::exec();
    }
}

void PeakDetectionDialog::refreshCompoundDatabases()
{
    map<string, int>::iterator itr;
    map<string, int> dbnames = DB.getDatabaseNames();

    // Clearing so that old value is not appended with the new values
    compoundDatabase->clear();
    identificationDatabase->clear();
    identificationDatabase->addItem("None");
    for (itr = dbnames.begin(); itr != dbnames.end(); itr++) {
        string db = (*itr).first;
        if (!db.empty()) {
            compoundDatabase->addItem(QString(db.c_str()));
            identificationDatabase->addItem(QString(db.c_str()));
        }
    }
}

void PeakDetectionDialog::toggleFragmentation()
{
    QString selectedDbName = "";
    if (dbSearch->isChecked()) {
        selectedDbName = compoundDatabase->currentText();
    } else if (featureOptions->isChecked()) {
        selectedDbName = identificationDatabase->currentText();
    }

    auto samples = mainwindow->getVisibleSamples();
    auto iter = find_if(begin(samples),
                        end(samples),
                        [](mzSample* s) {
                           return (s->ms1ScanCount() > 0
                                   && s->ms2ScanCount() > 0
                                   && s->msMsType() == mzSample::MsMsType::DDA);
                        });
    bool foundDda = iter != end(samples);

    if (foundDda && DB.isSpectralLibrary(selectedDbName.toStdString())) {
        matchFragmentationOptions->setEnabled(true);
    } else {
        matchFragmentationOptions->setChecked(false);
        matchFragmentationOptions->setEnabled(false);
        mustHaveMs2->setChecked(false);
    }
}

void PeakDetectionDialog::setDetectionMode(bool detectionModeOn)
{
    _inDetectionMode = detectionModeOn;
    if (_inDetectionMode) {
        featureOptions->setDisabled(true);
        dbSearch->setDisabled(true);
        matchFragmentationOptions->setDisabled(true);
        peakScoringOptions->setDisabled(true);
        computeButton->setDisabled(true);
        resetButton->setDisabled(true);
        peakMl->setDisabled(true);
    } else {
        featureOptions->setEnabled(true);
        dbSearch->setEnabled(true);
        matchFragmentationOptions->setEnabled(true);
        peakScoringOptions->setEnabled(true);
        computeButton->setEnabled(true);
        resetButton->setEnabled(true);
        peakMl->setEnabled(true);
    }
}

/**
 * PeakDetectionDialog::findPeaks This is the function which excuite functions
 * related to peak detection
 * Peak detection happens in a different thread called "peakupdater"
 * According to the settings that is selected by the user, peak detection can
 * happen using different algorithm
 *
 * In this funtion all the paremeters that are input by the user is taken into
 * the maven parameter class
 *
 * Then the needed algorithm is ran
 */

// TODO: Sahil. Refactored this whole function. Merged with mainwindow of 776.
// RECHECK IT AGAIN. IMPORTANT
void PeakDetectionDialog::findPeaks()
{
    emit findPeaksClicked();
    setDetectionMode(true);

    // IMPORTANT: we have to make sure that maven parameters are updated before we start finding peaks.
    // there are not a lot of settings that need to be updated,hence it's not late to update them right now.
    emit updateSettings(peakSettings);
    
    if (mainwindow == NULL) return;
    if (peakupdater == NULL) return;
    if (peakupdater->isRunning()) cancel();
    if (peakupdater->isRunning()) return;

    updateQSettingsWithUserInput(settings);
    setMavenParameters(settings);

    QString dbName = "";
    QString mode = "";
    if (dbSearch->isChecked() && !(featureOptions->isChecked())) {
        _featureDetectionType = CompoundDB;
        mode = "Targeted";
        mainwindow->massCutoffWindowBox->setValue(compoundPPMWindow->value());
        dbName = compoundDatabase->currentText();
    } else if (!(dbSearch->isChecked()) && (featureOptions->isChecked())) {
        _featureDetectionType = FullSpectrum;
        mode = "Untargeted";
        mainwindow->massCutoffWindowBox->setValue(ppmStep->value());
        if (identificationDatabase->currentIndex() != 0)
            dbName = identificationDatabase->currentText();
    } else {
        _featureDetectionType = FullSpectrum;
    }
    if (mustHaveMs2->isChecked()) {
        mainwindow->getAnalytics()->hitEvent("Peak Detection",
                                             mode,
                                             "Filter for MS2 events");
    } else {
        mainwindow->getAnalytics()->hitEvent("Peak Detection",
                                             mode,
                                             "No filter");
    }

    TableDockWidget* peaksTable = mainwindow->addPeaksTable(dbName, 
                                                            mainwindow->mavenParameters->classifyUsingPeakMl);

    // disconnect prvevious connections
    disconnect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), 0, 0);
    disconnect(peakupdater, SIGNAL(finished()), 0, 0);

    // connect new connections
    connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable,
           SLOT(addPeakGroup(PeakGroup*)));
    connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
    connect(peakupdater,
            &BackgroundOpsThread::finished,
            this,
            [this] {
                mainwindow->mavenParameters->allgroups.clear();
                setDetectionMode(false);
                close();
            });
    connect(peakupdater, 
            &BackgroundOpsThread::toggleCancel, 
            this,
            [this] {
                if (cancelButton->isEnabled()) {
                    cancelButton->setEnabled(false);
                } else {
                    cancelButton->setEnabled(true);
                }     
            });

    
    peakupdater->setPeakDetector(new PeakDetector(peakupdater->mavenParameters));

    // RUN THREAD
    if (_featureDetectionType == FullSpectrum)
        runBackgroupJob("findFeatures");
    else
        runBackgroupJob("computePeaks");

}

void PeakDetectionDialog::showIntensityQuantileStatus(int value) {
    mainwindow->mavenParameters->quantileIntensity= value;
    QString qstat;
    if (value) {
        std::string stat(std::to_string(value) + "% peaks above minimum intensity");
        qstat = QString::fromStdString(stat);
    }
    else {
        std::string stat("1 peak above minimum intensity");
        qstat = QString::fromStdString(stat);
    }
    intensityQuantileStatus->setText(qstat);
}

void PeakDetectionDialog::showQualityQuantileStatus(int value) {
    mainwindow->mavenParameters->quantileQuality= value;
    QString qstat;
    if (value) {
        std::string stat(std::to_string(value) + "% peaks above minimum quality");
        qstat = QString::fromStdString(stat);
    }
    else {
        std::string stat("1 peak above minimum quality");
        qstat = QString::fromStdString(stat);
    }
    qualityQuantileStatus->setText(qstat);
}

void PeakDetectionDialog::showBaselineQuantileStatus(int value) {
    mainwindow->mavenParameters->quantileSignalBaselineRatio= value;
    QString qstat;
    if (value) {
        std::string stat(std::to_string(value) + "% peaks above minimum signal/baseline ratio");        
        qstat = QString::fromStdString(stat);
    }
    else {
        std::string stat("1 peak above minimum signal/baseline ratio");
        qstat = QString::fromStdString(stat);
    }
    baselineQuantileStatus->setText(qstat);
}

void PeakDetectionDialog::showBlankQuantileStatus(int value) {
    mainwindow->mavenParameters->quantileSignalBlankRatio= value;
    QString qstat;
    if (value) {
        std::string stat(std::to_string(value) + "% peaks above minimum signal/blank ratio");
        qstat = QString::fromStdString(stat);
    }
    else {
        std::string stat("1 peak above minimum signal/blank ratio");
        qstat = QString::fromStdString(stat);
    }
    blankQuantileStatus->setText(qstat);
}

void PeakDetectionDialog::showPeakWidthQuantileStatus(int value)
{
    mainwindow->mavenParameters->quantilePeakWidth = value;
    QString qstat;
    if (value) {
        std::string stat(std::to_string(value)
                         + "% peaks above minimum peak width");
        qstat = QString::fromStdString(stat);
    } else {
        std::string stat("1 peak above minimum peak width");
        qstat = QString::fromStdString(stat);
    }
    widthQuantileStatus->setText(qstat);
}

void PeakDetectionDialog::updateQSettingsWithUserInput(QSettings* settings) {
    // Peak Scoring and Filtering
    // Compound DB search
    // Automated Peak Detection

    // Enabling feature detection or compound search
    ////////////////////////////////////////////////////////////
    // TODO: what is this?
    vector<mzSample*> samples = mainwindow->getSamples();
    //TODO Sabu: This has to be taken care in a better way 
    if (samples.size() > 0) {
        settings->setValue("avgScanTime", 
                samples[0]->getAverageFullScanTime());
    }
    // Time domain resolution(scans)
}

void PeakDetectionDialog::setMavenParameters(QSettings* settings) {
    if (peakupdater->isRunning()) return;
    
    if(_peakMlSet) {
        mainwindow->mavenParameters->classifyUsingPeakMl = true;
        mainwindow->mavenParameters->badGroupUpperLimit = _slider->GetLowerValue() / 10.0;
        mainwindow->mavenParameters->goodGroupLowerLimit = _slider->GetUpperValue() / 10.0;
        mainwindow->mavenParameters->peakMlModelType = modelTypes->currentText().toStdString();
    }

    MavenParameters* mavenParameters = mainwindow->mavenParameters;
    
    if (settings != NULL) {

        mavenParameters->writeCSVFlag = false;
        //Getting the classification model
        mavenParameters->clsf = mainwindow->getClassifier();

        if (dbSearch->isChecked()) {
            mavenParameters->setCompounds(DB.getCompoundsSubset(
                compoundDatabase->currentText().toStdString()));
        } else if (featureOptions->isChecked()
                   && identificationDatabase->currentIndex() != 0) {
            mavenParameters->setCompounds(DB.getCompoundsSubset(
                identificationDatabase->currentText().toStdString()));
        } else {
            mavenParameters->setCompounds({});
        }

        mavenParameters->avgScanTime = settings->value("avgScanTime").toDouble();

        mavenParameters->samples = mainwindow->getSamples();

        peakupdater->setMavenParameters(mavenParameters);

    }
}
/**
 * PeakDetectionDialog::runBackgroupJob This function will start the thread for
 * the given algorithm with which the peak detection has to be done.
 * @param funcName [description]
 */
void PeakDetectionDialog::runBackgroupJob(QString funcName) {
        //Making sure all the setting that is necessary for starting the
        //thread is being added
        //TODO: have to add to check if the mavenParameters is not null
        if (peakupdater == NULL)
                return;
        // Making sure that the already some peakdetector thread is not
        // working
        if (peakupdater->isRunning()) {
                 cancel();
        }
        //Starting the thread
        if (!peakupdater->isRunning()) {
                peakupdater->setRunFunction(funcName); //set thread function
                peakupdater->start(); //start a background thread
        }
}

/**
 * PeakDetectionDialog::showInfo This updates the status process with the
 * algorithm with which thepeak detection is going to happen
 * @param text this is the algorithm that the peakdetection process uses
 */
void PeakDetectionDialog::showInfo(QString text) {
        statusText->setText(text);
}

/**
 * PeakDetectionDialog::setProgressBar This functionupdates the progress bar
 * with the percentage ofcomplition of peak detectionprocess
 * @param text       Text to be updated in the label
 * @param progress   Progress
 * @param totalSteps Total number of steps
 */
void PeakDetectionDialog::setProgressBar(QString text, int progress,
                                         int totalSteps) {
        showInfo(text);
        progressBar->setRange(0, totalSteps);
        progressBar->setValue(progress);
}

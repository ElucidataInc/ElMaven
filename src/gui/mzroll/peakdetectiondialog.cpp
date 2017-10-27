#include "peakdetectiondialog.h"

PeakDetectionDialog::PeakDetectionDialog(QWidget *parent) :
        QDialog(parent) {
        setupUi(this);
        settings = NULL;
        mainwindow = NULL;

        setModal(false);
        peakupdater = NULL;

        peakupdater = new BackgroundPeakUpdate(this);
        if (mainwindow) peakupdater->setMainWindow(mainwindow);

        connect(computeButton, SIGNAL(clicked(bool)), SLOT(findPeaks()));
        connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(setOutputDirButton, SIGNAL(clicked(bool)), SLOT(setOutputDir()));
        connect(matchRt,SIGNAL(clicked(bool)),compoundRTWindow,SLOT(setEnabled(bool))); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(tabwidget,SIGNAL(currentChanged(int)),this,SLOT(showMethodSummary())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(tabwidget,SIGNAL(currentChanged(int)),this,SLOT(updatePeakTableList())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(reportIsotopesOptions,SIGNAL(clicked(bool)),this,SLOT(showMethodSummary())); 
        connect(saveMethodButton,SIGNAL(clicked()),this,SLOT(saveMethod())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(loadMethodButton,SIGNAL(clicked()),this,SLOT(loadMethod())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(loadModelButton,SIGNAL(clicked()),this,SLOT(loadModel()));

        connect(quantileIntensity,SIGNAL(valueChanged(int)),this, SLOT(showIntensityQuantileStatus(int)));
        connect(quantileQuality, SIGNAL(valueChanged(int)), this, SLOT(showQualityQuantileStatus(int)));
        connect(quantileSignalBaselineRatio, SIGNAL(valueChanged(int)), this, SLOT(showBaselineQuantileStatus(int)));
        connect(quantileSignalBlankRatio, SIGNAL(valueChanged(int)), this, SLOT(showBlankQuantileStatus(int)));
        //connect(qualityWeight,SIGNAL(valueChanged(int)), this,SLOT(showMethodSummary()));
        //connect(intensityWeight,SIGNAL(valueChanged(int)), this,SLOT(showMethodSummary()));
        //connect(deltaRTWeight,SIGNAL(valueChanged(int)), this,SLOT(showMethodSummary()));

        label_20->setVisible(false);
        chargeMin->setVisible(false);
        chargeMax->setVisible(false);
        
        connect(dbOptions, SIGNAL(clicked(bool)), SLOT(dbOptionsClicked()));
        featureOptions->setChecked(false);
        connect(featureOptions, SIGNAL(clicked(bool)), SLOT(featureOptionsClicked()));

        compoundRTWindow->setEnabled(false); //TODO: Sahil - Kiran, Added while merging mainwindow
        reportIsotopesOptions->setEnabled(true); //TODO: Sahil - Kiran, Added while merging mainwindow
        //_featureDetectionType = CompoundDB; //TODO: Sahil - Kiran, removed while merging mainwindow
        connect(changeIsotopeOptions,SIGNAL(clicked()),this, SLOT(showSettingsForm()));

}

void PeakDetectionDialog::showSettingsForm() {
    LOGD;
    mainwindow->settingsForm->exec();
    mainwindow->settingsForm->setIsotopeDetectionTab();
}

void PeakDetectionDialog::dbOptionsClicked() {
    if (dbOptions->isChecked()) {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(0);
        featureOptions->setChecked(false);
    } else {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(1);
        featureOptions->setChecked(true);
    }
}

void PeakDetectionDialog::featureOptionsClicked() {
    if (featureOptions->isChecked()) {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(1);
        dbOptions->setChecked(false);
    } else {
        mainwindow->alignmentDialog->peakDetectionAlgo->setCurrentIndex(0);
        dbOptions->setChecked(true);
    }
}

PeakDetectionDialog::~PeakDetectionDialog() {
    cancel();
    if (peakupdater) delete (peakupdater);
}
/**
 * PeakDetectionDialog::cancel Stoping the peak detection process
 */
void PeakDetectionDialog::cancel() {
    LOGD;
    if (peakupdater) {
        if (peakupdater->isRunning()) {
            peakupdater->completeStop();
            return;
        }
    }
    close();
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
        dbOptions->hide();
        featureOptions->hide();
    } else if (_featureDetectionType == FullSpectrum) {
        dbOptions->hide();
        featureOptions->show();
    } else if (_featureDetectionType == CompoundDB) {
        dbOptions->show();
        featureOptions->hide();
    }

    tabwidget->setCurrentIndex(
        0);  // TODO: Sahil - Kiran, Added while merging mainwindow
    adjustSize();
}

/*
@author: Sahil-Kiran
*/
// TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::show() {
    // Thi is merged to 776
    if (mainwindow == NULL) return;

    // delete(peakupdater);
    peakupdater = new BackgroundPeakUpdate(this);
    if (mainwindow) peakupdater->setMainWindow(mainwindow);

    connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
               SLOT(setProgressBar(QString, int,int)));
    connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
               mainwindow->alignmentDialog, SLOT(setProgressBar(QString, int,int)));

    // peakupdater->useMainWindowLabelOptions = false;

    inputInitialValuesPeakDetectionDialog();

}

/**
 * PeakDetectionDialog::loadModel This function works in the peakdector window
 * When theuser clicks on the Peak classifier Model file a dialog box appears
 * from which they can seect the model that is been trained
 */
void PeakDetectionDialog::loadModel() {
    LOGD;
    // This gives the name of the file that is selected by the user
    const QString name = QFileDialog::getOpenFileName(
        this, "Select Classification Model", ".", tr("Model File (*.model)"));
    // This is applying the text to the window that the user has selected
    classificationModelFilename->setText(name);
    // Getting the classifier instance from the main window
    Classifier* clsf = mainwindow->getClassifier();

    // Loading the model to the to the model instance
    if (clsf)
        clsf->loadModel(classificationModelFilename->text().toStdString());
}

/*
@author:Sahil-Kiran
*/
// TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::loadMethod() {
    LOGD;
    const QString filename = QFileDialog::getOpenFileName(
        this, "Load Settings", ".", tr("Settings File (*.method)"));
    peakupdater->loadSettings(filename);
    inputInitialValuesPeakDetectionDialog();
}

/*
@author:Sahil-Kiran
*/
// TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::saveMethod() {
    LOGD;
    const QString filename = QFileDialog::getSaveFileName(
        this, "Save Settings", ".", tr("Settings File (*.method)"));
    peakupdater->saveSettings(filename);
}

/**
 * PeakDetectionDialog::setOutputDir this function sets the directory of the
 * file to which the output has to be connected
 */
void PeakDetectionDialog::setOutputDir() {
    // Getting the user selected folder name and path
    const QString dirName = QFileDialog::getExistingDirectory(
        this, "Save Reports to a Folder", ".", QFileDialog::ShowDirsOnly);
    // Showing it in the output directory box
    outputDirName->setText(dirName);
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
        QSettings* settings = mainwindow->getSettings();
        if (settings) {

            // Peak Scoring and Filtering
            doubleSpinBoxMinQuality->setValue(
                settings->value("minQuality").toDouble());
            quantileQuality->setValue(
                settings->value("quantileQuality").toDouble());
            showQualityQuantileStatus(quantileQuality->value());
            minGoodGroupCount->setValue(
                settings->value("minGoodGroupCount").toInt());
            minNoNoiseObs->setValue(
                settings->value("minNoNoiseObs").toDouble());  // minPeakWidth
            sigBaselineRatio->setValue(
                settings->value("minSignalBaseLineRatio").toDouble());
            quantileSignalBaselineRatio->setValue(
                settings->value("quantileSignalBaselineRatio").toDouble());
            showBaselineQuantileStatus(quantileSignalBaselineRatio->value());
            sigBlankRatio->setValue(
                settings->value("minSignalBlankRatio").toDouble());
            quantileSignalBlankRatio->setValue(
                settings->value("quantileSignalBlankRatio").toDouble());
            showBlankQuantileStatus(quantileSignalBlankRatio->value());
            minGroupIntensity->setValue(
                settings->value("minGroupIntensity").toDouble());
            peakQuantitation->setCurrentIndex(
                settings->value("peakQuantitation").toInt());
            quantileIntensity->setValue(
                settings->value("quantileIntensity").toDouble());
            showIntensityQuantileStatus(quantileIntensity->value());

            // Compound DB search
            matchRt->setChecked(settings->value("matchRtFlag").toBool());
            compoundPPMWindow->setValue(
                settings->value("compoundPPMWindow").toDouble());
            compoundRTWindow->setValue(
                settings->value("compoundRTWindow").toDouble());
            eicMaxGroups->setValue(settings->value("eicMaxGroups").toInt());

            // Automated Peak Detection
            ppmStep->setValue(settings->value("ppmMerge").toDouble());
            rtStep->setValue(settings->value("rtStepSize").toDouble());
            rtMin->setValue(settings->value("minRT").toDouble());
            rtMax->setValue(settings->value("maxRT").toDouble());
            mzMin->setValue(settings->value("minMz").toDouble());
            mzMax->setValue(settings->value("maxMz").toDouble());
            minIntensity->setValue(settings->value("minIntensity").toDouble());
            maxIntensity->setValue(settings->value("maxIntensity").toDouble());
            chargeMin->setValue(settings->value("minCharge").toDouble());
            chargeMax->setValue(settings->value("maxCharge").toDouble());
            classificationModelFilename->setText(settings->value("clsfModelFilename").toString());

            // Isotope detection in peakdetection dialogue box
            reportIsotopesOptions->setChecked(
                settings->value("pullIsotopesFlag").toBool());
            // checkBox->setChecked(settings->value("checkBox").toBool());  // C13
            // checkBox_2->setChecked(
            //     settings->value("checkBox_2").toBool());  // N15
            // checkBox_3->setChecked(
            //     settings->value("checkBox_3").toBool());  // D2
            // checkBox_4->setChecked(
            //     settings->value("checkBox_4").toBool());  // S34

            // Fragment Score
            minFragMatchScore->setValue(
                settings->value("minFragmentMatchScore").toDouble());
            matchFragmentatioOptions->setChecked(
                settings->value("matchFragmentation").toBool());

            // Enabling feature detection or compound search
            dbOptions->setChecked(settings->value("dbOptions").toBool());
            featureOptions->setChecked(
                settings->value("featureOptions").toBool());
        }
        /**
         * Getting the database present and updating in the dropdown of the
         * peak detection windows
         */
        map<string, int>::iterator itr;
        map<string, int> dbnames = DB.getDatabaseNames();

        // Clearing so that old value is not appended with the new values
        compoundDatabase->clear();
        for (itr = dbnames.begin(); itr != dbnames.end(); itr++) {
            string db = (*itr).first;
            if (!db.empty()) compoundDatabase->addItem(QString(db.c_str()));
        }

        // selecting the compound database that is selected by the user in the
        // ligand widget
        QString selectedDB = mainwindow->ligandWidget->getDatabaseName();
        compoundDatabase->setCurrentIndex(
            compoundDatabase->findText(selectedDB));

        // EIC extraction windows ppm value that is set in the main
        // window is been set to the GUI
        compoundPPMWindow->setValue(mainwindow->getUserPPM());
        QDialog::exec();
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
void PeakDetectionDialog::findPeaks() {
    LOGD;
    if (mainwindow == NULL) return;
    if (peakupdater == NULL) return;
    if (peakupdater->isRunning()) cancel();
    if (peakupdater->isRunning()) return;

    updateQSettingsWithUserInput(settings);
    setMavenParameters(settings);
    mainwindow->settingsForm->setIsotopeAtom();

    mainwindow->setTotalCharge();


    if (dbOptions->isChecked() && !(featureOptions->isChecked())) {
        _featureDetectionType = CompoundDB;
    } else if (!(dbOptions->isChecked()) && (featureOptions->isChecked())) {
        _featureDetectionType = FullSpectrum;
    } else if (!(dbOptions->isChecked()) && !(featureOptions->isChecked())) {
        _featureDetectionType = QQQ;
    } else {
        _featureDetectionType = FullSpectrum;
    }

    QString title;
    if (_featureDetectionType == FullSpectrum)
        title = "Peak Table " + QString::number(mainwindow->noOfPeakTables) + "\nDetected Features \n";
    else if (_featureDetectionType == CompoundDB)
        title = "Peak Table " + QString::number(mainwindow->noOfPeakTables) + "\nDB Search " + compoundDatabase->currentText();
    else if (_featureDetectionType == QQQ)
        title = "Peak Table " + QString::number(mainwindow->noOfPeakTables) + "\nQQQ DB Search " + compoundDatabase->currentText();

    TableDockWidget* peaksTable = mainwindow->getBookmarkedPeaks();
    int peakTableIdx = outputTableComboBox->currentIndex();

    if (peakTableIdx == 0) {
        peaksTable = mainwindow->addPeaksTable(title);
    } else if (peakTableIdx == 1) {
        peaksTable = mainwindow->getBookmarkedPeaks();
    } else if (peakTableIdx >= 2) {
        QList<QPointer<TableDockWidget> > peaksTableList =
            mainwindow->getPeakTableList();
        if (peaksTableList.size() >= 1 and
            peakTableIdx - 2 < peaksTableList.size()) {
            peaksTable = peaksTableList[peakTableIdx - 2];
        }
    }


    // disconnect prvevious connections
    disconnect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), 0, 0);
    disconnect(peakupdater, SIGNAL(finished()), 0, 0);
    // disconnect(peakupdater, SIGNAL(terminated()), 0, 0);

    // connect new connections
    connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable,
           SLOT(addPeakGroup(PeakGroup*)));
    connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
    // connect(peakupdater, SIGNAL(terminated()), peaksTable,
            // SLOT(showAllGroups()));
    connect(peakupdater, SIGNAL(finished()), this, SLOT(close()));
    // connect(peakupdater, SIGNAL(terminated()), this, SLOT(close()));
    peakupdater->setPeakDetector(new PeakDetector(peakupdater->mavenParameters));

    // RUN THREAD
    if (_featureDetectionType == QQQ) {
        runBackgroupJob("findPeaksQQQ");
    } else if (_featureDetectionType == FullSpectrum) {
        runBackgroupJob("processMassSlices");
    } else {
        runBackgroupJob("computePeaks");
    }
}

void PeakDetectionDialog::showIntensityQuantileStatus(int value) {
    mainwindow->mavenParameters->quantileIntensity= value;
    settings->setValue("quantileIntensity", (quantileIntensity->value()));
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
    settings->setValue("quantileQuality", (quantileQuality->value()));
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
    settings->setValue("quantileSignalBaselineRatio", (quantileSignalBaselineRatio->value()));
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
    settings->setValue("quantileSignalBlankRatio", (quantileSignalBlankRatio->value()));
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

void PeakDetectionDialog::updateQSettingsWithUserInput(QSettings* settings) {
    // Peak Scoring and Filtering
    settings->setValue("quantileQuality", quantileQuality->value());
    settings->setValue("minQuality",doubleSpinBoxMinQuality->value());
    settings->setValue("minGoodGroupCount", minGoodGroupCount->value());
    settings->setValue("minNoNoiseObs", minNoNoiseObs->value());
    settings->setValue("minSignalBaseLineRatio", sigBaselineRatio->value());
    settings->setValue("quantileSignalBaselineRatio", quantileSignalBaselineRatio->value());
    settings->setValue("minSignalBlankRatio", sigBlankRatio->value());
    settings->setValue("quantileSignalBlankRatio", quantileSignalBlankRatio->value());
    settings->setValue("minGroupIntensity", minGroupIntensity->value());
    settings->setValue("peakQuantitation", peakQuantitation->currentIndex());
    settings->setValue("quantileIntensity", quantileIntensity->value());
    // Compound DB search
    settings->setValue("matchRtFlag", matchRt->isChecked());
    settings->setValue("compoundPPMWindow", compoundPPMWindow->value());
    settings->setValue("compoundRTWindow", compoundRTWindow->value());
    settings->setValue("eicMaxGroups", eicMaxGroups->value());
    // Automated Peak Detection
    settings->setValue("ppmMerge", ppmStep->value());
    settings->setValue("rtStepSize", rtStep->value());
    settings->setValue("minRT", rtMin->value());
    settings->setValue("maxRT", rtMax->value());
    settings->setValue("minMz", mzMin->value());
    settings->setValue("maxMz", mzMax->value());
    settings->setValue("minIntensity", minIntensity->value());
    settings->setValue("maxIntensity", maxIntensity->value());
    settings->setValue("minCharge", chargeMin->value());
    settings->setValue("maxCharge", chargeMax->value());

    // Isotope detection in peakdetection dialogue box
    settings->setValue("pullIsotopesFlag", reportIsotopesOptions->isChecked());
    // settings->setValue("checkBox", checkBox->isChecked());      // C13
    // settings->setValue("checkBox_2", checkBox_2->isChecked());  // N15
    // settings->setValue("checkBox_3", checkBox_3->isChecked());  // D2
    // settings->setValue("checkBox_4", checkBox_4->isChecked());  // S34

    // Fragment Score
    settings->setValue("minFragmentMatchScore", minFragMatchScore->value());
    settings->setValue("matchFragmentation",
                       matchFragmentatioOptions->isChecked());

    // Enabling feature detection or compound search
    settings->setValue("dbOptions", dbOptions->isChecked());
    settings->setValue("featureOptions",
            featureOptions->isChecked());
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
    MavenParameters* mavenParameters = mainwindow->mavenParameters;
    if (settings != NULL) {
        // Peak Scoring and Filtering
        mavenParameters->minQuality = settings->value("minQuality").toDouble();
        mavenParameters->quantileQuality = settings->value("quantileQuality").toDouble();
        mavenParameters->minGoodGroupCount = settings->value("minGoodGroupCount").toInt();
        mavenParameters->minNoNoiseObs = settings->value("minNoNoiseObs").toDouble();
        mavenParameters->minSignalBaseLineRatio = settings->value("minSignalBaseLineRatio").toDouble();
        mavenParameters->quantileSignalBaselineRatio = settings->value("quantileSignalBaselineRatio").toDouble();
        mavenParameters->minSignalBlankRatio = settings->value("minSignalBlankRatio").toDouble();
        mavenParameters->quantileSignalBlankRatio = settings->value("quantileSignalBlankRatio").toDouble();
        mavenParameters->minGroupIntensity = settings->value("minGroupIntensity").toDouble();
        mavenParameters->peakQuantitation = settings->value("peakQuantitation").toInt();
        mavenParameters->quantileIntensity = settings->value("quantileIntensity").toDouble();

        // Peak Group Rank
        mavenParameters->qualityWeight = settings->value("qualityWeight").toInt();
        mavenParameters->intensityWeight = settings->value("intensityWeight").toInt();
        mavenParameters->deltaRTWeight = settings->value("deltaRTWeight").toInt();
        mavenParameters->deltaRtCheckFlag = settings->value("deltaRtCheckFlag").toBool();

        // Compound DB search
        mavenParameters->matchRtFlag = settings->value("matchRtFlag").toBool();
        mavenParameters->compoundPPMWindow = settings->value("compoundPPMWindow").toDouble();
        mavenParameters->compoundRTWindow = settings->value("compoundRTWindow").toDouble();
        mavenParameters->eicMaxGroups = settings->value("eicMaxGroups").toInt();

        // Automated Peak Detection
        mavenParameters->ppmMerge = settings->value("ppmMerge").toDouble();
        mavenParameters->rtStepSize = settings->value("rtStepSize").toDouble();
        mavenParameters->minRt = settings->value("minRT").toDouble();
        mavenParameters->maxRt = settings->value("maxRT").toDouble();
        mavenParameters->minMz = settings->value("minMz").toDouble();
        mavenParameters->maxMz = settings->value("maxMz").toDouble();
        mavenParameters->minIntensity =
            settings->value("minIntensity").toDouble();
        mavenParameters->maxIntensity =
            settings->value("maxIntensity").toDouble();
        mavenParameters->minCharge = settings->value("minCharge").toDouble();
        mavenParameters->maxCharge = settings->value("maxCharge").toDouble();

        // Isotope detection in peakdetection dialogue box
        mavenParameters->pullIsotopesFlag = settings->value("pullIsotopesFlag").toBool();

        mavenParameters->maxIsotopeScanDiff = settings->value(
                "maxIsotopeScanDiff").toDouble();
        mavenParameters->minIsotopicCorrelation = settings->value(
                "minIsotopicCorrelation").toDouble();
        mavenParameters->maxNaturalAbundanceErr = settings->value(
                "maxNaturalAbundanceErr").toDouble();
        mavenParameters->noOfIsotopes = settings->value(
                "noOfIsotopes").toInt();

        mavenParameters->C13Labeled_BPE =
                settings->value("C13Labeled_BPE").toBool();
        mavenParameters->N15Labeled_BPE =
                settings->value("N15Labeled_BPE").toBool();
        mavenParameters->S34Labeled_BPE =
                settings->value("S34Labeled_BPE").toBool();
        mavenParameters->D2Labeled_BPE = 
                settings->value("D2Labeled_BPE").toBool();

        mavenParameters->C13Labeled_Barplot =
                settings->value("C13Labeled_Barplot").toBool();
        mavenParameters->N15Labeled_Barplot =
                settings->value("N15Labeled_Barplot").toBool();
        mavenParameters->S34Labeled_Barplot =
                settings->value("S34Labeled_Barplot").toBool();
        mavenParameters->D2Labeled_Barplot = 
                settings->value("D2Labeled_Barplot").toBool();

        mavenParameters->C13Labeled_IsoWidget =
                settings->value("C13Labeled_IsoWidget").toBool();
        mavenParameters->N15Labeled_IsoWidget =
                settings->value("N15Labeled_IsoWidget").toBool();
        mavenParameters->S34Labeled_IsoWidget =
                settings->value("S34Labeled_IsoWidget").toBool();
        mavenParameters->D2Labeled_IsoWidget = 
                settings->value("D2Labeled_IsoWidget").toBool();
        // Fragment Score
        mavenParameters->minFragmentMatchScore =
           settings->value("minFragmentMatchScore").toDouble();
        mavenParameters->minFragmentMatchScore > 0
           ? mavenParameters->matchFragmentation = true
           : mavenParameters->matchFragmentation = false;

        //Pointing the output directory
        if (!outputDirName->text().isEmpty()) {
            mavenParameters->setOutputDir(outputDirName->text());
            mavenParameters->writeCSVFlag = true;
        } else {
            mavenParameters->writeCSVFlag = false;
        }
        //Getting the classification model
        mavenParameters->clsf = mainwindow->getClassifier();

        //Setting the ionization mode if the user specifies the ionization mode
        //then its given the priority else the ionization mode is taken from the
        //sample
        //TODO: See how the ionization mode is effected if the user selects
        //Neutral or autodetect
        // if (mainwindow->getIonizationMode()) {
        //     mavenParameters->ionizationMode = mainwindow->getIonizationMode();
        // } else {
        //     mavenParameters->setIonizationMode();
        // }

        mavenParameters->setCompounds(DB.getCopoundsSubset(
            compoundDatabase->currentText().toStdString()));

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
    if (!settings->contains("eic_smoothingAlgorithm"))
        settings->setValue("eic_smoothingAlgorithm", 0);
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

/*
@author:Sahil-Kiran
*/
void PeakDetectionDialog::showMethodSummary() {
    LOGD;
    //Merged to 776
    if(peakupdater) {
        updateQSettingsWithUserInput(settings);
        setMavenParameters(settings);
        methodSummary->clear();
        methodSummary->setPlainText(peakupdater->printSettings());
    }
    mainwindow->settingsForm->setIsotopeAtom();
}


/*
@author:Sahil-Kiran
*/
void PeakDetectionDialog::updatePeakTableList() {
    LOGD;
    //merged to 776
    if(mainwindow) {
        QList< QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
        for(int i=0; i < peaksTableList.size();i++) {
            QString tableName = peaksTableList[i]->objectName();
            if (outputTableComboBox->findText(tableName) == -1 ) {
                outputTableComboBox->addItem(tableName);
            }
        }
    }
}

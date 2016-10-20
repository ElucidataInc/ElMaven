#include "peakdetectiondialog.h"

PeakDetectionDialog::PeakDetectionDialog(QWidget *parent) :
        QDialog(parent) {
        setupUi(this);
        settings = NULL;
        mainwindow = NULL;

        setModal(false);
        peakupdater = NULL;

        connect(computeButton, SIGNAL(clicked(bool)), SLOT(findPeaks()));
        connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(setOutputDirButton, SIGNAL(clicked(bool)), SLOT(setOutputDir()));
        connect(matchRt,SIGNAL(clicked(bool)),compoundRTWindow,SLOT(setEnabled(bool))); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(tabwidget,SIGNAL(currentChanged(int)),this,SLOT(showMethodSummary())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(tabwidget,SIGNAL(currentChanged(int)),this,SLOT(updatePeakTableList())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(saveMethodButton,SIGNAL(clicked()),this,SLOT(saveMethod())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(loadMethodButton,SIGNAL(clicked()),this,SLOT(loadMethod())); //TODO: Sahil - Kiran, Added while merging mainwindow
        connect(loadModelButton,SIGNAL(clicked()),this,SLOT(loadModel()));

        connect(dbOptions, SIGNAL(clicked(bool)), SLOT(dbOptionsClicked()));
        featureOptions->setChecked(false);
        connect(featureOptions, SIGNAL(clicked(bool)), SLOT(featureOptionsClicked()));

        compoundRTWindow->setEnabled(false); //TODO: Sahil - Kiran, Added while merging mainwindow
        reportIsotopesOptions->setEnabled(true); //TODO: Sahil - Kiran, Added while merging mainwindow
        //_featureDetectionType = CompoundDB; //TODO: Sahil - Kiran, removed while merging mainwindow

}

void PeakDetectionDialog::dbOptionsClicked() {
    if (dbOptions->isChecked()) {
        featureOptions->setChecked(false);
    } else {
        featureOptions->setChecked(true);
    }
}

void PeakDetectionDialog::featureOptionsClicked() {
    if (featureOptions->isChecked()) {
        dbOptions->setChecked(false);
    } else {
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
    if (peakupdater) {
        if (peakupdater->isRunning()) {
            peakupdater->stop();
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

    if (peakupdater == NULL) {
        peakupdater = new BackgroundPeakUpdate(this);
        if (mainwindow) peakupdater->setMainWindow(mainwindow);

        connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
               SLOT(setProgressBar(QString, int,int)));

    }

    // peakupdater->useMainWindowLabelOptions = false;
    inputInitialValuesPeakDetectionDialog();
    //settingsform->Updatevalue(settings);
}
/**
 * PeakDetectionDialog::loadModel This function works in the peakdector window
 * When theuser clicks on the Peak classifier Model file a dialog box appears
 * from which they can seect the model that is been trained
 */
void PeakDetectionDialog::loadModel() {
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
        cerr << settings->value("eic_smoothingAlgorithm").toInt();
        if (settings) {
            // EIC Processing: Baseline calculation and Smoothing
             eic_smoothingAlgorithm->setCurrentIndex(
                 settings->value("eic_smoothingAlgorithm").toInt());
            eic_smoothingWindow->setValue(
                settings->value("eic_smoothingWindow").toDouble());
            grouping_maxRtDiff->setValue(
                settings->value("grouping_maxRtWindow").toDouble());

            // BaseLine Calculation
            baseline_smoothing->setValue(
                settings->value("baseline_smoothing").toInt()); // --@Giridhari
            baseline_quantile->setValue(
                settings->value("baseline_quantile").toInt()); // --@Giridhari

            // Peak Scoring and Filtering
            minGoodGroupCount->setValue(
                settings->value("minGoodGroupCount").toInt());
            minNoNoiseObs->setValue(
                settings->value("minNoNoiseObs").toDouble());  // minPeakWidth
            sigBaselineRatio->setValue(
                settings->value("minSignalBaseLineRatio").toDouble());
            sigBlankRatio->setValue(
                settings->value("minSignalBlankRatio").toDouble());
            minGroupIntensity->setValue(
                settings->value("minGroupIntensity").toDouble());

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
            intensityMin->setValue(settings->value("minIntensity").toDouble());
            intensityMax->setValue(settings->value("maxIntensity").toDouble());
            chargeMin->setValue(settings->value("minCharge").toDouble());
            chargeMax->setValue(settings->value("maxCharge").toDouble());

            // Isotope detection in peakdetection dialogue box
            reportIsotopesOptions->setChecked(
                settings->value("pullIsotopesFlag").toBool());
            checkBox->setChecked(settings->value("checkBox").toBool());  // C13
            checkBox_2->setChecked(
                settings->value("checkBox_2").toBool());  // N15
            checkBox_3->setChecked(
                settings->value("checkBox_3").toBool());  // D2
            checkBox_4->setChecked(
                settings->value("checkBox_4").toBool());  // S34

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
        QDialog::show();
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
    if (mainwindow == NULL) return;
    if (peakupdater == NULL) return;
    if (peakupdater->isRunning()) cancel();
    if (peakupdater->isRunning()) return;

    //updateQSettingsWithUserInput(settings);
    setMavenParameters(settings);

    QString title;
    if (_featureDetectionType == FullSpectrum)
        title = "Detected Features";
    else if (_featureDetectionType == CompoundDB)
        title = "DB Search " + compoundDatabase->currentText();
    else if (_featureDetectionType == QQQ)
        title = "QQQ DB Search " + compoundDatabase->currentText();

    if (dbOptions->isChecked() && !(featureOptions->isChecked())) {
        _featureDetectionType = CompoundDB;

    } else if (!(dbOptions->isChecked()) && (featureOptions->isChecked())) {
        _featureDetectionType = FullSpectrum;
    } else if (!(dbOptions->isChecked()) && !(featureOptions->isChecked())) {
        _featureDetectionType = QQQ;
    } else {
        _featureDetectionType = FullSpectrum;
    }

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

    peaksTable->setWindowTitle(title);

    // disconnect prvevious connections
    disconnect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), 0, 0);
    disconnect(peakupdater, SIGNAL(finished()), 0, 0);
    disconnect(peakupdater, SIGNAL(terminated()), 0, 0);

    // connect new connections
    connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable,
            SLOT(addPeakGroup(PeakGroup*)));
    connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
    connect(peakupdater, SIGNAL(terminated()), peaksTable,
            SLOT(showAllGroups()));
    connect(peakupdater, SIGNAL(finished()), this, SLOT(close()));
    connect(peakupdater, SIGNAL(terminated()), this, SLOT(close()));
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

void PeakDetectionDialog::updateQSettingsWithUserInput(QSettings* settings) {
    // EIC Processing: Baseline calculation and Smoothing

     cerr << "Value: "<< eic_smoothingAlgorithm->currentIndex();
    cerr << "Update Function";
    settings->setValue("eic_smoothingAlgorithm",
                       eic_smoothingAlgorithm->currentIndex());

    settings->setValue("eic_smoothingWindow", eic_smoothingWindow->value());

    settings->setValue("grouping_maxRtWindow", grouping_maxRtDiff->value());

    // BaseLine Calculation
    settings->setValue("baseline_smoothingWindow", baseline_smoothing->value());
    settings->setValue("baseline_dropTopX", baseline_quantile->value());

    // Peak Scoring and Filtering
    settings->setValue("minGoodGroupCount", minGoodGroupCount->value());
    settings->setValue("minNoNoiseObs", minNoNoiseObs->value());
    settings->setValue("minSignalBaseLineRatio", sigBaselineRatio->value());
    settings->setValue("minSignalBlankRatio", sigBlankRatio->value());
    settings->setValue("minGroupIntensity", minGroupIntensity->value());
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
    settings->setValue("minIntensity", intensityMin->value());
    settings->setValue("maxIntensity", intensityMax->value());
    settings->setValue("minCharge", chargeMin->value());
    settings->setValue("maxCharge", chargeMax->value());

    // Isotope detection in peakdetection dialogue box
    settings->setValue("pullIsotopesFlag", reportIsotopesOptions->isChecked());
    settings->setValue("checkBox", checkBox->isChecked());      // C13
    settings->setValue("checkBox_2", checkBox_2->isChecked());  // N15
    settings->setValue("checkBox_3", checkBox_3->isChecked());  // D2
    settings->setValue("checkBox_4", checkBox_4->isChecked());  // S34

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
    settings->setValue("avgScanTime", 
            samples[0]->getAverageFullScanTime());
    // Time domain resolution(scans)
}
// @author: Giridhari TODO: Change the updation method of mavenparameters 
void PeakDetectionDialog::setMavenParameters(QSettings* settings) {
    if (peakupdater->isRunning()) return;
    MavenParameters* mavenParameters = mainwindow->mavenParameters;
    if (settings != NULL) {
        // EIC Processing: Baseline calculation and Smoothing
        // cerr << "Maven Parameters Function: ";
        // cerr << "a: "<< eic_smoothingAlgorithm->currentIndex();
        //  settings->setValue("eic_smoothingAlgorithm",
        //                 eic_smoothingAlgorithm->currentIndex());
        // settingsform->eic_smoothingAlgorithm->setCurrentIndex(
        //           settings->value("eic_smoothingAlgorithm").toInt());
        // cerr<< "b: " <<settings->value("eic_smoothingAlgorithm").toDouble();
        mavenParameters->eic_smoothingAlgorithm = eic_smoothingAlgorithm->currentIndex();
        mavenParameters->eic_smoothingWindow = eic_smoothingWindow->value();
        // cerr << "c: " << mavenParameters->eic_smoothingAlgorithm ;
        mavenParameters->grouping_maxRtWindow = grouping_maxRtDiff->value();
        // BaseLine Calculation
        mavenParameters->baseline_smoothingWindow = baseline_smoothing->value();
        mavenParameters->baseline_dropTopX = baseline_quantile->value();

        // Peak Scoring and Filtering
        mavenParameters->minGoodGroupCount = minGoodGroupCount->value();
        mavenParameters->minNoNoiseObs = minNoNoiseObs->value();
        mavenParameters->minSignalBaseLineRatio = sigBaselineRatio->value();
        mavenParameters->minSignalBlankRatio = sigBlankRatio->value();
        mavenParameters->minGroupIntensity = minGroupIntensity->value();

        // Compound DB search
        mavenParameters->matchRtFlag = matchRt->isChecked();
        mavenParameters->compoundPPMWindow = compoundPPMWindow->value();
        mavenParameters->compoundRTWindow = compoundRTWindow->value();
        mavenParameters->eicMaxGroups = eicMaxGroups->value();

        // Automated Peak Detection
        mavenParameters->ppmMerge = ppmStep->value();
        mavenParameters->rtStepSize = rtStep->value();
        mavenParameters->minRt = rtMin->value();
        mavenParameters->maxRt = rtMax->value();
        mavenParameters->minMz = mzMin->value();
        mavenParameters->maxMz = mzMax->value();
        mavenParameters->minIntensity = intensityMin->value();
        mavenParameters->maxIntensity = intensityMax->value();
        mavenParameters->minCharge = chargeMin->value();
        mavenParameters->maxCharge = chargeMax->value();

        // Isotope detection in peakdetection dialogue box
        mavenParameters->pullIsotopesFlag = reportIsotopesOptions->isChecked();

        // mavenParameters->maxIsotopeScanDiff = maxIsotopeScanDiff->value();
        // mavenParameters->minIsotopicCorrelation = minIsotopicCorrelation->value();
        // mavenParameters->maxNaturalAbundanceErr = maxNaturalAbundanceErr->value();
        mavenParameters->C13Labeled = checkBox->isChecked();  // C13
        mavenParameters->N15Labeled = checkBox_2->isChecked();                      // N15
        mavenParameters->D2Labeled = checkBox_3->isChecked();                      // D2
        mavenParameters->S34Labeled = checkBox_4->isChecked();  // S34
        // Fragment Score
        mavenParameters->minFragmentMatchScore =  minFragMatchScore->value();
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
        if (mainwindow->getIonizationMode()) {
            mavenParameters->ionizationMode = mainwindow->getIonizationMode();
        } else {
            mavenParameters->setIonizationMode();
        }

        mavenParameters->setCompounds(DB.getCopoundsSubset(
            compoundDatabase->currentText().toStdString()));
            vector<mzSample*> samples = mainwindow->getSamples();
        mavenParameters->avgScanTime = samples[0]->getAverageFullScanTime();
        
        mavenParameters->samples = mainwindow->getSamples();

        peakupdater->setMavenParameters(mavenParameters);
    //     // settings->setValue("eic_smoothingAlgorithm",
    //     //                eic_smoothingAlgorithm->currentIndex());
    //    // int val = eic_smoothingAlgorithm->currentIndex();
    //     //  settingsform->eic_smoothingAlgorithm->itemData(eic_smoothingAlgorithm->currentIndex());
    //     //settingsform->eic_smoothingWindow = NULL;
    //     //  settingsform->eic_smoothingWindow->setValue(eic_smoothingWindow->value());
    //    // settingsform->Updatevalue(mavenParameters);
    //     cerr << "SettingValue: "<< settings->value("eic_smoothingAlgorithm").toInt();
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
    if (!settings->contains("eic_smoothingAlgorithm"))
        settings->setValue("eic_smoothingAlgorithm", 0);
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
    //Merged to 776
    if(peakupdater) {
        setMavenParameters(settings);
      //  updateQSettingsWithUserInput(settings);
        setMavenParameters(settings);
        methodSummary->clear();
        methodSummary->setPlainText(peakupdater->printSettings());
    }
}


/*
@author:Sahil-Kiran
*/
void PeakDetectionDialog::updatePeakTableList() {
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

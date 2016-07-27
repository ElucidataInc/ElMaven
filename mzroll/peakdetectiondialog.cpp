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
        connect(loadModelButton, SIGNAL(clicked(bool)), SLOT(loadModel()));
        connect(setOutputDirButton, SIGNAL(clicked(bool)), SLOT(setOutputDir()));

        _featureDetectionType = CompoundDB;

}

PeakDetectionDialog::~PeakDetectionDialog() {
        cancel();
        if (peakupdater)
                delete (peakupdater);
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
/**
 * PeakDetectionDialog::setFeatureDetection This function is being called when
 * the database search button or the feature detection button is cliecked
 * from the main window
 * @param type this is an enum which can take the following values
 * FullSpectrum, CompoundDB, QQQ
 */
void PeakDetectionDialog::setFeatureDetection(FeatureDetectionType type) {
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
        adjustSize();
}

/**
 * PeakDetectionDialog::loadModel This function works in the peakdector window
 * When theuser clicks on the Peak classifier Model file a dialog box appears
 * from which they can seect the model that is been trained
 */
void PeakDetectionDialog::loadModel() {
        //This gives the name of the file that is selected by the user
        const QString name = QFileDialog::getOpenFileName(this,
                                                          "Select Classification Model", ".", tr("Model File (*.model)"));
        //This is applying the text to the window that the user has selected
        classificationModelFilename->setText(name);
        //Getting the classifier instance from the main window
        Classifier* clsf = mainwindow->getClassifier();

        //Loading the model to the to the model instance
        if (clsf) {
                clsf->loadModel(name.toStdString());
        }
}

/**
 * PeakDetectionDialog::setOutputDir this function sets the directory of the
 * file to which the output has to be connected
 */
void PeakDetectionDialog::setOutputDir() {
        //Getting the user selected folder name and path
        const QString dirName = QFileDialog::getExistingDirectory(this,
                                                                  "Save Reports to a Folder", ".", QFileDialog::ShowDirsOnly);
        // Showing it in the output directory box
        outputDirName->setText(dirName);
}

/**
 * PeakDetectionDialog::show This function is being called when the
 * database search button or the feature detection button is cliecked
 * from the main window
 */
void PeakDetectionDialog::show() {
        //TODO: Why only this two variables are updated in the windows that is
        //selected by the user
        if (mainwindow != NULL) {
                QSettings* settings = mainwindow->getSettings();
                if (settings) {
                        eic_smoothingWindow->setValue(
                                settings->value("eic_smoothingWindow").toDouble());
                        grouping_maxRtDiff->setValue(
                                settings->value("grouping_maxRtWindow").toDouble());
                }
        }

        /**
         * Getting the database present and updating in the dropdown of the
         * peak detection windows
         */
        map<string, int>::iterator itr;
        map<string, int> dbnames = DB.getDatabaseNames();

        //Clearing so that old value is not appended with the new values
        compoundDatabase->clear();
        for (itr = dbnames.begin(); itr != dbnames.end(); itr++) {
                string db = (*itr).first;
                if (!db.empty())
                        compoundDatabase->addItem(QString(db.c_str()));
        }

        //selecting the compound database that is selected by the user in the
        //ligand widget
        QString selectedDB = mainwindow->ligandWidget->getDatabaseName();
        compoundDatabase->setCurrentIndex(compoundDatabase->findText(selectedDB));

        //EIC extraction windows ppm value that is set in the main
        //window is been set to the GUI
        compoundPPMWindow->setValue(mainwindow->getUserPPM());
        QDialog::show();
}

void PeakDetectionDialog::findPeaks() {
        if (mainwindow == NULL)
                return;

        vector<mzSample*> samples = mainwindow->getSamples();
        if (samples.size() == 0)
                return;

        if (peakupdater != NULL) {
                if (peakupdater->isRunning())
                        cancel();
                if (peakupdater->isRunning())
                        return;
        }

        if (peakupdater != NULL) {
                delete (peakupdater);
                peakupdater = NULL;
        }

        peakupdater = new BackgroundPeakUpdate(this);
        peakupdater->setMainWindow(mainwindow);

        MavenParameters* mavenParameters = mainwindow->mavenParameters;

        connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
                SLOT(setProgressBar(QString, int,int)));

        if (settings != NULL) {
                mavenParameters->eic_ppmWindow =
                        settings->value("eic_ppmWindow").toDouble();
                mavenParameters->eic_smoothingAlgorithm = settings->value(
                        "eic_smoothingAlgorithm").toInt();
        }

        mavenParameters->baseline_smoothingWindow = baseline_smoothing->value();
        mavenParameters->baseline_dropTopX = baseline_quantile->value();
        mavenParameters->eic_smoothingWindow = eic_smoothingWindow->value();
        mavenParameters->grouping_maxRtWindow = grouping_maxRtDiff->value();
        mavenParameters->matchRtFlag = matchRt->isChecked();
        mavenParameters->minGoodPeakCount = minGoodGroupCount->value();
        mavenParameters->minNoNoiseObs = minNoNoiseObs->value();
        mavenParameters->minSignalBaseLineRatio = sigBaselineRatio->value();
        mavenParameters->minSignalBlankRatio = sigBlankRatio->value();
        mavenParameters->minGroupIntensity = minGroupIntensity->value();
        mavenParameters->pullIsotopesFlag = reportIsotopes->isChecked();
        mavenParameters->ppmMerge = ppmStep->value();
        mavenParameters->compoundPPMWindow = compoundPPMWindow->value(); //convert to half window units.
        mavenParameters->compoundRTWindow = compoundRTWindow->value();
        mavenParameters->eicMaxGroups = eicMaxGroups->value();
        mavenParameters->avgScanTime = samples[0]->getAverageFullScanTime();
        mavenParameters->rtStepSize = rtStep->value();

        if (!outputDirName->text().isEmpty()) {
                mavenParameters->setOutputDir(outputDirName->text());
                mavenParameters->writeCSVFlag = true;
        } else {
                mavenParameters->writeCSVFlag = false;
        }

        QString title;
        if (_featureDetectionType == FullSpectrum)
                title = "Detected Features";
        else if (_featureDetectionType == CompoundDB)
                title = "DB Search " + compoundDatabase->currentText();
        else if (_featureDetectionType == QQQ)
                title = "QQQ DB Search " + compoundDatabase->currentText();

        TableDockWidget* peaksTable = mainwindow->addPeaksTable(title);
        peaksTable->setWindowTitle(title);

        connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable,
                SLOT(addPeakGroup(PeakGroup*)));
        connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
        connect(peakupdater, SIGNAL(terminated()), peaksTable,
                SLOT(showAllGroups()));
        connect(peakupdater, SIGNAL(finished()), this, SLOT(close()));
        connect(peakupdater, SIGNAL(terminated()), this, SLOT(close()));


        peakupdater->setMavenParameters(mavenParameters);
        peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

        //RUN THREAD
        if (_featureDetectionType == QQQ) {

                peakupdater->setMavenParameters(mavenParameters);
                peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("findPeaksQQQ");
        } else if (_featureDetectionType == FullSpectrum) {

                peakupdater->setMavenParameters(mavenParameters);
                peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("processMassSlices");
        } else {
                mavenParameters->setCompounds(
                        DB.getCopoundsSubset(
                                compoundDatabase->currentText().toStdString()));

                peakupdater->setMavenParameters(mavenParameters);
                peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("computePeaks");
        }
}

void PeakDetectionDialog::runBackgroupJob(QString funcName) {
        if (peakupdater == NULL)
                return;

        if (peakupdater->isRunning()) {
                cancel();
        }

        if (!peakupdater->isRunning()) {
                peakupdater->setRunFunction(funcName); //set thread function
                peakupdater->start(); //start a background thread
        }
}

void PeakDetectionDialog::showInfo(QString text) {
        statusText->setText(text);
}

void PeakDetectionDialog::setProgressBar(QString text, int progress,
                                         int totalSteps) {
        showInfo(text);
        progressBar->setRange(0, totalSteps);
        progressBar->setValue(progress);
}

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

void PeakDetectionDialog::initPeakDetectionDialogWindow(FeatureDetectionType type){
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
void PeakDetectionDialog::displayAppropriatePeakDetectionDialog(FeatureDetectionType type) {
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
void PeakDetectionDialog::inputInitialValuesPeakDetectionDialog() {
        //TODO: Why only this two variables are updated in the windows that is
        //selected by the user
        if (mainwindow != NULL) {
                QSettings* settings = mainwindow->getSettings();
                if (settings) {
                        eic_smoothingWindow->setValue(settings->value("eic_smoothingWindow").toDouble());
                        grouping_maxRtDiff->setValue(settings->value("grouping_maxRtWindow").toDouble());
                        baseline_smoothing->setValue(settings->value("baseline_smoothingWindow").toInt());
                        baseline_quantile->setValue(settings->value("baseline_dropTopX").toInt());
                        matchRt->setChecked(settings->value("matchRtFlag").toBool());
                        minGoodGroupCount->setValue(settings->value("minGoodGroupCount").toInt());
                        minNoNoiseObs->setValue(settings->value("minNoNoiseObs").toDouble());
                        sigBaselineRatio->setValue(settings->value("minSignalBaseLineRatio").toDouble());
                        sigBlankRatio->setValue(settings->value("minSignalBlankRatio").toDouble());
                        minGroupIntensity->setValue(settings->value("minGroupIntensity").toDouble());
                        reportIsotopes->setChecked(settings->value("pullIsotopesFlag").toBool());
                        ppmStep->setValue(settings->value("ppmMerge").toDouble());
                        compoundPPMWindow->setValue(settings->value("compoundPPMWindow").toDouble());
                        compoundRTWindow->setValue(settings->value("compoundRTWindow").toDouble());
                        eicMaxGroups->setValue(settings->value("eicMaxGroups").toInt());
                        rtStep->setValue(settings->value("rtStepSize").toDouble());

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
void PeakDetectionDialog::findPeaks() {
        if (mainwindow == NULL)
                return;

        //Setting all the samples that are uploaded by the user and if number of
        //samples are zero terminating the peakdetection process
        vector<mzSample*> samples = mainwindow->getSamples();
        if (samples.size() == 0)
                return;


        //Making sure that peak detector is not running
        //If its running  allowing it to complete so that the new peakdetection
        //process dosent case any problem to the already running peakdetection
        //process
        if (peakupdater != NULL) {
                if (peakupdater->isRunning())
                        cancel();
                if (peakupdater->isRunning())
                        return;
        }

        //Making sure that new peakupdater thread that is made is set to NULL
        //before starting another peak detection process
        if (peakupdater != NULL) {
                delete (peakupdater);
                peakupdater = NULL;
        }

        //Instance of peakdetector thread
        peakupdater = new BackgroundPeakUpdate(this);
        peakupdater->setMainWindow(mainwindow);

        MavenParameters* mavenParameters = mainwindow->mavenParameters;

        //populating the maven setting insatnces with with the samples
        mavenParameters->samples = mainwindow->getSamples();

        connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
                SLOT(setProgressBar(QString, int,int)));

        //TODO: Why is only this two paremeter set her from the settings
        if (settings != NULL) {
                // This is only present in peakdetection process with DB
                mavenParameters->eic_ppmWindow =
                        settings->value("eic_ppmWindow").toDouble();
                //This comes from the "Option" window
                mavenParameters->eic_smoothingAlgorithm = settings->value(
                        "eic_smoothingAlgorithm").toInt();
        }

        //Baseline Smoothing in scans
        settings->setValue("baseline_smoothingWindow",baseline_smoothing->value());
        mavenParameters->baseline_smoothingWindow = settings->value("baseline_smoothingWindow").toInt();
        
        //Drop top x% intensities from chromatogram in percentage
        settings->setValue("baseline_dropTopX",baseline_quantile->value());
        mavenParameters->baseline_dropTopX = settings->value("baseline_dropTopX").toInt();
        //This is present in both peakdetection and options diolog box
        //This is EIC Smoothing in scans
        settings->setValue("eic_smoothingWindow",eic_smoothingWindow->value());
        mavenParameters->eic_smoothingWindow = settings->value("eic_smoothingWindow").toDouble();
        //Peak grouping (Max Group Rt Difference)
        settings->setValue("grouping_maxRtWindow",grouping_maxRtDiff->value());
        mavenParameters->grouping_maxRtWindow = settings->value("grouping_maxRtWindow").toDouble();
        //Match Retension Times this is used in Peakdetection with DB
        settings->setValue("matchRtFlag",matchRt->isChecked());
        mavenParameters->matchRtFlag = settings->value("matchRtFlag").toBool();
        //Min. Good Peaks/Groups in numbers
        settings->setValue("minGoodGroupCount",minGoodGroupCount->value());
        mavenParameters->minGoodGroupCount = settings->value("minGoodGroupCount").toInt();
        //Min. Peak Width
        settings->setValue("minNoNoiseObs",minNoNoiseObs->value());
        mavenParameters->minNoNoiseObs = settings->value("minNoNoiseObs").toDouble();
        //Min.Signal/ Baseline Ratio in numbers
        settings->setValue("minSignalBaseLineRatio",sigBaselineRatio->value());
        mavenParameters->minSignalBaseLineRatio = settings->value("minSignalBaseLineRatio").toDouble();
        //Min. Signal/ Blank Ratio
        settings->setValue("minSignalBlankRatio",sigBlankRatio->value());
        mavenParameters->minSignalBlankRatio = settings->value("minSignalBlankRatio").toDouble();
        //Min. Group Intensity
        cerr << minGroupIntensity->value() << endl;
        settings->setValue("minGroupIntensity",minGroupIntensity->value());
        mavenParameters->minGroupIntensity = settings->value("minGroupIntensity").toDouble();
        //Report Isotopic Peaks this is used in finding peaks with DB
        settings->setValue("pullIsotopesFlag",reportIsotopes->isChecked());
        mavenParameters->pullIsotopesFlag = settings->value("pullIsotopesFlag").toBool();
        //Mass domain Resolution (ppm)
        settings->setValue("ppmMerge",ppmStep->value());
        mavenParameters->ppmMerge = settings->value("ppmMerge").toDouble();
        //EIC Extraction window +/- PPM
        settings->setValue("compoundPPMWindow",compoundPPMWindow->value());
        mavenParameters->compoundPPMWindow = settings->value("compoundPPMWindow").toDouble(); //convert to half window units.
        //Compound Retention Time Matching Window
        settings->setValue("compoundRTWindow",compoundRTWindow->value());
        mavenParameters->compoundRTWindow = settings->value("compoundRTWindow").toDouble();
        //Limit the number of reported groups per compound
        settings->setValue("eicMaxGroups",eicMaxGroups->value());
        mavenParameters->eicMaxGroups = settings->value("eicMaxGroups").toInt();
        //TODO: what is this?
        settings->setValue("avgScanTime",samples[0]->getAverageFullScanTime());
        mavenParameters->avgScanTime = settings->value("avgScanTime").toDouble();
        //Time domain resolution(scans)
        settings->setValue("rtStepSize",rtStep->value());
        mavenParameters->rtStepSize = settings->value("rtStepSize").toDouble();


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

        // Changing the mainwindow title according to the peak detection type
        QString title;
        if (_featureDetectionType == FullSpectrum)
                title = "Detected Features";
        else if (_featureDetectionType == CompoundDB)
                title = "DB Search " + compoundDatabase->currentText();
        else if (_featureDetectionType == QQQ)
                title = "QQQ DB Search " + compoundDatabase->currentText();
        //Getting the peak table
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

        //Running the thread
        if (_featureDetectionType == QQQ) {
                //TODO: remove this line
                //peakupdater->setMavenParameters(mavenParameters);
                //peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("findPeaksQQQ");
        } else if (_featureDetectionType == FullSpectrum) {
                //TODO: remove this line
                //peakupdater->setMavenParameters(mavenParameters);
                //peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("processMassSlices");
        } else {
                mavenParameters->setCompounds(
                        DB.getCopoundsSubset(
                                compoundDatabase->currentText().toStdString()));
                //TODO: remove this line
                //peakupdater->setMavenParameters(mavenParameters);
                //peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

                runBackgroupJob("computePeaks");
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

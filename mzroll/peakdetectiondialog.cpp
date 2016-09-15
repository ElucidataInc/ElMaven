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

        compoundRTWindow->setEnabled(false); //TODO: Sahil - Kiran, Added while merging mainwindow
        reportIsotopesOptions->setEnabled(true); //TODO: Sahil - Kiran, Added while merging mainwindow
        //_featureDetectionType = CompoundDB; //TODO: Sahil - Kiran, removed while merging mainwindow

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

        tabwidget->setCurrentIndex(0); //TODO: Sahil - Kiran, Added while merging mainwindow
        adjustSize();
}

/*
@author: Sahil-Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::show() {
    //Thi is merged to 776    
    if(mainwindow == NULL) return;

    if ( peakupdater == NULL ) {
        peakupdater = new BackgroundPeakUpdate(this);
        if(mainwindow) peakupdater->setMainWindow(mainwindow);

        //connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)),
        //        SLOT(setProgressBar(QString, int,int)));

    }
    
    //peakupdater->useMainWindowLabelOptions = false;
    
    inputInitialValuesPeakDetectionDialog();
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
        if (clsf ) clsf->loadModel( classificationModelFilename->text().toStdString() );

}

/*
@author:Sahil-Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::loadMethod() {
    const QString filename = QFileDialog::getOpenFileName(this,
                "Load Settings",".",tr("Settings File (*.method)"));
     peakupdater->loadSettings(filename);
     inputInitialValuesPeakDetectionDialog();
}

/*
@author:Sahil-Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::saveMethod() {
    const QString filename = QFileDialog::getSaveFileName(this,
                "Save Settings", ".", tr("Settings File (*.method)"));
    peakupdater->saveSettings(filename);
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
                        reportIsotopesOptions->setChecked(settings->value("pullIsotopesFlag").toBool());
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

 //TODO: Sahil. Refactored this whole function. Merged with mainwindow of 776. RECHECK IT AGAIN. IMPORTANT
void PeakDetectionDialog::findPeaks() {

        
	if (mainwindow == NULL) return;
        if (peakupdater == NULL) return;
        if (peakupdater->isRunning() ) cancel();
        if (peakupdater->isRunning() ) return;


        //Setting all the samples that are uploaded by the user and if number of
        //samples are zero terminating the peakdetection process
        vector<mzSample*> samples = mainwindow->getSamples();
        if (samples.size() == 0)
                return;
	MavenParameters* mavenParameters = mainwindow->mavenParameters;

        //populating the maven setting insatnces with with the samples
        mavenParameters->samples = mainwindow->getSamples();							
        updateQSettingsWithUserInput(settings, samples);

        setMavenParameters(mavenParameters, settings);


	QString title;
	if (_featureDetectionType == FullSpectrum )  title = "Detected Features";
        else if (_featureDetectionType == CompoundDB ) title = "DB Search " + compoundDatabase->currentText();
        else if (_featureDetectionType == QQQ ) title = "QQQ DB Search " + compoundDatabase->currentText();

        if (dbOptions->isChecked() && !(featureOptions->isChecked())){
            _featureDetectionType = CompoundDB;
        
        } else if (!(dbOptions->isChecked()) && (featureOptions->isChecked())){
            _featureDetectionType = FullSpectrum;
        } else if (!(dbOptions->isChecked()) && !(featureOptions->isChecked())){
            _featureDetectionType = QQQ;
        } else{
            _featureDetectionType = FullSpectrum;
        }



        TableDockWidget* peaksTable = mainwindow->getBookmarkedPeaks();
        int peakTableIdx = outputTableComboBox->currentIndex();


        if ( peakTableIdx == 0 ) {
                peaksTable = mainwindow->addPeaksTable(title);
        } else if (peakTableIdx == 1) {
                peaksTable = mainwindow->getBookmarkedPeaks();
        } else if (peakTableIdx >= 2 ) {
                QList< QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
                if ( peaksTableList.size() >= 1 and peakTableIdx-2 < peaksTableList.size() ) {
                    peaksTable = peaksTableList[peakTableIdx-2];
                }
        }

		peaksTable->setWindowTitle(title);

        //disconnect prvevious connections
        disconnect(peakupdater,SIGNAL(newPeakGroup(PeakGroup*)),0,0);
        disconnect(peakupdater,SIGNAL(finished()),0,0);
        disconnect(peakupdater,SIGNAL(terminated()),0,0);

        //connect new connections
        connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable, SLOT(addPeakGroup(PeakGroup*)));
        connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
        connect(peakupdater, SIGNAL(terminated()), peaksTable, SLOT(showAllGroups()));
        connect(peakupdater, SIGNAL(finished()), this, SLOT(close()));
        connect(peakupdater, SIGNAL(terminated()), this, SLOT(close()));
        peakupdater->setPeakDetector(new PeakDetector(mavenParameters));

        //RUN THREAD
        if ( _featureDetectionType == QQQ ) {
                runBackgroupJob("findPeaksQQQ");
        } else if ( _featureDetectionType == FullSpectrum ) {
                runBackgroupJob("processMassSlices");
        }  else {
                mavenParameters->setCompounds( DB.getCopoundsSubset(compoundDatabase->currentText().toStdString()) );
        runBackgroupJob("computePeaks");
        }
}

void PeakDetectionDialog::updateQSettingsWithUserInput(QSettings *settings, vector<mzSample*> samples) {

        //Baseline Smoothing in scans
        settings->setValue("baseline_smoothingWindow",baseline_smoothing->value());
        //Drop top x% intensities from chromatogram in percentage
        settings->setValue("baseline_dropTopX",baseline_quantile->value());
        //This is present in both peakdetection and options diolog box
        //This is EIC Smoothing in scans
        settings->setValue("eic_smoothingWindow",eic_smoothingWindow->value());
        //Peak grouping (Max Group Rt Difference)
        settings->setValue("grouping_maxRtWindow",grouping_maxRtDiff->value());
        //Match Retension Times this is used in Peakdetection with DB
        settings->setValue("matchRtFlag",matchRt->isChecked());
        //Min. Good Peaks/Groups in numbers
        settings->setValue("minGoodGroupCount",minGoodGroupCount->value());
        //Min. Peak Width
        settings->setValue("minNoNoiseObs",minNoNoiseObs->value());
        //Min.Signal/ Baseline Ratio in numbers
        settings->setValue("minSignalBaseLineRatio",sigBaselineRatio->value());
        //Min. Signal/ Blank Ratio
        settings->setValue("minSignalBlankRatio",sigBlankRatio->value());
        //Min. Group Intensity
        settings->setValue("minGroupIntensity",minGroupIntensity->value());
        //Report Isotopic Peaks this is used in finding peaks with DB
        settings->setValue("pullIsotopesFlag",reportIsotopesOptions->isChecked());
        //Mass domain Resolution (ppm)
        settings->setValue("ppmMerge",ppmStep->value());
        //EIC Extraction window +/- PPM
        settings->setValue("compoundPPMWindow",compoundPPMWindow->value());
        //Compound Retention Time Matching Window
        settings->setValue("compoundRTWindow",compoundRTWindow->value());
        //Limit the number of reported groups per compound
        settings->setValue("eicMaxGroups",eicMaxGroups->value());
        //TODO: what is this?
        settings->setValue("avgScanTime",samples[0]->getAverageFullScanTime());
        //Time domain resolution(scans)
        settings->setValue("rtStepSize",rtStep->value());
}

void PeakDetectionDialog::setMavenParameters(MavenParameters* mavenParameters, QSettings *settings) {

    //TODO: Why is only this two paremeter set her from the settings
        if (settings != NULL) {
            // This is only present in peakdetection process with DB
            mavenParameters->eic_ppmWindow =
                    settings->value("eic_ppmWindow").toDouble();
            //This comes from the "Option" window
            mavenParameters->eic_smoothingAlgorithm = settings->value(
                    "eic_smoothingAlgorithm").toInt();

            //Baseline Smoothing in scans
            mavenParameters->baseline_smoothingWindow = settings->value("baseline_smoothingWindow").toInt();
            //Drop top x% intensities from chromatogram in percentage
            mavenParameters->baseline_dropTopX = settings->value("baseline_dropTopX").toInt();
            //This is present in both peakdetection and options diolog box
            //This is EIC Smoothing in scans
            mavenParameters->eic_smoothingWindow = settings->value("eic_smoothingWindow").toDouble();
            //Peak grouping (Max Group Rt Difference)
            mavenParameters->grouping_maxRtWindow = settings->value("grouping_maxRtWindow").toDouble();
            //Match Retension Times this is used in Peakdetection with DB
            mavenParameters->matchRtFlag = settings->value("matchRtFlag").toBool();
            //Min. Good Peaks/Groups in numbers
            mavenParameters->minGoodGroupCount = settings->value("minGoodGroupCount").toInt();
            //Min. Peak Width
            mavenParameters->minNoNoiseObs = settings->value("minNoNoiseObs").toDouble();
            //Min.Signal/ Baseline Ratio in numbers
            mavenParameters->minSignalBaseLineRatio = settings->value("minSignalBaseLineRatio").toDouble();
            //Min. Signal/ Blank Ratio
            mavenParameters->minSignalBlankRatio = settings->value("minSignalBlankRatio").toDouble();
            //Min. Group Intensity
            mavenParameters->minGroupIntensity = settings->value("minGroupIntensity").toDouble();
            //Report Isotopic Peaks this is used in finding peaks with DB
            mavenParameters->pullIsotopesFlag = settings->value("pullIsotopesFlag").toBool();
            //Mass domain Resolution (ppm)
            mavenParameters->ppmMerge = settings->value("ppmMerge").toDouble();
            //EIC Extraction window +/- PPM
            mavenParameters->compoundPPMWindow = settings->value("compoundPPMWindow").toDouble(); //convert to half window units.
            //Compound Retention Time Matching Window
            mavenParameters->compoundRTWindow = settings->value("compoundRTWindow").toDouble();
            //Limit the number of reported groups per compound
            mavenParameters->eicMaxGroups = settings->value("eicMaxGroups").toInt();
            //TODO: what is this?
            mavenParameters->avgScanTime = settings->value("avgScanTime").toDouble();
            //Time domain resolution(scans)
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

/*
@author:Sahil-Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::showMethodSummary() {
    cerr << "PeakDetectionDialog::showMethodSummary()" << endl;
    if(peakupdater) {

        vector<mzSample*> samples = mainwindow->getSamples();
        if (samples.size() == 0)
                return;
	MavenParameters* mavenParameters = mainwindow->mavenParameters;

        //populating the maven setting insatnces with with the samples
        mavenParameters->samples = mainwindow->getSamples();							
        updateQSettingsWithUserInput(settings, samples);
        setMavenParameters(mavenParameters, settings);

        cerr << "PeakDetectionDialog::showMethodSummary() Inside" << endl;
        methodSummary->clear();
        methodSummary->setPlainText(peakupdater->printSettings());
    }
}


/*
@author:Sahil-Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
void PeakDetectionDialog::updatePeakTableList() {
    cerr << "PeakDetectionDialog::updatePeakTableList()" << endl;
    if(mainwindow) {
        cerr << "PeakDetectionDialog::updatePeakTableList() inside" << endl;
        QList< QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
        for(int i=0; i < peaksTableList.size();i++) {
            QString tableName = peaksTableList[i]->objectName();
            if (outputTableComboBox->findText(tableName) == -1 ) {
                outputTableComboBox->addItem(tableName);
            }
        }
    }
}

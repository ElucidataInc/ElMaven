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

    _featureDetectionType= CompoundDB;

}

PeakDetectionDialog::~PeakDetectionDialog() { 
	cancel();
	if (peakupdater) delete(peakupdater);
}

void PeakDetectionDialog::cancel() { 

	if (peakupdater) {
		if( peakupdater->isRunning() ) { peakupdater->stop();  return; }
	}
	close(); 
}

void PeakDetectionDialog::setFeatureDetection(FeatureDetectionType type) {
    _featureDetectionType = type;

    if (_featureDetectionType == QQQ ) {
            dbOptions->hide();  
            featureOptions->hide();
    } else if (_featureDetectionType == FullSpectrum ) {
            dbOptions->hide();  
            featureOptions->show();
    } else if (_featureDetectionType == CompoundDB ) {
            dbOptions->show();  
            featureOptions->hide();
    }
	adjustSize();
}
void PeakDetectionDialog::loadModel() { 

    const QString name = QFileDialog::getOpenFileName(this,
				"Select Classification Model", ".",tr("Model File (*.model)"));

	classificationModelFilename->setText(name); 
	Classifier* clsf = mainwindow->getClassifier();	//get classification model
	if (clsf ) clsf->loadModel( classificationModelFilename->text().toStdString() );
}

void PeakDetectionDialog::setOutputDir() { 
    const QString dirName = QFileDialog::getExistingDirectory(this, 
					"Save Reports to a Folder", ".", QFileDialog::ShowDirsOnly);
	outputDirName->setText(dirName);
}

void PeakDetectionDialog::show() { 
    if ( mainwindow != NULL ) {
        QSettings* settings = mainwindow->getSettings();
        if ( settings ) {
            eic_smoothingWindow->setValue(settings->value("eic_smoothingWindow").toDouble());
            grouping_maxRtDiff->setValue(settings->value("grouping_maxRtWindow").toDouble());
        }
    }

    map<string,int>::iterator itr;
    map<string,int>dbnames = DB.getDatabaseNames();

    compoundDatabase->clear();
    for(itr=dbnames.begin(); itr != dbnames.end(); itr++ )  {
        string db = (*itr).first;
        if (!db.empty()) compoundDatabase->addItem(QString(db.c_str()));
    }


    QString selectedDB = mainwindow->ligandWidget->getDatabaseName();
    compoundDatabase->setCurrentIndex(compoundDatabase->findText(selectedDB));
    compoundPPMWindow->setValue( mainwindow->getUserPPM() );  //total ppm window, not half sized.

    QDialog::show();
}

void PeakDetectionDialog::findPeaks() {
		if (mainwindow == NULL) return;

		vector<mzSample*>samples = mainwindow->getSamples();
		if ( samples.size() == 0 ) return;

		if ( peakupdater !=  NULL )  {
			if ( peakupdater->isRunning() ) cancel();
			if ( peakupdater->isRunning() ) return;
		}

		if ( peakupdater != NULL ) {
				delete(peakupdater); 
				peakupdater=NULL; 
		}

		peakupdater = new BackgroundPeakUpdate(this);
		peakupdater->setMainWindow(mainwindow);

		connect(peakupdater, SIGNAL(updateProgressBar(QString,int,int)), 
						SLOT(setProgressBar(QString, int,int)));
	

	
		if (settings != NULL ) { 
             peakupdater->eic_ppmWindow = settings->value("eic_ppmWindow").toDouble();
             peakupdater->eic_smoothingAlgorithm = settings->value("eic_smoothingAlgorithm").toInt();
		}

        peakupdater->baseline_smoothingWindow = baseline_smoothing->value();
        peakupdater->baseline_dropTopX        = baseline_quantile->value();
		peakupdater->eic_smoothingWindow= eic_smoothingWindow->value();
		peakupdater->grouping_maxRtWindow = grouping_maxRtDiff->value();
		peakupdater->matchRtFlag =  matchRt->isChecked();
		peakupdater->minGoodPeakCount = minGoodGroupCount->value();
		peakupdater->minNoNoiseObs = minNoNoiseObs->value();
		peakupdater->minSignalBaseLineRatio = sigBaselineRatio->value();
		peakupdater->minSignalBlankRatio = sigBlankRatio->value();
		peakupdater->minGroupIntensity = minGroupIntensity->value();
        peakupdater->pullIsotopesFlag = reportIsotopes->isChecked();
        peakupdater->ppmMerge =  ppmStep->value();
        peakupdater->compoundPPMWindow = compoundPPMWindow->value();  //convert to half window units.
		peakupdater->compoundRTWindow = compoundRTWindow->value();
        peakupdater->eicMaxGroups = eicMaxGroups->value();
		peakupdater->avgScanTime = samples[0]->getAverageFullScanTime();
		peakupdater->rtStepSize = rtStep->value();

        if ( ! outputDirName->text().isEmpty()) {
            peakupdater->setOutputDir(outputDirName->text());
		    peakupdater->writeCSVFlag=true;
        } else {
		    peakupdater->writeCSVFlag=false;
		}

		QString title;
		if (_featureDetectionType == FullSpectrum )  title = "Detected Features";
                else if (_featureDetectionType == CompoundDB ) title = "DB Search " + compoundDatabase->currentText();
                else if (_featureDetectionType == QQQ ) title = "QQQ DB Search " + compoundDatabase->currentText();
     
		TableDockWidget* peaksTable = mainwindow->addPeaksTable(title);
		peaksTable->setWindowTitle(title);
   		connect(peakupdater, SIGNAL(newPeakGroup(PeakGroup*)), peaksTable, SLOT(addPeakGroup(PeakGroup*)));
   		connect(peakupdater, SIGNAL(finished()), peaksTable, SLOT(showAllGroups()));
   		connect(peakupdater, SIGNAL(terminated()), peaksTable, SLOT(showAllGroups()));
   		connect(peakupdater, SIGNAL(finished()), this, SLOT(close()));
   		connect(peakupdater, SIGNAL(terminated()), this, SLOT(close()));
		
		
		//RUN THREAD
		if ( _featureDetectionType == QQQ ) {
			runBackgroupJob("findPeaksQQQ");
		} else if ( _featureDetectionType == FullSpectrum ) {
			runBackgroupJob("processMassSlices");
		}  else {
 			peakupdater->setCompounds( DB.getCopoundsSubset(compoundDatabase->currentText().toStdString()) );
			runBackgroupJob("computePeaks");
		}
}

void PeakDetectionDialog::runBackgroupJob(QString funcName) { 
	if (peakupdater == NULL ) return;

	if ( peakupdater->isRunning() ) { 
			cancel(); 
	}

	if ( ! peakupdater->isRunning() ) { 
		peakupdater->setRunFunction(funcName);			//set thread function
		peakupdater->start();	//start a background thread
	}
}

void PeakDetectionDialog::showInfo(QString text){ statusText->setText(text); }

void PeakDetectionDialog::setProgressBar(QString text, int progress, int totalSteps){
	showInfo(text);
	progressBar->setRange(0,totalSteps);
    progressBar->setValue(progress);
}


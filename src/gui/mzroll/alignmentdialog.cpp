#include "alignmentdialog.h"

AlignmentDialog::AlignmentDialog(QWidget *parent) : QDialog(parent) { 
		setupUi(this); 
		setModal(false);

		workerThread = NULL;
		workerThread = new BackgroundPeakUpdate(this);

		if (peakDetectionAlgo->currentIndex() == 0) {
			selectDatabase->setVisible(true);
		}

		if (alignAlgo->currentIndex() == 0) {
			label_7->setVisible(true);
			label_8->setVisible(true);
		}
		connect(alignAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
		connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
		connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
		connect(alignWrtExpectedRt,SIGNAL(clicked(bool)),SLOT(setAlignWrtExpectedRt(bool)));
		connect(local, SIGNAL(clicked(bool)),this, SLOT(setInitPenalty(bool)));
		connect(restoreDefaultObiWarpParams, SIGNAL(clicked(bool)), this, SLOT(restorDefaultValues(bool)));
		connect(showAdvanceParams, SIGNAL(clicked(bool)), this, SLOT(showAdvanceParameters(bool)));
		connect(alignButton, SIGNAL(clicked()), SLOT(align()));
		// @# not sure about error [ is currently not there on devlop ]
		// connect(UndoAlignment, SIGNAL(clicked()), SLOT(UndoAlignment()));

		QRect rec = QApplication::desktop()->screenGeometry();
		int height = rec.height();
		setFixedHeight(height-height/10);
}

AlignmentDialog::~AlignmentDialog() {
	if (workerThread) delete (workerThread);
}
void AlignmentDialog::setAlignWrtExpectedRt(bool checked){
	_mw->mavenParameters->alignWrtExpectedRt=checked;
}
void AlignmentDialog::setInitPenalty(bool checked){
	initPenalty->setVisible(checked);
	labelInitPenalty->setVisible(checked);
}
void AlignmentDialog::cancel() {
    if (workerThread) {
        if (workerThread->isRunning()) {
            workerThread->completeStop();
            return;
        }
    }
    close();
}

void AlignmentDialog::setMainWindow(MainWindow* mw) {
    _mw=mw;

}

void AlignmentDialog::show() {

	_mw->getAnalytics()->hitScreenView("AlignmentDialog");
    inputInitialValuesAlignmentDialog();
	intialSetup();
	QDialog::exec();

}

void AlignmentDialog::inputInitialValuesAlignmentDialog() {

	minGoodPeakCount->setValue(_mw->mavenParameters->minGoodGroupCount);
	groupingWindow->setValue(_mw->mavenParameters->rtStepSize);

	minGroupIntensity->setValue(_mw->mavenParameters->minGroupIntensity);
	minSN->setValue(_mw->mavenParameters->minSignalBaseLineRatio);
	minPeakWidth->setValue(_mw->mavenParameters->minNoNoiseObs);
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);

}

void AlignmentDialog::setProgressBar(QString text, int progress,
                                         int totalSteps) {
        showInfo(text);
        progressBar->setRange(0, totalSteps);
        progressBar->setValue(progress);
}

void AlignmentDialog::showInfo(QString text) {
        statusText->setText(text);
}

void AlignmentDialog::intialSetup() {
	setProgressBar("Status", 0, 1);
	setDatabase();
	setDatabase(_mw->ligandWidget->getDatabaseName());
	algoChanged();
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);
}

void AlignmentDialog::restorDefaultValues(bool checked){

	scoreObi->setCurrentText("cor");
	factorGap->setValue(1);
	factorDiag->setValue(2);
	gapExtend->setValue(3.4);
	gapInit->setValue(0.2);
	binSizeObiWarp->setValue(0.6);
	responseObiWarp->setValue(20);
	noStdNormal->setChecked(false);
	local->setChecked(false);
	initPenalty->setValue(0);
	restoreDefaultObiWarpParams->setChecked(false);
	initPenalty->setVisible(false);
	labelInitPenalty->setVisible(false);

}

void AlignmentDialog::showAdvanceParameters(bool checked){
	toggleObiParams(checked);
	
	groupBox->setVisible(0);
	groupBox_2->setVisible(0);
}

void AlignmentDialog::toggleObiParams(bool show){

	restoreDefaultObiWarpParams->setVisible(show);
	responseObiWarp->setVisible(show);
	binSizeObiWarp->setVisible(show);
	gapInit->setVisible(show);
	gapExtend->setVisible(show);
	factorDiag->setVisible(show);
	factorGap->setVisible(show);
	initPenalty->setVisible(show);
	noStdNormal->setVisible(show);
	local->setVisible(show);
	scoreObi->setVisible(show);

	labelRestoreDefaultObiWarpParams->setVisible(show);
	labelResponseObiWarp->setVisible(show);
	labelBinSizeObiWarp->setVisible(show);
	labelGapInit->setVisible(show);
	labelGapExtend->setVisible(show);
	labelFactorDiag->setVisible(show);
	labelFactorGap->setVisible(show);
	labelInitPenalty->setVisible(show);
	labelNoStdNormal->setVisible(show);
	labelLocal->setVisible(show);
	labelScoreObi->setVisible(show);

	if(show)
		setInitPenalty(local->isChecked());
}
void AlignmentDialog::algoChanged() {

	bool obiWarp = (alignAlgo->currentIndex() == 2);
	toggleObiParams(obiWarp);
	showAdvanceParameters(showAdvanceParams->isChecked() && obiWarp);
	showAdvanceParams->setVisible(obiWarp);
	labelShowAdvanceParams->setVisible(obiWarp);

	if (peakDetectionAlgo->currentIndex() == 0) {
		selectDatabase->setVisible(true);
		selectDatabaseComboBox->setVisible(true);
		label_10->setVisible(false);
		label_11->setVisible(false);
		minIntensity->setVisible(false);
		maxIntensity->setVisible(false);

	} else {
		selectDatabase->setVisible(false);
		selectDatabaseComboBox->setVisible(false);
		label_10->setVisible(true);
		label_11->setVisible(true);
		minIntensity->setVisible(true);
		maxIntensity->setVisible(true);
	}

	if (alignAlgo->currentIndex() == 0) {
		label_7->setVisible(true);
		maxItterations->setVisible(true);
		label_8->setVisible(true);
		polynomialDegree->setVisible(true);
	} else {
		label_7->setVisible(false);
		maxItterations->setVisible(false);
		label_8->setVisible(false);
		polynomialDegree->setVisible(false);
	}

	groupBox->setVisible(!obiWarp);
	groupBox_2->setVisible(!obiWarp);
}

void AlignmentDialog::setDatabase() {

	selectDatabaseComboBox->disconnect(SIGNAL(currentIndexChanged(QString)));
	selectDatabaseComboBox->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
		if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
			set.insert( DB.compoundsDB[i]->db.c_str() );
	}

	QIcon icon(rsrcPath + "/dbsearch.png");
	QSetIterator<QString> i(set);
	int pos=0;
	while (i.hasNext()) { 
		selectDatabaseComboBox->addItem(icon,i.next());
	}
}


void AlignmentDialog::setDatabase(QString db) {

	selectDatabaseComboBox->setCurrentIndex(selectDatabaseComboBox->findText(db));
	
}



//  <---  updated alignment dialog begind here  --->

void AlignmentDialog::UndoAlignment() {
	if(alignAlgo->currentIndex() == 2){
		for (int i = 0; i < _mw->samples.size(); i++) {
			for(int j = 0; j < _mw->samples[i]->scans.size(); ++j)
				if(_mw->samples[i]->scans[j]->originalRt >= 0)
					_mw->samples[i]->scans[j]->rt = _mw->samples[i]->scans[j]->originalRt;
		}

		_mw->eicWidget->replotForced();
		_mw->alignmentDialog->close();
		return;
	}

	for (int i = 0; i < _mw->samples.size(); i++) {
		if (_mw->samples[i])
			_mw->samples[i]->restoreOriginalRetentionTimes();
	}
	_mw->getEicWidget()->replotForced();

	_mw->mavenParameters->alignButton = 0;

	QList<PeakGroup> listGroups;
	for (unsigned int i = 0; i<_mw->mavenParameters->undoAlignmentGroups.size(); i++) {
			listGroups.append(_mw->mavenParameters->undoAlignmentGroups.at(i));
	}

	// not sure about the argument
	// Q_EMIT(AlignmentDialog::UndoAlignment(listGroups));

}


void AlignmentDialog::updateParameters() {
	
	_mw->mavenParameters->minGoodGroupCount =
			_mw->alignmentDialog->minGoodPeakCount->value();
	_mw->mavenParameters->limitGroupCount =
			_mw->alignmentDialog->limitGroupCount->value();
	_mw->mavenParameters->minGroupIntensity =
			_mw->alignmentDialog->minGroupIntensity->value();
	_mw->mavenParameters->minIntensity =
			_mw->alignmentDialog->minIntensity->value();
	_mw->mavenParameters->maxIntensity =
			_mw->alignmentDialog->maxIntensity->value();

	//TODO: Sahil Re-verify this two parameters. Values are same
	// _mw->mavenParameters->eic_smoothingWindow =
	// 		_mw->alignmentDialog->groupingWindow->value(); //TODO: Sahil-Kiran, Added while merging mainwindow
	_mw->mavenParameters->rtStepSize =
			_mw->alignmentDialog->groupingWindow->value(); //TODO: Sahil-Kiran, Added while merging mainwindow

	_mw->mavenParameters->minSignalBaseLineRatio = _mw->alignmentDialog->minSN->value();
	_mw->mavenParameters->minNoNoiseObs = _mw->alignmentDialog->minPeakWidth->value();


    _mw->mavenParameters->minSignalBlankRatio = 0; //TODO: Sahil-Kiran, Added while merging mainwindow
    _mw->mavenParameters->alignMaxItterations = _mw->alignmentDialog->maxItterations->value(); //TODO: Sahil-Kiran, Added while merging mainwindow
    _mw->mavenParameters->alignPolynomialDegree = _mw->alignmentDialog->polynomialDegree->value(); //TODO: Sahil-Kiran, Added while merging mainwindow

    _mw->mavenParameters->checkConvergance=false; //TODO: Sahil-Kiran, Added while merging mainwindow
	_mw->mavenParameters->alignSamplesFlag = true;
	_mw->mavenParameters->keepFoundGroups = true;
    _mw->mavenParameters->eicMaxGroups = _mw->peakDetectionDialog->eicMaxGroups->value();

	_mw->mavenParameters->samples = _mw->getSamples();
	_mw->mavenParameters->stop = false;

	
    //connect new connections
    // connect(workerThread, SIGNAL(terminated()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow

	// workerThread->start();
}


/* @# dont know what to name the func */
/* @# this call occurs in the middle of process slices, but not sure if it is okay to call after */
void AlignmentDialog::func() {
	checkCornerCases();
	if (_mw->mavenParameters->alignSamplesFlag && !_mw->mavenParameters->stop) {
		Q_EMIT(updateProgressBar("Aligning Samples", 0, 0));
	}
}

void AlignmentDialog::processSlices(vector<mzSlice*>&slices, string setName, PeakDetector peakDetector) {
        
		
        
		peakDetector.processSlices(slices, setName);
       
        if (_mw->mavenParameters->showProgressFlag && _mw->mavenParameters->pullIsotopesFlag) {
			Q_EMIT(updateProgressBar("Calculation Isotopes", 1, 100));
        }
        writeCSVRep(setName, peakDetector);
}

void AlignmentDialog::writeCSVRep(string setName, PeakDetector peakDetector) {

        //write reports
        CSVReports* csvreports = NULL;
        if (_mw->mavenParameters->writeCSVFlag) {
                //Added to pass into csvreports file when merged with Maven776 - Kiran
                bool includeSetNamesLine=true;
                string groupfilename = _mw->mavenParameters->outputdir + setName + ".csv";
                csvreports = new CSVReports(_mw->mavenParameters->samples);
                csvreports->setMavenParameters(_mw->mavenParameters);
                csvreports->setUserQuantType(_mw->getUserQuantType());
                //Added to pass into csvreports file when merged with Maven776 - Kiran
                csvreports->openGroupReport(groupfilename,includeSetNamesLine);
        }

        peakDetector.pullAllIsotopes();

        for (int j = 0; j < _mw->mavenParameters->allgroups.size(); j++) {
			PeakGroup& group = _mw->mavenParameters->allgroups[j];

			if (csvreports != NULL) csvreports->addGroup(&group);


			if (_mw->mavenParameters->keepFoundGroups) {

				Q_EMIT(newPeakGroup(&(_mw->mavenParameters->allgroups[j])));
				QCoreApplication::processEvents();
			}
        }

        if (csvreports != NULL) {
                csvreports->closeFiles();
                delete (csvreports);
                csvreports = NULL;
        }
        Q_EMIT(updateProgressBar("Done", 1, 1));
}


void AlignmentDialog::checkCornerCases() {
	if (_mw->mavenParameters->alignSamplesFlag && _mw->mavenParameters->alignButton > 0) {
			;
	} else if (_mw->mavenParameters->alignSamplesFlag && _mw->mavenParameters->alignButton ==0){
			_mw->mavenParameters->alignButton++;
			_mw->mavenParameters->undoAlignmentGroups = _mw->mavenParameters->allgroups;
	} else if (_mw->mavenParameters->alignSamplesFlag && _mw->mavenParameters->alignButton == -1) {
			;
	} else {
			_mw->mavenParameters->alignButton = -1;
			_mw->mavenParameters->undoAlignmentGroups = _mw->mavenParameters->allgroups;
	}
}

void AlignmentDialog::connectStatements() {
	// connect(workerThread, SIGNAL(finished()), eicWidget, SLOT(replotForced()));
	// connect(workerThread, SIGNAL(finished()), alignmentDialog, SLOT(close()));

	// @# worker thread..what to do??
	// connect(workerThread, SIGNAL(newPeakGroup(PeakGroup*)), bookmarkedPeaks, SLOT(addPeakGroup(PeakGroup*))); //TODO: Sahil-Kiran, Added while merging mainwindow
	// connect(workerThread, SIGNAL(finished()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow
	// connect(this,SIGNAL(alignmentError(QString)),mainwindow,SLOT(showAlignmetErrorDialog(QString)));
	
	if (_mw->mavenParameters->alignSamplesFlag) {
        connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), _mw, SLOT(showAlignmentWidget()));
    }

	qRegisterMetaType<QList<PeakGroup> >("QList<PeakGroup>");
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), _mw, SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), _mw->alignmentVizWidget, SLOT(setCurrentGroups(QList<PeakGroup>)));
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), _mw->alignmentPolyVizDockWidget, SLOT(plotGraph()));
        
}

void AlignmentDialog::align() {
	if (_mw->samples.size() < 2) {
		return;
	}

	int algoToPerform = alignAlgo->currentIndex();
	vector<PeakGroup*> groups(_mw->mavenParameters->allgroups.size());
	for (int i = 0; i < _mw->mavenParameters->allgroups.size(); i++)
		groups[i] = &_mw->mavenParameters->allgroups[i];

	if (algoToPerform == 0) {
		// start polyfit
		if (peakDetectionAlgo->currentText() == "Compound Database Search") {
			// workerThread = newWorkerThread("alignUsingDatabase");

			_mw->mavenParameters->setCompounds(DB.getCopoundsSubset(selectDatabaseComboBox->currentText().toStdString()));
			updateParameters();

			connectStatements();

			

			PeakDetector peakDetector;
			vector<mzSlice*> slices = peakDetector.processCompounds(_mw->mavenParameters->compounds, "compounds");
        
			processSlices(slices, "compounds", peakDetector);

			func();
			
			PolyFit *polyFit = new PolyFit(groups,_mw->getSamples());
			if (_mw->mavenParameters->alignSamplesFlag && !_mw->mavenParameters->stop) {
				polyFit->polyFitAlgo();
			}

			_mw->deltaRt = polyFit->getDeltaRt();
			_mw->mavenParameters->alignSamplesFlag = false;
			
			QList<PeakGroup> listGroups;
			for (unsigned int i = 0; i<_mw->mavenParameters->allgroups.size(); i++) {
					listGroups.append(_mw->mavenParameters->allgroups.at(i));
			}
       		// Q_EMIT(alignmentComplete(listGroups));

		}
		else {
			updateParameters();

			connectStatements();

			PeakDetector peakDetector;

			Q_EMIT (updateProgressBar("Computing Mass Slices", 0, 0));

			// @# commented for now
        	// _mw->mavenParameters->sig.connect(boost::bind(&BackgroundPeakUpdate::qtSignalSlot, this, _1, _2, _3));
        	peakDetector.processMassSlices();

			func();

			if (_mw->mavenParameters->showProgressFlag && _mw->mavenParameters->pullIsotopesFlag) {
					Q_EMIT(updateProgressBar("Calculation Isotopes", 1, 100));
			}

        	writeCSVRep("allslices",peakDetector);

			vector<PeakGroup*> groups(_mw->mavenParameters->allgroups.size());
			for (int i = 0; i < _mw->mavenParameters->allgroups.size(); i++)
				groups[i] = &_mw->mavenParameters->allgroups[i];
			
			PolyFit *polyFit = new PolyFit(groups,_mw->getSamples());
			if (_mw->mavenParameters->alignSamplesFlag && !_mw->mavenParameters->stop) {
				polyFit->polyFitAlgo();

			}

			_mw->deltaRt = polyFit->getDeltaRt();
			_mw->mavenParameters->alignSamplesFlag = false;
			
			QList<PeakGroup> listGroups;
			for (unsigned int i = 0; i<_mw->mavenParameters->allgroups.size(); i++) {
					listGroups.append(_mw->mavenParameters->allgroups.at(i));
			}

       		// Q_EMIT(alignmentComplete(listGroups));
		}


	}
	else if (algoToPerform == 1) {
		// start loessfit

		if (peakDetectionAlgo->currentText() == "Compound Database Search") {
			_mw->mavenParameters->setCompounds(DB.getCopoundsSubset(selectDatabaseComboBox->currentText().toStdString()));
			updateParameters();

			connectStatements();

			PeakDetector peakDetector;
			vector<mzSlice*> slices = peakDetector.processCompounds(_mw->mavenParameters->compounds, "compounds");
        
			processSlices(slices, "compounds", peakDetector);

			func();

			LoessFit *loessFit = new LoessFit(groups, _mw->mavenParameters->alignWrtExpectedRt, _mw->getSamples());
			if (_mw->mavenParameters->alignSamplesFlag && !_mw->mavenParameters->stop) {
				loessFit->loessFit();
			}

			_mw->deltaRt = loessFit->getDeltaRt();
			_mw->mavenParameters->alignSamplesFlag = false;
			
			QList<PeakGroup> listGroups;
			for (unsigned int i = 0; i<_mw->mavenParameters->allgroups.size(); i++) {
					listGroups.append(_mw->mavenParameters->allgroups.at(i));
			}

       		// Q_EMIT(alignmentComplete(listGroups));

		}
		else {
			updateParameters();

			connectStatements();

			PeakDetector peakDetector;

			Q_EMIT (updateProgressBar("Computing Mass Slices", 0, 0));

			// @# comment for now
        	// _mw->mavenParameters->sig.connect(boost::bind(&BackgroundPeakUpdate::qtSignalSlot, this, _1, _2, _3));
        	peakDetector.processMassSlices();

			func();

			if (_mw->mavenParameters->showProgressFlag
				&& _mw->mavenParameters->pullIsotopesFlag) {
					Q_EMIT(updateProgressBar("Calculation Isotopes", 1, 100));
			}

        	writeCSVRep("allslices", peakDetector);

			LoessFit *loessFit = new LoessFit(groups, _mw->mavenParameters->alignWrtExpectedRt, _mw->getSamples());

			if (_mw->mavenParameters->alignSamplesFlag && !_mw->mavenParameters->stop) {
				loessFit->loessFit();

			}

			_mw->deltaRt = loessFit->getDeltaRt();
			_mw->mavenParameters->alignSamplesFlag = false;
			
			QList<PeakGroup> listGroups;
			for (unsigned int i = 0; i<_mw->mavenParameters->allgroups.size(); i++) {
					listGroups.append(_mw->mavenParameters->allgroups.at(i));
			}

       		// Q_EMIT(alignmentComplete(listGroups));
		}

	}
	else if (algoToPerform == 2) {
		// start obiwarp

		
		ObiParams *obiParams = new ObiParams(
			scoreObi->currentText().toStdString(),
            local->isChecked(),
            factorDiag->value(),
            factorGap->value(),
            gapInit->value(),
            gapExtend->value(),
            initPenalty->value(),
            responseObiWarp->value(),
            noStdNormal->isChecked(),
            binSizeObiWarp->value());

		Q_EMIT(updateProgressBar("Aligning Samples", 0, 0));

		ObiWarpAlign *obiWarpAlign = new ObiWarpAlign(obiParams, _mw->getSamples());

		/** threaded **/

		obiWarpAlign->obiWarpAlign();

		// QThread *thread = QThread::create([] { obiWarp.obiWarp(); });
		// thread->start();

		/** change in ui **/
		_mw->alignmentPolyVizDockWidget->plotGraph();



	}
	else {
		// throw error
	}
}
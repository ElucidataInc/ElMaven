#include "background_peaks_update.h"

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
	PeakDetector* pd = new PeakDetector();
	setPeakDetector(pd);

	mainwindow = NULL;
	_stopped = true;
	//	setTerminationEnabled(false);
	runFunction = "computeKnowsPeaks";
}

BackgroundPeakUpdate::~BackgroundPeakUpdate() {
	peakDetector.cleanup();
}

void BackgroundPeakUpdate::run(void) {

	if (mainwindow == NULL) {
		quit();
		return;
	}
	_stopped = false;

	if (peakDetector.samples.size() == 0)
		peakDetector.samples = mainwindow->getSamples(); //get samples
	peakDetector.clsf = mainwindow->getClassifier(); //get classification modej

	if (peakDetector.samples.size() > 0
			&& peakDetector.samples[0]->getPolarity() > 0)
		peakDetector.ionizationMode = +1;
	else
		peakDetector.ionizationMode = -1; //set ionization mode for compound matching
	if (mainwindow->getIonizationMode())
		peakDetector.ionizationMode = mainwindow->getIonizationMode(); //user specified ionization mode

	if (runFunction == "findPeaksQQQ") {
		findPeaksQQQ();
	} else if (runFunction == "processSlices") {
		processSlices();
	} else if (runFunction == "processMassSlices") {
		processMassSlices();
	} else if (runFunction == "pullIsotopes") {
		pullIsotopes(peakDetector._group);
	} else if (runFunction == "computePeaks") {
		computePeaks();
	} else {
		qDebug() << "Unknown Function " << runFunction.c_str();
	}

	quit();
	return;
}

void BackgroundPeakUpdate::writeCSVRep(string setName) {
	//write reports
	CSVReports* csvreports = NULL;
	if (peakDetector.writeCSVFlag) {
		string groupfilename = peakDetector.outputdir + setName + ".csv";
		csvreports = new CSVReports(peakDetector.samples);
		csvreports->setUserQuantType(mainwindow->getUserQuantType());
		csvreports->openGroupReport(groupfilename);
	}

	for (int j = 0; j < peakDetector.allgroups.size(); j++) {
		PeakGroup& group = peakDetector.allgroups[j];
		Compound* compound = group.compound;

		if (peakDetector.pullIsotopesFlag && !group.isIsotope())
			pullIsotopes(&group);
		if (csvreports != NULL)
			csvreports->addGroup(&group);

		if (compound) {
			if (!compound->hasGroup()
					|| group.groupRank < compound->getPeakGroup()->groupRank)
				compound->setPeakGroup(group);
		}

		if (peakDetector.keepFoundGroups) {
			emit(newPeakGroup(&peakDetector.allgroups[j]));
			QCoreApplication::processEvents();
		}

		if (peakDetector.showProgressFlag && peakDetector.pullIsotopesFlag
				&& j % 10 == 0) {
			emit(updateProgressBar("Calculating Isotopes", j,
					peakDetector.allgroups.size()));
		}
	}

	if (csvreports != NULL) {
		csvreports->closeFiles();
		delete (csvreports);
		csvreports = NULL;
	}
	emit(updateProgressBar("Done", 1, 1));
}

void BackgroundPeakUpdate::getPullIsotopeSettings() {
	if (mainwindow) {
		QSettings* settings = mainwindow->getSettings();
		if (settings) {
			peakDetector.maxIsotopeScanDiff = settings->value(
					"maxIsotopeScanDiff").toDouble();
			peakDetector.minIsotopicCorrelation = settings->value(
					"minIsotopicCorrelation").toDouble();
			peakDetector.maxNaturalAbundanceErr = settings->value(
					"maxNaturalAbundanceErr").toDouble();
			peakDetector.C13Labeled = settings->value("C13Labeled").toBool();
			peakDetector.N15Labeled = settings->value("N15Labeled").toBool();
			peakDetector.S34Labeled = settings->value("S34Labeled").toBool();
			peakDetector.D2Labeled = settings->value("D2Labeled").toBool();
			QSettings* settings = mainwindow->getSettings();
			peakDetector.eic_smoothingAlgorithm = settings->value(
					"eic_smoothingAlgorithm").toInt();

			//Feng note: assign labeling state to sample
			peakDetector.samples[0]->_C13Labeled = peakDetector.C13Labeled;
			peakDetector.samples[0]->_N15Labeled = peakDetector.N15Labeled;
			peakDetector.samples[0]->_S34Labeled = peakDetector.S34Labeled;
			peakDetector.samples[0]->_D2Labeled = peakDetector.D2Labeled;
			//End Feng addition
		}
	}
}

void BackgroundPeakUpdate::processSlices() {
	processSlices();
}

void BackgroundPeakUpdate::processSlice(mzSlice& slice) {
	vector<mzSlice*> slices;
	slices.push_back(&slice);
	processSlices(slices, "sliceset");
}

void BackgroundPeakUpdate::getProcessSlicesSettings() {
	QSettings* settings = mainwindow->getSettings();
	peakDetector.amuQ1 = settings->value("amuQ1").toDouble();
	peakDetector.amuQ3 = settings->value("amuQ3").toDouble();
	peakDetector.baseline_smoothingWindow = settings->value(
			"baseline_smoothing").toInt();
	peakDetector.baseline_dropTopX =
			settings->value("baseline_quantile").toInt();

}

void BackgroundPeakUpdate::align() {

	if (peakDetector.alignSamplesFlag) {
		//		emit(updateProgressBar("Aligning Samples", 1, 100));
		vector<PeakGroup*> groups(peakDetector.allgroups.size());
		for (int i = 0; i < peakDetector.allgroups.size(); i++)
			groups[i] = &peakDetector.allgroups[i];
		Aligner aligner;
		aligner.setMaxItterations(
				mainwindow->alignmentDialog->maxItterations->value());
		aligner.setPolymialDegree(
				mainwindow->alignmentDialog->polynomialDegree->value());
		aligner.doAlignment(groups);
	}
}

void BackgroundPeakUpdate::processSlices(vector<mzSlice*>&slices,
		string setName) {
	getProcessSlicesSettings();
	peakDetector.processSlices(slices, setName);
	align();
	writeCSVRep(setName);
}

void BackgroundPeakUpdate::processCompounds(vector<Compound*> set,
		string setName) {
	vector<mzSlice*> slices = peakDetector.processCompounds(set, setName);
	processSlices(slices, setName);
	delete_all(slices);
}

void BackgroundPeakUpdate::processMassSlices() {
	peakDetector.processMassSlices();
}

void BackgroundPeakUpdate::computePeaks() {
	if (peakDetector.compounds.size() == 0) {
		return;
	}
	processCompounds(peakDetector.compounds, "compounds");
}

void BackgroundPeakUpdate::findPeaksQQQ() {
	if (mainwindow == NULL)
		return;
	vector<mzSlice*> slices = mainwindow->getSrmSlices();
	processSlices(slices, "QQQ Peaks");
	delete_all(slices);
	slices.clear();
}

void BackgroundPeakUpdate::setRunFunction(QString functionName) {
	runFunction = functionName.toStdString();
}

void BackgroundPeakUpdate::pullIsotopes(PeakGroup* parentgroup) {
	getPullIsotopeSettings();
	peakDetector.pullIsotopes(parentgroup);
}

/*
 bool BackgroundPeakUpdate::covertToMzXML(QString filename, QString outfile) {

 QFile test(outfile); if(test.exists()) return true;

 QString command = QString("ReAdW.exe --centroid --mzXML \"%1\" \"%2\"").
 arg(filename).
 arg(outfile);

 qDebug() << command;

 QProcess *process = new QProcess();
 //connect(process, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));
 process->start(command);

 if (!process->waitForStarted()){
 process->kill();
 return false;
 }

 while (!process->waitForFinished()) {};
 QFile testOut(outfile); return testOut.exists();
 }*/


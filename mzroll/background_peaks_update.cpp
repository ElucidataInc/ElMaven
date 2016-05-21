#include "background_peaks_update.h"

#include <qdebug.h>
#include <vector>

#include "../libmaven/mzSample.h"
#include "mainwindow.h"

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
	mainwindow = NULL;
	PeakDetector* pd = new PeakDetector(mainwindow);
	setPeakDetector(pd);

	_stopped = true;
//  setTerminationEnabled(false);
}

void BackgroundPeakUpdate::run(void) {

	if (mainwindow == NULL) {
		quit();
		return;
	}
	_stopped = false;

	vector<mzSample*> samples = peakDetector.getSamples();
	if (samples.size() == 0)
		samples = mainwindow->getSamples(); //get samples
	peakDetector.setClassifier(mainwindow->getClassifier()); //get classification modej

	if (samples.size() > 0 && samples[0]->getPolarity() > 0)
		peakDetector.ionizationMode = +1;
	else
		peakDetector.ionizationMode = -1; //set ionization mode for compound matching
	if (mainwindow->getIonizationMode())
		peakDetector.ionizationMode = mainwindow->getIonizationMode(); //user specified ionization mode

	if (runFunction == "findPeaksQQQ") {
		peakDetector.findPeaksQQQ();
	} else if (runFunction == "processSlices") {
		peakDetector.processSlices();
	} else if (runFunction == "processMassSlices") {
		peakDetector.processMassSlices();
	} else if (runFunction == "computeKnowsPeaks") {
		peakDetector.computeKnowsPeaks();
	} else if (runFunction == "pullIsotopes") {
		peakDetector.pullIsotopes(peakDetector.getPeakGroup());
	} else if (runFunction == "computePeaks") {
		peakDetector.computePeaks();
	} else {
		qDebug() << "Unknown Function " << runFunction.c_str();
	}

	quit();
	return;
}

void BackgroundPeakUpdate::setRunFunction(QString functionName) {
	runFunction = functionName.toStdString();
}

#include "PeakDetector.h"

#include <qdebug.h>
#include <qfile.h>
#include <qprocess.h>
#include <climits>
#include <iostream>

#include "mzUtils.h"

PeakDetector::PeakDetector() {
	clsf = NULL;    //initially classifier is not loaded
	runFunction = "computeKnowsPeaks";
	writePdfReportFlag = false;
	alignSamplesFlag = false;
	processKeggFlag = false;
	processMassSlicesFlag = false;
	pullIsotopesFlag = false;
	matchRtFlag = false;
	checkConvergance = false;

	outputdir = "reports" + string(DIR_SEPARATOR_STR);

	writeCSVFlag = false;
	ionizationMode = -1;
	keepFoundGroups = true;
	showProgressFlag = true;

	mzBinStep = 0.01;
	rtStepSize = 20;
	ppmMerge = 30;
	qcut = 0.05;
	avgScanTime = 0.2;

	limitGroupCount = INT_MAX;

	//peak detection
	eic_smoothingWindow = 10;
	eic_smoothingAlgorithm = 0;

	//peak grouping across samples
	grouping_maxRtWindow = 0.5;

	//peak filtering criteria
	minGoodPeakCount = 1;
	minSignalBlankRatio = 2;
	minNoNoiseObs = 1;
	minSignalBaseLineRatio = 2;
	minGroupIntensity = 500;

	//compound detection setting
	compoundPPMWindow = 10;
	compoundRTWindow = 2;
	eicMaxGroups = INT_MAX;
}


vector<EIC*> PeakDetector::pullEICs(mzSlice* slice,
		std::vector<mzSample*>&samples, int peakDetect, int smoothingWindow,
		int smoothingAlgorithm, float amuQ1, float amuQ3) {
	vector<EIC*> eics;
	vector<mzSample*> vsamples;

	for (unsigned int i = 0; i < samples.size(); i++) {
		if (samples[i] == NULL)
			continue;
		if (samples[i]->isSelected == false)
			continue;
		vsamples.push_back(samples[i]);
	}

	if (vsamples.size()) {
		/*EicLoader::PeakDetectionFlag pdetect = (EicLoader::PeakDetectionFlag) peakDetect;
		 QFuture<EIC*>future = QtConcurrent::mapped(vsamples, EicLoader(slice, pdetect,smoothingWindow, smoothingAlgorithm, amuQ1,amuQ3));

		 //wait for async operations to finish
		 future.waitForFinished();

		 QFutureIterator<EIC*> itr(future);

		 while(itr.hasNext()) {
		 EIC* eic = itr.next();
		 if ( eic && eic->size() > 0) eics.push_back(eic);
		 }
		 */

		/*
		 QList<EIC*> _eics = result.results();
		 for(int i=0; i < _eics.size(); i++ )  {
		 if ( _eics[i] && _eics[i]->size() > 0) {
		 eics.push_back(_eics[i]);
		 }
		 }*/
	}

	// single threaded version

	for (unsigned int i = 0; i < vsamples.size(); i++) {
		mzSample* sample = vsamples[i];
		Compound* c = slice->compound;

		EIC* e = NULL;

		if (!slice->srmId.empty()) {
			//cout << "computeEIC srm:" << slice->srmId << endl;
			e = sample->getEIC(slice->srmId);
		} else if (c && c->precursorMz > 0 && c->productMz > 0) {
			//cout << "computeEIC qqq: " << c->precursorMz << "->" << c->productMz << endl;
			e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz,
					amuQ1, amuQ3);
		} else {
			//cout << "computeEIC mzrange" << setprecision(7) << slice->mzmin  << " " << slice->mzmax << slice->rtmin  << " " << slice->rtmax << endl;
			e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin,
					slice->rtmax, 1);
		}

		if (e) {
			EIC::SmootherType smootherType =
					(EIC::SmootherType) smoothingAlgorithm;
			e->setSmootherType(smootherType);
			e->getPeakPositions(smoothingWindow);
			eics.push_back(e);
		}
	}
	return eics;
}

void PeakDetector::printSettings() {
	cerr << "#Output folder=" << outputdir << endl;
	cerr << "#ionizationMode=" << ionizationMode << endl;
	cerr << "#keepFoundGroups=" << keepFoundGroups << endl;
	cerr << "#showProgressFlag=" << showProgressFlag << endl;

	cerr << "#rtStepSize=" << rtStepSize << endl;
	cerr << "#ppmMerge=" << ppmMerge << endl;
	cerr << "#qcut=" << qcut << endl;
	cerr << "#avgScanTime=" << avgScanTime << endl;

	//peak detection
	cerr << "#eic_smoothingWindow=" << eic_smoothingWindow << endl;

	//peak grouping across samples
	cerr << "#grouping_maxRtWindow=" << grouping_maxRtWindow << endl;

	//peak filtering criteria
	cerr << "#minGoodPeakCount=" << minGoodPeakCount << endl;
	cerr << "#minSignalBlankRatio=" << minSignalBlankRatio << endl;
	cerr << "#minNoNoiseObs=" << minNoNoiseObs << endl;
	cerr << "#minSignalBaseLineRatio=" << minSignalBaseLineRatio << endl;
	cerr << "#minGroupIntensity=" << minGroupIntensity << endl;

	//compound detection setting
	cerr << "#compoundPPMWindow=" << compoundPPMWindow << endl;
	cerr << "#compoundRTWindow=" << compoundRTWindow << endl;
}

bool PeakDetector::covertToMzXML(QString filename, QString outfile) {

	QFile test(outfile);
	if (test.exists())
		return true;

	QString command = QString("ReAdW.exe --centroid --mzXML \"%1\" \"%2\"").arg(
			filename).arg(outfile);

	qDebug() << command;

	QProcess *process = new QProcess();
	//connect(process, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));
	process->start(command);

	if (!process->waitForStarted()) {
		process->kill();
		return false;
	}

	while (!process->waitForFinished()) {
	};
	QFile testOut(outfile);
	return testOut.exists();
}

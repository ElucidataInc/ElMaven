#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H

#include <qstring.h>
#include <string>
#include <vector>

#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"
#include <qdatetime.h>
#include <qdebug.h>
#include <algorithm>
#include <deque>
#include <iostream>
#include <map>

#include "../mzroll/classifier.h"
#include "mzMassSlicer.h"

class Classifier;

class PeakDetector {
public:
	PeakDetector();
	void setOutputDir(QString outdir) {
		outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
	}
	void setMaxGroupCount(int x) {
		limitGroupCount = x;
	}
	void setCompounds(vector<Compound*> set) {
		compounds = set;
	}
	void setSlices(vector<mzSlice*> set) {
		_slices = set;
	}
	void setPeakGroup(PeakGroup* p) {
		_group = p;
	}
	void setSamples(vector<mzSample*>&set);

	bool writeCSVFlag;
	bool alignSamplesFlag;
	bool keepFoundGroups;
	bool processMassSlicesFlag;
	bool pullIsotopesFlag;
	bool showProgressFlag;
	bool matchRtFlag;
	bool checkConvergance;

	int ionizationMode; //default ionization mode used by mass spec

	//mass slicing parameters
	float mzBinStep;
	float rtStepSize;
	float avgScanTime;
	float ppmMerge;

	//peak detection
	float eic_smoothingWindow; //smoothing window
	int eic_smoothingAlgorithm;
	float eic_ppmWindow;
	int baseline_smoothingWindow;
	int baseline_dropTopX;

	//peak filtering
	int minGoodPeakCount;
	float minSignalBlankRatio;
	float minNoNoiseObs;
	float minSignalBaseLineRatio;
	float minGroupIntensity;

	//eic window around compound
	float compoundPPMWindow;
	float compoundRTWindow;
	int eicMaxGroups;

	//grouping of peaks across samples
	float grouping_maxRtWindow;	//do no group peaks that are greater than differ more than X in retention time

	//stop looking for groups if group count is greater than X
	int limitGroupCount;

	//triple quad compound matching
	float amuQ1;
	float amuQ3;

	double maxIsotopeScanDiff;
	double maxNaturalAbundanceErr;
	double minIsotopicCorrelation;
	bool C13Labeled;
	bool N15Labeled;
	bool S34Labeled;
	bool D2Labeled;
	string outputdir;
	vector<PeakGroup> allgroups;
	MassCalculator mcalc;
	Classifier* clsf;
	PeakGroup* _group;
	vector<mzSample*> samples;
	vector<Compound*> compounds;
	vector<mzSlice*> _slices;

	void processSlices(void);
	void processSlice(mzSlice& slice);
	void processMassSlices();
	void pullIsotopes(PeakGroup *group);
	void processSlices(vector<mzSlice*>&slices, string setName);
	vector<mzSlice*> processCompounds(vector<Compound*> set, string setName);
	void cleanup();

	//CLASS FUNCTIONS
	static vector<EIC*> pullEICs(mzSlice* slice, std::vector<mzSample*>&samples,
			int peakDetect, int smoothingWindow, int smoothingAlgorithm,
			float amuQ1, float amuQ3, int baselineSmoothingWindow,
			int baselineDropTopX);

private:
	bool addPeakGroup(PeakGroup& group);
	void printSettings();

};

struct EicLoader {
	enum PeakDetectionFlag {
		NoPeakDetection = 0, PeakDetection = 1
	};

	EicLoader(mzSlice* islice, PeakDetectionFlag iflag = NoPeakDetection,
			int smoothingWindow = 5, int smoothingAlgorithm = 0, float amuQ1 =
					0.1, float amuQ2 = 0.5, int baselineSmoothingWindow = 5,
			int baselineDropTopX = 40) {

		slice = islice;
		pdetect = iflag;
		eic_smoothingWindow = smoothingWindow;
		eic_smoothingAlgorithm = smoothingAlgorithm;
		eic_amuQ1 = amuQ1;
		eic_amuQ2 = amuQ2;
		eic_baselne_dropTopX = baselineDropTopX;
		eic_baselne_smoothingWindow = baselineSmoothingWindow;
	}

	typedef EIC* result_type;

	EIC* operator()(mzSample* sample) {
		EIC* e = NULL;
		Compound* c = slice->compound;

		if (!slice->srmId.empty()) {
			//cout << "computeEIC srm:" << slice->srmId << endl;
			e = sample->getEIC(slice->srmId);
		} else if (c && c->precursorMz > 0 && c->productMz > 0) {
			//cout << "computeEIC qqq: " << c->precursorMz << "->" << c->productMz << endl;
			e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz,
					eic_amuQ1, eic_amuQ2);
		} else {
			//cout << "computeEIC mzrange" << setprecision(7) << slice->mzmin  << " " << slice->mzmax << slice->rtmin  << " " << slice->rtmax << endl;
			e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin,
					slice->rtmax, 1);
		}

		if (e) {
			e->setBaselineSmoothingWindow(eic_baselne_smoothingWindow);
			e->setBaselineDropTopX(eic_baselne_dropTopX);
			e->setSmootherType((EIC::SmootherType) (eic_smoothingAlgorithm));
		}

		if (pdetect == PeakDetection && e) {
			e->getPeakPositions(eic_smoothingWindow);
		}
		return e;
	}

	mzSlice* slice;
	PeakDetectionFlag pdetect;
	int eic_smoothingWindow;
	int eic_smoothingAlgorithm;
	float eic_amuQ1;
	float eic_amuQ2;
	int eic_baselne_smoothingWindow;
	int eic_baselne_dropTopX;
};

#endif // PEAKDETECTOR_H

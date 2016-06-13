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

/**
 * @class PeakDetector
 * @ingroup libmaven
 * @brief all peak detection logic resides here.
 * @author Elucidata
 */
class PeakDetector {
public:
	PeakDetector();

	/**
	 * [setOutputDir description]
	 * @method setOutputDir
	 * @param  outdir       [description]
	 */
	void setOutputDir(QString outdir) {
		outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
	}

	/**
	 * [setMaxGroupCount description]
	 * @method setMaxGroupCount
	 * @param  x                [description]
	 */
	void setMaxGroupCount(int x) {
		limitGroupCount = x;
	}

	/**
	 * [setCompounds description]
	 * @method setCompounds
	 * @param  set          [description]
	 */
	void setCompounds(vector<Compound*> set) {
		compounds = set;
	}

	/**
	 * [setSlices description]
	 * @method setSlices
	 * @param  set       [description]
	 */
	void setSlices(vector<mzSlice*> set) {
		_slices = set;
	}

	/**
	 * [setPeakGroup description]
	 * @method setPeakGroup
	 * @param  p            [description]
	 */
	void setPeakGroup(PeakGroup* p) {
		_group = p;
	}

	/**
	 * [setAverageScanTime description]
	 * @method setAverageScanTime
	 */
	void setAverageScanTime();

	/**
	 * [setIonizationMode description]
	 * @method setIonizationMode
	 */
	void setIonizationMode();

	/**
	 * [setSamples description]
	 * @method setSamples
	 * @param  set        [description]
	 */
	void setSamples(vector<mzSample*>&set);

	bool writeCSVFlag;
	bool alignSamplesFlag;
	bool keepFoundGroups;
	bool processMassSlicesFlag;
	bool pullIsotopesFlag;
	bool showProgressFlag;
	bool matchRtFlag;
	bool checkConvergance;

	/**
	 * default ionization mode used by mass spec
	 */
	int ionizationMode;

	//mass slicing parameters
	float mzBinStep;
	float rtStepSize;
	float avgScanTime;
	float ppmMerge;

	//peak detection

	/**
	 * smoothing window
	 */
	float eic_smoothingWindow;
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

	/**
	 * eic window around compound, compound detection setting
	 */
	float compoundPPMWindow;
	float compoundRTWindow;
	int eicMaxGroups;


	/**
	 * grouping of peaks across samples
	 * do no group peaks that are greater than differ more than X in retention time
	 */
	float grouping_maxRtWindow;

	/**
	 * stop looking for groups if group count is greater than X
	 */
	int limitGroupCount;


	/**
	 * triple quad compound matching Q1
	 */
	float amuQ1;

	/**
	 * triple quad compound matching Q3
	 */
	float amuQ3;

	float minQuality;
	string ligandDbFilename;

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

	/**
	 * [alignSamples description]
	 * @method alignSamples
	 */
	void alignSamples();

	/**
	 * [processSlices description]
	 * @method processSlices
	 */
	void processSlices(void);

	/**
	 * [processSlice description]
	 * @method processSlice
	 * @param  slice        [description]
	 */
	void processSlice(mzSlice& slice);

	/**
	 * [processMassSlices description]
	 * @method processMassSlices
	 */
	void processMassSlices();

	/**
	 * [pullIsotopes description]
	 * @method pullIsotopes
	 * @param  group        [description]
	 */
	void pullIsotopes(PeakGroup *group);

	/**
	 * [processSlices description]
	 * @method processSlices
	 * @param  slices        [description]
	 * @param  setName       [description]
	 */
	void processSlices(vector<mzSlice*>&slices, string setName);

	/**
	 *
	 */
	vector<mzSlice*> processCompounds(vector<Compound*> set, string setName);

	/**
	 * [cleanup description]
	 * @method cleanup
	 */
	void cleanup();

	/**
	 *
	 */
	static vector<EIC*> pullEICs(mzSlice* slice, std::vector<mzSample*>&samples,
			int peakDetect, int smoothingWindow, int smoothingAlgorithm,
			float amuQ1, float amuQ3, int baselineSmoothingWindow,
			int baselineDropTopX);

private:
	/**
	 * [addPeakGroup description]
	 * @method addPeakGroup
	 * @param  group        [description]
	 * @return [description]
	 */
	bool addPeakGroup(PeakGroup& group);

	/**
	 * [printSettings description]
	 * @method printSettings
	 */
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

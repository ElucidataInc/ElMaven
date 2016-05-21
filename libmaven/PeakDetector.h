#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H

#include <qstring.h>
#include <string>
#include <vector>

#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"

class Classifier;
class MainWindow;

class PeakDetector {
public:
	PeakDetector();
	PeakDetector(MainWindow* mainwindow);
	~PeakDetector();

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
	void setMainWindow(MainWindow* mw) {
		mainWindow = mw;
	}
	void setOutputDir(QString outdir) {
		outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
	}
	void setMaxGroupCount(int x) {
		limitGroupCount = x;
	}

	vector<mzSample*> getSamples() {
		return samples;
	}

	void setClassifier(Classifier* cl) {
		clsf = cl;
	}


	PeakGroup* getPeakGroup() {
		return _group;
	}
	void pullIsotopes(PeakGroup *group);
	void processMassSlices();
	void computeKnowsPeaks(void);
	void computePeaks();
	void processSlices(void);
	void processSlices(vector<mzSlice*>&slices, string setName);
	void processSlice(mzSlice& slice);

	/** Processes Mass Slices using two algorithms A and B.
	 */
	void findPeaksOrbi(void);
	void findPeaksQQQ(void);

	bool writePdfReportFlag;
	bool writeCSVFlag;
	bool alignSamplesFlag;
	bool keepFoundGroups;
	bool processKeggFlag;
	bool processMassSlicesFlag;
	bool pullIsotopesFlag;
	bool showProgressFlag;
	bool matchRtFlag;
	bool checkConvergance;

	/** default ionization mode used by mass spec
	 */
	int ionizationMode;

	/** mass slicing parameters
	 */
	float mzBinStep;
	float rtStepSize;
	float avgScanTime;
	float ppmMerge;
	float qcut;

	//peak detection

	/** smoothing window
	 */
	float eic_smoothingWindow;
	int eic_smoothingAlgorithm;
	float eic_ppmWindow;
	int baseline_smoothing;
	int baseline_quantile;

	/** merging of ajacent peaks within eic
	 */
	bool eic_mergeOverlapingPeaks;
	float eic_mergePPMCutoff;
	float eic_mergeMaxHeightDrop;

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

	/** do no group peaks that are greater than differ more than X in retention time
	 */
	float grouping_maxRtWindow;
	float grouping_maxMzWindow;

	/** stop looking for groups if group count is greater than X
	 */
	int limitGroupCount;
	bool addPeakGroup(PeakGroup& group);

	//CLASS FUNCTIONS
	static vector<EIC*> pullEICs(mzSlice* slice, std::vector<mzSample*>&samples,
			int peakDetect, int smoothingWindow, int smoothingAlgorithm,
			float amuQ1, float amuQ3);

private:
	MainWindow * mainWindow;
	MassCalculator mcalc;
	Classifier* clsf;
	PeakGroup* _group;
	vector<mzSample*> samples;
	vector<Compound*> compounds;
	vector<mzSlice*> _slices;
	vector<PeakGroup> allgroups;
	string outputdir;

	void printSettings();
	bool covertToMzXML(QString filename, QString outfile);
	void processCompounds(vector<Compound*> set, string setName);

	void cleanup();
};

#endif // PEAKDETECTOR_H

#ifndef MAVENPARAMETERS_H
#define MAVENPARAMETERS_H

#include <qstring.h>
#include <string>
#include <vector>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "Compound.h"

class Classifier;

class MavenParameters {
public:
	MavenParameters();
	boost::signals2::signal< void (const string&,unsigned int , int ) > sig;

	/**
	 * [set Output Directory]
	 * @method setOutputDir
	 * @param  outdir       [name of the output directory]
	 */
	void setOutputDir(QString outdir) {
		outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
	}

	/**
	 * [set Maximum Group Count]
	 * @method setMaxGroupCount
	 * @param  x                [Maximum Group Count]
	 */
	void setMaxGroupCount(int x) {
		limitGroupCount = x;
	}
	
	int getCharge(Compound* compound = NULL);

	/**
	 * [set Compounds]
	 * @method setCompounds
	 * @param  set          [vector of pointer to Compound]
	 */
	void setCompounds(vector<Compound*> set) {
		compounds = set;
	}

	/**
	 * [set mass Slices]
	 * @method setSlices
	 * @param  set       [vector of pointer to mzSlice]
	 */
	void setSlices(vector<mzSlice*> set) {
		_slices = set;
	}

	/**
	 * [set Peak Group]
	 * @method setPeakGroup
	 * @param  p            [pointer to Peak Group]
	 */
	void setPeakGroup(PeakGroup* p) {
		_group = p;
	}

	/**
	 * [set Average Scan Time]
	 * @method setAverageScanTime
	 */
	void setAverageScanTime();

	/**
	 * [set Ionization Mode]
	 * @method setIonizationMode
	 */
	void setIonizationMode();

	/**
	 * [set Samples]
	 * @method setSamples
	 * @param  set        [pointer to vector of pointer to mzSample]
	 */
	void setSamples(vector<mzSample*>&set);

	/**
	 * [cleanup all groups]
	 * @method cleanup
	 */
	void cleanup();
	vector<mzSample*> getVisibleSamples();

	bool writeCSVFlag;
	bool alignSamplesFlag;
	bool keepFoundGroups;
	bool processAllSlices;
	bool pullIsotopesFlag;
	bool showProgressFlag;
	bool matchRtFlag;
	bool checkConvergance;

	/**
	 * default ionization mode used by mass spec
	 */
	int ionizationMode;
	int charge;
	bool formulaFlag = false;

	// For quantile intensity and qualityWeight
	double quantileQuality;
	double quantileIntensity;
	double quantileSignalBaselineRatio;
	double quantileSignalBlankRatio;

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
	int baseline_smoothingWindow;
	int baseline_dropTopX;

	double minSignalBaselineDifference;
	double isotopicMinSignalBaselineDifference;

	int eicType;

	//peak filtering
	int minGoodGroupCount;
	float minSignalBlankRatio;
	float minNoNoiseObs;
	float minSignalBaseLineRatio;
	float minGroupIntensity;
	int peakQuantitation;

	// Peak Group Rank
	int qualityWeight;
	int intensityWeight;
	int deltaRTWeight;
	bool deltaRtCheckFlag;

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

	string filterline;

	float minQuality;
	string ligandDbFilename;

	double maxIsotopeScanDiff;
	double maxNaturalAbundanceErr;
	double minIsotopicCorrelation;
	int noOfIsotopes;
	bool C13Labeled_BPE;
	bool N15Labeled_BPE;
	bool S34Labeled_BPE;
	bool D2Labeled_BPE;
	
	bool C13Labeled_Barplot;
	bool N15Labeled_Barplot;
	bool S34Labeled_Barplot;
	bool D2Labeled_Barplot;
    
	bool C13Labeled_IsoWidget;
	bool N15Labeled_IsoWidget;
	bool S34Labeled_IsoWidget;
	bool D2Labeled_IsoWidget;


	float minRt;
    float maxRt;
    float minMz;
    float maxMz;
    float minIntensity;
    float maxIntensity;
    float minCharge;
    float maxCharge;

    string outputdir;

    vector<PeakGroup> allgroups;
	vector<PeakGroup> undoAlignmentGroups;
	int alignButton;
    MassCalculator mcalc;
    Classifier* clsf;
	PeakGroup* _group;
	vector<mzSample*> samples;
	vector<Compound*> compounds;
	vector<mzSlice*> _slices;
	map <string, bool> isotopeAtom;
	bool stop;

	int alignMaxItterations; //TODO: Sahil - Kiran, Added while merging mainwindow
	int alignPolynomialDegree; //TODO: Sahil - Kiran, Added while merging mainwindow

    float minFragmentMatchScore;
    bool matchFragmentation;
    float fragmentMatchPPMTolr;
    /**
     * [print parameter Settings]
     * @method printSettings
     */
    void printSettings();

	//options dialog::peak grouping tab-widget
	double distXWeight;
	double distYWeight;
	double minPeakRtDiff;
	bool useOverlap;

};

#endif // MAVENPARAMETERS_H

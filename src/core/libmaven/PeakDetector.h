#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H

#include <map>
#include <deque>
#include <cmath>
#include <string>
#include <cfloat>
#include <vector>
#include <utility>
#include <climits>
#include <iomanip>
#include <qdebug.h>
#include <iostream>
#include <qstring.h>
#include <algorithm>
#include <qdatetime.h>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#ifndef __APPLE__
#include <omp.h>
#endif

#include "mzUtils.h"
#include "Compound.h"
#include "mzSample.h"
#include "mzAligner.h"
#include "constants.h"
#include "classifier.h"
#include "mzMassSlicer.h"
#include "peakFiltering.h"
#include "groupFiltering.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "isotopeDetection.h"

/**
 * @class PeakDetector
 * @ingroup libmaven
 * @brief all peak detection logic resides here.
 * @author Elucidata
 */
class PeakDetector {
public:
    boost::signals2::signal< void (const string&,unsigned int , int ) > boostSignal;
	// vector<vector<float>> vector_intensity_array;
	// vector<float> vector_mz_array;
	// vector<float> vector_rt_array;
	PeakDetector();
	PeakDetector(MavenParameters* mp);


	//copy constructor	
	PeakDetector(const PeakDetector &obj) {
		mavenParameters = obj.mavenParameters; 
	}
	
	//assignment constructor
	PeakDetector& operator=(const PeakDetector& obj) {
		mavenParameters = obj.mavenParameters;
		return *this;
	}

    
    void sendBoostSignal( const string& progressText, unsigned int completed_slices, int total_slices)
    {
    boostSignal(progressText, completed_slices, total_slices);
    }

	void deleteDuplicateGroup();
	// bool duplicateComparator(struct customGroup x,struct customGroup y);

	void resetProgressBar();

	/**
	 * [get Maven Parameters]
	 * @return [params]
	 */
	MavenParameters* getMavenParameters() {
		return mavenParameters;
	}

	/**
	 * [set Maven Parameters]
	 * @param mp [params]
	 */
	void setMavenParameters(MavenParameters* mp) {
		mavenParameters = mp;
	}

	/**
	 * [align Samples using Aligner class]
	 * @method alignSamples
	 */
	void alignSamples();

	/**
	 * [process Slices]
	 * @method processSlices
	 */
	void processSlices(void);
        void pullAllIsotopes();
        /**
	 * [process one Slice]
	 * @method processSlice
	 * @param  slice        [pointer to mzSlice]
	 */
	void processSlice(mzSlice& slice);

	/**
	 * [process Mass Slices]
	 * @method processMassSlices
	 */
	void processMassSlices();

	/**
	 * [process Slices]
	 * @method processSlices
	 * @param  slices        [pointer to vector of pointer to mzSlice]
	 * @param  setName       [name of set]
	 */
	void processSlices(vector<mzSlice*>&slices, string setName);

	/**
	 * @brief Filter groups on the basis of user-defined parameters
	 * @param peakgroups vector of Peakgroup objects
	 * @param slice mzSlice object bound to PeakGroup
	 * @return filteredGroups Filtered PeakGroups
	 * @see PeakGroup
	 */
	vector<PeakGroup*> groupFiltering(vector<PeakGroup> &peakgroups, mzSlice* slice);

	/**
	 * [process Compounds]
	 * @method processCompounds
	 * @param  set        [vector of pointer to Compound]
	 * @param  setName       [name of set]
	 * @return [vector of pointer to mzSlice]
	 */
	vector<mzSlice*> processCompounds(vector<Compound*> set, string setName);

	static vector<EIC*> pullEICs(mzSlice* slice, std::vector<mzSample*>&samples,
			int peakDetect, int smoothingWindow, int smoothingAlgorithm,
			float amuQ1, float amuQ3, int baselineSmoothingWindow,
			int baselineDropTopX, double minSignalBaselineDifference, int eicType, string filterline);

private:

	/**
	 * [check overlap between RT for each group through all the samples; if a certain degree of overlap is present, do not create a new group]
	 * @method addPeakGroup
	 * @param  group        [pointer to PeakGroup]
	 * @return [True if group is added to all groups, else False]
	 */
	bool addPeakGroup(PeakGroup& grup1);
	MavenParameters* mavenParameters;
	bool zeroStatus;
};
/**
 * struct for duplicateDeletion
 */
struct customGroup{
	vector<float> intensity_vec;
	PeakGroup actual_vec;
	float mz;
	float rt;
};

/**
 * struct for EicLoader
 */
struct EicLoader {

	enum PeakDetectionFlag {
		NoPeakDetection = 0, PeakDetection = 1
	};

	EicLoader(mzSlice* islice, int eic_type, string filter_line, PeakDetectionFlag iflag = NoPeakDetection,
			int smoothingWindow = 5, int smoothingAlgorithm = 0, float amuQ1 =
					0.5, float amuQ3 = 0.5, int baselineSmoothingWindow = 5,
			int baselineDropTopX = 40, double minSignalBaselineDiff = 0) {

		slice = islice;
		eicType = eic_type;
		filterline = filter_line;
		pdetect = iflag;
		eic_smoothingWindow = smoothingWindow;
		eic_smoothingAlgorithm = smoothingAlgorithm;
		eic_amuQ1 = amuQ1;
		eic_amuQ3 = amuQ3;
		eic_baselne_dropTopX = baselineDropTopX;
		eic_baselne_smoothingWindow = baselineSmoothingWindow;
		minSignalBaselineDifference = minSignalBaselineDiff;
	}

	typedef EIC* result_type;

	/**
	 * [operator]
	 * @return [eic]
	 */
	EIC* operator()(mzSample* sample) {
		EIC* e = NULL;
		Compound* c = slice->compound;

		if (!slice->srmId.empty()) {
			//cout << "computeEIC srm:" << slice->srmId << endl;
			e = sample->getEIC(slice->srmId, eicType);
		} else if (c && c->precursorMz > 0 && c->productMz > 0) {
			//cout << "computeEIC qqq: " << c->precursorMz << "->" << c->productMz << endl;
			e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz, eicType,
					filterline, eic_amuQ1, eic_amuQ3);
		} else {
			//cout << "computeEIC mzrange" << setprecision(7) << slice->mzmin  << " " << slice->mzmax << slice->rtmin  << " " << slice->rtmax << endl;
			e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin,
					slice->rtmax, 1, eicType, filterline);
		}

		if (e) {
			e->setBaselineSmoothingWindow(eic_baselne_smoothingWindow);
			e->setBaselineDropTopX(eic_baselne_dropTopX);
			e->setSmootherType((EIC::SmootherType) (eic_smoothingAlgorithm));
		}

		if (pdetect == PeakDetection && e) {
			e->setFilterSignalBaselineDiff(minSignalBaselineDifference);
			e->getPeakPositions(eic_smoothingWindow);
		}
		return e;
	}

	mzSlice* slice;
	PeakDetectionFlag pdetect;
	int eicType;
	string filterline;
	int eic_smoothingWindow;
	int eic_smoothingAlgorithm;
	float eic_amuQ1;
	float eic_amuQ3;
	int eic_baselne_smoothingWindow;
	int eic_baselne_dropTopX;
	double minSignalBaselineDifference;
};

#endif // PEAKDETECTOR_H

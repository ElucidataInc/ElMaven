#ifndef EICLOGIC_H
#define EICLOGIC_H

#include <deque>
#include <vector>
#include <cmath>
#include <string>

#include "PeakDetector.h"
#include "mzSample.h"
#include "Compound.h"

class EICLogic {
public:
	EICLogic();

	vector<EIC*> getEICs() {
		return eics;
	}
	vector<PeakGroup>& getPeakGroups() {
		return peakgroups;
	}
	PeakGroup* getSelectedGroup() {
		return selectedGroup;
	}
	mzSlice& getMzSlice() {
		return _slice;
	}

	PeakGroup* selectGroupNearRt(float rt,
								PeakGroup* selGroup,
								bool matchRtFlag,
								float compoundRTWindow,
								int qualityWeight,
								int intensityWeight,
								int deltaRTWeight);

	void groupPeaks(float eic_smoothingWindow,
					float grouping_maxRtWindow,
					double minQuality,
					double distXWeight,
					double distYWeight,
					double overlapWeight,
					bool useOverlap,
					double minSignalBaselineDifference);

	mzSlice setMzSlice(float mz1, double ppm, float mz2 = 0.0);

	//	find absolute min and max for extracted ion chromatograms
	mzSlice visibleEICBounds();

	//	find absolute min and max for samples
	mzSlice visibleSamplesBounds(vector<mzSample*> samples);

	/**
	 * @param slice mzSlice object
	 * @param samples vector of samples
	 * @param eic_smoothingWindow EIC smoothing window (in Scans)
	 * @param eic_smoothingAlgorithm Smoothing algorithm
	 * @param amuQ1 precusor m/z delta
	 * @param amuQ3 product m/z delta
	 * @param baseline_smoothing Baseline smoothing window
	 * @param baseline_quantile Drop top x% of intensities in an EIC
	 *  for calculating baseline
	 * @param minSignalBaselineDifference Minimun Signal to baseline
	 *  difference
	 * @param eicType Sum or Max
	 * @param filterline Select filterline (srmId) (others will be rejected)
	 * @see mzSlice
	 * @see EIC
	 * @return vector<EIC*> vector of EICs
	 */
	void getEIC(mzSlice bounds, vector<mzSample*> samples,
			int eic_smoothingWindow, int eic_smoothingAlgorithm, float amuQ1,
			float amuQ3, int baseline_smoothing, int baseline_quantile,
			double minSignalBaselineDifference, int eicType, string filterline, set<string> &multipleTransitions, string &currentTransition);

	//associate compound names with peak groups
	void associateNameWithPeakGroups();

	mzSlice _slice;						// current slice
	vector<EIC*> eics;				// vectors mass slices one from each sample
	deque<EIC*> tics;				// vectors total chromatogram intensities
	vector<PeakGroup> peakgroups;	    //peaks grouped across samples
	PeakGroup* selectedGroup;			//currently selected peak group
	PeakGroup _integratedGroup;		//manually integrated peak group

private:
	void addPeakGroup(PeakGroup& group);
};

#endif // EICLOGIC_H

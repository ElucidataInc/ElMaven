#ifndef EICLOGIC_H
#define EICLOGIC_H

#include <deque>
#include <vector>
#include <cmath>
#include <string>

#include "PeakDetector.h"
#include "mzSample.h"

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

	PeakGroup* selectGroupNearRt(float rt, PeakGroup* selGroup);
	void groupPeaks(float eic_smoothingWindow, float grouping_maxRtWindow, double minQuality);
	mzSlice setMzSlice(float mz, pair<string,double> pr);

	//	find absolute min and max for extracted ion chromatograms
	mzSlice visibleEICBounds();

	//	find absolute min and max for samples
	mzSlice visibleSamplesBounds(vector<mzSample*> samples);

	void getEIC(mzSlice bounds, vector<mzSample*> samples,
			int eic_smoothingWindow, int eic_smoothingAlgorithm, float amuQ1,
			float amuQ3, int baseline_smoothing, int baseline_quantile);

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

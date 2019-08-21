#ifndef EICLOGIC_H
#define EICLOGIC_H

#include "datastructures/mzSlice.h"
#include "PeakGroup.h"
#include "standardincludes.h"

class Compound;
class EIC;
class mzSlice;
class MassCutoff;
class mzSample;
class MavenParameters;

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
								int qualityWeight,
								int intensityWeight,
								int deltaRTWeight);

    void groupPeaks(float eic_smoothingWindow,
                    mzSlice* slice,
                    float grouping_maxRtWindow,
                    double minQuality,
                    double distXWeight,
                    double distYWeight,
                    double overlapWeight,
                    bool useOverlap,
                    double minSignalBaselineDifference,
                    float productPpmTolerance,
                    string scoringAlgo);

	mzSlice setMzSlice(float mz1, MassCutoff *massCutoff, float mz2 = 0.0);

	//	find absolute min and max for extracted ion chromatograms
	mzSlice visibleEICBounds();

	//	find absolute min and max for samples
	mzSlice visibleSamplesBounds(vector<mzSample*> samples);

    void getEIC(mzSlice bounds, vector<mzSample*> samples, MavenParameters* mp);

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

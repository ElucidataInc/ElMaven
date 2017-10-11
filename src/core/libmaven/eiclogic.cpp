#include "eiclogic.h"

EICLogic::EICLogic() {
	_slice = mzSlice(0, 0.01, 0, 0.01);
	selectedGroup = NULL;
}

void EICLogic::addPeakGroup(PeakGroup& group) {
	//qDebug <<" EicWidget::addPeakGroup(PeakGroup& group)";
	peakgroups.push_back(group);
}

void EICLogic::associateNameWithPeakGroups() {

	if (_slice.compound) {
		for (unsigned int i = 0; i < peakgroups.size(); i++) {
			peakgroups[i].compound = _slice.compound;
		}
	}
	if (!_slice.srmId.empty()) {
		for (unsigned int i = 0; i < peakgroups.size(); i++) {

			peakgroups[i].srmId = _slice.srmId;
		}
	}
}

PeakGroup* EICLogic::selectGroupNearRt(float rt,
									   	PeakGroup* selGroup,
										bool deltaRtCheckFlag,
										float compoundRTWindow,
										int qualityWeight,
										int intensityWeight,
										int deltaRTWeight) {

	for (unsigned int i = 0; i < peakgroups.size(); i++) {
		float diff = abs(peakgroups[i].meanRt - rt);
		if (diff < 2) {
			if (selGroup == NULL) {
				selGroup = &peakgroups[i];
				continue;
			}

			selGroup->calGroupRank(deltaRtCheckFlag,
									compoundRTWindow,
									qualityWeight,
									intensityWeight,
									deltaRTWeight);
			peakgroups[i].calGroupRank(deltaRtCheckFlag,
									compoundRTWindow,
									qualityWeight,
									intensityWeight,
									deltaRTWeight);

			if (selGroup != NULL
					&& peakgroups[i].groupRank < selGroup->groupRank) {
				selGroup = &peakgroups[i];
			}
		}
	}
	return selGroup;
}

void EICLogic::groupPeaks(float eic_smoothingWindow,
							float grouping_maxRtWindow,
							double minQuality,
							double distXWeight,
							double distYWeight,
							double overlapWeight,
							bool useOverlap,
							double minSignalBaselineDifference) {

	peakgroups = EIC::groupPeaks(eics,
								eic_smoothingWindow,
								grouping_maxRtWindow,
								minQuality,
                                distXWeight,
                                distYWeight,
                                overlapWeight,
                                useOverlap,
								minSignalBaselineDifference);


	//keep only top X groups ( ranked by intensity )
	EIC::removeLowRankGroups(peakgroups, 50);
}

mzSlice EICLogic::setMzSlice(float mz1, double ppm, float mz2) {

	mzSlice x(_slice.mzmin, _slice.mzmax, _slice.rtmin, _slice.rtmax);
	x.mz = mz1;
	x.mzmin = mz1 - mz1 / 1e6 * ppm;
	x.mzmax = mz1 + mz1 / 1e6 * ppm;
	if (mz2 > 0) {
		Compound* c = new Compound("1", "", "C", 0);
		c->precursorMz = mz1;
		c->productMz = mz2;
		x.compound = c;
	}
	return x;
}

void EICLogic::getEIC(mzSlice bounds, vector<mzSample*> samples,
		int eic_smoothingWindow, int eic_smoothingAlgorithm, float amuQ1,
		float amuQ3, int baseline_smoothing, int baseline_quantile,
		double minSignalBaselineDifference, int eicType, string filterline) {

	mzSlice slice = _slice;
	slice.rtmin = bounds.rtmin;
	slice.rtmax = bounds.rtmax;

	//get eics
	eics = PeakDetector::pullEICs(&slice, samples, EicLoader::PeakDetection,
			eic_smoothingWindow, eic_smoothingAlgorithm, amuQ1, amuQ3,
			baseline_smoothing, baseline_quantile, minSignalBaselineDifference, eicType,
			filterline);

	//find peaks
	//for(int i=0; i < eics.size(); i++ )  eics[i]->getPeakPositions(eic_smoothingWindow);
	//for(int i=0; i < eics.size(); i++ ) mzUtils::printF(eics[i]->intensity);
	////qDebug << tr("computeEICs() Done. ElepsTime=%1 msec").arg(timer.elapsed());

}

mzSlice EICLogic::visibleEICBounds() {
	mzSlice bounds(0, 0, 0, 0);

	for (unsigned int i = 0; i < eics.size(); i++) {
		EIC* eic = eics[i];
		if (i == 0 || eic->rtmin < bounds.rtmin)
			bounds.rtmin = eic->rtmin;
		if (i == 0 || eic->rtmax > bounds.rtmax)
			bounds.rtmax = eic->rtmax;
		if (i == 0 || eic->mzmin < bounds.mzmin)
			bounds.mzmin = eic->mzmin;
		if (i == 0 || eic->mzmax > bounds.mzmax)
			bounds.mzmax = eic->mzmax;
		if (i == 0 || eic->maxIntensity > bounds.ionCount)
			bounds.ionCount = eic->maxIntensity;
	}
	return bounds;
	//} Feng note: move this bracket to above "return bounds" fixes a maximum retention time bug.
}

mzSlice EICLogic::visibleSamplesBounds(vector<mzSample*> samples) {
	mzSlice bounds(0, 0, 0, 0);
	for (unsigned int i = 0; i < samples.size(); i++) {
		mzSample* sample = samples[i];
		if (i == 0 || sample->minRt < bounds.rtmin)
			bounds.rtmin = sample->minRt;
		if (i == 0 || sample->maxRt > bounds.rtmax)
			bounds.rtmax = sample->maxRt;
		if (i == 0 || sample->minMz < bounds.mzmin)
			bounds.mzmin = sample->minMz;
		if (i == 0 || sample->maxMz > bounds.mzmax)
			bounds.mzmax = sample->maxMz;
		if (i == 0 || sample->maxIntensity > bounds.ionCount)
			bounds.ionCount = sample->maxIntensity;
	}
	return bounds;
}

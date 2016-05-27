#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <limits>

#include "mzSample.h"
#include "mzUtils.h"

PeakGroup::PeakGroup() {
	groupId = 0;
	metaGroupId = 0;
	groupRank = std::numeric_limits<int>::max();

	maxIntensity = 0;
	meanRt = 0;
	meanMz = 0;

	blankMax = 0;
	blankSampleCount = 0;
	blankMean = 0;

	sampleMax = 0;
	sampleCount = 0;
	sampleMean = 0;

	totalSampleCount = 0;
	maxNoNoiseObs = 0;
	maxPeakFracionalArea = 0;
	maxSignalBaseRatio = 0;
	maxSignalBaselineRatio = 0;
	maxPeakOverlap = 0;
	maxQuality = 0;

	expectedRtDiff = -1;
	expectedAbundance = 0;
	isotopeC13count = 0;

	minRt = 0;
	maxRt = 0;

	minMz = 0;
	maxMz = 0;

	parent = NULL;
	//adduct = NULL;
	compound = NULL;

	isFocused = false;
	label = 0;				//classification label

	goodPeakCount = 0;
	_type = None;

	changePValue = 0;
	changeFoldRatio = 0;
	//children.reserve(0);
	peaks.reserve(0);

}

void PeakGroup::copyObj(const PeakGroup& o) {
	groupId = o.groupId;
	metaGroupId = o.metaGroupId;
	groupRank = o.groupRank;

	maxIntensity = o.maxIntensity;
	meanRt = o.meanRt;
	meanMz = o.meanMz;

	blankMax = o.blankMax;
	blankSampleCount = o.blankSampleCount;
	blankMean = o.blankMean;

	sampleMax = o.sampleMax;
	sampleCount = o.sampleCount;
	sampleMean = o.sampleMean;

	totalSampleCount = o.totalSampleCount;
	maxNoNoiseObs = o.maxNoNoiseObs;
	maxPeakFracionalArea = o.maxPeakFracionalArea;
	maxSignalBaseRatio = o.maxSignalBaseRatio;
	maxSignalBaselineRatio = o.maxSignalBaselineRatio;
	maxPeakOverlap = o.maxPeakOverlap;
	maxQuality = o.maxQuality;
	expectedRtDiff = o.expectedRtDiff;
	expectedAbundance = o.expectedAbundance;
	isotopeC13count = o.isotopeC13count;

	minRt = o.minRt;
	maxRt = o.maxRt;

	minMz = o.minMz;
	maxMz = o.maxMz;

	parent = o.parent;
	compound = o.compound;

	srmId = o.srmId;
	isFocused = o.isFocused;
	label = o.label;

	goodPeakCount = o.goodPeakCount;
	_type = o._type;
	tagString = o.tagString;

	changeFoldRatio = o.changeFoldRatio;
	changePValue = o.changePValue;
	peaks = o.peaks;
	children = o.children;
	for (int i = 0; i < children.size(); i++)
		children[i].parent = this;
}

PeakGroup::~PeakGroup() {
	clear();
}

bool PeakGroup::isPrimaryGroup() {
	if (compound && compound->getPeakGroup() == this)
		return true;
	return false;
}

void PeakGroup::clear() {
	deletePeaks();
	deleteChildren();
	meanMz = 0;
	groupRank = std::numeric_limits<int>::max();
}

Peak* PeakGroup::getSamplePeak(mzSample* sample) {
	for (int i = 0; i < peaks.size(); i++) {
		if (peaks[i].getSample() == sample)
			return &peaks[i];
	}
	return NULL;
}

void PeakGroup::deletePeaks() {
	peaks.clear();
}

bool PeakGroup::deletePeak(unsigned int index) {
	if (index < children.size()) {
		peaks.erase(peaks.begin() + index);
		return true;
	}
	return false;
}

float PeakGroup::meanRtW() {
	if (peakCount() == 0)
		return 0;

	float mean = 0;
	float Wtotal = 0;
	for (int i = 0; i < peakCount(); i++)
		Wtotal += peaks[i].peakIntensity;

	if (Wtotal > 0) {
		for (int i = 0; i < peakCount(); i++)
			mean += peaks[i].peakIntensity / Wtotal * peaks[i].rt;
		return mean;
	} else {
		for (int i = 0; i < peakCount(); i++)
			mean += peaks[i].rt;
		return mean / peakCount();
	}
}

float PeakGroup::medianRt() {
	vector<float> rts(peaks.size(), 0);
	for (int i = 0; i < peakCount(); i++)
		rts[i] = peaks[i].rt;
	return mzUtils::median(rts);
}

void PeakGroup::deleteChildren() {
	children.clear();
}

bool PeakGroup::deleteChild(unsigned int index) {
	if (index < children.size()) {
		children.erase(children.begin() + index);
		return true;
	}
	return false;
}

bool PeakGroup::deleteChild(PeakGroup* child) {
	if (!child)
		return false;

	deque<PeakGroup>::iterator it;
	it = find(children.begin(), children.end(), child);
	if (*it == child) {
		cerr << "deleteChild: setting child to empty";
		child->clear();
		return true;
		//sort(children.begin(), children.end(),PeakGroup::compIntensity);
		//for(int i=0; i < children.size(); i++ ) { cerr << &children[i] << endl; }
	}

	return false;
}

//return intensity vectory ordered by samples
vector<float> PeakGroup::getOrderedIntensityVector(vector<mzSample*>& samples,
		QType type) {

	if (samples.size() == 0) {
		vector<float> x;    //empty vector;
		return x;
	}

	map<mzSample*, float> sampleOrder;
	vector<float> maxIntensity(samples.size(), 0);

	for (unsigned int j = 0; j < samples.size(); j++) {
		sampleOrder[samples[j]] = j;
		maxIntensity[j] = 0;
	}

	for (unsigned int j = 0; j < peaks.size(); j++) {
		Peak& peak = peaks.at(j);
		mzSample* sample = peak.getSample();

		if (sampleOrder.count(sample) > 0) {
			int s = sampleOrder[sample];
			float y = 0;
			switch (type) {
			case AreaTop:
				y = peak.peakAreaTop;
				break;
			case Area:
				y = peak.peakAreaCorrected;
				break;
			case Height:
				y = peak.peakIntensity;
				break;
			case RetentionTime:
				y = peak.rt;
				break;
			case Quality:
				y = peak.quality;
				break;
			case SNRatio:
				y = peak.signalBaselineRatio;
				break;
			default:
				y = peak.peakAreaTop;
				break;
			}

			//normalize
			if (sample)
				y *= sample->getNormalizationConstant();
			if (maxIntensity[s] < y) {
				maxIntensity[s] = y;
			}
		}
	}
	return maxIntensity;
}

void PeakGroup::computeAvgBlankArea(const vector<EIC*>& eics) {

	if (peaks.size() == 0)
		return;

	//find range to fill in
	float rtmin = peaks[0].rtmin;
	float rtmax = peaks[0].rtmax;

	for (int i = 1; i < peaks.size(); i++) {
		if (peaks[i].rtmin < rtmin)
			rtmin = peaks[i].rtmin;
		if (peaks[i].rtmax > rtmax)
			rtmax = peaks[i].rtmax;
	}
	rtmin = rtmin - 0.25;
	rtmax = rtmax + 0.25;

	float sum = 0;
	int len = 0;
	for (unsigned int i = 0; i < eics.size(); i++) {
		EIC* eic = eics[i];
		if (eic->sample != NULL && eic->sample->isBlank == false)
			continue;
		for (int pos = 0; pos < eic->intensity.size(); pos++) {
			if (eic->rt[pos] >= rtmin && eic->rt[pos] <= rtmax
					&& eic->intensity[pos] > 0) {
				sum += eic->intensity[pos];
				len++;
			}
		}
	}
	this->blankMean = 0; 	//default zero
	if (len > 0)
		this->blankMean = (float) sum / len;
}

void PeakGroup::fillInPeaks(const vector<EIC*>& eics) {

	if (peaks.size() == eics.size())
		return;
	if (peaks.size() == 0)
		return;

	//find range to fill in
	float rtmin = peaks[0].rtmin;
	float rtmax = peaks[0].rtmax;

	for (int i = 1; i < peaks.size(); i++) {
		if (peaks[i].rtmin < rtmin)
			rtmin = peaks[i].rtmin;
		if (peaks[i].rtmax > rtmax)
			rtmax = peaks[i].rtmax;
	}

	int filledInCount = 0;

	for (unsigned int i = 0; i < eics.size(); i++) {
		EIC* eic = eics[i];
		if (eic == NULL)
			continue;
		if (eic->spline == NULL)
			continue;
		if (eic->intensity.size() == 0)
			continue;

		bool missing = true;

		for (unsigned int j = 0; j < peaks.size(); j++) {
			if (peaks[j].getEIC() == eic) {
				missing = false;
				break;
			}
		}

		if (missing) { //fill in peak
			int maxpos = 0;
			for (int pos = 1; pos < eic->intensity.size() - 1; pos++) {
				if (eic != NULL && eic->intensity[pos] != 0 && eic->mz[pos] != 0
						&& eic->rt[pos] >= rtmin && eic->rt[pos] <= rtmax
						&& eic->spline[pos] > eic->spline[pos - 1]
						&& eic->spline[pos] > eic->spline[pos + 1]) {
					if (maxpos != 0
							&& eic->intensity[pos] > eic->intensity[maxpos]) {
						maxpos = pos;
					} else {
						maxpos = pos;
					}
				}
			}

			if (maxpos != 0 && eic->intensity[maxpos] != 0) {
				Peak peak(eic, maxpos);
				eic->findPeakBounds(peak);
				eic->getPeakDetails(peak);
				this->addPeak(peak);
				filledInCount++;
			}
		}
	}

	//cerr << "fillInPeaks" << rtmin << " " << rtmax << " " << eics.size() << " " peaks.size() << endl;
	//    if (filledInCount > 0) { this->fillInPeaks(eics); }
}

void PeakGroup::reduce() { // make sure there is only one peak per sample

	map<mzSample*, Peak> maxPeaks;
	map<mzSample*, Peak>::iterator itr;
	if (peaks.size() < 2)
		return;

	float groupMeanRt = 0;
	float totalWeight = 1;
	for (unsigned int i = 0; i < peaks.size(); i++) {
		totalWeight += peaks[i].peakIntensity;
	}
	for (unsigned int i = 0; i < peaks.size(); i++) {
		groupMeanRt += peaks[i].rt * peaks[i].peakIntensity / totalWeight;
	}

	for (unsigned int i = 0; i < peaks.size(); i++) {
		mzSample* c = peaks[i].getSample();
		float rtdiff = abs(groupMeanRt - peaks[i].rt);

		if (maxPeaks.count(c) == 0) {
			maxPeaks[c].copyObj(peaks[i]);
		} else if (maxPeaks.count(c) > 0
				&& rtdiff < abs(groupMeanRt - maxPeaks[c].rt)) {
			maxPeaks[c].copyObj(peaks[i]);
		}
	}

	peaks.clear();
	for (itr = maxPeaks.begin(); itr != maxPeaks.end(); itr++) {
		const Peak& peak = (*itr).second;
		addPeak(peak);
	}
	//	cerr << "\t\t\treduce() from " << startSize << " to " << peaks.size() << endl;
}

void PeakGroup::updateQuality() {
	maxQuality = 0;
	goodPeakCount = 0;
	for (unsigned int i = 0; i < peaks.size(); i++) {
		if (peaks[i].quality > maxQuality)
			maxQuality = peaks[i].quality;
		if (peaks[i].quality > 0.5)
			goodPeakCount++;
	}
}

void PeakGroup::groupStatistics() {
	float rtSum = 0;
	float mzSum = 0;
	maxIntensity = 0;
	totalSampleCount = 0;

	blankMax = 0;
	blankSampleCount = 0;

	sampleMax = 0;
	sampleCount = 0;
	sampleMean = 0;

	maxNoNoiseObs = 0;
	minRt = 0;
	maxRt = 0;
	minMz = 0;
	maxMz = 0;

	maxPeakFracionalArea = 0;
	maxQuality = 0;
	goodPeakCount = 0;
	maxSignalBaselineRatio = 0;
	int nonZeroCount = 0;

	for (unsigned int i = 0; i < peaks.size(); i++) {
		if (peaks[i].pos != 0) {
			rtSum += peaks[i].rt;
			mzSum += peaks[i].baseMz;
			nonZeroCount++;
		}
		if (peaks[i].peakIntensity > 0)
			totalSampleCount++;

		if (peaks[i].peakIntensity > maxIntensity) {
			maxIntensity = peaks[i].peakIntensity;
			meanMz = peaks[i].baseMz;
			meanRt = peaks[i].rt;
		}

		if (peaks[i].noNoiseObs > maxNoNoiseObs)
			maxNoNoiseObs = peaks[i].noNoiseObs;
		if (minRt == 0 || peaks[i].rtmin < minRt)
			minRt = peaks[i].rtmin;
		if (maxRt == 0 || peaks[i].rtmax > maxRt)
			maxRt = peaks[i].rtmax;
		if (minMz == 0 || peaks[i].mzmin < minMz)
			minMz = peaks[i].mzmin;
		if (maxMz == 0 || peaks[i].mzmax > maxMz)
			maxMz = peaks[i].mzmax;
		if (peaks[i].peakAreaFractional > maxPeakFracionalArea)
			maxPeakFracionalArea = peaks[i].peakAreaFractional;
		if (peaks[i].quality > maxQuality)
			maxQuality = peaks[i].quality;
		if (peaks[i].quality > 0.5)
			goodPeakCount++;
		if (peaks[i].signalBaselineRatio > maxSignalBaselineRatio)
			maxSignalBaselineRatio = peaks[i].signalBaselineRatio;

		if (peaks[i].fromBlankSample) {
			blankSampleCount++;
			if (peaks[i].peakIntensity > blankMax)
				blankMax = peaks[i].peakIntensity;
		} else {
			sampleMean += peaks[i].peakIntensity;
			sampleCount++;
			if (peaks[i].peakIntensity > sampleMax)
				sampleMax = peaks[i].peakIntensity;
		}
	}

	if (sampleCount > 0)
		sampleMean = sampleMean / sampleCount;
	if (nonZeroCount) {
		meanRt = rtSum / nonZeroCount;
		meanMz = mzSum / nonZeroCount;
	}

	groupOverlapMatrix();
}

void PeakGroup::groupOverlapMatrix() {

	for (unsigned int i = 0; i < peaks.size(); i++)
		peaks[i].groupOverlapFrac = 0;

	for (unsigned int i = 0; i < peaks.size(); i++) {
		Peak& a = peaks[i];
		for (unsigned int j = i; j < peaks.size(); j++) {
			Peak& b = peaks[j];
			float overlap = checkOverlap(a.rtmin, a.rtmax, b.rtmin, b.rtmax); //check for overlap
			if (overlap > 0) {
				b.groupOverlapFrac += log(overlap);
				a.groupOverlapFrac += log(overlap);
			}

			/*
			 if ( overlap > 0.1 ) {
			 b.peakAreaFractional < 1 ? a.groupOverlapFrac += log(1-b.peakAreaFractional) : a.groupOverlapFrac += log(0.01);
			 a.peakAreaFractional < 1 ? b.groupOverlapFrac += log(1-a.peakAreaFractional) : b.groupOverlapFrac += log(0.01);
			 }
			 */
		}
	}
	//normalize
	for (unsigned int i = 0; i < peaks.size(); i++)
		peaks[i].groupOverlapFrac /= peaks.size();
}

void PeakGroup::summary() {
	cerr << tagString << endl;
	cerr << "\t" << "meanRt=" << meanRt << endl << "\t" << "meanMz=" << meanMz
			<< endl << "\t" << "goodPeakCount=" << goodPeakCount << endl << "\t"
			<< "maxQuality=" << maxQuality << endl << "\t" << "maxNoNoiseObs="
			<< maxNoNoiseObs << endl << "\t" << "sampleCount=" << sampleCount
			<< endl << "\t" << "maxSignalBaselineRatio="
			<< maxSignalBaselineRatio << endl << "\t" << "maxPeakFracionalArea="
			<< maxPeakFracionalArea << endl << "\t" << "blankMean=" << blankMean
			<< endl << "\t" << "sampleMean=" << sampleMean << endl << "\t"
			<< "maxIntensity=" << maxIntensity << endl << endl;

	for (int i = 0; i < peaks.size(); i++) {
		cerr << "\t\t" << "Q:" << peaks[i].quality << " " << "pAf:"
				<< peaks[i].peakAreaFractional << " " << "noNf"
				<< peaks[i].noNoiseFraction << " " << "noObs:"
				<< peaks[i].noNoiseObs << " " << "w:" << peaks[i].width << " "
				<< "sn:" << peaks[i].signalBaselineRatio << " " << "ovp:"
				<< peaks[i].groupOverlapFrac << endl;
	}

	for (int i = 0; i < children.size(); i++)
		children[i].summary();
}

PeakGroup::PeakGroup(const PeakGroup& o) {
	copyObj(o);
}

PeakGroup& PeakGroup::operator=(const PeakGroup& o) {
	copyObj(o);
	return *this;
}

bool PeakGroup::operator==(const PeakGroup* o) {
	if (this == o) {
		cerr << o << " " << this << endl;
		return true;
	}
	return false;
}

Peak* PeakGroup::getPeak(mzSample* s) {
	if (s == NULL)
		return NULL;
	for (int i = 0; i < peaks.size(); i++) {
		if (peaks[i].getSample() == s) {
			return &peaks[i];
		}
	}
	return NULL;
}

void PeakGroup::reorderSamples() {
	std::sort(peaks.begin(), peaks.end(), Peak::compIntensity);
	for (int i = 0; i < peaks.size(); i++) {
		mzSample* s = peaks[i].getSample();
		if (s != NULL)
			s->setSampleOrder(i);
	}
}

string PeakGroup::getName() {
	string tag;
	if (compound)
		tag = compound->name;
	if (tagString.empty())
		tag += " | " + tagString;
	if (srmId.empty())
		tag += " | " + srmId;
	if (tag.empty())
		tag = integer2string(groupId);
	return tag;
}


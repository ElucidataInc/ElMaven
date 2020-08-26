//@Kailash: getting features of a group

#include "groupFeatures.h"
#include "PeakGroup.h"

vector<float> getFeatures(PeakGroup* grp) {
	vector<float> features(8, 0);
	
	features[0] = getAvgPeakAreaFractional(grp);
	features[1] = getAvgNoNoiseFraction(grp);
	features[2] = getAvgSymmetry(grp) / (getAvgWidth(grp) + 1) * log2(getAvgWidth(grp) + 1);
	features[3] = getGroupOverlapFrac(grp);
	features[4] = getMinGaussFitR2(grp) * 100;
	
	if (getLogAvgSignalBaselineRatio(grp) > 0) {
		features[5] = getLogAvgSignalBaselineRatio(grp) / 10;
	}
	else features[5] = 0.0;
	
	if (getLogAvgPeakIntensity(grp) > 0) {
		features[6] = log(getLogAvgPeakIntensity(grp));
	}
	else features[6] = 0.0;
	
	if (getAvgWidth(grp) <= 3.0 && getLogAvgSignalBaselineRatio(grp) >= 3.0) {
		features[7] = 1;
	}
	else features[7] = 0;
	
	return features;
}

float getAvgPeakAreaFractional(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].peakAreaFractional;
		}
	}
	return sum / float(validPeaks);
}

float getAvgNoNoiseFraction(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].noNoiseFraction;
		}
	}
	return sum / float(validPeaks);
}

float getAvgSymmetry(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].symmetry;
		}
	}
	return sum / float(validPeaks);
}

float getAvgWidth(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].width;
		}
	}
	return sum / float(validPeaks);
}

float getLogAvgSignalBaselineRatio(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=log2(grp->peaks[i].signalBaselineRatio);
		}
	}
	return exp2(sum / float(validPeaks));
}

float getLogAvgPeakIntensity(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=log(grp->peaks[i].peakIntensity);
		}
	}
	return exp(sum / float(validPeaks));
}

float getMinGaussFitR2(PeakGroup* grp)
{
	int i = 0;
	while(grp->peaks[i].width < 0 && i < grp->peaks.size()) i++;
	float minGaussFitR2 = grp->peaks[i].gaussFitR2;
	while(i < grp->peaks.size()) {
		if (grp->peaks[i].width > 0) {
			if (grp->peaks[i].gaussFitR2 < minGaussFitR2) {
				minGaussFitR2 = grp->peaks[i].gaussFitR2;
			}
		}
		i++;
	}
	return minGaussFitR2;
}

float getGroupOverlapFrac(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=exp(grp->peaks[i].groupOverlapFrac);
		}
	}
	return log(sum / float(validPeaks));
}

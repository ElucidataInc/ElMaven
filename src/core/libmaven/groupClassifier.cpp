#include "groupClassifier.h"

groupClassifier::groupClassifier() {
	num_features = 8;
	hidden_layer = 4;
	num_outputs = 1;
	trainingSize = 0;
	network = NULL;
}

groupClassifier::~groupClassifier() {
	if (network)
		delete (network);
	network = NULL;
}

bool groupClassifier::hasModel() {
	return network != NULL;
}

void groupClassifier::loadModel(string filename) {
	if (!fileExists(filename)) {
		cerr << "Can't load " << filename << endl;
		return;
	}
	if (network != NULL)
		delete (network);
	network = new nnwork(num_features, hidden_layer, num_outputs);
	network->load((char*) filename.c_str());
	cout << "Read in classification model " << filename << endl;
}

vector<float> groupClassifier::getFeatures(PeakGroup* grp) {
	vector<float> features(num_features, 0);
	
	features[0] = getAvgPeakAreaFractional(grp);
	features[1] = getAvgNoNoiseFraction(grp);
	features[2] = getAvgSymmetry(grp) / (getAvgWidth(grp) + 1) * log2(getAvgWidth(grp) + 1);
	features[3] = getGroupOverlapFrac(grp);
	features[4] = getMinGaussFitR2(grp);
	if (getAvgSignalBaselineRatio(grp) > 0) {
		features[5] = log2(getAvgSignalBaselineRatio(grp));
	}
	else features[5] = 0.0;
	if (getAvgPeakIntensity(grp) > 0) {
		features[6] = log(getAvgPeakIntensity(grp));
	}
	else features[6] = 0.0;
	if (getAvgWidth(grp) <= 3.0 && getAvgSignalBaselineRatio(grp) >= 3.0) {
		features[7] = 1;
	}
	else features[7] = 0;
	
	return features;
}

void groupClassifier::classify(PeakGroup* grp) {
	if (network == NULL)
		return;
	
	grp->groupQuality=scoreGroup(grp);
}

float groupClassifier::scoreGroup(PeakGroup* grp) {
    float result[1] = {0.1};
    if(network != NULL) {
        float fts[num_features];
        vector<float> features = getFeatures(grp);
        for(int k=0;k<num_features;k++)
        	fts[k]=features[k];
        network->run(fts, result);
    }

    return result[0];
}

float groupClassifier::getAvgPeakAreaFractional(PeakGroup* grp)
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

float groupClassifier::getAvgNoNoiseFraction(PeakGroup* grp)
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

float groupClassifier::getAvgSymmetry(PeakGroup* grp)
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

float groupClassifier::getAvgWidth(PeakGroup* grp)
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

float groupClassifier::getAvgSignalBaselineRatio(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].signalBaselineRatio;
		}
	}
	return sum / float(validPeaks);
}

float groupClassifier::getAvgPeakIntensity(PeakGroup* grp)
{
	int validPeaks = 0;
	float sum = 0;
	for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
		if (grp->peaks[i].width > 0)
		{
			validPeaks++;
			sum+=grp->peaks[i].peakIntensity;
		}
	}
	return sum / float(validPeaks);
}

float groupClassifier::getMinGaussFitR2(PeakGroup* grp)
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

float groupClassifier::getGroupOverlapFrac(PeakGroup* grp)
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
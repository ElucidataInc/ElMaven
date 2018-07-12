//@Pawan: Function to compute feature values for MLP Classifier

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

float groupClassifier::getLogAvgSignalBaselineRatio(PeakGroup* grp)
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

float groupClassifier::getLogAvgPeakIntensity(PeakGroup* grp)
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
#ifndef GROUP_CLASSIFIER
#define GROUP_CLASSIFIER

#include "mzSample.h"
#include <nnwork.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "EIC.h"

using namespace std;

class groupClassifier {
public:
	groupClassifier();
	~groupClassifier();
	void classify(PeakGroup* grp);
	void loadModel(string filename);
	bool hasModel();
    vector<float> getFeatures(PeakGroup* grp);
	float scoreGroup(PeakGroup* grp);
	float getAvgPeakAreaFractional(PeakGroup* grp);
	float getAvgNoNoiseFraction(PeakGroup* grp);
	float getAvgSymmetry(PeakGroup* grp);
	float getAvgWidth(PeakGroup* grp);
	float getAvgSignalBaselineRatio(PeakGroup* grp);
	float getAvgPeakIntensity(PeakGroup* grp);
	float getMinGaussFitR2(PeakGroup* grp);
	float getGroupOverlapFrac(PeakGroup* grp);
private:
	

	//neural net specific features
	nnwork* network;
	int hidden_layer;
	int num_features;
	int num_outputs;
	float trainingSize;

};
#endif

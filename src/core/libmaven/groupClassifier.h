//@Pawan: Function to compute feature values for MLP Classifier

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
#include "groupFeatures.h"

using namespace std;

class groupClassifier {
public:
	groupClassifier();
	~groupClassifier();
	void classify(PeakGroup* grp);
	float scoreGroup(PeakGroup* grp);
	void loadModel(string filename);
	bool hasModel();
private:
	

	//neural net specific features
	nnwork* network;
	int hidden_layer;
	int num_features;
	int num_outputs;
	float trainingSize;

};
#endif

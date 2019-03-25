//@Kailash: Functions to compute feature values for MLP Classifier

#include "groupClassifier.h"

groupClassifier::groupClassifier() {
	num_features = 8;
	hidden_layer = 4;
	num_outputs = 1;
	trainingSize = 0;
        network = nullptr;
}

groupClassifier::~groupClassifier() {
	if (network)
		delete (network);
        network = nullptr;
}

bool groupClassifier::hasModel() {
        return network != nullptr;
}

void groupClassifier::loadModel(string filename) {
	if (!fileExists(filename)) {
		cerr << "Can't load " << filename << endl;
		return;
	}
        if (network != nullptr)
		delete (network);
	network = new nnwork(num_features, hidden_layer, num_outputs);
	network->load((char*) filename.c_str());
	cout << "Read in classification model " << filename << endl;
}

void groupClassifier::classify(PeakGroup* grp) {
        if (network == nullptr)
		return;
	
	grp->groupQuality=scoreGroup(grp);
}

float groupClassifier::scoreGroup(PeakGroup* grp) {
    float result[1] = {0.1};
    if(network != nullptr) {
        float fts[num_features];
        vector<float> features = getFeatures(grp);
        for(int k=0;k<num_features;k++)
        	fts[k]=features[k];
        network->run(fts, result);
    }

    return result[0];
}

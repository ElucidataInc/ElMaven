//@Kailash: Function to compute feature values for MLP Classifier

#ifndef GROUP_CLASSIFIER
#define GROUP_CLASSIFIER

#include "standardincludes.h"

class PeakGroup;
class nnwork;

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

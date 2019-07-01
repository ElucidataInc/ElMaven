#ifndef CLASSIFER_NEURALNET
#define CLASSIFER_NEURALNET

#include "classifier.h"
#include "standardincludes.h"

class EIC;

using namespace std;

class ClassifierNeuralNet: public Classifier {
public:
	ClassifierNeuralNet();
	~ClassifierNeuralNet();
	void classify(PeakGroup* grp);
	void train(vector<PeakGroup*>& groups);
	void refineModel(PeakGroup* grp);
	void saveModel(string filename);
	void loadModel(string filename);
	bool hasModel();
    vector<float> getFeatures(Peak& p);
	float scorePeak(Peak& p);
	void scoreEICs(vector<EIC*> &eics);
private:
	

	//neural net specific features
	nnwork* brain;
	int hidden_layer;
	int num_outputs;
	float trainingSize;

};
#endif

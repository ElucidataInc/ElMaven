#ifndef CLASSIFER_BAYES
#define CLASSIFER_BAYES

#include "classifier.h"
#include "standardincludes.h"

class Peak;
class PeakGroup;

using namespace std;

class ClassifierNaiveBayes: public Classifier {

public:
	ClassifierNaiveBayes();
	~ClassifierNaiveBayes();
	void classify(PeakGroup* grp);
	void classify(vector<PeakGroup*>& groups);
	void train(vector<PeakGroup*>& groups);
	void refineModel(PeakGroup* grp);
	void saveModel(string filename);
	void loadModel(string filename);
	bool hasModel();

private:
	vector<float> getFeatures(Peak& p);
	void classify(Peak&p);
	void findOptimalSplit(vector<Peak*>&peaks, int fNum);

};
#endif

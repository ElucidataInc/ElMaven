#ifndef CLASSIFER_H
#define CLASSIFER_H

#include "mzSample.h"
#include "../libmaven/Matrix.h"
#include <nnwork.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

class Classifier
{

public:
	Classifier();
	virtual ~Classifier();
	
	//reimplemented in children
	virtual void classify(PeakGroup* grp)=0;
	virtual void train(vector<PeakGroup*>& groups)=0;
	virtual void refineModel(PeakGroup* grp)=0;
	virtual void saveModel(string filename)=0;
	virtual void loadModel(string filename)=0;
	virtual bool hasModel()=0;
	
	//common non virtal functions
	void classify(vector<PeakGroup*>& groups);
	void saveFeatures(vector<PeakGroup*>& groups, string filename);
	vector<Peak*> removeRedundacy(vector<Peak*>&peaks);

	inline string getModelFilename() { return _filename; }
	int getLabelsCount () { return labels.size(); }
	int getNumFeatures() { return num_features; }
	string getClassifierType() { return clsf_type; }
	void printLabelDistribution();

private:
	virtual vector<float> getFeatures(Peak& p) = 0;

protected: 
	string _filename;

	string clsf_type;
	int num_features;
    vector<string> features_names;
	vector< vector<float> > FEATURES;
	vector<char>labels;
};
#endif

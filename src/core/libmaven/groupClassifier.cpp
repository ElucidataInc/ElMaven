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
/*
void groupClassifier::saveModel(string filename) {
	if (network == NULL)
		return;
	network->save((char*) filename.c_str());
}
*/
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
/*
void groupClassifier::scoreEICs(vector<EIC*> &eics)
{

	for (unsigned int i = 0; i < eics.size(); i++)
	{
		for (unsigned int j = 0; j < eics[i]->peaks.size(); j++ ) {
			eics[i]->peaks[j].quality = scorePeak(eics[i]->peaks[j]);
		}
	}
}
*/
float groupClassifier::scoreGroup(PeakGroup* grp) {
   //Merged with Maven776 - Kiran
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

/*
void groupClassifier::refineModel(PeakGroup* grp) {
	if (grp == NULL)
		return;
	if (network == NULL)
		network = new nnwork(num_features, hidden_layer, num_outputs);

	if (grp->label == 'g' || grp->label == 'b') {
		for (unsigned int j = 0; j < grp->peaks.size(); j++) {
			Peak& p = grp->peaks[j];
			p.label = grp->label;
			if (p.width < 2)
				p.label = 'b';
			if (p.signalBaselineRatio <= 1)
				p.label = 'b';

			float result[2] = { 0.1, 0.1 };
			p.label == 'g' ? result[0] = 0.9 : result[1] = 0.9;

			if (p.label == 'g' || p.label == 'b') {
				vector<float> features = getFeatures(grp->peaks[j]);
				FEATURES.push_back(features);
				labels.push_back(p.label);

				float fts[1000];
				for (int k = 0; k < num_features; k++)
					fts[k] = features[k];
				trainingSize++;
				network->train(fts, result, 0.0000001 / trainingSize,
						0.001 / trainingSize);
			}
		}
	}
}

void groupClassifier::train(vector<PeakGroup*>& groups) {
	FEATURES.clear();
	labels.clear();
	trainingSize = 0;
	for (unsigned int i = 0; i < groups.size(); i++) {
		PeakGroup* grp = groups[i];
		refineModel(grp);
	}
}
*/
/*
 if (grp == NULL )continue;
 if (grp->label == 'g' || grp->label == 'b' ) {
 for (unsigned int j=0; j < grp->peaks.size(); j++ ) {
 Peak& p = grp->peaks[j];
 p.label = grp->label;
 if ( p.width < 2 )  p.label='b';
 if ( p.signalBaselineRatio <= 1 )  p.label='b';
 if ( p.label == 'b' ) {  badpeaks.push_back(&p); }
 if ( p.label == 'g' ) {  goodpeaks.push_back(&p); }

 allpeaks.push_back(&p);
 labels.push_back(p.label);
 }
 }
 }
 if (goodpeaks.size() == 0 || badpeaks.size() == 0) return;

 goodpeaks = removeRedundacy(goodpeaks);
 badpeaks =  removeRedundacy(badpeaks);

 int totalPeaks = goodpeaks.size() + badpeaks.size();
 int g=0;
 int b=0;

 for(int i=0; i < totalPeaks; i++ ) {
 Peak* p= NULL;
 if (i % 2 == 0 ) {
 if (g < goodpeaks.size() ) p = goodpeaks[g];
 g++;
 } else  {
 if (b < badpeaks.size() ) p = badpeaks[b];
 b++;
 }

 if (p) {
 float result[2] = {0.1, 0.1};
 p->label == 'g' ? result[0]=0.9 : result[1]=0.9;
 //cerr << p->label << endl;
 //
 float fts[1000];
 vector<float> features = getFeatures(*p);
 for(int k=0;k<num_features;k++) fts[k]=features[k];

 network->train(fts, result, 0.0000001/(i+1), 0.2/(i+1));
 }
 if (g >= goodpeaks.size() && b >= badpeaks.size()) break;
 }
 cerr << "Done training. " << endl;

 */

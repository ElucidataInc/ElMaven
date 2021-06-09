#include "classifierNaiveBayes.h"
#include "mzSample.h"
#include "Peak.h"
#include "PeakGroup.h"

ClassifierNaiveBayes::ClassifierNaiveBayes() {
	clsf_type = "NaiveBayes";
	num_features = 11;
	FEATURES.clear();
	features_names.push_back("peakAreaFractional"); //f1
	features_names.push_back("noNoiseFraction");    //f2
	features_names.push_back("symmetry");           //f3
	features_names.push_back("groupOverlap");		//f4
	features_names.push_back("gaussFitR2");			//f5
	features_names.push_back("S/N");				//f6
	features_names.push_back("peakRank");			//f7
	features_names.push_back("zeroBaseLine");		//f8
	features_names.push_back("tinyPeak");			//f9
	features_names.push_back("symmetry<5");             //f10
	features_names.push_back("S/N<3");             //f11
}

ClassifierNaiveBayes::~ClassifierNaiveBayes() {
	FEATURES.clear();
	labels.clear();
}

bool ClassifierNaiveBayes::hasModel() {
	return (FEATURES.size()) > 0;
}

void ClassifierNaiveBayes::saveModel(string filename) {
	ofstream fout(filename.c_str());
	if (!fout.is_open())
		return;

	fout << '#' << getClassifierType() << endl;
	fout << "#label,";
	for (int i = 0; i < num_features; i++) {
		fout << features_names[i];
		if (i + 1 < num_features)
			fout << ",";
	}
	fout << endl;

	for (unsigned int i = 0; i < FEATURES.size(); i++) {
		fout << labels[i] << ",";
		fout << setprecision(8);
		for (unsigned int j = 0; j < FEATURES[i].size(); j++) {
			fout << FEATURES[i][j];
			if (j + 1 < num_features)
				fout << ",";
		}
		fout << endl;
	}
	fout.close();
}

void ClassifierNaiveBayes::loadModel(string filename) {
	ifstream myfile(filename.c_str());
	if (!myfile.is_open())
		return;

	FEATURES.clear();
	labels.clear();

	string line;
	vector<string> fields;
	while (getline(myfile, line)) {
		if (!line.empty() && line[0] == '#')
			continue;
		fields.clear();
        fields = mzUtils::split(line, ",");
		if (fields.size() == 0)
			continue;

		char label = fields[0][0];

		if (label == 'g' || label == 'b') {
			if (num_features >= fields.size())
				continue;

			vector<float> featureVector(num_features, 0);
			for (int j = 1; j <= num_features; j++) {
				//offset by one, first column is label
				featureVector[j - 1] = string2float(fields[j]);
			}

			FEATURES.push_back(featureVector);
			labels.push_back(label);
		}
	}
	cerr << "Load Model: Feature Count=" << labels.size() << endl;
	myfile.close();
	printLabelDistribution();
}

vector<float> ClassifierNaiveBayes::getFeatures(Peak& p) {
	vector<float> set(num_features, 0);
	if (p.width > 0) {
		set[0] = p.peakAreaFractional;
		set[1] = p.noNoiseFraction;
		set[2] = p.symmetry / (p.width + 1) * log2(p.width + 1);
		set[3] = abs(p.groupOverlapFrac) > 0 ? log(abs(p.groupOverlapFrac)) : 0;
		set[4] = p.gaussFitR2 * 100.0;
		set[5] =
				p.signalBaselineRatio > 0 ?
						log2(p.signalBaselineRatio) / 10.0 : 0;
		set[6] = p.peakRank / 10.0;
		set[7] = p.peakBaseLineLevel < 100.0 ? 1.0 : 0.0;
		set[8] = p.width <= 3 ? 1 : 0;
		set[9] = p.symmetry <= 5 ? 1 : 0;
		set[10] = p.signalBaselineRatio <= 3 ? 1.0 : 0.0;
		if (p.peakRank / 10.0 > 1)
			set[6] = 1;
	}
	return set;
}

void ClassifierNaiveBayes::classify(Peak&p) {
	if (FEATURES.size() == 0)
		return;

	vector<float> A = getFeatures(p);
	if (A.size() == 0)
		return;

	int Gcount = 0;
	int Bcount = 0;
	for (unsigned int i = 0; i < labels.size(); i++) {
		if (labels[i] == 'g')
			Gcount++;
		else if (labels[i] == 'b')
			Bcount++;
	}
	if (Gcount == 0 || Bcount == 0)
		return;

	cerr << "Gcount=" << Gcount << " Bcount=" << Bcount << " LABEL=" << p.label
			<< endl;
	p.quality = 0.5;

	for (int jj = 0; jj < num_features; jj++) {
		float rssG = 0;
		float rssB = 0;
		StatisticsVector<float> distG;
		StatisticsVector<float> distB;

		for (unsigned int ii = 0; ii < labels.size(); ii++)
			if (labels[ii] == 'g') {
                                float rss = SQUARE(A[jj] - FEATURES[ii][jj]);
				distG.push_back(rss);
				rssG += rss;
			}

		for (unsigned int ii = 0; ii < labels.size(); ii++)
			if (labels[ii] == 'b') {
                                float rss = SQUARE(A[jj] - FEATURES[ii][jj]);
				distB.push_back(rss);
				rssB += rss;
			}

		rssB = distB.mean();
		rssG = distG.mean();
		if (rssG == 0 && rssB == 0)
			continue;
		if (rssG < rssB)
			if (rssG == 0)
				rssG = 1 / 10 * rssB;
		float w = 0.3 + 1 / (1 + exp(-1 * rssB / rssG));
		p.quality *= w;
		cerr << features_names[jj] << "=" << A[jj] << "\trss(b,g)=" << rssB
				<< " " << rssG << "\tw=" << w << "\tq=" << p.quality << endl;
	}

	if (p.quality > 1.0)
		p.quality = 1;
	if (p.quality < 0.0)
		p.quality = 0;

	/*
	 if (p.label == 'g' && p.quality < 0.5 ) cerr << "MISSCLASSIFIED";
	 if (p.label == 'b' && p.quality > 0.5 ) cerr << "MISSCLASSIFIED";
	 cerr << endl;
	 */
}

void ClassifierNaiveBayes::classify(PeakGroup* grp) {
	for (unsigned int i = 0; i < grp->peaks.size(); i++) {
		Peak& p = grp->peaks[i];
		classify(p);
	}
}

/*
 for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
 Peak& p = grp->peaks[i];
 vector<float> A = getFeatures(p);


 for(unsigned int jj=0; jj < num_features; jj++ ) {
 for(unsigned int ii=0; ii < FEATURES.size(); ii++ ) {
 if (FEATURES[ii][jj] < A[jj] ) {
 //if (cache[ii].label == 'g' ) { pG++; } else { pB++; }
 }
 }
 }
 }
 */

/*
 double BEST=1e6;
 for (unsigned int i=0; i < grp->peaks.size(); i++ ) {
 Peak& p = grp->peaks[i];
 vector<float> A = getFeatures(p);

 for (unsigned int j=0; j < cache.size(); j++ ) {
 vector<float> B = getFeatures(cache[j]);

 double RSS=0;
 for(int k=0; k<B.size(); k++ ) { RSS += POW2(A[k]-B[k]); }
 RSS /= B.size();

 if ( RSS < BEST ) {
 cerr << "RSS=" << RSS << endl;
 BEST=RSS;  p.label=cache[j].label;
 }
 }

 if(p.label=='g') p.quality=1.0;
 if(p.label=='b') p.quality=0.0;
 }
 cerr << endl;
 */

/*
 if ( p.gaussFitR2*100.0 < 1 &&  p.peakAreaFractional > 0.02 &&  p.noNoiseFraction>0.02
 && p.symmetry > 2  && p.signalBaselineRatio > 3) {
 p.quality=1.0;
 }
 */

void ClassifierNaiveBayes::refineModel(PeakGroup* grp) {
	if (grp == NULL)
        return;
    if (grp->userLabel() == 'g' || grp->userLabel() == 'b') {
		for (unsigned int j = 0; j < grp->peaks.size(); j++) {
			Peak& p = grp->peaks[j];
            p.label = grp->userLabel();
			if (p.width < 2 || p.signalBaselineRatio <= 1)
				p.label = 'b';
			if (p.label == 'g' || p.label == 'b') {
				labels.push_back(p.label);
				FEATURES.push_back(getFeatures(p));
			}
		}
	}

	printLabelDistribution();
}

void ClassifierNaiveBayes::train(vector<PeakGroup*>& groups) {

	labels.clear();
	FEATURES.clear();

	for (unsigned int i = 0; i < groups.size(); i++) {
		PeakGroup* grp = groups[i];
		refineModel(grp);
	}
}

void ClassifierNaiveBayes::findOptimalSplit(vector<Peak*>&peaks, int fNum) {

	StatisticsVector<float> X(peaks.size());
	map<char, float> totals;
	vector<char> labels(peaks.size());

	for (unsigned int i = 0; i < peaks.size(); i++) {
		Peak* p = peaks[i];
		vector<float> features = getFeatures(*p);
		if (fNum < features.size()) {
			X[i] = features[fNum];
			labels[i] = p->label;
			totals[p->label]++;
		}
	}

	float minX = X.minimum();
	float maxX = X.maximum();
	if (minX == maxX) {
		minX -= 1;
		maxX += 1;
	}
	float step = (maxX - minX) / 50;

	for (float cut = minX; cut < maxX; cut += step) {
		map<char, float> counts;
		float pG = 0;
		float pB = 0;
		for (unsigned int i = 0; i < X.size(); i++)
			if (X[i] < cut)
				counts[labels[i]]++;
		if (totals.count('g'))
			pG = counts['g'] / totals['g'];
		if (totals.count('b'))
			pB = 1.0 - (counts['b'] / totals['b']);
		cerr << setprecision(2) << "CUT=" << cut << " pG=" << pG << " pB=" << pB
				<< " GAIN=" << 1 - (pG * pG + pB * pB) << endl;
	}
}


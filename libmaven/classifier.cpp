#include "classifier.h"

Classifier::Classifier() {
}
Classifier::~Classifier() {
}

vector<Peak*> Classifier::removeRedundacy(vector<Peak*>&peaks) {

	vector<Peak*> nrPeaks;
	if (peaks.size() > 0)
		nrPeaks.push_back(peaks[0]);

	for (unsigned int i = 1; i < peaks.size(); i++) {
		Peak* p1 = peaks[i];
		vector<float> A = getFeatures(*p1);

		bool unique = true;
		for (unsigned int j = 0; j < nrPeaks.size(); j++) {
			Peak* p2 = nrPeaks[j];
			vector<float> B = getFeatures(*p2);

			float rss = 0;
			for (int k = 0; k < num_features; k++)
				rss += POW2(A[k] - B[k]);
			rss /= num_features;
			if (rss < 0.02) {
				unique = false;
				break;
			}
		}

		if (unique)
			nrPeaks.push_back(p1);
	}
	cerr << "removeRedundacy() " << nrPeaks.size() << " " << peaks.size()
			<< endl;
	return nrPeaks;
}

void Classifier::saveFeatures(vector<PeakGroup*>& groups, string filename) {
	ofstream fout(filename.c_str());

	//print out header
	if (features_names.size() > 0) {
		fout << "class,groupId,quality,";
		for (int i = 0; i < num_features; i++) {
			fout << features_names[i];
			if (i + 1 < num_features)
				fout << ",";
		}
		fout << endl;
	}

	for (unsigned int g = 0; g < groups.size(); g++) {
		PeakGroup* grp = groups[g];
		if (grp == NULL)
			continue;
		for (unsigned int j = 0; j < grp->peaks.size(); j++) {
			vector<float> features = getFeatures(grp->peaks[j]);
			fout << grp->peaks[j].label << ",";
			fout << grp->groupId << ",";
			fout << grp->peaks[j].quality << ",";
			for (int i = 0; i < num_features; i++) {
				fout << features[i];
				if (i + 1 < num_features)
					fout << ",";
			}
			fout << endl;
		}
	}
	fout.close();
}

void Classifier::classify(vector<PeakGroup*>& groups) {
	for (unsigned int i = 0; i < groups.size(); i++) {
		PeakGroup* grp = groups[i];
		classify(grp);
	}
}

void Classifier::printLabelDistribution() {
	map<char, int> counts;
	map<char, int>::iterator itr;

	for (unsigned int i = 0; i < labels.size(); i++)
		counts[labels[i]]++;
	cerr << "refineModel FEATURES=" << labels.size() << endl;
	for (itr = counts.begin(); itr != counts.end(); itr++) {
		cerr << "\t\t label=" << (*itr).first << " count=" << (*itr).second
				<< endl;
	}

	cerr << "\tfeature\t\tClass=b\t\tClass=g" << endl;
	for (int jj = 0; jj < num_features; jj++) {
		StatisticsVector<float> distG;
		StatisticsVector<float> distB;
		for (unsigned int ii = 0; ii < labels.size(); ii++) {
			if (labels[ii] == 'g')
				distG.push_back(FEATURES[ii][jj]);
			else if (labels[ii] == 'b')
				distB.push_back(FEATURES[ii][jj]);
		}
		cerr << setprecision(3) << features_names[jj] << "\t\t" << distB.mean()
				<< "+/-" << distB.stddev() << "\t\t" << distG.mean() << "+/-"
				<< distG.stddev() << endl;
	}
}


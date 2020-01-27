#include "comparesampleslogic.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "PeakGroup.h"

CompareSamplesLogic::CompareSamplesLogic() {
}

void CompareSamplesLogic::shuffle(StatisticsVector<float>& groupA,
		StatisticsVector<float>& groupB) {
	int n1 = groupA.size();
	int n2 = groupB.size();
	int n3 = n1 + n2;
	if (n1 == 0 || n2 == 0)
		return;

	//combine two sets
	//cerr << "N3=" << n3 << endl;
	StatisticsVector<float> groupC(n1 + n2);
	for (int i = 0; i < n1; i++)
		groupC[i] = groupA[i];
	for (int i = 0; i < n2; i++)
		groupC[n1 + i] = groupB[i];
	//for(int i=0; i < n3; i++ ) cerr << groupC[i] << " "; cerr << endl;

	//float realT = ttest(groupA,ratio>100groupB);

	for (int s = 0; s < 10; s++) {
		//shuffle combined set
		for (int i = 0; i < n3; i++) {
			int r = randInt(0,n3 - 1);
			assert(r <= n3 - 1);
			if (i == r)
				continue;
			float tmp = groupC[i];
			groupC[i] = groupC[r];
			groupC[r] = tmp; //swap
		}

		//split random vector
		StatisticsVector<float> tmpA(n1);
		StatisticsVector<float> tmpB(n2);
		for (int i = 0; i < n1; i++)
			tmpA[i] = groupC[i];
		for (int i = 0; i < n2; i++)
			tmpB[i] = groupC[n1 + i];

		//for(int i=0; i < n1; i++ ) cerr << tmpA[i] << " "; cerr << endl;
		//for(int i=0; i < n2; i++ ) cerr << tmpB[i] << " "; cerr << endl;

		//ttest
		float randT = abs(mzUtils::ttest(tmpA, tmpB));
		//cerr << "RealT=" << realT << "  RandT=" << randT << endl;
		rand_scores.push_back(randT);
	}
	/*
	 std::sort(rand_scores.begin(), rand_scores.end());
	 int rank = countBelow(rand_scores,  ttest(groupA,groupB));
	 cerr << "Q=" << rank << " p-value=" << 1.0-(float) rank / rand_scores.size() << endl;
	 */

}

void CompareSamplesLogic::FDRCorrection(QList<PeakGroup*> allgroups,
		int correction) {

	int Ngroups = allgroups.size();
	int j = 0;
	sort(allgroups.begin(), allgroups.end(), PeakGroup::compPvalue);
	for (int i = 0; i < Ngroups; i++) {
		PeakGroup* group = allgroups[i];
		if (group->changeFoldRatio == 0)
			continue;
		//cerr << group->changePValue << " " ;
		if (correction == 1) {	 	//Bonferroni
			group->changePValue *= Ngroups;
		}
		if (correction == 2) { 	//HOLMS
			group->changePValue *= Ngroups - j;
		}
		if (correction == 3) { 	//Benjamini
			group->changePValue *= Ngroups / (j + 1);
		}
		//cerr << group->changePValue << endl;
		j++;
	}

}

void CompareSamplesLogic::computeMinPValue(QList<PeakGroup*> allgroups) {

	std::sort(rand_scores.begin(), rand_scores.end()); //sort random scores,
	for (int i = 0; i < allgroups.size(); i++) {
		PeakGroup* group = allgroups[i];
		if (group->changeFoldRatio == 0)
			continue;
		int rank = countBelow(rand_scores, group->changePValue); //calculate p-value
		//cerr << group->changePValue << " " <<  ((float) rank)/rand_scores.size() << endl;
		group->changePValue = 1 - ((float) rank) / rand_scores.size();

	}
}

void CompareSamplesLogic::computeStats(PeakGroup* group,
		vector<mzSample*> sampleSet, vector<mzSample*> sset1,
		vector<mzSample*> sset2, float _missingValue) {
	group->changeFoldRatio = 0;		//TODO why?
	group->changePValue = 1;		//TODO why?

    //group->groupStatistics();
	vector<float> yvalues = group->getOrderedIntensityVector(sampleSet,
			PeakGroup::AreaTop);

	StatisticsVector<float> groupA(sset1.size());
	StatisticsVector<float> groupB(sset2.size());

	// Was missing - Kiran
	// TODO: misingSet1 and 2 is not used anywhere - Kiran
    int missingSet1=0;
	int missingSet2=0;
	for(unsigned int i=0; i < sset1.size(); i++ ) { groupA[i]=yvalues[i]; if (groupA[i] == 0) missingSet1++; }
	for(unsigned int i=0; i < sset2.size(); i++ ) { groupB[i]=yvalues[ sset1.size()+i ]; if (groupB[i] ==0 ) missingSet2++; }
	
	for (unsigned int i = 0; i < groupA.size(); i++)
		if (groupA[i] < _missingValue)
			groupA[i] = _missingValue;
	for (unsigned int i = 0; i < groupB.size(); i++)
		if (groupB[i] < _missingValue)
			groupB[i] = _missingValue;

    //skip empty
	float meanA = abs(groupA.mean());
	float meanB = abs(groupB.mean());

    //Equation Updated -Kiran
    if (meanA <= 0)
		meanA = 1;
	if (meanB <= 0)
		meanB = 1;

    //Equation Updated -Kiran
    group->changeFoldRatio = log2(meanA/meanB);
	group->changePValue = abs(mzUtils::ttest(groupA, groupB));
	shuffle(groupA, groupB);
}

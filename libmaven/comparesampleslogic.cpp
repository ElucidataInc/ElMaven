#include "comparesampleslogic.h"

CompareSamplesLogic::CompareSamplesLogic() {
}

void CompareSamplesLogic::shuffle(StatisticsVector<float>& groupA,
		StatisticsVector<float>& groupB) {
	int n1 = groupA.size();
	int n2 = groupB.size();
	int n3 = n1 + n2;
	if (n1 == 0 || n2 == 0)
		return;
	MTRand mtrand;

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
			int r = mtrand.randInt(n3 - 1);
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

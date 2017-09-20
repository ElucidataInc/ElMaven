#ifndef COMPARESAMPLESLOGIC_H
#define COMPARESAMPLESLOGIC_H

#include <cstdlib>
#include <vector>

#include "mzUtils.h"
#include "MersenneTwister.h"
#include "statistics.h"
#include "assert.h"
#include <QList>
#include <cmath>
#include "mzSample.h"

class CompareSamplesLogic {
public:
	CompareSamplesLogic();

	void shuffle(StatisticsVector<float>& groupA,
			StatisticsVector<float>& groupB);
	void FDRCorrection(QList<PeakGroup*> allgroups, int correction);
	void computeMinPValue(QList<PeakGroup*> allgroups);
	void computeStats(PeakGroup* group, vector<mzSample*> sampleSet,
			vector<mzSample*> sset1, vector<mzSample*> sset2,
			float _missingValue);

	StatisticsVector<float> rand_scores;
	StatisticsVector<float> real_scores;

};
#endif // COMPARESAMPLESLOGIC_H

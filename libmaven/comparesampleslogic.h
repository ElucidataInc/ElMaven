#ifndef COMPARESAMPLESLOGIC_H
#define COMPARESAMPLESLOGIC_H

#include <cstdlib>
#include <vector>

#include "mzUtils.h"
#include "MersenneTwister.h"
#include "statistics.h"
#include "assert.h"

#include <cmath>

class CompareSamplesLogic {
public:
	CompareSamplesLogic();
	void shuffle(StatisticsVector<float>& groupA,
			StatisticsVector<float>& groupB);
	StatisticsVector<float> rand_scores;
	StatisticsVector<float> real_scores;
};
#endif // COMPARESAMPLESLOGIC_H

#ifndef COMPARESAMPLESLOGIC_H
#define COMPARESAMPLESLOGIC_H

#include <QList>

#include "assert.h"
#include "standardincludes.h"
#include "statistics.h"

class mzSample;
class PeakGroup;

class CompareSamplesLogic {
public:
	CompareSamplesLogic();

	void shuffle(StatisticsVector<float>& groupA,
			StatisticsVector<float>& groupB);
    void FDRCorrection(QList<std::shared_ptr<PeakGroup> > allgroups, int correction);
    void computeMinPValue(QList<std::shared_ptr<PeakGroup> > allgroups);
        void computeStats(PeakGroup* group, std::vector<mzSample*> sampleSet,
                        std::vector<mzSample*> sset1, std::vector<mzSample*> sset2,
			float _missingValue);
        int countBelow(std::vector<float>& y, float ymax);
	StatisticsVector<float> rand_scores;
	StatisticsVector<float> real_scores;

};
#endif // COMPARESAMPLESLOGIC_H

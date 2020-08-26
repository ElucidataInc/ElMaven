//@Kailash: getting features of a group

#ifndef GROUP_FEATURES_H
#define GROUP_FEATURES_H

#include "standardincludes.h"

class PeakGroup;

using namespace std;

vector<float> getFeatures(PeakGroup* grp);
float getAvgPeakAreaFractional(PeakGroup* grp);
float getAvgNoNoiseFraction(PeakGroup* grp);
float getAvgSymmetry(PeakGroup* grp);
float getAvgWidth(PeakGroup* grp);
float getLogAvgSignalBaselineRatio(PeakGroup* grp);
float getLogAvgPeakIntensity(PeakGroup* grp);
float getMinGaussFitR2(PeakGroup* grp);
float getGroupOverlapFrac(PeakGroup* grp);

#endif

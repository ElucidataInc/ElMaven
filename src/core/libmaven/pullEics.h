#ifndef PULLEICS_H
#define PULLEICS_H

#include <map>
#include <vector>
#include <iostream>

#include "mzSample.h"
#include "Compound.h"

using namespace std;

class PullEICs
{

    public: 
        /**
         * @param slice mzSlice object
         * @param samples vector of samples
         * @param peakDetect enum in EicLoader
         * @param soomthingWindow EIC smoothing window (in Scans)
         * @param smoothingAlgorithm Smoothing algorithm
         * @param amuQ1 precusor m/z delta
         * @param amuQ3 product m/z delta
         * @param baselineSmoothingWindow Baseline smoothing window
         * @param baselineDropTopX Drop top x% of intensities in an EIC
         *  for calculating baseline
         * @param minSignalBaselineDifference Minimun Signal to baseline
         *  difference
         * @param eicType Sum or Max
         * @param filterline Select filterline (srmId) (others will be rejected)
         * @see EicLoader
         * @see mzSlice
         * @see EIC
         * @return vector<EIC*> vector of EICs
         */
        static vector<EIC*> pullEICs(mzSlice* slice, vector<mzSample*>&samples,
            int peakDetect, int smoothingWindow, int smoothingAlgorithm,
            float amuQ1, float amuQ3, int baselineSmoothingWindow,
            int baselineDropTopX, double minSignalBaselineDifference, int eicType, string filterline);


};

#endif //PULLEICS_H

#ifndef DECONVOLUTION_H
#define DECONVOLUTION_H

#include "standardincludes.h"

class EIC;

namespace Deconvolution
{
    using namespace std;

    // here "C" is our target signal
    enum class Pattern {
        C,
        BC,
        CD,
        ABC,
        BCD,
        CDE,
        ABCD,
        BCDE,
        ABCDE
    };

    struct SignalVector {
        Pattern pattern;
        size_t leftBound;
        size_t rightBound;
        vector<double> twoLeft;
        vector<double> oneLeft;
        vector<double> target;
        vector<double> oneRight;
        vector<double> twoRight;
    };

    vector<pair<size_t, size_t>> modelPeakRegions(EIC *eic,
                                                  int smoothingWindow,
                                                  float sigma,
                                                  int averagePeakWidth);

    SignalVector convolutedSignals(const pair<size_t, size_t>& roi,
                                   const vector<pair<size_t, size_t>>& regions,
                                   const EIC *eic);

    vector<float> execute(SignalVector convoluted,
                          vector<double> trueChromatogram);
};

#endif // DECONVOLUTION_H

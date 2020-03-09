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

    vector<pair<size_t, size_t>> modelPeakRegions(EIC *eic,
                                                  int smoothingWindow,
                                                  float sigma,
                                                  int averagePeakWidth);

    Pattern getPattern(bool hasOneLeft,
                       bool hasTwoLeft,
                       bool hasOneRight,
                       bool hasTwoRight);

    pair<Pattern, vector<vector<float>>>
    convolutedSignals(pair<size_t, size_t> roi,
                      vector<pair<size_t, size_t>> regions,
                      EIC* eic);
};

#endif // DECONVOLUTION_H

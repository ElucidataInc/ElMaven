#ifndef DECONVOLUTION_H
#define DECONVOLUTION_H

#include "standardincludes.h"

class EIC;

namespace Deconvolution
{
    using namespace std;

    /**
     * All the different patterns of complexed signals that can be deconvoluted.
     * Here "C" is our signal of interest. "A" and "B" are signals to the left
     * of our signal and "D" and "E" are signals to the right of it. In every
     * pattern "C" will certainly be present.
     */
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

    /**
     * @brief The SignalVector struct, as its name implies, is a collection of
     * signals (as intensity vectors), where one of them is of our interest (the
     * target) and the others surround our target (up to two on each side). The
     * exact pattern of covolution will be stored as an attribute. Two index
     * values are also present to denote the overall starting and ending point,
     * of the combined signals, w.r.t. the parent EIC's intensity vector.
     */
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

    /**
     * @brief Find regions in an EIC, which can be considered as a
     * chromatographic "peak", based on its shape and symmetry.
     * @details This method primarily checks for two properties of a candidate
     * "peak region", before accepting or rejecting it. The first property is
     * the sharpness value of a peak which is mean to check whether peaks in a
     * local region are symmetric enough in terms of noise units (see Stein et
     * al., 1999). The second property is the ideal slope which checks if there
     * are not too many spiky variations in a peak.
     * @param eic The extracted chromatogram in which peak regions need to be
     * detected.
     * @param smoothingWindow The smoothing window for Savitzky-Golay filter
     * that will be applied on the EIC's total signal, before searching for
     * local maxima.
     * @return A vector of pairs of indexes which denote regions in the given
     * EIC that can be considered to as "model" peaks.
     */
    vector<pair<size_t, size_t>> modelPeakRegions(EIC *eic,
                                                  int smoothingWindow);

    /**
     * @brief Given a region of interest (ROI), and its surrounding regions of
     * model peaks, finds which signals (from the given regions) overlap with
     * the signal at ROI.
     * @param roi The pair of boundaries that represent the region (and signal)
     * of interest. This pair should be present in the `regions` vector.
     * @param regions All model peak regions from an EIC.
     * @param eic The EIC in which the regions belong. This will be used to get
     * local intensity segments.
     * @return A `SignalVector` object, containing the target signal, the
     * neighbouring signals multiplexed with target (if needing deconvolution),
     * the pattern of convolution (`Pattern::C` would mean no deconvolution is
     * needed) and the left and right bounds (EIC indexes) for all component
     * signals that combine to form the true signal.
     */
    SignalVector convolutedSignals(const pair<size_t, size_t>& roi,
                                   const vector<pair<size_t, size_t>>& regions,
                                   const EIC *eic);

    /**
     * @brief execute Perform deconvolution on the given `SignalVector` data.
     * @param convoluted A `SignalVector` (probably obtained using the method
     * `convolutedSignals`), which contains a pattern of convolution and the
     * respective signals.
     * @param trueChromatogram The true chromatogram for the entire range of
     * scans from the far-left to the far-right of the signal vector.
     * @return An intensity vector representing the "true" elution profile of
     * the target signal.
     */
    vector<float> execute(SignalVector convoluted,
                          vector<double> trueChromatogram);
};

#endif // DECONVOLUTION_H

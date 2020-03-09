#include "deconvolution.h"
#include "EIC.h"
#include "mzUtils.h"
#include "Peak.h"

/**
 * @brief Local helper method that ensures that model peak regions are of at
 * least average width.
 * @details Consequently, some regions might end up sharing some left or right
 * boundaries (or maybe even overlap), with other regions. This makes them a
 * candidate for deconvolution.
 */
vector<pair<size_t, size_t>>
_refineModelRegions(EIC *eic,
                    vector<pair<size_t, size_t>> regions,
                    int averagePeakWidth)
{
    vector<pair<size_t, size_t>> refinedRegions;
    for (auto& region : regions) {
        auto signal = eic->intensitySegment(region.first, region.second, true);
        size_t peakTop = distance(begin(signal),
                                  max_element(begin(signal), end(signal)));
        if (peakTop < 0 || peakTop >= signal.size())
            continue;

        size_t peakLeft = 0;
        for (size_t i = peakTop; i > 0; --i) {
            if (peakTop - i < (averagePeakWidth / 2))
                continue;
            if (signal[i - 1] >= signal[i]) {
                peakLeft = i;
                break;
            }
        }
        size_t peakRight = signal.size() - 1;
        for (size_t i = peakTop; i < signal.size() - 1; ++i) {
            if (i - peakTop < (averagePeakWidth / 2))
                continue;
            if (signal[i] <= signal[i + 1]) {
                peakRight = i;
                break;
            }
        }

        // translate peak boundaries to positions on parent EIC
        peakLeft += region.first;
        peakRight += region.first;

        // if this segment already exists (probably the last one), we ignore it
        auto finalSegment = make_pair(peakLeft, peakRight);
        if (!refinedRegions.empty() && finalSegment == refinedRegions.back())
            continue;

        signal = eic->intensitySegment(peakLeft, peakRight, true);
        if (signal.empty())
            continue;

        // peak regions must not be too noisy
        auto idealSlope = mzUtils::idealSlopeValue(signal);
        // TODO: increase threshold when, derivative method is replaced.
        if (isnan(idealSlope) || idealSlope < 0.90)
            continue;

        // the final segment should now be as wide as the average peak
        refinedRegions.push_back(finalSegment);
    }
    return refinedRegions;
}

vector<pair<size_t, size_t>>
Deconvolution::modelPeakRegions(EIC* eic,
                                int smoothingWindow,
                                float sigma,
                                int averagePeakWidth)
{
    if (eic->size() == 0)
        return {};

    // "currently this value should be enough for LC", why?
    float halfPoint = 10.0f;
    size_t filterSize = 2 * static_cast<size_t>(halfPoint) + 1;
    vector<float> filter;
    for (int i = 0; i < filterSize; i++) {
        float coeff = (1 - powf((-halfPoint + i) / sigma, 2.0f))
                       * exp(-0.5 * powf((-halfPoint + i) / sigma, 2.0f));
        filter.push_back(coeff);
    }

    // this will destructively modify the peaks within the EIC,
    // but then we intend to find better peaks, so it should be fine
    eic->getPeakPositions(smoothingWindow);
    if (eic->peaks.empty())
        return {};

    vector<pair<size_t, size_t>> peakRegions;
    sort(begin(eic->peaks), end(eic->peaks), Peak::compRt);
    for (const auto& peak : eic->peaks) {
        if (peak.width < 3)
            continue;

        peakRegions.push_back(make_pair(peak.minpos, peak.maxpos));
    }

    vector<pair<size_t, size_t>> modelRegions;
    for (size_t i = 0; i < peakRegions.size(); i++) {
        float sum = 0.0f;
        for (int j = -1 * halfPoint; j <= halfPoint; j++) {
            if (i + j < 0 || i + j > peakRegions.size() - 1) {
                continue;
            } else {
                auto signal = eic->intensitySegment(peakRegions.at(i + j).first,
                                                    peakRegions.at(i + j).second,
                                                    true);
                auto sharpness = mzUtils::sharpnessValue(signal);
                sum += sharpness * filter[static_cast<size_t>(j + halfPoint)];
            }
        }

        // these zero sum regions do not contain any "model" peaks
        if (sum == 0.0f)
            continue;

        modelRegions.push_back(peakRegions.at(i));
    }

    return _refineModelRegions(eic, modelRegions, averagePeakWidth);
}

Deconvolution::Pattern Deconvolution::getPattern(bool hasOneLeft,
                                                 bool hasTwoLeft,
                                                 bool hasOneRight,
                                                 bool hasTwoRight)
{
    if (!hasOneLeft && !hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Pattern::C;
    if (hasOneLeft && !hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Pattern::BC;
    if (!hasOneLeft && !hasTwoLeft && hasOneRight && !hasTwoRight)
        return Pattern::CD;
    if (hasOneLeft && !hasTwoLeft && hasOneRight && !hasTwoRight)
        return Pattern::BCD;
    if (hasOneLeft && hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Pattern::ABC;
    if (!hasOneLeft && !hasTwoLeft && hasOneRight && hasTwoRight)
        return Pattern::CDE;
    if (hasOneLeft && hasTwoLeft && hasOneRight && !hasTwoRight)
        return Pattern::ABCD;
    if (hasOneLeft && !hasTwoLeft && hasOneRight && hasTwoRight)
        return Pattern::BCDE;
    if (hasOneLeft && hasTwoLeft && hasOneRight && hasTwoRight)
        return Pattern::ABCDE;
    return Pattern::C;
}

pair<Deconvolution::Pattern, vector<vector<float>>>
Deconvolution::convolutedSignals(pair<size_t, size_t> roi,
                                 vector<pair<size_t, size_t>> regions,
                                 EIC* eic)
{
    bool hasOneLeft = false;
    bool hasOneRight = false;
    bool hasTwoLeft = false;
    bool hasTwoRight = false;

    auto foundAt = find(begin(regions), end(regions), roi);
    if (foundAt == end(regions))
        return {Pattern::C, {}};

    int positionOfRoi = foundAt - begin(regions);

    // the next few checks, all assume that segments were sent in increasing
    // order of retention time, while checking whether the ROI has peaks to the
    // left or right and, if so, whether their boundaries overlap with ROI
    if (positionOfRoi > 1
        && regions.at(positionOfRoi - 2).second >= roi.first) {
        hasTwoLeft = true;
    }
    if (positionOfRoi > 0
        && regions.at(positionOfRoi - 1).second >= roi.first) {
        hasOneLeft = true;
    }
    if (positionOfRoi < static_cast<int>(regions.size()) - 2
        && regions.at(positionOfRoi + 2).first <= roi.second) {
        hasTwoRight = true;
    }
    if (positionOfRoi < static_cast<int>(regions.size()) - 1
        && regions.at(positionOfRoi + 1).first <= roi.second) {
        hasOneRight = true;
    }

    float leftBound = roi.first;
    float rightBound = roi.second;
    if (hasTwoLeft) {
        leftBound = min(roi.first,
                        min(regions.at(positionOfRoi - 2).first,
                            regions.at(positionOfRoi - 1).first));
    } else if (hasOneLeft) {
        leftBound = min(roi.first,
                        regions.at(positionOfRoi - 1).first);
    }
    if (hasTwoRight) {
        rightBound = max(roi.second,
                         max(regions.at(positionOfRoi + 2).second,
                             regions.at(positionOfRoi + 1).second));
    } else if (hasOneRight) {
        rightBound = max(roi.second,
                         regions.at(positionOfRoi + 1).second);
    }

    // create 5 model signals that can possibly be resolved using our
    // deconvolution techniques - at max two on the left and two on the
    // right of our target signal
    vector<float> targetSignal;
    vector<float> oneLeftSignal;
    vector<float> twoLeftSignal;
    vector<float> oneRightSignal;
    vector<float> twoRightSignal;

    // fill each signal based on its region's boundaries, padding zeros where
    // the index is outside the bounds such that all five are of the same length
    for (size_t i = leftBound; i <= rightBound; ++i) {
        if (roi.first > i || roi.second < i) {
            targetSignal.push_back(0);
        } else {
            targetSignal.push_back(eic->intensity[i]);
        }

        if (hasTwoLeft) {
            if (regions.at(positionOfRoi - 2).first > i
                || regions.at(positionOfRoi - 2).second < i) {
                twoLeftSignal.push_back(0);
            } else {
                twoLeftSignal.push_back(eic->intensity[i]);
            }
        }

        if (hasOneLeft || hasTwoLeft) {
            if (regions.at(positionOfRoi - 1).first > i
                || regions.at(positionOfRoi - 1).second > i) {
                oneLeftSignal.push_back(0);
            } else {
                oneLeftSignal.push_back(eic->intensity[i]);
            }
        }

        if (hasTwoRight) {
            if (regions.at(positionOfRoi + 2).first > i
                || regions.at(positionOfRoi + 2).second < i) {
                twoRightSignal.push_back(0);
            } else {
                twoRightSignal.push_back(eic->intensity[i]);
            }
        }

        if (hasOneRight || hasTwoRight) {
            if (regions.at(positionOfRoi + 1).first > i
                || regions.at(positionOfRoi + 1).second > i) {
                oneRightSignal.push_back(0);
            } else {
                oneRightSignal.push_back(eic->intensity[i]);
            }
        }
    }

    vector<vector<float>> signalVector;
    if (hasTwoLeft)                             // A
        signalVector.push_back(twoLeftSignal);
    if (hasOneLeft)                             // B
        signalVector.push_back(oneLeftSignal);
    signalVector.push_back(targetSignal);       // C
    if (hasOneRight)                            // D
        signalVector.push_back(oneRightSignal);
    if (hasTwoRight)                            // E
        signalVector.push_back(twoRightSignal);

    auto pattern = getPattern(hasOneLeft, hasTwoLeft, hasOneRight, hasTwoRight);
    return make_pair(pattern, signalVector);
}

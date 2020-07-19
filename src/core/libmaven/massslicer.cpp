#include <omp.h>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "massslicer.h"
#include "EIC.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "datastructures/mzSlice.h"
#include "masscutofftype.h"
#include "mzUtils.h"
#include "Matrix.h"
#include "PeakDetector.h"
#include "Scan.h"

using namespace mzUtils;

MassSlicer::MassSlicer()
{
    _maxSlices = numeric_limits<int>::max();
    _minRt = numeric_limits<float>::min();
    _minMz = numeric_limits<float>::min();
    _minIntensity = numeric_limits<float>::min();
    _maxRt = numeric_limits<float>::max();
    _maxMz = numeric_limits<float>::max();
    _maxIntensity = numeric_limits<float>::max();
    _minCharge = 0;
    _maxCharge = numeric_limits<int>::max();
    _massCutoff = nullptr;
}

MassSlicer::~MassSlicer() { delete_all(slices); }

void MassSlicer::sendSignal(const string& progressText,
                unsigned int completed_samples,
                int total_samples)
{
    _mavenParameters->sig(progressText, completed_samples, total_samples);
}

void MassSlicer::stopSlicing() {
    if (slices.size() > 0) {
        delete_all(slices);
        slices.clear();
    }
}

void MassSlicer::findFeatureSlices(MassCutoff* massCutoff, int rtStep )
{
    // clear all previous data
    delete_all(slices);
    slices.clear();

    float rtWindow = 2.0f;
    this->_massCutoff = massCutoff;

    int totalScans = 0;
    int currentScans = 0;

    // Calculate the total number of scans
    for (auto s : _samples)
        totalScans += s->scans.size();

    // Calculating the rt window using average distance between RTs and
    // mutiplying it with rtStep (default 2.0)
    if (_samples.size() > 0 and rtStep > 0) {
        rtWindow = accumulate(begin(_samples),
                              end(_samples),
                              0.0f,
                              [rtStep](float sum, mzSample* sample) {
                                  return sum + (sample->getAverageFullScanTime()
                                                * rtStep);
                              }) / static_cast<float>(_samples.size());
    }
    cerr << "RT window used: " << rtWindow << endl;

    sendSignal("Status", 0 , 1);

    // #pragma omp parallel for ordered
    // Looping over every sample
    for (unsigned int i = 0; i < _samples.size(); i++) {
        if (slices.size() > _maxSlices) break;

        // Check if Peak detection has been cancelled by the user
        if (_mavenParameters->stop) {
            stopSlicing();
            break;
        }

        // updating progress on samples
        if (_mavenParameters->showProgressFlag) {
            string progressText = "Processing "
                                  + to_string(i + 1)
                                  + " out of "
                                  + to_string(_mavenParameters->samples.size())
                                  + " sample(s)…";
            sendSignal(progressText, currentScans, totalScans);
        }

        // #pragma omp cancel for
        // for loop for iterating over every scan of a sample
        for (auto scan : _samples[i]->scans) {
            // Check if Peak detection has been cancelled by the user
            if (_mavenParameters->stop) {
                stopSlicing();
                break;
            }

            currentScans++;

            if (scan->mslevel != 1)
                continue;

            // Checking if RT is in the given min to max RT range
            if (_maxRt && !isBetweenInclusive(scan->rt, _minRt, _maxRt))
                continue;

            float rt = scan->rt;

            for (unsigned int k = 0; k < scan->nobs(); k++) {
                float mz = scan->mz[k];
                float intensity = scan->intensity[k];

                // Checking if mz, intensity are within specified ranges
                if (_maxMz && !isBetweenInclusive(mz,
                                                  _minMz,
                                                  _maxMz)) {
                    continue;
                }
                if (_maxIntensity && !isBetweenInclusive(intensity,
                                                         _minIntensity,
                                                         _maxIntensity)) {
                    continue;
                }

                // create new slice with the given bounds
                float cutoff = massCutoff->massCutoffValue(mz);
                mzSlice* s = new mzSlice(mz - cutoff,
                                         mz + cutoff,
                                         rt - rtWindow,
                                         rt + rtWindow);
                s->ionCount = intensity;
                s->rt = scan->rt;
                s->mz = mz;
                slices.push_back(s);
            }

            // progress update 
            if (_mavenParameters->showProgressFlag ) {
                string progressText = "Processing "
                                      + to_string(i + 1)
                                      + " out of "
                                      + to_string(_mavenParameters->samples.size())
                                      + " sample(s)…\n"
                                      + to_string(slices.size())
                                      + " slices created";
                sendSignal(progressText,currentScans,totalScans);
            }
        }
    }

    cerr << "Found " << slices.size() << " slices" << endl;

    // before reduction sort by mz first then by rt
    sort(begin(slices),
         end(slices),
         [](const mzSlice* slice, const mzSlice* compSlice) {
             if (slice->mz == compSlice->mz) {
                 return slice->rt < compSlice->rt;
             }
             return slice->mz < compSlice->mz;
         });
    _reduceSlices();

    cerr << "Reduced to " << slices.size() << " slices" << endl;

    sort(slices.begin(), slices.end(), mzSlice::compMz);
    _mergeSlices(massCutoff, rtWindow);
    _adjustSlices();

    cerr << "After final merging and adjustments, "
         << slices.size()
         << " slices remain"
         << endl;
    sendSignal("Mass slicing done.", 1 , 1);
}

void MassSlicer::_reduceSlices()
{
    for (auto first = begin(slices); first != end(slices); ++first) {
        if (_mavenParameters->stop) {
            stopSlicing();
            break;
        }

        auto firstSlice = *first;
        if (mzUtils::almostEqual(firstSlice->ionCount, -1.0f))
            continue;

        // we will use this to terminate large shifts in slices, where they
        // might end up losing their original information completely
        auto originalMax = firstSlice->mzmax;

        for (auto second = next(first); second != end(slices); ++second) {
            auto secondSlice = *second;

            // stop iterating if the rest of the slices are too far
            if (originalMax < secondSlice->mzmin
                || firstSlice->mzmax < secondSlice->mzmin)
                break;

            if (mzUtils::almostEqual(secondSlice->ionCount, -1.0f))
                continue;

            // check if center of one of the slices lies in the other
            if ((firstSlice->mz > secondSlice->mzmin
                 && firstSlice->mz < secondSlice->mzmax
                 && firstSlice->rt > secondSlice->rtmin
                 && firstSlice->rt < secondSlice->rtmax)
                ||
                (secondSlice->mz > firstSlice->mzmin
                 && secondSlice->mz < firstSlice->mzmax
                 && secondSlice->rt > firstSlice->rtmin
                 && secondSlice->rt < firstSlice->rtmax)) {
                firstSlice->ionCount = std::max(firstSlice->ionCount,
                                                secondSlice->ionCount);
                firstSlice->rtmax = std::max(firstSlice->rtmax,
                                             secondSlice->rtmax);
                firstSlice->rtmin = std::min(firstSlice->rtmin,
                                             secondSlice->rtmin);
                firstSlice->mzmax = std::max(firstSlice->mzmax,
                                             secondSlice->mzmax);
                firstSlice->mzmin = std::min(firstSlice->mzmin,
                                             secondSlice->mzmin);

                firstSlice->mz = (firstSlice->mzmin + firstSlice->mzmax) / 2.0f;
                firstSlice->rt = (firstSlice->rtmin + firstSlice->rtmax) / 2.0f;
                float cutoff = _massCutoff->massCutoffValue(firstSlice->mz);

                // make sure that mz window does not get out of control
                if (firstSlice->mzmin < firstSlice->mz - cutoff)
                    firstSlice->mzmin =  firstSlice->mz - cutoff;
                if (firstSlice->mzmax > firstSlice->mz + cutoff)
                    firstSlice->mzmax =  firstSlice->mz + cutoff;

                // recalculate center mz in case bounds changed
                firstSlice->mz = (firstSlice->mzmin + firstSlice->mzmax) / 2.0f;

                // flag this slice as already merged, and ignore henceforth
                secondSlice->ionCount = -1.0f;
            }
        }
        sendSignal("Reducing redundant slices…",
                   first - begin(slices),
                   slices.size());
    }

    // remove merged slices
    slices.erase(remove_if(slices.begin(),
                           slices.end(),
                           [](mzSlice* slice) {
                               return (slice->ionCount == -1.0f);
                           }),
                 slices.end());
}

void MassSlicer::_mergeSlices(const MassCutoff* massCutoff,
                              const float rtTolerance)
{
    // lambda to help expand a given slice by merging a vector of slices into it
    auto expandSlice = [&](mzSlice* mergeInto, vector<mzSlice*> slices) {
        if (slices.empty())
            return;

        for (auto slice : slices) {
            mergeInto->ionCount = std::max(mergeInto->ionCount, slice->ionCount);
            mergeInto->rtmax = std::max(mergeInto->rtmax, slice->rtmax);
            mergeInto->rtmin = std::min(mergeInto->rtmin, slice->rtmin);
            mergeInto->mzmax = std::max(mergeInto->mzmax, slice->mzmax);
            mergeInto->mzmin = std::min(mergeInto->mzmin, slice->mzmin);
        }

        // calculate the new midpoints
        mergeInto->mz = (mergeInto->mzmin + mergeInto->mzmax) / 2.0f;
        mergeInto->rt = (mergeInto->rtmin + mergeInto->rtmax) / 2.0f;

        // make sure that mz window does not get out of control
        auto cutoff = massCutoff->massCutoffValue(mergeInto->mz);
        if (mergeInto->mzmin < mergeInto->mz - cutoff)
            mergeInto->mzmin =  mergeInto->mz - cutoff;
        if (mergeInto->mzmax > mergeInto->mz + cutoff)
            mergeInto->mzmax =  mergeInto->mz + cutoff;

        mergeInto->mz = (mergeInto->mzmin + mergeInto->mzmax) / 2.0f;
    };

    for(auto it = begin(slices); it != end(slices); ++it) {
        if (_mavenParameters->stop) {
            stopSlicing();
            break;
        }

        sendSignal("Merging adjacent slices…",
                   it - begin(slices),
                   slices.size());

        auto slice = *it;
        vector<mzSlice*> slicesToMerge;

        // search ahead
        for (auto ahead = next(it);
             ahead != end(slices) && it != end(slices);
             ++ahead) {
            auto comparisonSlice = *ahead;
            auto comparison = _compareSlices(_samples,
                                             slice,
                                             comparisonSlice,
                                             massCutoff,
                                             rtTolerance);
            auto shouldMerge = comparison.first;
            auto continueIteration = comparison.second;
            if (shouldMerge)
                slicesToMerge.push_back(comparisonSlice);
            if (!continueIteration)
                break;
        }

        // search behind
        for (auto behind = prev(it);
             behind != begin(slices) && it != begin(slices);
             --behind) {
            auto comparisonSlice = *behind;
            auto comparison = _compareSlices(_samples,
                                             slice,
                                             comparisonSlice,
                                             massCutoff,
                                             rtTolerance);
            auto shouldMerge = comparison.first;
            auto continueIteration = comparison.second;
            if (shouldMerge)
                slicesToMerge.push_back(comparisonSlice);
            if (!continueIteration)
                break;
        }

        // expand the current slice by merging all slices classified to be
        // part of the same, and then remove (and free) the slices already
        // merged
        expandSlice(slice, slicesToMerge);
        for (auto merged : slicesToMerge) {
            slices.erase(remove_if(begin(slices),
                                   end(slices),
                                   [&](mzSlice* s) { return s == merged; }),
                         slices.end());
            delete merged;
        }
        it = find_if(begin(slices),
                     end(slices),
                     [&](mzSlice* s) { return s == slice; });
    }
}

pair<bool, bool> MassSlicer::_compareSlices(vector<mzSample*>& samples,
                                            mzSlice* slice,
                                            mzSlice* comparisonSlice,
                                            const MassCutoff *massCutoff,
                                            const float rtTolerance)
{
    auto mz = slice->mz;
    auto mzMin = slice->mzmin;
    auto mzMax = slice->mzmax;
    auto rtMin = slice->rtmin;
    auto rtMax = slice->rtmax;
    auto comparisonMz = comparisonSlice->mz;
    auto comparisonMzMin = comparisonSlice->mzmin;
    auto comparisonMzMax = comparisonSlice->mzmax;
    auto comparisonRtMin = comparisonSlice->rtmin;
    auto comparisonRtMax = comparisonSlice->rtmax;
    auto mzCenter = (mz + comparisonMz) / 2.0f;

    // check to make sure slices are close to each other (or have some
    // overlap in mz domain); the tolerance is multiplied 10x so as to
    // include slices that may be further apart but should be merged
    float massTolerance = 10.0f * massCutoff->massCutoffValue(mzCenter);
    if (!(abs(mzCenter - mz) <= massTolerance
          && abs(mzCenter - comparisonMz) <= massTolerance)) {
        return make_pair(false, false);
    }

    // check if common RT regions exist between the slices being compared
    auto commonLowerRt = 0.0f;
    auto commonUpperRt = 0.0f;
    if (rtMin <= comparisonRtMin && rtMax >= comparisonRtMax) {
        commonLowerRt = comparisonRtMin;
        commonUpperRt = comparisonRtMax;
    } else if (rtMin >= comparisonRtMin && rtMax <= comparisonRtMax) {
        commonLowerRt = rtMin;
        commonUpperRt  = rtMax;
    } else if (rtMin >= comparisonRtMin && rtMin <= comparisonRtMax) {
        commonLowerRt = rtMin;
        commonUpperRt = min(rtMax, comparisonRtMax);
    } else if (rtMax >= comparisonRtMin && rtMax <= comparisonRtMax) {
        commonLowerRt = max(rtMin, comparisonRtMin);
        commonUpperRt = rtMax;
    }
    if (commonLowerRt == 0.0f && commonUpperRt == 0.0f)
        return make_pair(false, true);

    auto highestIntensity = 0.0f;
    auto mzAtHighestIntensity = 0.0f;
    auto rtAtHighestIntensity = 0.0f;
    auto highestCompIntensity = 0.0f;
    auto mzAtHighestCompIntensity = 0.0f;
    auto rtAtHighestCompIntensity = 0.0f;
#pragma omp parallel
    {
        vector<vector<float>> eicValues;
        vector<vector<float>> comparisonEicValues;
#pragma omp for nowait
        for (size_t i = 0; i < samples.size(); ++i) {
            auto sample = samples.at(i);

            // obtain EICs for the two slices
            auto eic = sample->getEIC(mzMin,
                                      mzMax,
                                      rtMin,
                                      rtMax,
                                      1,
                                      1,
                                      "");
            auto comparisonEic = sample->getEIC(comparisonMzMin,
                                                comparisonMzMax,
                                                comparisonRtMin,
                                                comparisonRtMax,
                                                1,
                                                1,
                                                "");
            eicValues.push_back({eic->maxIntensity,
                                 eic->rtAtMaxIntensity,
                                 eic->mzAtMaxIntensity});
            comparisonEicValues.push_back({comparisonEic->maxIntensity,
                                           comparisonEic->rtAtMaxIntensity,
                                           comparisonEic->mzAtMaxIntensity});
            delete eic;
            delete comparisonEic;
        }
#pragma omp critical
        // obtain the highest intensity's mz and rt
        // these updates should happen in a single thread
        for (auto values : eicValues) {
            if (highestIntensity < values[0]) {
                highestIntensity = values[0];
                rtAtHighestIntensity = values[1];
                mzAtHighestIntensity = values[2];
            }
        }
        for (auto values : comparisonEicValues) {
            if (highestCompIntensity < values[0]) {
                highestCompIntensity = values[0];
                rtAtHighestCompIntensity = values[1];
                mzAtHighestCompIntensity = values[2];
            }
        }
    }

    if (highestIntensity == 0.0f && highestCompIntensity == 0.0f)
        return make_pair(false, true);

    // calculate and check for rt difference and mz difference, if
    // conditions are satisfied, mark the comparison slice to be merged
    auto rtDelta = abs(rtAtHighestIntensity - rtAtHighestCompIntensity);
    auto mzCenterForIntensity = (mzAtHighestIntensity
                              + mzAtHighestCompIntensity) / 2.0f;
    auto massToleranceForIntensity =
        massCutoff->massCutoffValue(mzCenterForIntensity);
    auto mzDeltaNeg = abs(mzCenterForIntensity - mzAtHighestIntensity );
    auto mzDeltaPos = abs(mzAtHighestCompIntensity - mzCenterForIntensity );
    if (rtDelta <= rtTolerance
        && mzDeltaNeg <= massToleranceForIntensity
        && mzDeltaPos <= massToleranceForIntensity) {
        return make_pair(true, true);
    }

    return make_pair(false, true);
}

void MassSlicer::_adjustSlices()
{
    size_t progressCount = 0;
    for (auto slice : slices) {
        if (_mavenParameters->stop) {
            stopSlicing();
            break;
        }

        auto eics = PeakDetector::pullEICs(slice,
                                           _mavenParameters->samples,
                                           _mavenParameters);
        float highestIntensity = 0.0f;
        float mzAtHighestIntensity = 0.0f;
        for (auto eic : eics) {
            size_t size = eic->intensity.size();
            for (int i = 0; i < size; ++i) {
                if (eic->spline[i] > highestIntensity) {
                    highestIntensity = eic->spline[i];
                    mzAtHighestIntensity = eic->mz[i];
                }
            }
        }
        float cutoff = _mavenParameters->massCutoffMerge
                                       ->massCutoffValue(mzAtHighestIntensity);
        slice->mzmin =  mzAtHighestIntensity - cutoff;
        slice->mzmax =  mzAtHighestIntensity + cutoff;
        slice->mz = (slice->mzmin + slice->mzmax) / 2.0f;

        delete_all(eics);

        ++progressCount;
        sendSignal("Adjusting slices…", progressCount, slices.size());
    }
}

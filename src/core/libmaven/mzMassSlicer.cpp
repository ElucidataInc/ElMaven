#include "EIC.h"
#include "mavenparameters.h"
#include "mzMassSlicer.h"
#include "mzSample.h"
#include "datastructures/mzSlice.h"
#include "masscutofftype.h"
#include "mzUtils.h"
#include "Matrix.h"
#include "Scan.h"

using namespace mzUtils;

MassSlices::MassSlices()
{
    _maxSlices=INT_MAX;
    _minRt=FLT_MIN; _minMz=FLT_MIN; _minIntensity=FLT_MIN;
    _maxRt=FLT_MAX; _maxMz=FLT_MAX; _maxIntensity=FLT_MAX;
    _minCharge=0; _maxCharge=INT_MAX;
    massCutoff=NULL;
}

MassSlices::~MassSlices() { delete_all(slices); cache.clear(); }

void MassSlices::sendSignal(const string& progressText,
                unsigned int completed_samples,
                int total_samples)
{
    mavenParameters->sig(progressText, completed_samples, total_samples);
}

/**
 * MassSlices::algorithmA This is function is called when mass Slicing using 
 * AlgorithmB returns no slices. The slices here are created using the filterLine
 * in Mzml and Mzxml files.
 */
void MassSlices::algorithmA() {
    // clear cache
    delete_all(slices);
    slices.clear();
    cache.clear();
    map< string, int> seen;

    //#pragma omp parallel for ordered
    // Iterate over every sample
    for(unsigned int i=0; i < samples.size(); i++) {
        // Iterate over every scan 
        for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {
            // Make temprorary element scan with present scan
            Scan* scan = samples[i]->scans[j];

            // Check if filterLine(SRM transition for MS-MS, segment for LC-MS) for the scan is empty
            if ( scan->filterLine.empty() ) continue;

            if ( seen.count( scan->filterLine ) ) continue;

            //Create new slice for every filterLine
            mzSlice* s = new mzSlice(scan->filterLine);
            slices.push_back(s);
            seen[ scan->filterLine ]=1;
        }
    }
    cerr << "#algorithmA" << slices.size() << endl;
}

void MassSlices::stopSlicing() {
    if (slices.size() > 0) {
        delete_all(slices);
        slices.clear();
        cache.clear();
    }
}

void MassSlices::algorithmB(MassCutoff* massCutoff, int rtStep )
{
    // clear all previous data
    delete_all(slices);
    slices.clear();
    cache.clear();

    float rtWindow = 2.0f;
    this->massCutoff = massCutoff;

    int totalScans = 0;
    int currentScans = 0;

    sort(begin(samples), end(samples), mzSample::compSampleSort);

    // Calculate the total number of scans
    for (auto s : samples)
        totalScans += s->scans.size();

    // Calculating the rt window using average distance between RTs and
    // mutiplying it with rtStep (default 2.0)
    if (samples.size() > 0 and rtStep > 0)
        rtWindow = (samples.at(0)->getAverageFullScanTime() * rtStep);

    sendSignal("Status", 0 , 1);

    // #pragma omp parallel for ordered
    // Looping over every sample
    for (unsigned int i = 0; i < samples.size(); i++) {
        if (slices.size() > _maxSlices) break;

        // Check if Peak detection has been cancelled by the user
        if (mavenParameters->stop) {
            stopSlicing();
            break;
        }

        string num;
        if (i==0) {
            num = "st";
        } else if (i==1) {
            num = "nd";
        } else if (i==2) {
            num = "rd";
        } else {
            num = "th";
        }

        // updating progress on samples
        if (mavenParameters->showProgressFlag ) {
            string progressText = to_string(i + 1)
                                  + num
                                  + " out of "
                                  + to_string(mavenParameters->samples.size())
                                  + " sample(s) processing…";
            sendSignal(progressText, currentScans, totalScans);
        }

        // #pragma omp cancel for
        // for loop for iterating over every scan of a sample
        for (auto scan : samples[i]->scans) {
            // Check if Peak detection has been cancelled by the user
            if (mavenParameters->stop) {
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

                // Define max mz and min mz for this slice
                float cutoff = massCutoff->massCutoffValue(mz);
                float mzmax = mz + cutoff;
                float mzmin = mz - cutoff;

                // #pragma omp ordered
                // sliceExists() returns a the best slice or a null based on
                // whether a slice exists at that location or not
                mzSlice* Z = sliceExists(mzmin,
                                         mzmax,
                                         rt - (2.0f * rtWindow),
                                         rt + (2.0f * rtWindow));
                if (Z) {
                    // If slice exists take the max of the intensity, rt and mz
                    // (max and min)
                    Z->ionCount = std::max(Z->ionCount, intensity);
                    Z->rtmax = std::max(Z->rtmax, rt + (2.0f * rtWindow));
                    Z->rtmin = std::min(Z->rtmin, rt - (2.0f * rtWindow));
                    Z->mzmax = std::max(Z->mzmax, mzmax);
                    Z->mzmin = std::min(Z->mzmin, mzmin);

                    // Make sure that mz window does not get out of control
                    if (Z->mzmin < mz - cutoff)
                        Z->mzmin =  mz - cutoff;
                    if (Z->mzmax > mz + cutoff)
                        Z->mzmax =  mz + cutoff;

                    Z->mz = (Z->mzmin + Z->mzmax) / 2.0f;
                    Z->rt = (Z->rtmin + Z->rtmax) / 2.0f;
                } else {
                    // Make a new slice if no slice returned by sliceExists and
                    // push it into cache
                    mzSlice* s = new mzSlice(mzmin,
                                             mzmax,
                                             rt - (2.0f * rtWindow),
                                             rt + (2.0f * rtWindow));
                    s->ionCount = intensity;
                    s->rt = scan->rt;
                    s->mz = mz;
                    slices.push_back(s);
                    int mzRange = mz * 10;
                    cache.insert(pair<int, mzSlice*>(mzRange, s));
                }
            }

            // progress update 
            if (mavenParameters->showProgressFlag ) {
                string progressText = to_string(i + 1)
                                      + num
                                      + " out of "
                                      + to_string(mavenParameters->samples.size())
                                      + " sample(s) processing…\n"
                                      + to_string(slices.size())
                                      + " slices created ";
                sendSignal(progressText,currentScans,totalScans);
            }
        }
    }

    cerr << "Found " << slices.size() << " slices" << endl;

    float threshold = 100;
    removeDuplicateSlices(massCutoff, threshold);
    sort(slices.begin(), slices.end(), mzSlice::compIntensity);

    mergeOverlappingSlices(0.05f, 0.90f);

    ofstream fs("slices.csv");
    fs << "mz,rt,mzMin,mzMax,rtMin,rtMax,ionCount,srmId\n";
    for (auto s : slices) {
        fs << s->mz << ","
           << s->rt << ","
           << s->mzmin << ","
           << s->mzmax << ","
           << s->rtmin << ","
           << s->rtmax << ","
           << s->ionCount << ","
           << s->srmId << "\n";
    }
    fs.close();

    cerr << "After removing duplicate slices, with threshold "
         << threshold
         << ", found "
         << slices.size()
         << " slices"
         << endl;
    sendSignal("Mass slicing done.", 1 , 1);
}

void MassSlices::algorithmC(float ppm, float minIntensity, float rtWindow) {
    delete_all(slices);
    slices.clear();
    cache.clear();

    for(unsigned int i=0; i < samples.size(); i++) {
        mzSample* s = samples[i];
        for(unsigned int j=0; j < s->scans.size(); j++) {
            Scan* scan = samples[i]->scans[j];
            if (scan->mslevel != 1 ) continue;
            vector<int> positions = scan->intensityOrderDesc();
            for(unsigned int k=0; k< positions.size() && k<10; k++ ) {
                int pos = positions[k];
                if (scan->intensity[pos] < minIntensity) continue;
                float rt = scan->rt;
                float mz = scan->mz[ pos ];
                float mzmax = mz + mz/1e6*ppm;
                float mzmin = mz - mz/1e6*ppm;
                if(! sliceExists(mzmin, mzmax, rt-2*rtWindow, rt+2*rtWindow) ) {
                    mzSlice* s = new mzSlice(mzmin,mzmax, rt-2*rtWindow, rt+2*rtWindow);
                    s->ionCount = scan->intensity[pos];
                    s->rt=scan->rt;
                    s->mz=mz;
                    slices.push_back(s);
                    int mzRange = mz*10;
                    cache.insert( pair<int,mzSlice*>(mzRange, s));
                }
            }
        }
    }
    cerr << "#algorithmC" << slices.size() << endl;
}

mzSlice* MassSlices::sliceExists(float mzMinBound,
                                 float mzMaxBound,
                                 float rtMinBound,
                                 float rtMaxBound)
{
    // calculate center for given bounds
    float mz = (mzMinBound + mzMaxBound) / 2.0f;
    float rt = (rtMinBound + rtMaxBound) / 2.0f;

    // putting all mz slices stored in cache within a particular range in ppp
    auto subcache = cache.equal_range(int(mz * 10));
    auto it = subcache.first;

    float bestDist = FLT_MAX;
    mzSlice* best = nullptr;

    // For loop to iterate till best MZ slice becomes second
    for (; it != subcache.second; ++it) {
        mzSlice* slice = (*it).second;
        float sliceMzMin = slice->mzmin;
        float sliceMzMax = slice->mzmax;
        float sliceRtMin = slice->rtmin;
        float sliceRtMax = slice->rtmax;

        // calculate center for the current slice
        float sliceMz = slice->mz;
        float sliceRt = slice->rt;

        // check if center of given bounds lies within slice, or whether center
        // of the slice lies within the given bounds
        if ((mz > sliceMzMin
             && mz < sliceMzMax
             && rt > sliceRtMin
             && rt < sliceRtMax)
            ||
            (sliceMz > mzMinBound
             && sliceMz < mzMaxBound
             && sliceRt > rtMinBound
             && sliceRt < rtMaxBound)) {
            float dist = hypotf(sliceMz - mz, sliceRt - rt);
            if (dist < bestDist) {
                best = slice;
                bestDist = dist;
            }
        }
    }
    return best;
}

void MassSlices::mergeOverlappingSlices(float rtTolerance,
                                        float intensityRatioCutoff)
{
    // lambda to help expand a given slice by merging a vector of slices into it
    auto expandSlice = [](mzSlice* mergeInto, vector<mzSlice*> slices) {
        if (slices.empty())
            return;

        for (auto slice : slices) {
            mergeInto->ionCount = std::max(mergeInto->ionCount, slice->ionCount);
            mergeInto->rtmax = std::max(mergeInto->rtmax, slice->rtmax);
            mergeInto->rtmin = std::min(mergeInto->rtmin, slice->rtmin);
            mergeInto->mzmax = std::max(mergeInto->mzmax, slice->mzmax);
            mergeInto->mzmin = std::min(mergeInto->mzmin, slice->mzmin);
        }

        mergeInto->mz = (mergeInto->mzmin + mergeInto->mzmax) / 2.0f;
        mergeInto->rt = (mergeInto->rtmin + mergeInto->rtmax) / 2.0f;
    };

    for (size_t n = 0; n < samples.size(); ++n) {
        if (mavenParameters->stop) {
            stopSlicing();
            break;
        }

        auto sample = samples.at(n);
        for(auto it = begin(slices); it != end(slices); ++it) {
            if (mavenParameters->stop) {
                stopSlicing();
                break;
            }

            int progress = (n * slices.size()) + (it - begin(slices));
            int total = samples.size() * slices.size();
            sendSignal("Filtering slices from samples…", progress, total);

            auto slice = *it;
            auto mz = slice->mz;
            auto rt = slice->rt;
            auto mzMin = slice->mzmin;
            auto mzMax = slice->mzmax;
            auto rtMin = slice->rtmin;
            auto rtMax = slice->rtmax;

            vector<mzSlice*> slicesToMerge;
            for (auto rest = it; rest != end(slices); ++rest) {
                auto comparisonSlice = *rest;
                if (slice == comparisonSlice)
                    continue;

                auto comparisonMz = comparisonSlice->mz;
                auto comparisonRt = comparisonSlice->rt;
                auto comparisonMzMin = comparisonSlice->mzmin;
                auto comparisonMzMax = comparisonSlice->mzmax;
                auto comparisonRtMin = comparisonSlice->rtmin;
                auto comparisonRtMax = comparisonSlice->rtmax;

                // check if center of either slice lies within the bounds of the
                // other slice, and if not, move on to the next slice
                if (!(mz > comparisonMzMin
                      && mz < comparisonMzMax
                      && rt > comparisonRtMin
                      && rt < comparisonRtMax)
                    &&
                    !(comparisonMz > mzMin
                      && comparisonMz < mzMax
                      && comparisonRt > rtMin
                      && comparisonRt < rtMax)) {
                    continue;
                }

                // find common boundaries between the two slices being compared
                auto commonLowerBound = 0.0f;
                auto commonUpperBound = 0.0f;
                if (mzMin <= comparisonMzMin
                    && mzMax >= comparisonMzMax) {
                    commonLowerBound = comparisonMzMin;
                    commonUpperBound = comparisonMzMax;
                } else if (mzMin >= comparisonMzMin
                           && mzMax <= comparisonMzMax) {
                    commonLowerBound = mzMin;
                    commonUpperBound = mzMax;
                } else if (mzMin >= comparisonMzMin
                           && mzMin <= comparisonMzMax) {
                    commonLowerBound = mzMin;
                    commonUpperBound = min(mzMax, comparisonMzMax);
                } else if (mzMax >= comparisonMzMin
                           && mzMax <= comparisonMzMax) {
                    commonLowerBound = max(mzMin, comparisonMzMin);
                    commonUpperBound = mzMax;
                }

                if (commonLowerBound == 0.0f && commonUpperBound == 0.0f)
                    continue;

                // obtain EICs for the two slices
                auto eic = sample->getEIC(commonLowerBound,
                                          commonUpperBound,
                                          rtMin,
                                          rtMax,
                                          1,
                                          1,
                                          "");
                auto comparisonEic = sample->getEIC(commonLowerBound,
                                                    commonUpperBound,
                                                    comparisonRtMin,
                                                    comparisonRtMax,
                                                    1,
                                                    1,
                                                    "");

                // find out the highest intensity (and its rt) in the EICs
                auto highestIntensity = 0.0f;
                auto rtAtHighestIntensity = 0.0f;
                for (size_t i = 0; i < eic->size(); ++i) {
                    auto rtAtIdx = eic->rt[i];
                    auto intensityAtIdx = eic->intensity[i];
                    if (intensityAtIdx > highestIntensity) {
                        highestIntensity = intensityAtIdx;
                        rtAtHighestIntensity = rtAtIdx;
                    }
                }
                auto highestCompIntensity = 0.0f;
                auto rtAtHighestCompIntensity = 0.0f;
                for (size_t i = 0; i < comparisonEic->size(); ++i) {
                    auto rtAtIdx = comparisonEic->rt[i];
                    auto intensityAtIdx = comparisonEic->intensity[i];
                    if (intensityAtIdx > highestCompIntensity) {
                        highestCompIntensity = intensityAtIdx;
                        rtAtHighestCompIntensity = rtAtIdx;
                    }
                }

                // calculate and check for rt difference and intensity ratio,
                // if conditions are satisfied, mark the comparison slice to be
                // merged
                auto rtDelta = abs(rtAtHighestIntensity
                                   - rtAtHighestCompIntensity);
                auto inRatio = highestCompIntensity < highestIntensity
                                   ? (highestCompIntensity / highestIntensity)
                                   : (highestIntensity / highestCompIntensity);
                if (rtDelta <= rtTolerance && inRatio >= intensityRatioCutoff) {
                    slicesToMerge.push_back(comparisonSlice);
                }
            }

            // expand the current slice by merging all slices classified to be
            // part of the same, and then remove (and free) the slices already
            // merged
            expandSlice(slice, slicesToMerge);
            for (auto merged : slicesToMerge) {
                slices.erase(remove_if(it,
                                       end(slices),
                                       [&](mzSlice* s) { return s == merged; }),
                             slices.end());
                delete merged;
            }
        }
    }
}

void MassSlices::removeDuplicateSlices(MassCutoff *massCutoff, float threshold){

    vector<mzSlice*> returnSlices;
    mzSlice* slice;
    multimap<int,int>vectorCache;
    
   for(int i=0; i<slices.size(); i++) {
        slice = slices[i];
        float mz = slice->mz;
        multimap<int, int>::iterator start, end;
        
        start = vectorCache.equal_range( (int) (mz* 10 - 1) ).first;
        end = vectorCache.equal_range( (int) (mz* 10 + 1) ).second;
        float mzOverlap =  0.0;
        float rtOverlap = 0.0;
        float overlapArea, bestOverlapArea = 0.0;
        int bestSliceNum = -1;

        for(; start != end; start++) {
            int thisSliceNum = (*start).second;
            mzSlice *thisSlice = returnSlices[thisSliceNum];

            float low = thisSlice->mzmin > slice->mzmin ? thisSlice->mzmin : slice->mzmin;
            float high = thisSlice->mzmax < slice->mzmax ? thisSlice->mzmax : slice->mzmax;
            mzOverlap = high-low;

            low = thisSlice->rtmin > slice->rtmin ? thisSlice->rtmin : slice->rtmin;
            high = thisSlice->rtmax < slice->rtmax ? thisSlice->rtmax : slice->rtmax;
            rtOverlap = high-low;

            if(mzOverlap>0 && rtOverlap>0) overlapArea = mzOverlap * rtOverlap;
            else overlapArea = 0;

            float area1 = (thisSlice->mzmax-thisSlice->mzmin) * (thisSlice->rtmax-thisSlice->rtmin);
            float area2 = (slice->mzmax-slice->mzmin) * (slice->rtmax-slice->rtmin);
            float area = area1 < area2 ? area1 : area2;

            if (overlapArea/area >= threshold/100 && overlapArea > bestOverlapArea){
                bestOverlapArea = overlapArea;
                bestSliceNum = thisSliceNum;
            }
            
        }

        if(bestSliceNum >= 0){
            mzSlice* Z = returnSlices[bestSliceNum];
            
            Z->ionCount = std::max((float) Z->ionCount, (float ) slice->ionCount);
            Z->rtmax = std::max((float)Z->rtmax, (float)slice->rtmax);
            Z->rtmin = std::min((float)Z->rtmin, (float)slice->rtmin);
            Z->mzmax = std::max((float)Z->mzmax, (float)slice->mzmax);
            Z->mzmin = std::min((float)Z->mzmin, (float)slice->mzmin);

            //make sure that mz windown doesn't get out of control
            if (Z->mzmin < mz-massCutoff->massCutoffValue(mz)) Z->mzmin =  mz-massCutoff->massCutoffValue(mz);
            if (Z->mzmax > mz+massCutoff->massCutoffValue(mz)) Z->mzmax =  mz+massCutoff->massCutoffValue(mz);
            Z->mz = (Z->mzmin + Z->mzmax) / 2; Z->rt=(Z->rtmin + Z->rtmax) / 2;
        }
        else{
            vectorCache.insert(pair<int,int>(int (mz*10),returnSlices.size()));
            returnSlices.push_back(slice);
        }
    }
    slices = returnSlices;
}

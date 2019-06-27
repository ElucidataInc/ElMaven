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

            // Looping over every observation in the scan
            for (unsigned int k = 0; k < scan->nobs(); k++) {
                // Checking if mz, intensity are within specified
                // ranges
                if (_maxMz && !isBetweenInclusive(scan->mz[k],
                                                  _minMz,
                                                  _maxMz)) {
                    continue;
                }
                if (_maxIntensity
                    && !isBetweenInclusive(scan->intensity[k],
                                           _minIntensity,
                                           _maxIntensity)) {
                    continue;
                }

                // Define max mz and min mz for this slice
                float mz = scan->mz[k];
                float cutoff = massCutoff->massCutoffValue(mz);
                float mzmax = mz + cutoff;
                float mzmin = mz - cutoff;

                // #pragma omp ordered
                // sliceExists() returns a the best slice or a null based on
                // whether a slice exists at that location or not
                mzSlice* Z = sliceExists(mz, rt);
                if (Z) {
                    // If slice exists take the max of the intensity, rt and mz
                    // (max and min)
                    Z->ionCount = std::max(float(Z->ionCount),
                                           float(scan->intensity[k]));
                    Z->rtmax = std::max(float(Z->rtmax),
                                        rt + (2.0f * rtWindow));
                    Z->rtmin = std::min(float(Z->rtmin),
                                        rt - (2.0f * rtWindow));
                    Z->mzmax = std::max(float(Z->mzmax), mzmax);
                    Z->mzmin = std::min(float(Z->mzmin), mzmin);

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
                    s->ionCount = scan->intensity[k];
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
                if(! sliceExists(mz,rt) ) {
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

mzSlice* MassSlices::sliceExists(float mz, float rt)
{
    // putting all mz slices stored in cache within a particular range in ppp
    auto subcache = cache.equal_range(int(mz * 10));
    auto it = subcache.first;

    float bestDist = FLT_MAX;
    mzSlice* best = nullptr;

    // For loop to iterate till best MZ slice becomes second
    for (; it != subcache.second; ++it) {
        mzSlice* slice = (*it).second;
        if (mz > x->mzmin && mz < x->mzmax && rt > x->rtmin && rt < x->rtmax) {
            float d = (mz-x->mzmin) + (x->mzmax-mz);
            if ( d < bestDist ) {
                best=x; bestDist=d;
            }
        }
    }
    return best;
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

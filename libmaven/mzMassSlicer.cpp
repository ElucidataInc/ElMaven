#include "mzMassSlicer.h"

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
/**
 * MassSlices::algorithmB This is the main function that does the peakdetection
 * This does not need a DB to check the peaks. The function essentially loops over
 * every observation in every scan in every sample. Every observation is checked if
 * it is already present in a slice or not. If present in a slice MZmax, MZmin, RTmin,
 * RTmax, intensity, MZ and RT are modified and the  slice then put back into cache. If 
 * not then then a new slice is created and added to the slice.
 * @param userPPM      The user defined PPM for MZ range
 * @param rtStep       Minimum RT range for RT window
 */
void MassSlices::algorithmB(float userPPM,int rtStep) {
    //clear all previous data
    delete_all(slices);
    slices.clear();
    cache.clear();

	float rtWindow=2.0;
	this->_precursorPPM=userPPM;

    int totalScans = 0,currentScans = 0;

    //Calculate the total number of scans
    for(unsigned int i=0; i < samples.size(); i++) totalScans += samples[i]->scans.size();

    //Calculating the rt window using average distance between RTs and mutiplying it with RTstep (default 20a)
    if (samples.size() > 0 and rtStep > 0) rtWindow = (samples[0]->getAverageFullScanTime()*rtStep);

    sendSignal("Status", 0 , 1);

    // #pragma omp parallel for ordered
    // Looping over every sample
    for(unsigned int i=0; i < samples.size(); i++) {
        if (slices.size() > _maxSlices) break;

        // Check if Peak detection has been cancelled by the user
        if (mavenParameters->stop) {
            stopSlicing();
            break;
        }

        string num;
        if(i==0) num = "st";
        else if(i==1) num = "nd";
        else if(i==2) num = "rd";
        else num = "th ";

        // updating progress on samples
        if (mavenParameters->showProgressFlag ) {
            string progressText = to_string(i+1) + num + " out of " + to_string(mavenParameters->samples.size()) 
                                  + " Sample(s) Processing.....";
            sendSignal(progressText,currentScans,totalScans);
        }

        // #pragma omp cancel for
        // for loop for iterating over every scan of a sample
        for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {

            // Check if Peak detection has been cancelled by the user
            if (mavenParameters->stop) {
                stopSlicing();
                break;
            }
            currentScans++;
            Scan* scan = samples[i]->scans[j];
            if (scan->mslevel != 1 ) continue;

            // Checking if RT is in the given min to max RT range
            if (_maxRt and !isBetweenInclusive(scan->rt,_minRt,_maxRt)) continue;
            float rt = scan->rt;

            vector<int> charges;
            if (_minCharge > 0 or _maxCharge > 0) charges = scan->assignCharges(userPPM);

            // Looping over every observation in the scan
            for(unsigned int k=0; k < scan->nobs(); k++ ) {

                // Checking if mz, intensity and charge are within specified range
                if (_maxMz and !isBetweenInclusive(scan->mz[k],_minMz,_maxMz)) continue;
                if (_maxIntensity and !isBetweenInclusive(scan->intensity[k],_minIntensity,_maxIntensity)) continue;
                if ((_minCharge or _maxCharge) and !isBetweenInclusive(charges[k],_minCharge,_maxCharge)) continue;

                // Define mz max and min for this slice
                float mz = scan->mz[k];
                float mzmax = mz + mz / 1e6 * _precursorPPM;
                float mzmin = mz - mz / 1e6 * _precursorPPM;

                // #pragma omp ordered
                // sliceExists() returns a the best slice or a null based on whether a slice exists at that location or not
                mzSlice* Z = sliceExists(mz,rt);

                if (Z) {
                    //cerr << "Merged Slice " <<  Z->mzmin << " " << Z->mzmax << " " << scan->intensity[k] << "  " << Z->ionCount << endl;
                    // If slice exists take the max of the intensity, rt and mz (max and min)
                    Z->ionCount = std::max((float) Z->ionCount, (float ) scan->intensity[k]);
                    Z->rtmax = std::max((float)Z->rtmax, rt+2*rtWindow);
                    Z->rtmin = std::min((float)Z->rtmin, rt-2*rtWindow);
                    Z->mzmax = std::max((float)Z->mzmax, mzmax);
                    Z->mzmin = std::min((float)Z->mzmin, mzmin);


                    //make sure that mz windown doesn't get out of control
                    if (Z->mzmin < mz-(mz/1e6*userPPM)) Z->mzmin =  mz-(mz/1e6*userPPM);
                    if (Z->mzmax > mz+(mz/1e6*userPPM)) Z->mzmax =  mz+(mz/1e6*userPPM);
                    Z->mz = (Z->mzmin + Z->mzmax) / 2; Z->rt=(Z->rtmin + Z->rtmax) / 2;
                    //cerr << Z->mz << " " << Z->mzmin << " " << Z->mzmax << " " << ppmDist((float)Z->mzmin,mz) << endl;
                } else {
                    //cerr << "\t" << rt << "  " << mzmin << "  "  << mzmax << endl;
                    //Make a new slice if no slice returned by sliceExists and push it into cache
                    mzSlice* s = new mzSlice(mzmin, mzmax, rt - 2 * rtWindow, rt + 2 * rtWindow);
                    s->ionCount = scan->intensity[k];
                    s->rt=scan->rt;
                    s->mz=mz;
                    //cerr << "New Slice " <<  s->mzmin << " " << s->mzmax << " " << s->ionCount << endl;
                    slices.push_back(s);
                    int mzRange = mz * 10;
                    cache.insert( pair<int,mzSlice*>(mzRange, s));
                }
            }
            // progress update 
            if (mavenParameters->showProgressFlag ) {
                string progressText = to_string(i+1) + num + " out of " + to_string(mavenParameters->samples.size()) 
                                  + " Sample(s) Processing.....\n"
                                  + to_string(slices.size()) + " Slices Created ";
                sendSignal(progressText,currentScans,totalScans);
            }

        }
    }
    cerr << "Found=" << slices.size() << " slices" << endl;
    float threshold = 100;
    removeDuplicateSlices(userPPM, threshold);
    sort(slices.begin(),slices.end(), mzSlice::compIntensity);
    cerr << "After removing duplicate slices. Threshold : "<< threshold <<", Found="<< slices.size()<< " slices" <<endl;
    sendSignal("Mass Slices Processed", 1 , 1);
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

//Function to check if slice is already present in cache
mzSlice*  MassSlices::sliceExists(float mz, float rt) {
    pair< multimap<int, mzSlice*>::iterator,  multimap<int, mzSlice*>::iterator > ppp;
    // putting all mz slices in cache in a particular range in ppp
    ppp = cache.equal_range( (int) (mz* 10) );
    multimap<int, mzSlice*>::iterator it2 = ppp.first;

    float bestDist=FLT_MAX; 
    mzSlice* best=NULL;

    // For loop to iterate till best MZ slice becomes second
    for(; it2 != ppp.second; ++it2 ) {
        mzSlice* x = (*it2).second;
        if (mz > x->mzmin && mz < x->mzmax && rt > x->rtmin && rt < x->rtmax) {
            float d = (mz-x->mzmin) + (x->mzmax-mz);
            if ( d < bestDist ) { best=x; bestDist=d; }
        }
    }
    return best;
}

void MassSlices::removeDuplicateSlices(float userPPM, float threshold){

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
            if (Z->mzmin < mz-(mz/1e6*userPPM)) Z->mzmin =  mz-(mz/1e6*userPPM);
            if (Z->mzmax > mz+(mz/1e6*userPPM)) Z->mzmax =  mz+(mz/1e6*userPPM);
            Z->mz = (Z->mzmin + Z->mzmax) / 2; Z->rt=(Z->rtmin + Z->rtmax) / 2;
        }
        else{
            vectorCache.insert(pair<int,int>(int (mz*10),returnSlices.size()));
            returnSlices.push_back(slice);
        }
    }
    slices = returnSlices;
}

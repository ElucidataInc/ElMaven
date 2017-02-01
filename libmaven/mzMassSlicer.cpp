#include "mzMassSlicer.h"

void MassSlices::algorithmA() {
    delete_all(slices);
    slices.clear();
    cache.clear();
    map< string, int> seen;

//#pragma omp parallel for ordered
    for(unsigned int i=0; i < samples.size(); i++) {
        for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {
            Scan* scan = samples[i]->scans[j];
            if ( scan->filterLine.empty() ) continue;
            if ( seen.count( scan->filterLine ) ) continue;
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
 * This dont need a DB to check the peaks. This function finds all the peaks
 * that are present in the mzxml files
 * @param userPPM      [description]
 * @param minIntensity [description]
 * @param rtStep       [description]
 */
void MassSlices::algorithmB(float userPPM,int rtStep) {
    delete_all(slices);
    slices.clear();
    cache.clear();

	float rtWindow=2.0;
	this->_precursorPPM=userPPM;

    int totalScans = 0,currentScans = 0;
    for(unsigned int i=0; i < samples.size(); i++) totalScans += samples[i]->scans.size();

    if (samples.size() > 0 and rtStep > 0) rtWindow = (samples[0]->getAverageFullScanTime()*rtStep);

    sendSignal("Status", 0 , 1);

// #pragma omp parallel for ordered
    for(unsigned int i=0; i < samples.size(); i++) {
        if (slices.size() > _maxSlices) break;

        if (mavenParameters->stop) {
            stopSlicing();
            break;
        }

        string num;
        if(i==0) num = "st";
        else if(i==1) num = "nd";
        else if(i==2) num = "rd";
        else num = "th ";

        if (mavenParameters->showProgressFlag ) {
            string progressText = to_string(i+1) + num + " out of " + to_string(mavenParameters->samples.size()) 
                                  + " Sample(s) Processing.....";
            sendSignal(progressText,currentScans,totalScans);
        }

// #pragma omp cancel for
        for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {
            if (mavenParameters->stop) {
                stopSlicing();
                break;
            }
            currentScans++;
            Scan* scan = samples[i]->scans[j];
            if (scan->mslevel != 1 ) continue;
            if (_maxRt and !isBetweenInclusive(scan->rt,_minRt,_maxRt)) continue;
            float rt = scan->rt;

            vector<int> charges;
            if (_minCharge > 0 or _maxCharge > 0) charges = scan->assignCharges(userPPM);

            for(unsigned int k=0; k < scan->nobs(); k++ ) {
                if (_maxMz and !isBetweenInclusive(scan->mz[k],_minMz,_maxMz)) continue;
                if (_maxIntensity and !isBetweenInclusive(scan->intensity[k],_minIntensity,_maxIntensity)) continue;
                if ((_minCharge or _maxCharge) and !isBetweenInclusive(charges[k],_minCharge,_maxCharge)) continue;

                float mz = scan->mz[k];
                float mzmax = mz + mz / 1e6 * _precursorPPM;
                float mzmin = mz - mz / 1e6 * _precursorPPM;

// #pragma omp ordered
                mzSlice* Z = sliceExists(mz,rt);

                if (Z) {
                    //cerr << "Merged Slice " <<  Z->mzmin << " " << Z->mzmax << " " << scan->intensity[k] << "  " << Z->ionCount << endl;

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

            if (mavenParameters->showProgressFlag ) {
                string progressText = to_string(i+1) + num + " out of " + to_string(mavenParameters->samples.size()) 
                                  + " Sample(s) Processing.....\n"
                                  + to_string(slices.size()) + " Slices Created ";
                sendSignal(progressText,currentScans,totalScans);
            }

        }
    }
    cerr << "Found=" << slices.size() << " slices" << endl;
    sort(slices.begin(),slices.end(), mzSlice::compIntensity);
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

mzSlice*  MassSlices::sliceExists(float mz, float rt) {
    pair< multimap<int, mzSlice*>::iterator,  multimap<int, mzSlice*>::iterator > ppp;
    ppp = cache.equal_range( (int) (mz* 10) );
    multimap<int, mzSlice*>::iterator it2 = ppp.first;

    float bestDist=FLT_MAX; 
    mzSlice* best=NULL;

    for(; it2 != ppp.second; ++it2 ) {
        mzSlice* x = (*it2).second;
        if (mz > x->mzmin && mz < x->mzmax && rt > x->rtmin && rt < x->rtmax) {
            float d = (mz-x->mzmin) + (x->mzmax-mz);
            if ( d < bestDist ) { best=x; bestDist=d; }
        }
    }
    return best;
}

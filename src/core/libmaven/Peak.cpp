#include "Peak.h"
#include "EIC.h"
#include "Scan.h"
#include "mzSample.h"
#include "mzUtils.h"

using namespace mzUtils;

Peak::Peak() {
    pos = 0;
    eic = NULL;
    sample = NULL;
    baseMz = 0;
    fromBlankSample = false;
    groupOverlap = 0;
    groupNum = 0;
    groupOverlapFrac = 0;
    label = 0;
    localMaxFlag = 0;
    maxscan = 0;
    medianMz = 0;
    minpos = pos;
    maxpos = pos;
    splineminpos = pos;
    splinemaxpos = pos;
    minscan = 0;
    mzmax = 0;
    mzmin = 0;
    noNoiseObs = 0;
    noNoiseFraction = 0;
    peakArea = 0;
    peakSplineArea = 0;
    peakAreaCorrected = 0;
    peakAreaFractional = 0;
    peakAreaTop = 0;
    peakAreaTopCorrected = 0;
    peakIntensity = 0;
    peakMz = 0;
    peakBaseLineLevel = 0;
    quality = 0;
    rt = 0;
    rtmax = 0;
    rtmin = 0;
    scan = 0;
    signalBaselineRatio = 0;
    signalBaselineDifference = 0;
    symmetry = 0;
    width = 0;
    gaussFitSigma = 10;
    gaussFitR2 = FLT_MAX;
    peakRank = INT_MAX;
}

Peak::Peak(EIC* e, int p) { 
    pos=p; 
    eic = e;
    sample = NULL;
    baseMz=0;
    fromBlankSample=false;
    groupOverlap=0;
    groupNum=0;
    groupOverlapFrac=0;
    label=0;
    localMaxFlag=0;
    maxscan=0;
    medianMz=0;
    minpos=pos;
    maxpos=pos;
    splineminpos=pos;
    splinemaxpos=pos;
    minscan=0;
    mzmax=0;
    mzmin=0;
    noNoiseObs=0;  
    noNoiseFraction=0;
    peakArea=0;
    peakSplineArea=0;
    peakAreaCorrected=0;
    peakAreaFractional=0;
    peakAreaTop=0;
    peakAreaTopCorrected=0;
    peakIntensity=0; 
    peakMz=0;
    peakBaseLineLevel=0;
    quality=0;
    rt=0;
    rtmax=0;
    rtmin=0;
    scan=0;
    signalBaselineRatio=0;
    signalBaselineDifference=0;
    symmetry=0;
    width=0;
    gaussFitSigma=10;
    gaussFitR2=FLT_MAX;
    peakRank=INT_MAX;
    if (sample == NULL && eic != NULL ) sample = eic->sample;
}


void Peak::copyObj(const Peak& o ) {
    pos= o.pos;
    eic = o.eic;
    sample = o.sample;
    baseMz= o.baseMz;
    fromBlankSample= o.fromBlankSample;
    groupOverlap= o.groupOverlap;
    groupOverlapFrac= o.groupOverlapFrac;
    groupNum=o.groupNum;
    label= o.label;
    localMaxFlag= o.localMaxFlag;
    maxscan= o.maxscan;
    medianMz= o.medianMz;
    minpos= o.minpos;
    maxpos= o.maxpos;
    splineminpos= o.splineminpos;
    splinemaxpos= o.splinemaxpos;
    minscan= o.minscan;
    mzmax= o.mzmax;
    mzmin= o.mzmin;
    noNoiseObs= o.noNoiseObs;
    noNoiseFraction= o.noNoiseFraction;
    peakArea= o.peakArea;
    peakSplineArea = o.peakSplineArea;
    peakAreaCorrected=o.peakAreaCorrected;
    peakAreaFractional=o.peakAreaFractional;
    peakAreaTop=o.peakAreaTop;
    peakAreaTopCorrected=o.peakAreaTopCorrected;
    peakIntensity=o.peakIntensity;
    peakMz= o.peakMz;
    quality= o.quality;
    peakBaseLineLevel=o.peakBaseLineLevel;
    rt=o.rt;
    rtmax= o.rtmax;
    rtmin= o.rtmin;
    scan= o.scan;
    signalBaselineRatio= o.signalBaselineRatio;
    signalBaselineDifference= o.signalBaselineDifference;
    symmetry= o.symmetry;
    width= o.width;
    gaussFitSigma=o.gaussFitSigma;
    gaussFitR2= o.gaussFitR2;
    peakRank=o.peakRank;
}

Peak& Peak::operator=(const Peak& o)  {
    copyObj(o);
    return *this;
}

Peak::Peak(const Peak& o ) { copyObj(o); }

vector<mzLink> Peak::findCovariants() {

    vector<mzLink>covariants;
    if (sample == NULL ) return covariants;

    //find scan range in which we will be checking for covariance
    vector<Scan*> scans;
    for(unsigned int i=0; i<sample->scans.size(); i++ ) {
        Scan* _scan = sample->scans[i];
        if(_scan == NULL ) continue;
        if(_scan->mslevel != 1) continue;
        if(_scan->rt < rt-0.1) continue;
        if(_scan->rt > rt+0.1) break;
        scans.push_back(_scan);
    }
    if ( scans.size() == 0 ) return covariants;
    int scanCount= scans.size();
    cerr << "Scans=" << scanCount << endl;

    //constuct Map
    map<int, vector<float> >M;
    map<int, vector<float> >::iterator itr;
    for(unsigned int i=0; i<scans.size(); i++ ) {
        Scan* _scan = scans[i];
        for(unsigned int j=0; j<_scan->nobs(); j++ ) {
            int rmz = int(_scan->mz[j]*1000);
            if (M[rmz].size()==0 )  M[rmz].resize(scanCount);
            M[rmz][i]= _scan->intensity[j];
        }
    } 

    //merge ajacent slices
    int lastMz=0;
    // for(itr = M.begin(); itr != M.end(); itr++) {
    for(itr = M.begin(); itr != M.end(); ++itr) { //naman: Prefer prefix ++/-- operators for non-primitive types. 
        float rmz = (*itr).first;
        if ( lastMz != 0 && rmz-lastMz < 2 ) {//merge
            for (int j=0; j < scanCount; j++ ) { 
                M[rmz][j] = max( M[lastMz][j], M[rmz][j]);
                M[lastMz][j]=0;
            }
        }
        lastMz = (*itr).first;
    }

    //refvector
    int refbinguess = int(peakMz*1000);
    int maxobs=0;
    vector<float> yref;

    for (int i=refbinguess-5; i < refbinguess+5; i++ ) {
        if ( M.count(i) ) {
            vector<float> y =  M[i];
            int nonzerCount=0; 
            for (int j=0; j<scanCount; j++ ) if (y[j]!=0) nonzerCount++;
            if ( nonzerCount > maxobs ) { yref=y; maxobs=nonzerCount; }
        }
    }
    if (yref.size()==0) return covariants;

    cerr << "Reference" << endl;
    for (int j=0; j<scanCount; j++ ) cerr << yref[j]; cerr << endl;

    // for(itr = M.begin(); itr != M.end(); itr++) { 
    for(itr = M.begin(); itr != M.end(); ++itr) { //naman: Prefer prefix ++/-- operators for non-primitive types. 
        int rmz = (*itr).first;
        vector<float> y =  (*itr).second;
        //float score=matchScore(yref, y );
        float score = mzUtils::correlation(yref,y);
        if ( (float) score < 0.5) continue;
        mzLink link;
        link.mz1 = peakMz;
        link.mz2 = rmz/1000.0;
        link.value1 = pos;
        link.correlation = score;
        link.note = "Covariant";
        covariants.push_back(link);
    }
    return covariants;
}

Scan* Peak::getScan() {
    if (sample) {
        return sample->getScan(scan);
    } else {
        return NULL;
    }
}

bool Peak::compSampleName(const Peak& a, const Peak& b) {
    return a.sample->getSampleName() < b.sample->getSampleName();
}

bool Peak::compSampleOrder(const Peak& a, const Peak& b) {
    return a.sample->getSampleOrder() < b.sample->getSampleOrder();
}

float Peak::overlap(const Peak& a, const Peak& b) {
    return( checkOverlap(a.rtmin, a.rtmax, b.rtmin, b.rtmax)); 
}

bool Peak::compMz(const Peak& a, const Peak& b ) { 
    return a.peakMz < b.peakMz; 
}
        
bool Peak::compArea(const Peak& a, const Peak& b ) { 
    return b.peakAreaFractional < a.peakAreaFractional; 
}

bool Peak::compQuality(const Peak& a, const Peak& b )
{
    return b.quality < a.quality;
}

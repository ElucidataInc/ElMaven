#include "PeakGroup.h"
#include "Compound.h"
#include "mzSample.h"

PeakGroup::PeakGroup()  {
    groupId=0;
    metaGroupId=0;
    clusterId = 0;
    groupRank=INT_MAX;

    maxIntensity=0;
    maxAreaTopIntensity = 0;
    maxAreaIntensity = 0;
    maxHeightIntensity = 0;
    maxAreaNotCorrectedIntensity = 0;
    maxAreaTopNotCorrectedIntensity = 0;

    currentIntensity = 0;
    meanRt=0;
    meanMz=0;
    expectedMz=0;

    blankMax=0;
    blankSampleCount=0;
    blankMean=0;

    sampleMax=0;
    sampleCount=0;
    sampleMean=0;

    deletedFlag = false;

    totalSampleCount=0;
    maxNoNoiseObs=0;
    maxPeakFracionalArea=0;
    maxSignalBaseRatio=0;
    maxSignalBaselineRatio=0;
    maxPeakOverlap=0;
    maxQuality=0;
    minQuality = 0.2;
    minIntensity = 0;

    //quantileIntensityPeaks = 0;
    //quantileQualityPeaks = 0;

    expectedRtDiff=-1;
    expectedAbundance=0;
    isotopeC13count=0;

    minRt=0;
    maxRt=0;

    minMz=0;
    maxMz=0;

    parent = NULL;
    //adduct = NULL;
    compound = NULL;

    isFocused=false;
    label=0;    //classification label

    goodPeakCount=0;
    _type = None;

    changePValue=0;
    changeFoldRatio=0;
    //children.reserve(0);
    peaks.resize(0);
}

void PeakGroup::copyObj(const PeakGroup& o)  {
    groupId= o.groupId;
    metaGroupId= o.metaGroupId;
    clusterId = o.clusterId;
    groupRank= o.groupRank;

    minQuality = o.minQuality;
    minIntensity = o.minIntensity;
    maxIntensity= o.maxIntensity;
    maxAreaTopIntensity = o.maxAreaTopIntensity;
    maxAreaIntensity = o.maxAreaIntensity;
    maxHeightIntensity = o.maxHeightIntensity;
    maxAreaNotCorrectedIntensity = o.maxAreaNotCorrectedIntensity;
    maxAreaTopNotCorrectedIntensity = o.maxAreaTopNotCorrectedIntensity;
    currentIntensity = o.currentIntensity;
    meanRt=o.meanRt;
    meanMz=o.meanMz;
    expectedMz=o.expectedMz;

    blankMax=o.blankMax;
    blankSampleCount=o.blankSampleCount;
    blankMean=o.blankMean;

    //quantileIntensityPeaks = o.quantileIntensityPeaks;
    //quantileQualityPeaks = o.quantileQualityPeaks;

    sampleMax=o.sampleMax;
    sampleCount=o.sampleCount;
    sampleMean=o.sampleMean;

    totalSampleCount=o.totalSampleCount;
    maxNoNoiseObs=o.maxNoNoiseObs;
    maxPeakFracionalArea=o.maxPeakFracionalArea;
    maxSignalBaseRatio=o.maxSignalBaseRatio;
    maxSignalBaselineRatio=o.maxSignalBaselineRatio;
    maxPeakOverlap=o.maxPeakOverlap;
    maxQuality=o.maxQuality;
    expectedRtDiff=o.expectedRtDiff;
    expectedAbundance = o.expectedAbundance;
    isotopeC13count=o.isotopeC13count;

    deletedFlag = o.deletedFlag;

    minRt=o.minRt;
    maxRt=o.maxRt;

    minMz=o.minMz;
    maxMz=o.maxMz;

    parent = o.parent;
    compound = o.compound;

    srmId=o.srmId;
    isFocused=o.isFocused;
    label=o.label;

    goodPeakCount=o.goodPeakCount;
    _type = o._type;
    tagString = o.tagString;

    changeFoldRatio = o.changeFoldRatio;
    changePValue    = o.changePValue;
    peaks = o.peaks;
    samples=o.samples;
    copyChildren(o);
}

PeakGroup::~PeakGroup() {
    clear();
}

void PeakGroup::copyChildren(const PeakGroup& o) {
    children = o.children;
    for(unsigned int i=0; i < children.size(); i++ ) children[i].parent = this;
}

bool PeakGroup::isPrimaryGroup() {
    if(compound && compound->getPeakGroup() == this) return true;
    return false;
}

void PeakGroup::clear() {
    deletePeaks();
    deleteChildren();
    meanMz  = 0;
    expectedMz = 0;
    groupRank=INT_MAX;
}

//TODO: a duplicate function getPeak exists. Delete this function
Peak* PeakGroup::getSamplePeak(mzSample* sample) {
    for (unsigned int i=0; i< peaks.size(); i++ ) {
        if (peaks[i].getSample() == sample ) return &peaks[i];
    }
    return NULL;
}

void PeakGroup::deletePeaks() {
    peaks.clear();
}

bool PeakGroup::deletePeak(unsigned int index) {
    if ( index < children.size() ) {
        peaks.erase(peaks.begin()+index);
        return true;
    }
    return false;
}

float PeakGroup::meanRtW() {
    if (peakCount() == 0) return 0;

    float mean=0; float Wtotal=0;
    for(unsigned int i=0; i < peakCount(); i++ ) Wtotal +=peaks[i].peakIntensity;

    if (Wtotal > 0 ) {
        for(unsigned int i=0; i < peakCount(); i++ ) mean +=  peaks[i].peakIntensity/Wtotal * peaks[i].rt;
        return mean;
    } else {
        for(unsigned int i=0; i < peakCount(); i++ ) mean += peaks[i].rt;
        return mean / peakCount();
    }
}

float PeakGroup::medianRt() {
    float * rts = new float[peaks.size()];
    for(unsigned int i=0; i < peakCount(); i++ ) rts[i]=peaks[i].rt;
    float medianValue = mzUtils::median(rts, peakCount());
    delete[] rts;
    return medianValue;
}

void PeakGroup::deleteChildren() {
    children.clear();
}

bool PeakGroup::deleteChild(unsigned int index) {
    if ( index < children.size() ) {
        children.erase(children.begin()+index);
        return true;
    }
    return false;
}

bool PeakGroup::deleteChild(PeakGroup* child ) {
    if (!child) return false;

    vector<PeakGroup>::iterator it;
    it = find(children.begin(),children.end(),child);
    if ( *it == child ) {
        cerr << "deleteChild: setting child to empty";
        child->clear();
        return true;
        //sort(children.begin(), children.end(),PeakGroup::compIntensity);
        //for(int i=0; i < children.size(); i++ ) { cerr << &children[i] << endl; }
    }

    return false;
}

//return intensity vectory ordered by samples 
vector<float> PeakGroup::getOrderedIntensityVector(vector<mzSample*>& samples, QType type) {

    if (samples.size() == 0) { vector<float>x; return x; } //empty vector;

    map<mzSample*,float> sampleOrder;
    vector<float>maxIntensity(samples.size(),0);

    for( unsigned int j=0; j < samples.size(); j++) {
        sampleOrder[samples[j]]=j;
        maxIntensity[j]=0;
    }

    for( unsigned int j=0; j < peaks.size(); j++) {
        Peak& peak = peaks.at(j);
        mzSample* sample = peak.getSample();

        if ( sampleOrder.count(sample) > 0 ) {
            int s  = sampleOrder[ sample ];
            float y = 0;
            switch (type)  {
                case AreaTop: y = peak.peakAreaTopCorrected; break;
                case Area: y = peak.peakAreaCorrected; break;
                case Height: y = peak.peakIntensity; break;
                case AreaNotCorrected: y = peak.peakArea; break;
                case AreaTopNotCorrected: y = peak.peakAreaTop; break;
                case RetentionTime: y = peak.rt; break;
                case Quality: y = peak.quality; break;
                case SNRatio: y = peak.signalBaselineRatio; break;
                default: y = peak.peakIntensity; break;
            }

            //normalize
            if(sample) y *= sample->getNormalizationConstant();
            if(maxIntensity[s] < y) { maxIntensity[s]=y;}
        }
    }
    return maxIntensity;
}

void PeakGroup::computeAvgBlankArea(const vector<EIC*>& eics) {

    if (peaks.size() == 0 ) return;

    //find range to fill in
    float rtmin = peaks[0].rtmin;
    float rtmax = peaks[0].rtmax;

    for (unsigned int i=1; i < peaks.size(); i++ ) {
        if (peaks[i].rtmin < rtmin) rtmin = peaks[i].rtmin;
        if (peaks[i].rtmax > rtmax) rtmax = peaks[i].rtmax;
    }
    rtmin = rtmin-0.25;
    rtmax = rtmax+0.25;

    float sum=0; int len=0;
    for(unsigned int i=0; i < eics.size(); i++ ) {
        EIC* eic = eics[i];
        if(eic->sample != NULL && eic->sample->isBlank == false) continue;
        for(unsigned int pos=0; pos < eic->intensity.size(); pos++ ) {
            if ( eic->rt[pos] >= rtmin && eic->rt[pos] <= rtmax
                    && eic->intensity[pos] > 0) {
                sum += eic->intensity[pos];
                len++;
            }
        }
    }
    this->blankMean = 0; //default zero
    if ( len > 0 ) this->blankMean = (float) sum / len;
}

void PeakGroup::fillInPeaks(const vector<EIC*>& eics) {

    if (peaks.size() == eics.size()) return;
    if (peaks.size() == 0 ) return;

    //find range to fill in
    float rtmin = peaks[0].rtmin;
    float rtmax = peaks[0].rtmax;

    for (unsigned int i=1; i < peaks.size(); i++ ) {
        if (peaks[i].rtmin < rtmin) rtmin = peaks[i].rtmin;
        if (peaks[i].rtmax > rtmax) rtmax = peaks[i].rtmax;
    }

    int filledInCount=0;

    for(unsigned int i=0; i < eics.size(); i++ ) {
        EIC* eic = eics[i];
        if (eic == NULL ) continue;
        if (eic->spline == NULL ) continue;
        if (eic->intensity.size() == 0) continue;

        bool missing=true;

        for(unsigned int j=0; j < peaks.size(); j++ ) {
            if ( peaks[j].getEIC() == eic) {
                missing = false;
                break;
            }
        }

        if (missing) { //fill in peak
            int maxpos = 0;
            for(unsigned int pos=1; pos < eic->intensity.size()-1; pos++ ) {
                if ( eic != NULL && eic->intensity[pos] != 0 && eic->mz[pos] != 0 &&
                        eic->rt[pos] >= rtmin && eic->rt[pos] <= rtmax
                        && eic->spline[pos] > eic->spline[pos-1] && eic->spline[pos] > eic->spline[pos+1]
                   ) {
                    if (maxpos != 0 && eic->intensity[pos] > eic->intensity[maxpos]) {
                        maxpos=pos;
                    } else {
                        maxpos=pos;
                    }
                }
            }

            if (maxpos != 0 && eic->intensity[maxpos] != 0 ) {
                Peak peak(eic,maxpos);
                eic->findPeakBounds(peak);
                eic->getPeakDetails(peak);
                this->addPeak(peak);
                filledInCount++;
            }
        }
    }

    //cerr << "fillInPeaks" << rtmin << " " << rtmax << " " << eics.size() << " " peaks.size() << endl;
    //    if (filledInCount > 0) { this->fillInPeaks(eics); }
}

void PeakGroup::reduce() { // make sure there is only one peak per sample

    map <mzSample*, Peak> maxPeaks;
    map <mzSample*, Peak> :: iterator itr;
    if (peaks.size() < 2 ) return;

    float groupMeanRt=0;
    float totalWeight=1;


    for( unsigned int i=0; i < peaks.size(); i++)  { totalWeight +=  peaks[i].peakIntensity; }
    for( unsigned int i=0; i < peaks.size(); i++)  { groupMeanRt += peaks[i].rt * peaks[i].peakIntensity/totalWeight;  }


    //In each group, take peak that closest to the mean retention time of a group



    for( unsigned int i=0; i < peaks.size(); i++) {
        mzSample* c = peaks[i].getSample();
        //float rtdiff = abs(groupMeanRt-peaks[i].rt);

        /*
        //In each group, take peak that closest to the mean retention time of a group
        if ( maxPeaks.count(c) == 0 ||  rtdiff < abs( groupMeanRt - maxPeaks[c].rt) ) {
        maxPeaks[c].copyObj(peaks[i]);
        }
        */

        //In each group, take the hghest peak
        if ( maxPeaks.count(c) == 0 || maxPeaks[c].peakIntensity < peaks[i].peakIntensity) {
            maxPeaks[c].copyObj(peaks[i]);
        }
    }

    peaks.clear();
    for( itr = maxPeaks.begin(); itr != maxPeaks.end(); ++itr ) {
        const Peak& peak = (*itr).second;
        addPeak(peak);
    }
    //	cerr << "\t\t\treduce() from " << startSize << " to " << peaks.size() << endl;
}

void PeakGroup::updateQuality() {
    maxQuality=0;
    goodPeakCount=0;
    for(unsigned int i=0; i< peaks.size(); i++) {
        if(peaks[i].quality > maxQuality) maxQuality = peaks[i].quality;
        if(peaks[i].quality > minQuality) goodPeakCount++; //Sabu
    }
}

// TODO: Remove this function as expected mz should be calculated while creating the group - Sahil
double PeakGroup::getExpectedMz(int charge) {

    float mz = 0;

    if (isIsotope() && childCount() == 0 && compound && !compound->formula.empty() && compound->mass > 0) { 
        return expectedMz;
    }
    else if (!isIsotope() && compound && compound->mass > 0) {
        if (!compound->formula.empty()) {
            mz = compound->adjustedMass(charge);
        } else {
            mz = compound->mass;
        }

        return mz;
    }
    else if (compound && compound->mass == 0 && compound->productMz > 0) {
        mz = compound->productMz;
        return mz;
    }

    return -1;

}

void PeakGroup::groupStatistics() {
    float rtSum = 0;
    float mzSum = 0;
    maxIntensity = 0;
    maxAreaTopIntensity = 0;
    maxAreaIntensity = 0;
    maxHeightIntensity = 0;
    maxAreaNotCorrectedIntensity = 0;
    maxAreaTopNotCorrectedIntensity = 0;
    currentIntensity = 0;
    totalSampleCount =  0;

    blankMax =0;
    blankSampleCount=0;

    sampleMax=0;
    sampleCount=0;
    sampleMean=0;

    maxNoNoiseObs=0;
    minRt = 0;
    maxRt = 0;
    minMz = 0;
    maxMz = 0;

    maxPeakFracionalArea=0;
    maxQuality=0;
    goodPeakCount=0;
    maxSignalBaselineRatio=0;
    //quantileIntensityPeaks;
    //quantileQualityPeaks;
    int nonZeroCount=0;

    for(unsigned int i=0; i< peaks.size(); i++) {
        if(peaks[i].pos != 0 && peaks[i].baseMz != 0) { rtSum += peaks[i].rt; mzSum += peaks[i].baseMz; nonZeroCount++; }
        if(peaks[i].peakIntensity > 0) totalSampleCount++;

        float max;
        switch(quantitationType){
            case AreaTop: max = peaks[i].peakAreaTopCorrected; break;
            case Area: max = peaks[i].peakAreaCorrected; break;
            case Height: max = peaks[i].peakIntensity; break;
            case AreaTopNotCorrected: max = peaks[i].peakAreaTop; break;
            case AreaNotCorrected: max = peaks[i].peakArea; break;
            default: max = peaks[i].peakIntensity; break;
        }

        if(peaks[i].peakAreaTopCorrected > maxAreaTopIntensity) maxAreaTopIntensity = peaks[i].peakAreaTopCorrected;
        if(peaks[i].peakAreaCorrected > maxAreaIntensity) maxAreaIntensity = peaks[i].peakAreaCorrected;
        if(peaks[i].peakIntensity > maxHeightIntensity) maxHeightIntensity = peaks[i].peakIntensity;
        if(peaks[i].peakArea > maxAreaNotCorrectedIntensity) maxAreaNotCorrectedIntensity = peaks[i].peakArea;
        if(peaks[i].peakAreaTop > maxAreaTopNotCorrectedIntensity) maxAreaTopNotCorrectedIntensity = peaks[i].peakAreaTop;

        //if(max > minIntensity) quantileIntensityPeaks++;
        //if(peaks[i].quality > minQuality) quantileQualityPeaks++;  

        if(max>maxIntensity) {
            maxIntensity = max;
            currentIntensity = max;
            meanMz=peaks[i].baseMz;
            meanRt=peaks[i].rt;
        }

        if(peaks[i].noNoiseObs>maxNoNoiseObs) maxNoNoiseObs = peaks[i].noNoiseObs;
        if(minRt == 0 || peaks[i].rtmin < minRt) minRt = peaks[i].rtmin;
        if(maxRt == 0 || peaks[i].rtmax > maxRt) maxRt = peaks[i].rtmax;
        if(minMz == 0 || peaks[i].mzmin < minMz) minMz = peaks[i].mzmin;
        if(maxMz == 0 || peaks[i].mzmax> maxMz) maxMz = peaks[i].mzmax;
        if(peaks[i].peakAreaFractional > maxPeakFracionalArea) maxPeakFracionalArea=peaks[i].peakAreaFractional;
        if(peaks[i].quality > maxQuality) maxQuality = peaks[i].quality;
        if(peaks[i].quality > minQuality) goodPeakCount++; //Sabu
        if ( peaks[i].signalBaselineRatio > maxSignalBaselineRatio) maxSignalBaselineRatio =  peaks[i].signalBaselineRatio;


        if(peaks[i].fromBlankSample) {
            blankSampleCount++;
            if(peaks[i].peakIntensity > blankMax) blankMax = peaks[i].peakIntensity;
        } else {
            sampleMean += peaks[i].peakIntensity;
            sampleCount++;
            if(peaks[i].peakIntensity > sampleMax) sampleMax = peaks[i].peakIntensity;
        }
    }

    if (sampleCount>0) sampleMean = sampleMean/sampleCount;
    if ( nonZeroCount ) {
        meanRt = rtSum/nonZeroCount;
        meanMz = mzSum/nonZeroCount;
    }

    groupOverlapMatrix();
}

void PeakGroup::groupOverlapMatrix() {

    for(unsigned int i=0; i< peaks.size(); i++) peaks[i].groupOverlapFrac=0;

    for(unsigned int i=0; i< peaks.size(); i++) {
        Peak& a = peaks[i];
        for(unsigned int j=i; j< peaks.size(); j++) {
            Peak& b = peaks[j];
            float overlap = checkOverlap(a.rtmin,a.rtmax,b.rtmin,b.rtmax); //check for overlap
            if (overlap > 0 ) { b.groupOverlapFrac += log(overlap); a.groupOverlapFrac += log(overlap); }

            /*
               if ( overlap > 0.1 ) {
               b.peakAreaFractional < 1 ? a.groupOverlapFrac += log(1-b.peakAreaFractional) : a.groupOverlapFrac += log(0.01);
               a.peakAreaFractional < 1 ? b.groupOverlapFrac += log(1-a.peakAreaFractional) : b.groupOverlapFrac += log(0.01);
               }
               */
        }
    }
    //normalize
    for(unsigned int i=0; i< peaks.size(); i++) peaks[i].groupOverlapFrac /= peaks.size();
}

void PeakGroup::summary() {
    cerr << tagString << endl;
    cerr
        <<"\t" << "meanRt=" << meanRt << endl
        <<"\t" << "meanMz=" << meanMz << endl
        <<"\t" << "expectedMz=" << expectedMz << endl
        <<"\t" << "goodPeakCount=" << goodPeakCount << endl
        <<"\t" << "maxQuality=" <<  maxQuality << endl
        <<"\t" << "maxNoNoiseObs=" << maxNoNoiseObs << endl
        <<"\t" << "sampleCount=" << sampleCount << endl
        <<"\t" << "maxSignalBaselineRatio=" << maxSignalBaselineRatio << endl
        <<"\t" << "maxPeakFracionalArea=" << maxPeakFracionalArea << endl
        <<"\t" << "blankMean=" << blankMean << endl
        <<"\t" << "sampleMean=" << sampleMean << endl
        <<"\t" << "maxIntensity=" << maxIntensity << endl
        << endl;

    for (unsigned int i=0; i < peaks.size(); i++ ) {
        cerr << "\t\t" << "Q:" << peaks[i].quality<< " "
            << "pAf:" << peaks[i].peakAreaFractional<< " "
            << "noNf" << peaks[i].noNoiseFraction << " "
            << "noObs:" << peaks[i].noNoiseObs   << " "
            << "w:"<< peaks[i].width<< " "
            << "sn:" << peaks[i].signalBaselineRatio << " "
            << "ovp:" << peaks[i].groupOverlapFrac << endl;
    }

    for(unsigned int i=0; i < children.size(); i++ ) children[i].summary();
}

PeakGroup::PeakGroup(const PeakGroup& o)  {
    copyObj(o);
}

PeakGroup& PeakGroup::operator=(const PeakGroup& o)  {
    copyObj(o);
    return *this;
}


bool PeakGroup::operator==(const PeakGroup* o)  {
    if ( this == o ) {
        cerr << o << " " << this << endl;
        return true;
    }
    return false;
}

Peak* PeakGroup::getPeak(mzSample* s ) {
    if ( s == NULL ) return NULL;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        if ( peaks[i].getSample() == s ) {
            return &peaks[i];
        }
    }
    return NULL;
}


void PeakGroup::reorderSamples() {
    std::sort(peaks.begin(), peaks.end(), Peak::compIntensity);
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* s = peaks[i].getSample();
        if ( s != NULL ) s->setSampleOrder(i);
    }
}

string PeakGroup::getName() {
    string tag;
    //compound is assigned in case of targeted search
    if (compound) tag = compound->name;
    //add isotopic label
    if (!tagString.empty()) tag += " | " + tagString;
    //add SRM ID for MS/MS data 
    if (!srmId.empty()) tag +=  " | " + srmId;
    //no compound in case of untargeted peak detection
    //group is referenced as MeanMz@MeanRT
    if (tag.empty() && meanMz && meanRt) {
        stringstream stream;
        stream << fixed << setprecision(6) << meanMz << "@" << setprecision(2) << meanRt;
        tag = stream.str();
    }
    //if all else fails, use group ID
    if (tag.empty()) tag = integer2string(groupId);
    return tag;
}

/*
@author: Sahil, Kiran
*/
//TODO: Sahil - Kiran, Added while merging mainwindow
vector<Scan*> PeakGroup::getRepresentativeFullScans() {
    vector<Scan*>matchedscans;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* sample = peaks[i].getSample();
        if ( sample == NULL ) continue;
        Scan* scan = sample->getScan(peaks[i].scan);
        if (scan and scan->mslevel == 1) matchedscans.push_back(scan);
    }
    return matchedscans;
}

/*
   @author: Sahil
   */ 
//TODO: Sahil, Added while merging Spectrawidget
vector<Scan*> PeakGroup::getFragmenationEvents() {
    vector<Scan*>matchedscans;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* sample = peaks[i].getSample();
        if ( sample == NULL ) continue;

        for( unsigned int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->scans[j];
            if (scan->mslevel <= 1) continue; //ms2 + scans only
            if (scan->rt < peaks[i].rtmin) continue;
            if (scan->rt > peaks[i].rtmax) break;
            if( scan->precursorMz >= minMz and scan->precursorMz <= maxMz) {
                matchedscans.push_back(scan);
            }
        }
    }
    return matchedscans;
}


/*
   @author: Sahil
   */
//TODO: Sahil, Added while merging spectrawidget
Scan* PeakGroup::getAverageFragmenationScan( MassCutoff *massCutoff) {

    int scanCount=0;
    map<float,double> mz_intensity_map;
    map<float,double> mz_bin_map;
    map<float,int> mz_count;

    vector<Scan*> scans = getFragmenationEvents();
    if (scans.size() == 0 ) return NULL;

    Scan* avgScan = new Scan(NULL,0,0,0,0,0);
    avgScan->deepcopy(scans[0]);

    if (scans.size() == 1) return avgScan;

    for(unsigned int s=0; s < scans.size(); s++) {
        Scan* scan = scans[s];
        scanCount++;

        //vector<int>positions = scan->intensityOrderDesc();
        //for(unsigned int p=0; p <positions.size() and p<100; p++ ) {
        for(unsigned int i=0; i < scan->nobs(); i++ ) {
            int pos = avgScan->findClosestHighestIntensityPos(scan->mz[i],massCutoff);
            float bin;
            if (pos >= 0) {
                bin = FLOATROUND(avgScan->mz[pos],1000);
            } else {
                bin = FLOATROUND(scan->mz[i],1000);
            }
            mz_intensity_map[bin] += ((double) scan->intensity[i]);
            mz_bin_map[bin] += ((double)(scan->intensity[i])*(scan->mz[i]));
            mz_count[bin]++;
        }


        //resort
        vector<float> avgmzs;
        vector<float> avgints;
        map<float,double>::iterator itr;
        for(itr = mz_intensity_map.begin(); itr != mz_intensity_map.end(); ++itr ) {
            avgmzs.push_back((*itr).first);
            avgints.push_back((*itr).second);
        }
        avgScan->mz = avgmzs;
        avgScan->intensity = avgints;
    }

    //average
    vector<float> avgmzs;
    vector<float> avgints;
    map<float,double>::iterator itr;
    for(itr = mz_intensity_map.begin(); itr != mz_intensity_map.end(); ++itr ) {
        float bin = (*itr).first;
        double totalIntensity=(*itr).second;
        double avgMz =  mz_bin_map[bin] / totalIntensity;
        avgmzs.push_back((float)avgMz);
        avgints.push_back((float) totalIntensity / scanCount);
    }

    avgScan->mz = avgmzs;
    avgScan->intensity = avgints;

    //cout << "getAverageScan() from:" << from << " to:" << to << " scanCount:" << scanCount << "scans. mzs=" << avgScan->nobs() << endl;
    return avgScan;
    }

void PeakGroup::calGroupRank(bool deltaRtCheckFlag,
                            float compoundRTWindow,
                            int qualityWeight,
                            int intensityWeight,
                            int deltaRTWeight) {

    float rtDiff = -1;

    if (compound != NULL && compound->expectedRt > 0)
    {
        rtDiff = abs(compound->expectedRt - (meanRt));
        expectedRtDiff = rtDiff;
    }

    // Peak Group Rank accoording to given weightage
    double A = (double) qualityWeight/10;
    double B = (double) intensityWeight/10;
    double C = (double) deltaRTWeight/10;

    if (deltaRtCheckFlag && compound != NULL && compound->expectedRt > 0) {
        groupRank = pow(rtDiff, 2*C) * pow((1.1 - maxQuality), A)
                                * (1 /( pow(log(maxIntensity + 1), B))); //TODO Formula to rank groups
    } else {

        groupRank = pow((1.1 - maxQuality), A)
                                * (1 /(pow(log(maxIntensity + 1), B)));

    }

}

void PeakGroup::setSelectedSamples(vector<mzSample*> vsamples){
    samples.clear();
    /**
     * @details- this method used for assigning samples to this group based on whether that samples
     * are marked as selected.
    */
    for(int i=0;i<vsamples.size();++i){
        if(vsamples[i]->isSelected) {
            samples.push_back(vsamples[i]);
        }
    }
}
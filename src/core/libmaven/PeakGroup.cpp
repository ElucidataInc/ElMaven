#include "PeakGroup.h"
#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "mzSample.h"
#include "EIC.h"
#include "Scan.h"
#include "mzSample.h"
#include "mzMassCalculator.h"

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

    ms2EventCount = 0;

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
    avgPeakQuality=0;
    groupQuality=0;
    weightedAvgPeakQuality=0;
    predictedLabel=0;
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

    // TODO: MAVEN (upstream) strikes again. Why was it commented out?
    adduct = NULL;

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

    ms2EventCount = o.ms2EventCount;
    fragMatchScore = o.fragMatchScore;
    fragmentationPattern = o.fragmentationPattern;
    adduct = o.adduct;

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
    avgPeakQuality=o.avgPeakQuality;
    groupQuality=o.groupQuality;
    weightedAvgPeakQuality=o.weightedAvgPeakQuality;
    predictedLabel=o.predictedLabel;
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

    markedBadByCloudModel = o.markedBadByCloudModel;
    markedGoodByCloudModel = o.markedGoodByCloudModel;


    copyChildren(o);
}

PeakGroup::~PeakGroup() {
    clear();
}

void PeakGroup::copyChildren(const PeakGroup& o) {
    children = o.children;
    childrenBarPlot = o.childrenBarPlot;
    for(unsigned int i=0; i < children.size(); i++ ) children[i].parent = this;
    for(unsigned int i=0; i < childrenBarPlot.size(); i++ )
        childrenBarPlot[i].parent = this;
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

bool PeakGroup::isMS1()
{
    if (peaks.size() == 0) return false;

    Peak peak = peaks[0];
    if (peak.getSample()) {
        Scan* scan = peak.getSample()->getScan(peak.scan);
        if (scan && scan->mslevel == 1)
            return true;
    }

    return false;
}

void PeakGroup::addPeak(const Peak &peak)
{
	peaks.push_back(peak);
	peaks.back().groupNum = groupId;
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

float PeakGroup::massCutoffDist(float cmass,MassCutoff *massCutoff)
{
    return mzUtils::massCutoffDist(cmass,meanMz,massCutoff);
}

void PeakGroup::updateQuality() {
    maxQuality=0;
    goodPeakCount=0;

    float peakQualitySum=0;
    float weightedSum=0;
    float sumWeights=0;
    for(const auto peak : peaks) {
        if(peak.quality > maxQuality) maxQuality = peak.quality;
        if(peak.quality > minQuality) goodPeakCount++; //Sabu
        peakQualitySum += peak.quality;
        weightedSum += peak.quality * peak.peakIntensity;
        sumWeights += peak.peakIntensity;
    }
    avgPeakQuality = peakQualitySum / peaks.size();
    weightedAvgPeakQuality = weightedSum / sumWeights;
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
    avgPeakQuality=0;
    groupQuality=0;
    weightedAvgPeakQuality=0;
    predictedLabel=0;
    goodPeakCount=0;
    maxSignalBaselineRatio=0;
    //quantileIntensityPeaks;
    //quantileQualityPeaks;
    int nonZeroCount=0;
    //@Kailash: Added for Avg Peak Quality and Intensity Weighted Peak Quality
    float peakQualitySum=0;
    float highestIntensity=0;
    float weightedSum = 0;
    float sumWeights = 0;

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

        weightedSum += peaks[i].quality * peaks[i].peakIntensity;
        sumWeights += peaks[i].peakIntensity;
        peakQualitySum += peaks[i].quality;
        if (peaks[i].peakIntensity > highestIntensity) highestIntensity = peaks[i].peakIntensity;
    }
    avgPeakQuality = peakQualitySum / peaks.size();
    weightedAvgPeakQuality = weightedSum/sumWeights;

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

map<float, float> PeakGroup::createAvgSpectra(MassCutoff* cutoff)
{
    map<float, float> mzIntensityMap;
    auto scans = getRepresentativeFullScans();

    // map all m/z to intensity from all scans in ascending order
    for (auto scan : scans) {
        for (size_t i = 0; i < scan->mz.size(); ++i)
            mzIntensityMap[scan->mz.at(i)] = scan->intensity.at(i);
    }

    // bucket based on mass cutoff and only keep average m/z and intensity
    // values for each bucket
    map<float, float> binMzIntensityMap;
    float currentMinMz = mzIntensityMap.begin()->first;
    float lastMz = mzIntensityMap.rbegin()->first;
    vector<float> mzBuffer;
    vector<float> intensityBuffer;
    for (auto elem : mzIntensityMap) {
        auto mz = elem.first;
        auto intensity = elem.second;
        if (!mzUtils::withinXMassCutoff(currentMinMz, mz, cutoff)
            || mz == lastMz) {
            if (mz == lastMz) {
                mzBuffer.push_back(mz);
                intensityBuffer.push_back(intensity);
            }
            float avgMz = accumulate(begin(mzBuffer),
                                     end(mzBuffer),
                                     0.0f) / mzBuffer.size();
            float avgIntensity = accumulate(begin(intensityBuffer),
                                            end(intensityBuffer),
                                            0.0f) / intensityBuffer.size();
            binMzIntensityMap[avgMz] = avgIntensity;
            mzBuffer.clear();
            intensityBuffer.clear();
            currentMinMz = mz;
        }
        mzBuffer.push_back(mz);
        intensityBuffer.push_back(intensity);
    }

    // normalize to range [0, 1000]
    auto maxIt = max_element(begin(binMzIntensityMap),
                             end(binMzIntensityMap),
                             [](const pair<float, float> l,
                                 const pair<float, float> r) -> bool {
                                 return l.second < r.second;
                             });
    float maxIntensity = maxIt->second;
    for_each(begin(binMzIntensityMap),
             end(binMzIntensityMap),
             [&](pair<const float, float>& p) {
                p.second = (p.second / maxIntensity) * 1000.0f;
             });

    // erase all intensities below 50 (less than 5% of maximum) as noise
    for (auto it = begin(binMzIntensityMap);
         it != end(binMzIntensityMap);) {
        if(it->second < 50) {
            it = binMzIntensityMap.erase(it);
        } else {
            ++it;
        }
    }

    return binMzIntensityMap;
}


vector<Scan*> PeakGroup::getFragmentationEvents()
{
    vector<Scan*> matchedScans;
    if (!this->isMS1()) return matchedScans;
    
    for(auto peak : peaks) {
        mzSample* sample = peak.getSample();
        if (sample == NULL) continue;
        mzSlice slice(minMz, maxMz, peak.rtmin, peak.rtmax);
        vector<Scan*> scans = sample->getFragmentationEvents(&slice);

        matchedScans.insert(matchedScans.end(), scans.begin(), scans.end());
    }
    return matchedScans;
}

void PeakGroup::computeFragPattern(float productPpmTolr)
{
    vector<Scan*> ms2Events = getFragmentationEvents();
    if (ms2Events.size() == 0) return;
    sort(ms2Events.begin(), ms2Events.end(), Scan::compIntensity);

    float minFractionalIntensity = 0.01;
    float minSignalNoiseRatio = 1;
    int maxFragmentSize = 1024;
    Fragment fragment(ms2Events[0],
                      minFractionalIntensity,
                      minSignalNoiseRatio,
                      maxFragmentSize);
    
    for(Scan* scan : ms2Events) {
        fragment.addBrotherFragment(new Fragment(scan,
                                                 minFractionalIntensity,
                                                 minSignalNoiseRatio,
                                                 maxFragmentSize));
    }
    
    fragment.buildConsensus(productPpmTolr);
    fragment.consensus->sortByMz();
    fragmentationPattern = fragment.consensus;
    ms2EventCount = ms2Events.size();
}

Scan* PeakGroup::getAverageFragmentationScan(float productPpmTolr)
{
    //build consensus ms2 specta
    computeFragPattern(productPpmTolr);
    Scan* avgScan = new Scan(NULL, 0, 2, 0, 0, 0);

    for(unsigned int i = 0; i < fragmentationPattern.mzValues.size(); i++) {
        avgScan->mz.push_back(fragmentationPattern.mzValues[i]);
        avgScan->intensity.push_back(fragmentationPattern.intensityValues[i]);
    }

    avgScan->precursorMz = meanMz;
    avgScan->rt = meanRt;
    
    return avgScan;
}

void PeakGroup::matchFragmentation(float ppmTolerance, string scoringAlgo)
{
    if (this->compound == NULL || ms2EventCount == 0) return;

    fragMatchScore = compound->scoreCompoundHit(&fragmentationPattern, ppmTolerance);
    fragMatchScore.mergedScore = fragMatchScore.getScoreByName(scoringAlgo);
}

void PeakGroup::calGroupRank(bool deltaRtCheckFlag,
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

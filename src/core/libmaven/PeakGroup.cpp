#include "PeakGroup.h"
#include "Compound.h"
#include "datastructures/adduct.h"
#include "datastructures/isotope.h"
#include "datastructures/mzSlice.h"
#include "mzSample.h"
#include "EIC.h"
#include "Scan.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "mzMassCalculator.h"

PeakGroup::PeakGroup(shared_ptr<MavenParameters> parameters,
                     IntegrationType integrationType)
{
    _parameters = parameters;
    _integrationType = integrationType;

    _groupId=0;
    _metaGroupId=0;
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
    _expectedMz = 0.0f;

    ms2EventCount = 0;

    blankMax=0;
    blankSampleCount=0;
    blankMean=0;

    sampleMax=0;
    sampleCount=0;
    sampleMean=0;

    deletedFlag = false;
    _isHiddenFromTable = false;

    totalSampleCount=0;
    maxNoNoiseObs=0;
    maxPeakFracionalArea=0;
    maxSignalBaseRatio=0;
    maxSignalBaselineRatio=0;
    maxPeakOverlap=0;
    maxQuality=0;
    avgPeakQuality=0;
    minQuality = 0.2;
    minIntensity = 0;

    _predictedLabel = ClassifiedLabel::None;
    _predictionProbability = 0.0f;
    
    peakMLBaseValue = 0.0;
    peakMLOutputValue = 0.0;
    isClassified = false;

    //quantileIntensityPeaks = 0;
    //quantileQualityPeaks = 0;

    _expectedAbundance = 0.0f;

    minRt=0;
    maxRt=0;

    minMz=0;
    maxMz=0;

    parent = nullptr;

    isFocused=false;
    _userLabel = '\0';    // default user classification label

    goodPeakCount=0;
    _type = GroupType::None;
    _sliceSet = false;

    _tableName = "";

    changePValue=0;
    changeFoldRatio=0;
    peaks.resize(0);
}

void PeakGroup::copyObj(const PeakGroup& o)  {
    _groupId = o._groupId;
    _metaGroupId = o._metaGroupId;
    clusterId = o.clusterId;
    groupRank = o.groupRank;

    minQuality = o.minQuality;
    minIntensity = o.minIntensity;
    maxIntensity = o.maxIntensity;
    maxAreaTopIntensity = o.maxAreaTopIntensity;
    maxAreaIntensity = o.maxAreaIntensity;
    maxHeightIntensity = o.maxHeightIntensity;
    maxAreaNotCorrectedIntensity = o.maxAreaNotCorrectedIntensity;
    maxAreaTopNotCorrectedIntensity = o.maxAreaTopNotCorrectedIntensity;
    currentIntensity = o.currentIntensity;
    meanRt = o.meanRt;
    meanMz = o.meanMz;
    _expectedMz = o._expectedMz;

    peakMLBaseValue = o.peakMLBaseValue;
    peakMLOutputValue = o.peakMLOutputValue;

    ms2EventCount = o.ms2EventCount;
    fragMatchScore = o.fragMatchScore;
    fragmentationPattern = o.fragmentationPattern;

    blankMax = o.blankMax;
    blankSampleCount = o.blankSampleCount;
    blankMean = o.blankMean;

    //quantileIntensityPeaks = o.quantileIntensityPeaks;
    //quantileQualityPeaks = o.quantileQualityPeaks;

    sampleMax = o.sampleMax;
    sampleCount = o.sampleCount;
    sampleMean = o.sampleMean;

    totalSampleCount = o.totalSampleCount;
    maxNoNoiseObs = o.maxNoNoiseObs;
    maxPeakFracionalArea = o.maxPeakFracionalArea;
    maxSignalBaseRatio = o.maxSignalBaseRatio;
    maxSignalBaselineRatio = o.maxSignalBaselineRatio;
    maxPeakOverlap = o.maxPeakOverlap;
    maxQuality = o.maxQuality;
    avgPeakQuality = o.avgPeakQuality;
    _expectedAbundance = o._expectedAbundance;
   
    deletedFlag = o.deletedFlag;

    minRt = o.minRt;
    maxRt = o.maxRt;

    minMz = o.minMz;
    maxMz = o.maxMz;

    parent = o.parent;
    setSlice(o.getSlice());

    srmId = o.srmId;
    isFocused = o.isFocused;
    _userLabel = o._userLabel;

    goodPeakCount = o.goodPeakCount;
    _type = o._type;
    _sliceSet = o.hasSlice();
    tagString = o.tagString;

    changeFoldRatio = o.changeFoldRatio;
    changePValue    = o.changePValue;
    peaks = o.peaks;
    samples=o.samples;

    markedBadByCloudModel = o.markedBadByCloudModel;
    markedGoodByCloudModel = o.markedGoodByCloudModel;

    _tableName = o.tableName();
    setPredictedLabel(o.predictedLabel(), o.predictionProbability());
    setPredictionInference(o.predictionInference());
    _correlatedGroups = o.getCorrelatedGroups();

    copyChildren(o);
    _parameters = make_shared<MavenParameters>(*(o.parameters().get()));
    _integrationType = o.integrationType();
}

PeakGroup::~PeakGroup() {
    _parameters.reset();
    clear();
}

void PeakGroup::copyChildren(const PeakGroup& o) {
    _childIsotopes.clear();
    for (auto child : o.childIsotopes())
        addIsotopeChild(*child);

    _childAdducts.clear();
    for (auto child : o.childAdducts())
        addAdductChild(*child);

    _childIsotopesBarPlot.clear();
    for (auto child : o.childIsotopesBarPlot())
        addIsotopeChildBarPlot(*child);
}

void PeakGroup::clear() {
    deletePeaks();
    deleteChildIsotopes();
    deleteChildAdducts();
    deleteChildIsotopesBarPlot();
    meanMz  = 0;
    _expectedMz = 0;
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

bool PeakGroup::hasCompoundLink() const
{
    if(hasSlice() && _slice.compound != NULL)
        return true;

    return false;
}

Compound* PeakGroup::getCompound() const
{
    if (hasSlice()) {
        return _slice.compound;
    }
    return NULL;
}

void PeakGroup::setCompound(Compound* compound)
{
    _slice.compound = compound;
    _sliceSet = true;
}

void PeakGroup::addPeak(const Peak &peak)
{
	peaks.push_back(peak);
    peaks.back().groupNum = _groupId;
}

void PeakGroup::setSlice(const mzSlice& slice)
{
    _slice = slice;
    _sliceSet = true;
    if (!_slice.isotope.isNone()) {
        tagIsotope(_slice.isotope.name,
                   _slice.isotope.mass,
                   _slice.isotope.abundance);
    }
    _updateType();
}

const mzSlice& PeakGroup::getSlice() const
{
    return _slice;
}

bool PeakGroup::hasSlice() const
{
    return _sliceSet;
}

bool PeakGroup::sliceIsZero() const
{
    if (((mzUtils::almostEqual(_slice.mzmin, 0.0f)
         && mzUtils::almostEqual(_slice.mzmax, 0.0f)))
        || (mzUtils::almostEqual(_slice.rtmin, 0.0f)
            && mzUtils::almostEqual(_slice.mzmin, 0.0f))) {
        return true;
    }
    return false;
}

void PeakGroup::deletePeaks() {
    peaks.clear();
}

bool PeakGroup::deletePeak(unsigned int index) {
    if (index < peakCount()) {
        peaks.erase(peaks.begin()+index);
        return true;
    }
    return false;
}

bool PeakGroup::deletePeak(mzSample* sample)
{
    auto peakIter = find_if(begin(peaks), end(peaks), [sample](Peak& peak) {
        return peak.getSample() == sample;
    });
    if (peakIter == end(peaks))
        return false;
    return deletePeak(distance(begin(peaks), peakIter));
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
    vector<float> rts;
    for(unsigned int i=0; i < peakCount(); i++ ) rts.push_back(peaks[i].rt);
    float medianValue = mzUtils::median(rts);
    return medianValue;
}

float PeakGroup::expectedRtDiff()
{
    auto associatedCompound = getCompound();
    if (associatedCompound != nullptr && associatedCompound->expectedRt() > 0) {
        return abs(associatedCompound->expectedRt() - meanRt);
    }
    return -1.0f;
}

void PeakGroup::deleteChildIsotopes()
{
    _childIsotopes.clear();
}

void PeakGroup::deleteChildAdducts()
{
    _childAdducts.clear();
}

void PeakGroup::deleteChildIsotopesBarPlot()
{
    _childIsotopesBarPlot.clear();
}

bool PeakGroup::removeChild(PeakGroup* child) {
    if (!child)
        return false;

    auto removeChildFromContainer =
        [this, child](vector<shared_ptr<PeakGroup>>& children) {
            children.erase(remove_if(begin(children),
                                     end(children),
                                     [&](shared_ptr<PeakGroup>& group) {
                                         return child == group.get();
                                     }),
                           children.end());
        };

    auto preDeletionChildCount = _childIsotopes.size()
                                 + _childAdducts.size()
                                 + _childIsotopesBarPlot.size();
    removeChildFromContainer(_childIsotopes);
    removeChildFromContainer(_childAdducts);
    removeChildFromContainer(_childIsotopesBarPlot);

    auto postDeletionChildCount = _childIsotopes.size()
                                  + _childAdducts.size()
                                  + _childIsotopesBarPlot.size();
    if (postDeletionChildCount == preDeletionChildCount)
        return false; // nothing was removed

    return true;
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

void PeakGroup::setLabel(char label)
{
    this->label = label;
}

float PeakGroup::massCutoffDist(float cmass,MassCutoff *massCutoff)
{
    return mzUtils::massCutoffDist(cmass,meanMz,massCutoff);
}

void PeakGroup::tagIsotope(string isotopeName,
                           float isotopeMass,
                           float isotopeAbundance)
{
    tagString = isotopeName;
    _expectedMz = isotopeMass;
    _expectedAbundance = isotopeAbundance;
}

float PeakGroup::getExpectedAbundance() const
{
    return _expectedAbundance;
}

void PeakGroup::updateQuality() {
    maxQuality=0;
    goodPeakCount=0;

    float peakQualitySum=0;
    for(const auto peak : peaks) {
        if(peak.quality > maxQuality) maxQuality = peak.quality;
        if(peak.quality > minQuality) goodPeakCount++; //Sabu
        peakQualitySum += peak.quality;
    }
    avgPeakQuality = peakQualitySum / peaks.size();
}

double PeakGroup::getExpectedMz(int charge)
{
    if (isIsotope()
        && hasSlice()
        && hasCompoundLink()) {
        return _expectedMz;
    } else if (!isIsotope()
               && hasSlice()
               && hasCompoundLink()
               && getCompound()->precursorMz() > 0
               && getCompound()->productMz() > 0) {
        return getCompound()->productMz();
    } else if (!isIsotope()
               && hasSlice()
               && hasCompoundLink()
               && getCompound()->mz() > 0) {
        Compound* compound = getCompound();
        float mz = 0.0;

        // for parent adducts, the global charge should be used and not that
        // of the associated adduct's
        if (adduct() != nullptr
            && !adduct()->isParent()
            && !compound->formula().empty()) {
            // computing the mass adjusted for adduct's mass
            auto mass = MassCalculator::computeNeutralMass(compound->formula());
            mz = adduct()->computeAdductMz(mass);
        } else if (adduct() != nullptr
                   && !adduct()->isParent()
                   && compound->neutralMass() != 0.0f) {
            // computing the mass adjusted for adduct's mass
            auto mass = compound->neutralMass();
            mz = adduct()->computeAdductMz(mass);
        } else if (!compound->formula().empty()
                   || compound->neutralMass() != 0.0f) {
            // regular adjusted mass if the formula or neutral mass is given
            mz = compound->adjustedMass(charge);
        } else if (compound->mz() > 0) {
            // m/z already present in the compound DB then just use that
            mz = compound->mz();
        }
        return mz;
    }

    return -1.0;
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
    goodPeakCount=0;
    maxSignalBaselineRatio=0;
    //quantileIntensityPeaks;
    //quantileQualityPeaks;
    int nonZeroCount=0;
    //@Kailash: Added for Avg Peak Quality and Intensity Weighted Peak Quality
    float peakQualitySum=0;
    float highestIntensity=0;

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

        peakQualitySum += peaks[i].quality;
        if (peaks[i].peakIntensity > highestIntensity) highestIntensity = peaks[i].peakIntensity;
    }
    avgPeakQuality = peakQualitySum / peaks.size();

    if (sampleCount>0) sampleMean = sampleMean/sampleCount;
    if (nonZeroCount) {
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

PeakGroup::PeakGroup(const PeakGroup& o, IntegrationType integrationType)
{
    copyObj(o);
    if (integrationType != IntegrationType::Inherit)
        _integrationType = integrationType;
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

string PeakGroup::getName() const {
    string tag;

    // compound is assigned in case of targeted search
    if (hasSlice() && _slice.compound != NULL)
        tag = _slice.compound->name();

    // no compound in case of untargeted peak detection
    // group is referenced as MeanMz@MeanRT
    if (tag.empty() && meanMz && meanRt) {
        stringstream stream;
        stream << fixed << setprecision(6) << meanMz << "@" << setprecision(2)
               << meanRt;
        tag = stream.str();
    }

    // add name of external charged species fused with adduct
    if (isAdduct())
        tag += " | " + adduct()->getName();

    // add isotopic label
    if (!tagString.empty())
        tag += " | " + tagString;

    // add SRM ID for MS/MS data
    if (!srmId.empty())
        tag += " | " + srmId;

    // if all else fails, use group ID
    if (tag.empty())
        tag = integer2string(_groupId);

    return tag;
}

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

vector<Scan*> PeakGroup::getFragmentationEvents()
{
    vector<Scan*> matchedScans;
    if (!this->isMS1()) return matchedScans;
    
    for(const auto& peak : peaks) {
        mzSample* sample = peak.getSample();
        if (sample == nullptr)
            continue;
        if (sample->ms2ScanCount() == 0
            || sample->msMsType() != mzSample::MsMsType::DDA) {
            continue;
        }

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
    if (this->getCompound() == nullptr
        || this->isAdduct()
        || ms2EventCount == 0) return;

    fragMatchScore = getCompound()->scoreCompoundHit(&fragmentationPattern,
                                                     ppmTolerance);
    fragMatchScore.mergedScore = fragMatchScore.getScoreByName(scoringAlgo);
}

void PeakGroup::calGroupRank(bool deltaRtCheckFlag,
                            int qualityWeight,
                            int intensityWeight,
                            int deltaRTWeight) {

    float rtDiff = expectedRtDiff();

    // Peak Group Rank accoording to given weightage
    double A = (double) qualityWeight/10;
    double B = (double) intensityWeight/10;
    double C = (double) deltaRTWeight/10;

    if (deltaRtCheckFlag && hasSlice() && _slice.compound != NULL && _slice.compound->expectedRt() > 0) {
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

Adduct* PeakGroup::adduct() const
{
    return getSlice().adduct;
}

void PeakGroup::setAdduct(Adduct* adduct)
{
    _slice.adduct = adduct;
    _updateType();
}

shared_ptr<PeakGroup> PeakGroup::addAdductChild(const PeakGroup& child)
{
    auto childCopy = make_shared<PeakGroup>(child);
    childCopy->parent = this;
    childCopy->_metaGroupId = _groupId;
    auto iter = mzUtils::insertSorted(_childAdducts,
                                      childCopy,
                                      [](const shared_ptr<PeakGroup>& a,
                                         const shared_ptr<PeakGroup>& b) {
                                          return a->getName() < b->getName();
                                      });
    return *iter;
}

Isotope PeakGroup::isotope() const
{
    return getSlice().isotope;
}

void PeakGroup::setIsotope(Isotope isotope)
{
    _slice.isotope = isotope;
    if (_slice.isotope.mass > 0.0) {
        tagIsotope(_slice.isotope.name,
                   _slice.isotope.mass,
                   _slice.isotope.abundance);
    }
    _updateType();
}

shared_ptr<PeakGroup> PeakGroup::addIsotopeChild(const PeakGroup& child)
{
    auto childCopy = make_shared<PeakGroup>(child);
    childCopy->parent = this;
    childCopy->_metaGroupId = _groupId;
    auto iter = mzUtils::insertSorted(_childIsotopes,
                                      childCopy,
                                      [](const shared_ptr<PeakGroup>& a,
                                         const shared_ptr<PeakGroup>& b) {
                                          return a->getName() < b->getName();
                                      });
    return *iter;
}

shared_ptr<PeakGroup> PeakGroup::addIsotopeChildBarPlot(const PeakGroup& child)
{
    auto childCopy = make_shared<PeakGroup>(child);
    childCopy->parent = this;
    childCopy->_metaGroupId = _groupId;
    _childIsotopesBarPlot.push_back(childCopy);
    return _childIsotopesBarPlot.back();
}

string PeakGroup::tableName() const
{
    return _tableName;
}

void PeakGroup::setTableName(string tableName)
{
    _tableName = tableName;
    for (auto child : _childIsotopes)
        child->setTableName(tableName);
    for (auto child : _childAdducts)
        child->setTableName(tableName);
}

void PeakGroup::_updateType()
{
    if (_slice.adduct != nullptr
        && _slice.adduct->getName() != MassCalculator::MinusHAdduct->getName()
        && _slice.adduct->getName() != MassCalculator::PlusHAdduct->getName()) {
        _type = GroupType::Adduct;
    } else if (!_slice.isotope.isNone() && !_slice.isotope.isParent()) {
        _type = GroupType::Isotope;
    } else {
        _type = GroupType::None;
    }
}
void PeakGroup::setGroupId(int groupId)
{
    _groupId = groupId;
    _metaGroupId = groupId;
    for (auto child : _childIsotopes)
        child->_metaGroupId = groupId;
    for (auto child : _childAdducts)
        child->_metaGroupId = groupId;
    for (auto child : _childIsotopesBarPlot)
        child->_metaGroupId = groupId;
    for (auto& peak : peaks) {
        peak.groupNum = groupId;
    }
}
void PeakGroup::setUserLabel(char label)
{
    if (label == 'g') {
        _predictedLabel = ClassifiedLabel::Signal;
    } else if (label == 'b') {
        _predictedLabel = ClassifiedLabel::Noise;
    } else if (label == '\0') {
        _predictedLabel = ClassifiedLabel::None;
    } else {
        // ignore invalid labels
        return;
    }
    _userLabel = label;
}

void PeakGroup::setPredictedLabel(const ClassifiedLabel label,
                                  const float probability)
{
    _predictedLabel = label;
    if (_predictedLabel == ClassifiedLabel::None) {
        this->_userLabel = '\0';
    } else if (_predictedLabel == ClassifiedLabel::MaybeGood) {
        this->_userLabel = '\0';
    } else if (_predictedLabel == ClassifiedLabel::Noise) {
        this->_userLabel = 'b';
    } else {
        this->_userLabel = 'g';
    }
    _predictionProbability = probability;
}

void PeakGroup::setPredictionInference(const multimap<float, string>& inference)
{
    _predictionInference = inference;
}

PeakGroup::ClassifiedLabel PeakGroup::predictedLabel() const
{
    return _predictedLabel;
}

float PeakGroup::predictionProbability() const
{
    return _predictionProbability;
}

multimap<float, string> PeakGroup::predictionInference() const
{
    return _predictionInference;
}

void PeakGroup::addCorrelatedGroup(int groupId,
                                   const float correlationFactor)
{
    _correlatedGroups[groupId] = correlationFactor;
}

map<int, float> PeakGroup::getCorrelatedGroups() const
{
    return _correlatedGroups;
}

void PeakGroup::setCorrelatedGroups(map<int, float> correlatedGroups)
{
    _correlatedGroups.clear();
    _correlatedGroups = correlatedGroups;
}
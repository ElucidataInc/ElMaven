#include "Compound.h"
#include "datastructures/adduct.h"
#include "datastructures/mzSlice.h"
#include "groupFiltering.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "PeakGroup.h"

GroupFiltering::GroupFiltering(MavenParameters *mavenParameters)
{
    _slice = nullptr;
    _mavenParameters = mavenParameters;
}

GroupFiltering::GroupFiltering(MavenParameters *mavenParameters, mzSlice* slice)
{
    _slice = slice;
    _mavenParameters = mavenParameters;
}

void GroupFiltering::filter(vector<PeakGroup> &peakgroups)
{
    size_t i = 0;
    while (i < peakgroups.size()) {
        if (filterByMS1(peakgroups[i]))
        {
            peakgroups.erase(peakgroups.begin() + i);
            continue;
        }

        // only filter for MS2 for groups having targets
        if (_mavenParameters->matchFragmentationFlag
            && peakgroups[i].getCompound() != nullptr
            && !(peakgroups[i].isAdduct())
            && peakgroups[i].ms2EventCount > 0
            && filterByMS2(peakgroups[i])) {
            peakgroups.erase(peakgroups.begin() + i);
            continue;
        }
        if (_mavenParameters->matchFragmentationFlag
            && !(peakgroups[i].isAdduct())
            && _mavenParameters->mustHaveFragmentation
            && peakgroups[i].ms2EventCount == 0) {
            peakgroups.erase(peakgroups.begin() + i);
            continue;
        }

        ++i;
    }

}

bool GroupFiltering::filterByMS1(PeakGroup &peakgroup)
{

    //TODO: remove compound assignment from filtering
    Compound* compound = _slice->compound;
    peakgroup.setQuantitationType((PeakGroup::QType)_mavenParameters->peakQuantitation);
    peakgroup.minQuality = _mavenParameters->minQuality;
    peakgroup.minIntensity = _mavenParameters->minGroupIntensity;

    peakgroup.groupStatistics();

    if (_mavenParameters->clsf->hasModel()) {
        _mavenParameters->clsf->classify(&peakgroup);
        peakgroup.updateQuality();
        if (peakgroup.goodPeakCount < _mavenParameters->minGoodGroupCount)
            return true;
    }

    if (peakgroup.maxNoNoiseObs < _mavenParameters->minNoNoiseObs)
        return true;
    if (quantileFilters(&peakgroup))
        return true;

    if (!_slice->srmId.empty())
        peakgroup.srmId = _slice->srmId;

    float rtDiff = peakgroup.expectedRtDiff();

    double A = (double)_mavenParameters->qualityWeight / 10;
    double B = (double)_mavenParameters->intensityWeight / 10;
    double C = (double)_mavenParameters->deltaRTWeight / 10;

    if (compound != NULL && compound->expectedRt() > 0)
    {
        if (_mavenParameters->deltaRtCheckFlag)
        {
            peakgroup.groupRank = pow(rtDiff, 2 * C) * pow((1.1 - peakgroup.maxQuality), A) * (1 / (pow(log(peakgroup.maxIntensity + 1), B))); //TODO Formula to rank groups
        }
        if (_mavenParameters->matchRtFlag && rtDiff > _mavenParameters->compoundRTWindow)
            return true;
    }

    if (!_mavenParameters->deltaRtCheckFlag || compound == NULL || compound->expectedRt() <= 0)
    {
        peakgroup.groupRank = pow((1.1 - peakgroup.maxQuality), A) * (1 / (pow(log(peakgroup.maxIntensity + 1), B)));
    }

    return false;

}

bool GroupFiltering::filterByMS2(PeakGroup& peakgroup)
{
    if (peakgroup.ms2EventCount == 0)
        return true;

    //TODO: remove MS2 stats calculation from filtering.
    //Already calculated during grouping
    peakgroup.computeFragPattern(_mavenParameters->fragmentTolerance);
    peakgroup.matchFragmentation(_mavenParameters->fragmentTolerance,
                                 _mavenParameters->scoringAlgo);
    FragmentationMatchScore score = peakgroup.fragMatchScore;

    if (score.numMatches < _mavenParameters->minFragMatch)
        return true;
    
    if (score.mergedScore < _mavenParameters->minFragMatchScore)
        return true;
    
    return false;
}

void GroupFiltering::filterBasedOnParent(PeakGroup& parent,
                                         GroupFiltering::ChildFilterType type,
                                         float maxRtDeviation,
                                         float minPercentCorrelation,
                                         MassCutoff* massCutoff)
{
    auto possibleChildren = parent.childIsotopes();
    if (type == ChildFilterType::BarplotIsotope) {
        possibleChildren = parent.childIsotopesBarPlot();
    } else if (type == ChildFilterType::Adduct) {
        possibleChildren = parent.childAdducts();
    }

    vector<PeakGroup*> nonChildren;
    for (auto& child : possibleChildren) {
        float rtDeviationSum = 0.0f;
        int numSamplesShared = 0;
        for (auto sample : _mavenParameters->samples) {
            auto childPeak = child->getPeak(sample);
            auto parentPeak = parent.getPeak(sample);
            if (parentPeak == nullptr || childPeak == nullptr)
                continue;

            ++numSamplesShared;
            auto childApexRt = childPeak->rt;
            auto parentApexRt = parentPeak->rt;
            auto deviation = abs(childApexRt - parentApexRt) * 60.0f;
            rtDeviationSum += deviation;
        }
        if (numSamplesShared == 0)
            continue;

        float avgRtDeviation = rtDeviationSum
                               / static_cast<float>(numSamplesShared);
        if (avgRtDeviation > maxRtDeviation)
            nonChildren.push_back(child.get());
    }

    for (auto& child : possibleChildren) {
        float corrSum = 0.0f;
        int numSamplesShared = 0;
        for (auto sample : _mavenParameters->samples) {
            auto parentPeak = parent.getPeak(sample);
            if (!parentPeak)
                continue;

            auto deviation = maxRtDeviation / 60.0f; // seconds to minutes
            double corr = sample->correlation(parent.meanMz,
                                              child->meanMz,
                                              massCutoff,
                                              parentPeak->rtmin - deviation,
                                              parentPeak->rtmax + deviation,
                                              _mavenParameters->eicType,
                                              _mavenParameters->filterline);
            corrSum += corr;
            ++numSamplesShared;
        }
        if (numSamplesShared == 0)
            continue;

        float avgPercentCorr = corrSum / static_cast<float>(numSamplesShared)
                               * 100.0f;
        if (avgPercentCorr < minPercentCorrelation)
            nonChildren.push_back(child.get());
    }

    for (auto group : nonChildren)
        parent.removeChild(group);
}

bool GroupFiltering::quantileFilters(PeakGroup *group) {
    if (group->maxIntensity < _mavenParameters->minGroupIntensity){
        return true;
    }
    if (group->maxSignalBaselineRatio < _mavenParameters->minSignalBaseLineRatio) {
        return true;
    }
    if (_mavenParameters->clsf->hasModel() && 
        group->maxQuality < _mavenParameters->minQuality) {
            return true;
    }
    if (group->maxIntensity < group->blankMax * _mavenParameters->minSignalBlankRatio){
        return true;
    }
    vector<Peak> peaks = group->getPeaks();
    int peaksAboveMinIntensity = 0;
    int peaksAboveBaselineRatio = 0;
    int peaksAboveBlankRatio = 0;
    int peaksAboveMinQuality = 0;
    for (int i = 0; i < peaks.size(); i++) {
        if (peaks[i].peakIntensity > _mavenParameters->minGroupIntensity) {
            peaksAboveMinIntensity++;
        }
        if (peaks[i].signalBaselineRatio > _mavenParameters->minSignalBaseLineRatio) {
            peaksAboveBaselineRatio++;
        }
        if (peaks[i].peakIntensity > group->blankMax * _mavenParameters->minSignalBlankRatio){
            peaksAboveBlankRatio++;
        }
        if (peaks[i].quality > _mavenParameters->minQuality) {
            peaksAboveMinQuality++;
        }
    }
    int noVisibleSamples = _mavenParameters->getVisibleSamples().size();
    if ((1.0*peaksAboveMinIntensity/noVisibleSamples) * 100 < _mavenParameters->quantileIntensity) {
        return true;
    }
    if ((1.0*peaksAboveMinQuality/noVisibleSamples) * 100 < _mavenParameters->quantileQuality) {
        return true;
    }
    if ((1.0*peaksAboveBaselineRatio/noVisibleSamples)*100 < _mavenParameters->quantileSignalBaselineRatio){
        return true;
    }
    if ((1.0*peaksAboveBlankRatio/noVisibleSamples)*100 < _mavenParameters->quantileSignalBlankRatio){
        return true;
    }
    return false;
}

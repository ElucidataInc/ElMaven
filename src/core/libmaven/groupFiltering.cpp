#include "groupFiltering.h"

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

    unsigned int i = 0;
    while (i < peakgroups.size())
    {
        if (filter(peakgroups[i]))
        {
            peakgroups.erase(peakgroups.begin() + i);
        }
        else
        {
            ++i;
        }
    }

}

bool GroupFiltering::filter(PeakGroup &peakgroup)
{

    Compound* compound = _slice->compound;
    peakgroup.setQuantitationType((PeakGroup::QType)_mavenParameters->peakQuantitation);
    peakgroup.minQuality = _mavenParameters->minQuality;
    peakgroup.minIntensity = _mavenParameters->minGroupIntensity;

    peakgroup.groupStatistics();

    if (_mavenParameters->clsf->hasModel() && peakgroup.goodPeakCount < _mavenParameters->minGoodGroupCount)
        return true;

    if (peakgroup.maxNoNoiseObs < _mavenParameters->minNoNoiseObs)
        return true;
    if (quantileFilters(&peakgroup))
        return true;

    if (compound)
        peakgroup.compound = compound;
    if (!_slice->srmId.empty())
        peakgroup.srmId = _slice->srmId;

    float rtDiff = -1;

    if (compound != NULL && compound->expectedRt > 0)
    {
        rtDiff = abs(compound->expectedRt - (peakgroup.meanRt));
        peakgroup.expectedRtDiff = rtDiff;
    }

    double A = (double)_mavenParameters->qualityWeight / 10;
    double B = (double)_mavenParameters->intensityWeight / 10;
    double C = (double)_mavenParameters->deltaRTWeight / 10;

    if (compound != NULL && compound->expectedRt > 0)
    {
        if (_mavenParameters->deltaRtCheckFlag)
        {
            peakgroup.groupRank = pow(rtDiff, 2 * C) * pow((1.1 - peakgroup.maxQuality), A) * (1 / (pow(log(peakgroup.maxIntensity + 1), B))); //TODO Formula to rank groups
        }
        if (_mavenParameters->matchRtFlag && peakgroup.expectedRtDiff > _mavenParameters->compoundRTWindow)
            return true;
    }

    if (!_mavenParameters->deltaRtCheckFlag || compound == NULL || compound->expectedRt <= 0)
    {
        peakgroup.groupRank = pow((1.1 - peakgroup.maxQuality), A) * (1 / (pow(log(peakgroup.maxIntensity + 1), B)));
    }

    return false;

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
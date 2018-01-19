#include "groupFiltering.h"

GroupFiltering::GroupFiltering(MavenParameters *mavenParameters)
{
    _mavenParameters = mavenParameters;
}

vector<PeakGroup*> GroupFiltering::groupFiltering(vector<PeakGroup> &peakgroups, mzSlice* slice)
{

    Compound* compound = slice->compound;
    vector<PeakGroup*> filteredGroups;
    for (int i = 0; i < peakgroups.size(); i++)
    {
        PeakGroup &group = peakgroups[i];
        group.setQuantitationType((PeakGroup::QType)_mavenParameters->peakQuantitation);
        group.minQuality = _mavenParameters->minQuality;
        group.minIntensity = _mavenParameters->minGroupIntensity;

        group.groupStatistics();

        if (_mavenParameters->clsf->hasModel() && group.goodPeakCount < _mavenParameters->minGoodGroupCount)
            continue;

        if (group.maxNoNoiseObs < _mavenParameters->minNoNoiseObs)
            continue;
        if (quantileFilters(&group))
            continue;

        if (compound)
            group.compound = compound;
        if (!slice->srmId.empty())
            group.srmId = slice->srmId;

        float rtDiff = -1;

        if (compound != NULL && compound->expectedRt > 0)
        {
            rtDiff = abs(compound->expectedRt - (group.meanRt));
            group.expectedRtDiff = rtDiff;
        }

        double A = (double)_mavenParameters->qualityWeight / 10;
        double B = (double)_mavenParameters->intensityWeight / 10;
        double C = (double)_mavenParameters->deltaRTWeight / 10;

        if (compound != NULL && compound->expectedRt > 0)
        {
            if (_mavenParameters->deltaRtCheckFlag)
            {
                group.groupRank = pow(rtDiff, 2 * C) * pow((1.1 - group.maxQuality), A) * (1 / (pow(log(group.maxIntensity + 1), B))); //TODO Formula to rank groups
            }
            if (_mavenParameters->matchRtFlag && group.expectedRtDiff > _mavenParameters->compoundRTWindow)
                continue;
        }

        if (!_mavenParameters->deltaRtCheckFlag || compound == NULL || compound->expectedRt <= 0)
        {
            group.groupRank = pow((1.1 - group.maxQuality), A) * (1 / (pow(log(group.maxIntensity + 1), B)));
        }

        filteredGroups.push_back(&group);
    }

    return filteredGroups;
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
    if ((1.0*peaksAboveMinIntensity/noVisibleSamples) * 100 < _mavenParameters->quantileQuality) {
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
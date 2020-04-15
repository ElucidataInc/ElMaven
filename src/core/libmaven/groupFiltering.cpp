#include <unordered_map>
#include <unordered_set>

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

    unsigned int i = 0;
    while (i < peakgroups.size())
    {
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

        i++;
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

void GroupFiltering::filterAllButSome(vector<PeakGroup>& groups,
                                      FilterType filter,
                                      int limit)
{
    if (filter == FilterType::Rank) {
        std::sort(begin(groups), end(groups), PeakGroup::compRank);
        if (groups.size() > limit)
            groups.erase(begin(groups) + limit, end(groups));
    } else {
        int charge = _mavenParameters->charge;
        unordered_map<float, vector<PeakGroup*>> sameMzClusters;
        for (auto& group : groups) {
            float expectedMz = group.getExpectedMz(charge);
            if (expectedMz == -1)
                continue;

            if (sameMzClusters.count(expectedMz) == 0)
                sameMzClusters[expectedMz] = {};

            // m/z should be exactly the same
            sameMzClusters[expectedMz].push_back(&group);
        }

        unordered_set<PeakGroup*> groupsToDiscard;
        float twoSeconds = 2.0f / 60.0f;
        for (auto& elem : sameMzClusters) {
            auto groupsWithSameMz = elem.second;
            if (groupsWithSameMz.size() == 1
                || groupsWithSameMz.size() <= limit) {
                continue;
            }

            vector<vector<PeakGroup*>> sameRtSubClusters;
            for (auto& thisGroup : groupsWithSameMz) {
                if (thisGroup == nullptr)
                    continue;

                vector<PeakGroup*> subCluster;
                subCluster.push_back(thisGroup);
                for (auto& otherGroup : groupsWithSameMz) {
                    if (otherGroup == nullptr || thisGroup == otherGroup)
                        continue;

                    if (fabs(thisGroup->meanRt - otherGroup->meanRt)
                        <= twoSeconds) {
                        subCluster.push_back(otherGroup);
                        otherGroup = nullptr; // mark as already grouped
                    }
                }
                sameRtSubClusters.push_back(subCluster);
                thisGroup = nullptr; // mark as already grouped
            }

            for (auto& groupsWithSameMzRt : sameRtSubClusters) {
                if (groupsWithSameMzRt.size() == 1
                    || groupsWithSameMzRt.size() <= limit) {
                    continue;
                }

                if (filter == FilterType::MzRt) {
                    multimap<float, PeakGroup*> scoredGroups;
                    for (auto& group : groupsWithSameMzRt) {
                        float mzDiff = fabs(group->meanMz
                                            - group->getExpectedMz(charge));
                        float rtDiff = group->expectedRtDiff();
                        if (rtDiff == -1.0f)
                            rtDiff = 1000.0f; // high penalty for missing RT

                        float errorScore = hypotf(mzDiff, rtDiff);
                        scoredGroups.insert(make_pair(errorScore, group));
                    }

                    // this way we can iterate over the multi-map with an index
                    size_t i = 0;
                    for (auto& elem : scoredGroups) {
                        if (i++ < limit)
                            continue;
                        groupsToDiscard.insert(elem.second);
                    }
                } else if (filter == FilterType::MsMsScore) {
                    vector<PeakGroup*> groupsWithFragmentation;
                    for (auto group : groupsWithSameMzRt) {
                        if (group->ms2EventCount > 0)
                            groupsWithFragmentation.push_back(group);
                    }
                    if (groupsWithFragmentation.size() <= limit)
                        continue;

                    sort(begin(groupsWithFragmentation),
                         end(groupsWithFragmentation),
                         [](PeakGroup* a, PeakGroup* b) {
                             auto aScore = a->fragMatchScore.mergedScore;
                             auto bScore = b->fragMatchScore.mergedScore;
                             return aScore > bScore;
                         });
                    groupsToDiscard.insert(begin(groupsWithFragmentation)
                                               + limit,
                                           end(groupsWithFragmentation));
                }
            }
        }
        groups.erase(remove_if(begin(groups),
                               end(groups),
                               [groupsToDiscard](PeakGroup& group) {
                                   return groupsToDiscard.count(&group) > 0;
                               }),
                     end(groups));
    }
}

#include "adductdetection.h"
#include "Compound.h"
#include "datastructures/adduct.h"
#include "datastructures/mzSlice.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "PeakDetector.h"

mzSlice* AdductDetection::createSliceForCompoundAdduct(Compound *compound,
                                                       Adduct *adduct,
                                                       MavenParameters* mp)
{
    MassCalculator massCalc;
    mzSlice* slice = new mzSlice;
    float neutralMass = compound->neutralMass();
    if (!compound->formula().empty())
        neutralMass = massCalc.computeNeutralMass(compound->formula());
    if (neutralMass <= 0)
        return slice;

    slice->compound = compound;
    slice->adduct = adduct;
    slice->mz = adduct->computeAdductMz(neutralMass);
    slice->calculateMzMinMax(mp->compoundMassCutoffWindow, adduct->getCharge());
    slice->calculateRTMinMax(mp->matchRtFlag, mp->compoundRTWindow);
    return slice;
}

vector<PeakGroup>
AdductDetection::findAdducts(const vector<PeakGroup>& parentIons,
                             const vector<Adduct *>& adductsList,
                             PeakDetector* detector)
{
    MavenParameters *mp = detector->getMavenParameters();
    vector<PeakGroup> adducts = parentIons;

    vector<mzSlice*> slices;
    for (const auto& parentGroup : parentIons) {
        if (!parentGroup.hasCompoundLink())
            continue;

        auto compound = parentGroup.getCompound();
        for (const auto adduct : adductsList) {
            if (parentGroup.getAdduct()) {
                if (*adduct == *(parentGroup.getAdduct()))
                    continue;
            }
            if (SIGN(adduct->getCharge()) != SIGN(mp->ionizationMode))
                continue;

            mzSlice* slice = createSliceForCompoundAdduct(compound, adduct, mp);

            auto foundAt = find_if(begin(slices),
                                   end(slices),
                                   [slice](mzSlice* other) {
                                       return *other == *slice;
                                   });
            if (foundAt != end(slices)) {
                delete slice;
                continue;
            }
            slices.push_back(slice);
        }
    }

    if (!slices.empty()) {
        // this will populate `mp->allgroups` with any new groups from slices
        detector->processSlices(slices, "adducts");
        for (const auto& group : mp->allgroups)
            adducts.push_back(group);
    }

    mzUtils::delete_all(slices);
    return adducts;
}

void AdductDetection::filterAdducts(vector<PeakGroup>& groups,
                                    MavenParameters* mp)
{
    for (auto it = begin(groups); it != end(groups); ) {
        auto& group = *it;
        if (group.getAdduct() != nullptr && !group.getAdduct()->isParent()) {
            // there can be multiple parent groups at different RT values
            vector<PeakGroup*> parentIons;
            for_each(begin(groups),
                     end(groups),
                     [&](PeakGroup& candidate) {
                         if (candidate.getCompound() == group.getCompound()
                             && candidate.getAdduct() != nullptr
                             && candidate.getAdduct()->isParent()) {
                             parentIons.push_back(&candidate);
                         }
                     });

            // if no parent ion's were found, eliminate
            if (parentIons.empty()) {
                it = groups.erase(it);
                continue;
            }

            vector<PeakGroup*> possibleParents;
            for (auto parentIon : parentIons) {
                bool tooFarFromParent = false;
                int numSamplesShared = 0;
                for (auto sample : mp->samples) {
                    auto groupPeak = group.getPeak(sample);
                    auto parentPeak = parentIon->getPeak(sample);
                    if (!parentPeak || !groupPeak)
                        continue;
                    ++numSamplesShared;
                    auto groupScanRt = groupPeak->rt;
                    auto parentScanRt = parentPeak->rt;
                    auto deviation = abs(groupScanRt - parentScanRt) * 60.0f;
                    if (deviation > mp->adductSearchWindow) {
                        tooFarFromParent = true;
                        break;
                    }
                }
                if (!tooFarFromParent && numSamplesShared > 0) {
                    possibleParents.push_back(parentIon);
                }
            }

            // no parent ion is close enough in the RT domain, eliminate
            if (possibleParents.empty()) {
                it = groups.erase(it);
                continue;
            }

            float highestPercentCorrelation = 0.0f;
            PeakGroup* bestMatch = nullptr;
            for (auto candidate : possibleParents) {
                float corrSum = 0.0f;
                int numSamples = 0;
                for (auto sample : mp->samples) {
                    auto parentPeak = candidate->getPeak(sample);
                    if (!parentPeak)
                        continue;

                    auto deviation = mp->adductSearchWindow / 60.0f;
                    double corr = sample->correlation(group.meanMz,
                                                      candidate->meanMz,
                                                      mp->massCutoffMerge,
                                                      parentPeak->rtmin - deviation,
                                                      parentPeak->rtmax + deviation,
                                                      mp->eicType,
                                                      mp->filterline);
                    corrSum += corr;
                    ++numSamples;
                }
                float avgPercentCorr = corrSum
                                       / static_cast<float>(numSamples)
                                       * 100.0f;
                if (avgPercentCorr > mp->adductPercentCorrelation
                    && avgPercentCorr > highestPercentCorrelation) {
                    highestPercentCorrelation = avgPercentCorr;
                    bestMatch = candidate;
                }
            }

            // candidate is not correlated to identified parent, erase
            if (highestPercentCorrelation == 0.0f || bestMatch == nullptr) {
                it = groups.erase(it);
                continue;
            }

            bestMatch->childAdducts.push_back(group);
            group.parentIon = bestMatch;
        }
        ++it;
    }
}

#include <unordered_map>
#include <unordered_set>

#include <omp.h>

#include <boost/bind.hpp>

#include "classifierNeuralNet.h"
#include "datastructures/adduct.h"
#include "datastructures/isotope.h"
#include "datastructures/mzSlice.h"
#include "peakdetector.h"
#include "EIC.h"
#include "mzUtils.h"
#include "Compound.h"
#include "mzSample.h"
#include "constants.h"
#include "classifier.h"
#include "massslicer.h"
#include "peakFiltering.h"
#include "groupFiltering.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "Scan.h"

PeakDetector::PeakDetector() {
    _mavenParameters = NULL;
}

PeakDetector::PeakDetector(MavenParameters* mp) {
    _mavenParameters = mp;
}

void PeakDetector::sendBoostSignal(const string &progressText,
                                   unsigned int completed_slices,
                                   int total_slices)
{
    boostSignal(progressText, completed_slices, total_slices);
}

void PeakDetector::resetProgressBar() {
    _zeroStatus = true;
}

vector<EIC*> PeakDetector::pullEICs(const mzSlice* slice,
                                    const std::vector<mzSample*>& samples,
                                    const MavenParameters* mp,
                                    bool filterUnselectedSamples)
{
    vector<mzSample*> vsamples;
    for (auto sample : samples) {
        if (sample == nullptr)
            continue;
        if (filterUnselectedSamples && !sample->isSelected)
            continue;
        vsamples.push_back(sample);
    }

    vector<EIC*> eics;
#pragma omp parallel
    {
        vector<EIC*> sharedEics;
#pragma omp for nowait
        for (unsigned int i = 0; i < vsamples.size(); i++) {
            // Samples been selected
            mzSample* sample = vsamples[i];
            // getting the slice with which EIC has to be pulled
            Compound* c = slice->compound;

            EIC* e = nullptr;

            if (!slice->srmId.empty()) {
                e = sample->getEIC(slice->srmId, mp->eicType);
            } else if (c && c->precursorMz() > 0 && c->productMz() > 0) {
                e = sample->getEIC(c->precursorMz(),
                                   c->collisionEnergy(),
                                   c->productMz(),
                                   mp->eicType,
                                   mp->filterline,
                                   mp->amuQ1,
                                   mp->amuQ3);
            } else {
                e = sample->getEIC(slice->mzmin,
                                   slice->mzmax,
                                   sample->minRt,
                                   sample->maxRt,
                                   1,
                                   mp->eicType,
                                   mp->filterline);
            }

            if (e) {
                // if eic exists, perform smoothing
                EIC::SmootherType smootherType =
                    (EIC::SmootherType)mp->eic_smoothingAlgorithm;
                e->setSmootherType(smootherType);

                // set appropriate baseline parameters
                if (mp->aslsBaselineMode) {
                    e->setBaselineMode(EIC::BaselineMode::AsLSSmoothing);
                    e->setAsLSSmoothness(mp->aslsSmoothness);
                    e->setAsLSAsymmetry(mp->aslsAsymmetry);
                } else {
                    e->setBaselineMode(EIC::BaselineMode::Threshold);
                    e->setBaselineSmoothingWindow(mp->baseline_smoothingWindow);
                    e->setBaselineDropTopX(mp->baseline_dropTopX);
                }
                e->computeBaseline();
                e->reduceToRtRange(slice->rtmin, slice->rtmax);
                if (slice->isotope.isNone()) {
                    e->setFilterSignalBaselineDiff(
                        mp->minSignalBaselineDifference);
                } else {
                    e->setFilterSignalBaselineDiff(
                        mp->isotopicMinSignalBaselineDifference);
                }
                e->getPeakPositions(mp->eic_smoothingWindow);

                // push eic to shared EIC vector
                sharedEics.push_back(e);
            }
        }
#pragma omp critical
        eics.insert(begin(eics), begin(sharedEics), end(sharedEics));
    }
    return eics;
}

void PeakDetector::processFeatures(const vector<Compound*>& identificationSet)
{
    _mavenParameters->showProgressFlag = true;

    // find average scan time
    _mavenParameters->setAverageScanTime();

    MassSlicer massSlicer(_mavenParameters);
    massSlicer.findFeatureSlices();

    // sort the slices based on their intensities to enurmerate good slices
    sort(massSlicer.slices.begin(),
         massSlicer.slices.end(),
         mzSlice::compIntensity);

    if (massSlicer.slices.empty())
        return;

    processSlices(massSlicer.slices, "feature groups");
    delete_all(massSlicer.slices);

    // identify features with known targets
    identifyFeatures(identificationSet);
}

void PeakDetector::identifyFeatures(const vector<Compound*>& identificationSet)
{
    if (identificationSet.empty())
        return;

    vector<Compound*> compoundsWithRawMzOnly;
    vector<Compound*> compoundsWithNeutralMassOnly;
    for (auto compound : identificationSet) {
        if (compound->formula().empty() && compound->neutralMass() > 0.0f) {
            compoundsWithNeutralMassOnly.push_back(compound);
        } else if (compound->formula().empty() && compound->mz() > 0.0f) {
            compoundsWithRawMzOnly.push_back(compound);
        }
    }

    sendBoostSignal("Preparing libraries for identification…", 0, 0);
    MassSlicer massSlicer(_mavenParameters);
    if (_mavenParameters->pullIsotopesFlag && _mavenParameters->searchAdducts) {
        massSlicer.generateIsotopeSlices(identificationSet);
        massSlicer.generateAdductSlices(identificationSet, true, false);
        massSlicer.generateCompoundSlices(compoundsWithNeutralMassOnly, false);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
    } else if (_mavenParameters->pullIsotopesFlag) {
        massSlicer.generateIsotopeSlices(identificationSet);
        massSlicer.generateCompoundSlices(compoundsWithNeutralMassOnly, false);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
    } else if (_mavenParameters->searchAdducts) {
        massSlicer.generateAdductSlices(identificationSet);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
    } else {
        massSlicer.generateCompoundSlices(identificationSet);
    }

    GroupFiltering groupFilter(_mavenParameters);
    vector<PeakGroup> toBeMerged;
    auto iter = _mavenParameters->allgroups.begin();
    while(iter != _mavenParameters->allgroups.end()) {
        auto& group = *iter;
        bool matchFound = false;
        for (auto slice : massSlicer.slices) {
            if (mzUtils::withinXMassCutoff(slice->mz,
                                           group.meanMz,
                                           _mavenParameters->massCutoffMerge)) {
                PeakGroup groupWithTarget(group);
                groupWithTarget.setCompound(slice->compound);
                groupWithTarget.setAdduct(slice->adduct);
                groupWithTarget.setIsotope(slice->isotope);

                // we should filter the annotated group based on its RT, if the
                // user has restricted RT range
                auto rtDiff = groupWithTarget.expectedRtDiff();
                if (_mavenParameters->identificationMatchRt
                    && rtDiff > _mavenParameters->identificationRtWindow) {
                    continue;
                }

                // since we are creating groups with targets, we should ensure
                // that the parent ion forms of these groups should at least
                // pass MS2 filtering criteria, if enabled
                if (_mavenParameters->matchFragmentationFlag
                    && groupWithTarget.adduct()->isParent()
                    && groupWithTarget.isotope().isParent()
                    && groupWithTarget.ms2EventCount > 0
                    && groupFilter.filterByMS2(groupWithTarget)) {
                    continue;
                }

                matchFound = true;
                toBeMerged.push_back(groupWithTarget);
            }
        }

        if (matchFound) {
            iter = _mavenParameters->allgroups.erase(iter);
        } else {
            ++iter;
        }

       sendBoostSignal("Identifying features using the given compound set…",
                       iter - _mavenParameters->allgroups.begin(),
                       _mavenParameters->allgroups.size());
    }
    delete_all(massSlicer.slices);

    if (!toBeMerged.empty()) {
        _mavenParameters->allgroups.insert(
            _mavenParameters->allgroups.begin(),
            make_move_iterator(toBeMerged.begin()),
            make_move_iterator(toBeMerged.end()));
    }

    performMetaGrouping();

    for (auto& group : _mavenParameters->allgroups) {
        if (group.isGhost())
            continue;

        if (group.isIsotope() || group.isAdduct())
            continue;

        if (group.hasCompoundLink()
            && _mavenParameters->pullIsotopesFlag) {
            if (_mavenParameters->linkIsotopeRtRange)
                linkParentIsotopeRange(group);

            sendBoostSignal("Filtering isotopologues…", 0, 0);
            if (_mavenParameters->filterIsotopesAgainstParent) {
                groupFilter.filterBasedOnParent(
                    group,
                    GroupFiltering::ChildFilterType::Isotope,
                    _mavenParameters->maxIsotopeScanDiff,
                    _mavenParameters->minIsotopicCorrelation,
                    _mavenParameters->massCutoffMerge);
            }
        }
        if (group.hasCompoundLink()
            && _mavenParameters->searchAdducts
            && _mavenParameters->filterAdductsAgainstParent) {
            sendBoostSignal("Filtering adducts…", 0, 0);
            groupFilter.filterBasedOnParent(
                group,
                GroupFiltering::ChildFilterType::Adduct,
                _mavenParameters->adductSearchWindow,
                _mavenParameters->adductPercentCorrelation,
                _mavenParameters->massCutoffMerge);
        }
    }
}

vector<mzSlice*> extractNonParentSlices(vector<mzSlice*>& slices,
                                        function<bool(mzSlice*)> isParent)
{
    vector<mzSlice*> nonParentSlices;
    vector<mzSlice*> remainingSlices;
    for (auto slice : slices) {
        if (!isParent(slice)) {
            nonParentSlices.push_back(slice);
        } else {
            remainingSlices.push_back(slice);
        }
    }
    slices = remainingSlices;
    return nonParentSlices;
}

void PeakDetector::processCompounds(vector<Compound*> compounds,
                                    bool applyGroupFilters,
                                    bool findBarplotIsotopes)
{
    if (compounds.size() == 0)
        return;

    sendBoostSignal("Preparing libraries for search…", 0, 0);
    string setName = "groups";

    bool srmTransitionPresent = false;
    vector<Compound*> compoundsWithRawMzOnly;
    vector<Compound*> compoundsWithNeutralMassOnly;
    for (auto compound : compounds) {
        if (compound->type() == Compound::Type::MRM) {
            setName = "transitions";
            srmTransitionPresent = true;
            break;
        }

        if (compound->formula().empty() && compound->neutralMass() > 0.0f) {
            compoundsWithNeutralMassOnly.push_back(compound);
        } else if (compound->formula().empty() && compound->mz() > 0.0f) {
            compoundsWithRawMzOnly.push_back(compound);
        }
    }

    MassSlicer massSlicer(_mavenParameters);
    if (_mavenParameters->pullIsotopesFlag
        && _mavenParameters->searchAdducts
        && !srmTransitionPresent) {
        massSlicer.generateIsotopeSlices(compounds, findBarplotIsotopes);
        massSlicer.generateAdductSlices(compounds, true, false);
        massSlicer.generateCompoundSlices(compoundsWithNeutralMassOnly, false);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
        setName = "isotopologues and adducts";
    } else if (_mavenParameters->pullIsotopesFlag && !srmTransitionPresent) {
        massSlicer.generateIsotopeSlices(compounds, findBarplotIsotopes);
        massSlicer.generateCompoundSlices(compoundsWithNeutralMassOnly, false);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
        setName = "isotopologues";
    } else if (_mavenParameters->searchAdducts && !srmTransitionPresent) {
        massSlicer.generateAdductSlices(compounds);
        massSlicer.generateCompoundSlices(compoundsWithRawMzOnly, false);
        setName = "adducts";
    } else {
        massSlicer.generateCompoundSlices(compounds);
    }

    // if parent is required, keep aside any slices that represent non-parents
    vector<mzSlice*> nonParentIsotopeSlices;
    if (_mavenParameters->pullIsotopesFlag
        && _mavenParameters->parentIsotopeRequired
        && !srmTransitionPresent) {
        nonParentIsotopeSlices = extractNonParentSlices(
            massSlicer.slices,
            [](mzSlice* slice) {
                return (slice->compound->formula().empty()
                        || slice->isotope.isParent());
            });
        setName = "parent groups";
    }
    vector<mzSlice*> nonParentAdductSlices;
    if (_mavenParameters->searchAdducts
        && _mavenParameters->parentAdductRequired
        && !srmTransitionPresent) {
        nonParentAdductSlices = extractNonParentSlices(
            massSlicer.slices,
            [](mzSlice* slice) {
                return slice->adduct->isParent();
            });
        setName = "parent groups";
    }

    processSlices(massSlicer.slices, setName, applyGroupFilters);
    delete_all(massSlicer.slices);

    unordered_set<Compound*> detectedCompounds;
    for (auto& group : _mavenParameters->allgroups) {
        if (group.hasCompoundLink())
            detectedCompounds.insert(group.getCompound());
    }

    // if parent was required, search for non-parent forms of detected compounds
    if (_mavenParameters->parentIsotopeRequired
        && !nonParentIsotopeSlices.empty()) {
        for (auto slice : nonParentIsotopeSlices) {
            if (detectedCompounds.count(slice->compound) > 0)
                massSlicer.slices.push_back(slice);
        }

        setName = "isotopologues for parent groups";
        processSlices(massSlicer.slices, setName, applyGroupFilters, true);
        delete_all(massSlicer.slices);
    }
    if (_mavenParameters->parentAdductRequired
        && !nonParentAdductSlices.empty()) {
        for (auto slice : nonParentAdductSlices) {
            if (detectedCompounds.count(slice->compound) > 0)
                massSlicer.slices.push_back(slice);
        }

        setName = "adducts for parent groups";
        processSlices(massSlicer.slices, setName, applyGroupFilters, true);
        delete_all(massSlicer.slices);
    }

    performMetaGrouping(applyGroupFilters, findBarplotIsotopes);

    // filter-out ghost parent groups if parent is required
    vector<size_t> ghostIndexes;
    for (size_t i = 0; i < _mavenParameters->allgroups.size(); ++i) {
        auto& group = _mavenParameters->allgroups[i];
        if (_mavenParameters->parentIsotopeRequired
            && group.isGhost()
            && group.childIsotopeCount() > 0) {
            ghostIndexes.push_back(i);
        }
        if (_mavenParameters->parentAdductRequired
            && group.isGhost()
            && group.childAdductsCount() > 0) {
            ghostIndexes.push_back(i);
        }
    }
    mzUtils::eraseIndexes(_mavenParameters->allgroups, ghostIndexes);

    GroupFiltering groupFilter(_mavenParameters);
    for (auto& group : _mavenParameters->allgroups) {
        sendBoostSignal("Filtering isotopologues and adducts…", 0, 0);
        if (_mavenParameters->stop) {
            _mavenParameters->allgroups.clear();
            return;
        }

        if (group.isGhost())
            continue;

        if (group.isIsotope() || group.isAdduct())
            continue;

        if (group.hasCompoundLink()
            && _mavenParameters->pullIsotopesFlag
            && !srmTransitionPresent) {
            if (_mavenParameters->linkIsotopeRtRange)
                linkParentIsotopeRange(group, findBarplotIsotopes);

            auto childType = GroupFiltering::ChildFilterType::Isotope;
            if (findBarplotIsotopes)
                childType = GroupFiltering::ChildFilterType::BarplotIsotope;
            if (_mavenParameters->filterIsotopesAgainstParent) {
                groupFilter.filterBasedOnParent(
                    group,
                    childType,
                    _mavenParameters->maxIsotopeScanDiff,
                    _mavenParameters->minIsotopicCorrelation,
                    _mavenParameters->compoundMassCutoffWindow);
            }
        }
        if (group.hasCompoundLink()
            && _mavenParameters->searchAdducts
            && _mavenParameters->filterAdductsAgainstParent
            && !srmTransitionPresent) {
            groupFilter.filterBasedOnParent(
                group,
                GroupFiltering::ChildFilterType::Adduct,
                _mavenParameters->adductSearchWindow,
                _mavenParameters->adductPercentCorrelation,
                _mavenParameters->compoundMassCutoffWindow);
        }
    }
}

void PeakDetector::processSlices(vector<mzSlice*>& slices,
                                 string setName,
                                 bool applyGroupFilters,
                                 bool appendNewGroups)
{
    if (slices.empty())
        return;

    // shared `MavenParameters` object
    auto mp = make_shared<MavenParameters>(*_mavenParameters);

    // lambda that adds detected groups to mavenparameters
    auto detectGroupsForSlice = [&](vector<EIC*>& eics, mzSlice* slice) {
        vector<PeakGroup> peakgroups =
            EIC::groupPeaks(eics,
                            slice,
                            mp,
                            PeakGroup::IntegrationType::Automated);

        // we do not filter non-parent adducts or non-parent isotopologues
        bool isParentGroup = slice->adduct == nullptr
                             || slice->isotope.isNone()
                             || (slice->adduct->isParent()
                                 && slice->isotope.isParent());
        if (isParentGroup && applyGroupFilters) {
            GroupFiltering groupFiltering(_mavenParameters, slice);
            groupFiltering.filter(peakgroups);
        }
        if (peakgroups.empty())
            return;

        _mavenParameters->allgroups.insert(
            _mavenParameters->allgroups.begin(),
            make_move_iterator(peakgroups.begin()),
            make_move_iterator(peakgroups.end()));
    };

    if (!appendNewGroups)
        _mavenParameters->allgroups.clear();

    sort(slices.begin(), slices.end(), mzSlice::compIntensity);
    for (unsigned int s = 0; s < slices.size(); s++) {
        if (_mavenParameters->stop) {
            _mavenParameters->allgroups.clear();
            break;
        }

        mzSlice* slice = slices[s];
        vector<EIC*> eics = pullEICs(slice,
                                     _mavenParameters->samples,
                                     _mavenParameters);

        if (_mavenParameters->clsf->hasModel())
            _mavenParameters->clsf->scoreEICs(eics);

        float eicMaxIntensity = 0;
        for (auto eic : eics) {
            float max = 0;
            switch (static_cast<PeakGroup::QType>(_mavenParameters->peakQuantitation))
            {
            case PeakGroup::AreaTop:
                max = eic->maxAreaTopIntensity;
                break;
            case PeakGroup::Area:
                max = eic->maxAreaIntensity;
                break;
            case PeakGroup::Height:
                max = eic->maxIntensity;
                break;
            case PeakGroup::AreaNotCorrected:
                max = eic->maxAreaNotCorrectedIntensity;
                break;
            case PeakGroup::AreaTopNotCorrected:
                max = eic->maxAreaTopNotCorrectedIntensity;
                break;
            default:
                max = eic->maxIntensity;
                break;
            }

            if (max > eicMaxIntensity)
                eicMaxIntensity = max;
        }

        // we only filter parent peak-groups on group filtering parameters
        bool isParentGroup = slice->adduct == nullptr
                             || slice->isotope.isNone()
                             || (slice->adduct->isParent()
                                 && slice->isotope.isParent());
        if (isParentGroup
            && applyGroupFilters
            && eicMaxIntensity < _mavenParameters->minGroupIntensity) {
            delete_all(eics);
            continue;
        }

        // TODO: maybe adducts should have their own filters?
        bool isIsotope = !(slice->isotope.isParent()
                           && slice->adduct->isParent());
        PeakFiltering peakFiltering(_mavenParameters, isIsotope);
        peakFiltering.filter(eics);

        detectGroupsForSlice(eics, slice);

        // cleanup
        delete_all(eics);

        if (_mavenParameters->allgroups.size()
            > _mavenParameters->limitGroupCount) {
            cerr << "Group limit exceeded!" << endl;
            break;
        }

        if (_zeroStatus) {
            sendBoostSignal("Status", 0, 1);
            _zeroStatus = false;
        }

        if (_mavenParameters->showProgressFlag) {
            string progressText = "Finding "
                                  + setName;
            sendBoostSignal(progressText,
                            s + 1,
                            std::min((int)slices.size(),
                                     _mavenParameters->limitGroupCount));
        }
    }
}

// filter for top N ranked parent peak-groups per compound
void _keepNBestRanked(unordered_map<Compound*, vector<size_t>>& compoundGroups,
                      vector<PeakGroup>& container,
                      int nBest)
{
    vector<size_t> indexesToDelete;
    for (auto& elem : compoundGroups) {
        vector<size_t>& groupIndexes = elem.second;
        if (groupIndexes.size() <= nBest)
            continue;

        sort(begin(groupIndexes), end(groupIndexes), [&](size_t a, size_t b) {
            PeakGroup& group = container[a];
            PeakGroup& otherGroup = container[b];
            return group.groupRank > otherGroup.groupRank;
        });
        for (size_t i = nBest; i < groupIndexes.size(); ++i)
            indexesToDelete.push_back(groupIndexes[i]);
        groupIndexes.erase(begin(groupIndexes) + nBest, end(groupIndexes));
    }

    // we can delete peak-groups only after filtering, to prevent erase-attempt
    // on invalid indexes
    mzUtils::eraseIndexes(container, indexesToDelete);
}

pair<map<size_t, size_t>, vector<size_t>>
_matchParentsToChildren(vector<size_t>& parentIndexes,
                        vector<size_t>& childIndexes,
                        vector<PeakGroup>& container,
                        function<string(PeakGroup*)> nameFunc)
{
    map<string, vector<size_t>> nameGroupedChildren;
    for (auto index : childIndexes) {
        PeakGroup& child = container[index];
        string subType = nameFunc(&child);
        if (nameGroupedChildren.count(subType) == 0)
            nameGroupedChildren[subType] = {};
        nameGroupedChildren[subType].push_back(index);
    }

    // lambda: checks whether the RT difference between the i-th and the k-th
    // peak-groups is less than that of the j-th and the k-th peak-groups
    auto lessRtDel = [&container](size_t i, size_t j, size_t k) {
        return abs(container[i].meanRt - container[k].meanRt)
               < abs(container[j].meanRt - container[k].meanRt);
    };

    // lambda: sorts object vector based on RT difference b/w object and subject
    auto sortObjects = [&lessRtDel](vector<size_t>& objects, size_t subject) {
        sort(begin(objects),
             end(objects),
             [&lessRtDel, subject](size_t o1, size_t o2) {
                 return lessRtDel(o1, o2, subject);
             });
    };

    // lambda: for a given parent or child group (subject), assign the most
    // preferred child or parent group (object), respectively; in case of a
    // clash, the loser must select its next preference (recursively); one
    // important assumption is that the number of competing subjects is less
    // than (or equal to) the number of available objects
    function<void(size_t,
                  map<size_t, size_t>&,
                  map<size_t, size_t>&,
                  map<size_t, vector<size_t>>&)> findPreferredMatch;
    findPreferredMatch =
        [&lessRtDel, &findPreferredMatch]
        (size_t subject,
         map<size_t, size_t>& subjectsWithObjects,
         map<size_t, size_t>& objectsWithSubjects,
         map<size_t, vector<size_t>>& priorityLists) -> void {
            auto priorityList = priorityLists[subject];
            for (size_t object : priorityList) {
                if (objectsWithSubjects.count(object)) {
                    auto competingSubject = objectsWithSubjects[object];
                    if (lessRtDel(subject, competingSubject, object)) {
                        subjectsWithObjects[subject] = object;
                        objectsWithSubjects[object] = subject;

                        auto iter = subjectsWithObjects.find(competingSubject);
                        if(iter != end(subjectsWithObjects))
                            subjectsWithObjects.erase(iter);
                        findPreferredMatch(competingSubject,
                                           subjectsWithObjects,
                                           objectsWithSubjects,
                                           priorityLists);
                        break;
                    }
                } else {
                    subjectsWithObjects[subject] = object;
                    objectsWithSubjects[object] = subject;
                    break;
                }
            }
        };

    vector<size_t> orphans;
    map<size_t, size_t> nonOrphans;
    for (auto& elem : nameGroupedChildren) {
        auto& childIndexes = elem.second;

        map<size_t, size_t> childrenWithParents;
        map<size_t, size_t> parentsWithChildren;
        map<size_t, vector<size_t>> priorityLists;
        if (childIndexes.size() <= parentIndexes.size()) {
            for (size_t childIndex : childIndexes) {
                vector<size_t> copyOfParentIndexes = parentIndexes;
                sortObjects(copyOfParentIndexes, childIndex);
                priorityLists[childIndex] = copyOfParentIndexes;
            }

            for (size_t childIndex : childIndexes) {
                findPreferredMatch(childIndex,
                                   childrenWithParents,
                                   parentsWithChildren,
                                   priorityLists);
            }
        } else {
            for (size_t parentIndex : parentIndexes) {
                vector<size_t> copyOfChildIndexes = childIndexes;
                sortObjects(copyOfChildIndexes, parentIndex);
                priorityLists[parentIndex] = copyOfChildIndexes;
            }

            for (size_t parentIndex : parentIndexes) {
                findPreferredMatch(parentIndex,
                                   parentsWithChildren,
                                   childrenWithParents,
                                   priorityLists);
            }

            // keep track of children that could not find parents
            for (auto childIndex : childIndexes) {
                if (childrenWithParents.count(childIndex) == 0)
                    orphans.push_back(childIndex);
            }
        }
        nonOrphans.insert(begin(childrenWithParents), end(childrenWithParents));
    }
    return make_pair(nonOrphans, orphans);
}

// given a compound and its child indexes, clubs them with their most likely
// parent-group if possible, otherwise adds them to a ghost parent
unordered_map<size_t, vector<size_t>>
_makeMeta(Compound* compound,
          vector<size_t>& childIndexes,
          unordered_map<Compound*, vector<size_t>>& parentCompounds,
          function<string(PeakGroup*)> nameFunc,
          MavenParameters* mp)
{
    auto& container = mp->allgroups;
    vector<size_t> orphans;
    map<size_t, size_t> nonOrphans;
    if (parentCompounds.count(compound) > 0) {
        auto& parentIndexes = parentCompounds[compound];
        auto result = _matchParentsToChildren(parentIndexes,
                                              childIndexes,
                                              container,
                                              nameFunc);
        nonOrphans = result.first;
        orphans = result.second;
    } else {
        for (auto index : childIndexes)
            orphans.push_back(index);
    }

    unordered_map<size_t, vector<size_t>> metaGroups;
    for (auto& elem : nonOrphans) {
        size_t parentIndex = elem.second;
        size_t childIndex = elem.first;
        if (metaGroups.count(parentIndex) == 0)
            metaGroups[parentIndex] = {};
        metaGroups[parentIndex].push_back(childIndex);
    }

    if (!orphans.empty()) {
        // for orphans, create a ghost, that will act as an empty parent
        PeakGroup parentGroup(make_shared<MavenParameters>(*mp),
                              PeakGroup::IntegrationType::Ghost);
        container.push_back(parentGroup);

        // set an appropriate slice for ghost parent
        mzSlice slice;
        slice.compound = compound;
        slice.calculateMzMinMax(mp->compoundMassCutoffWindow,
                                mp->getCharge(compound));
        slice.calculateRTMinMax(false, 0.0f);
        container.back().setSlice(slice);

        size_t totalSize = container.size();
        container.back().setGroupId(totalSize);
        metaGroups[totalSize - 1] = {};
        for (auto child : orphans)
            metaGroups[totalSize - 1].push_back(child);
    }

    return metaGroups;
}

void PeakDetector::performMetaGrouping(bool applyGroupFilters,
                                       bool barplotIsotopes)
{
    if (_mavenParameters->allgroups.empty())
        return;

    sendBoostSignal("Performing meta-grouping…", 0, 0);

    // lambda: club parent-group indexes based on their compounds
    auto compoundParentGroups = [this] {
        unordered_map<Compound*, vector<size_t>> parentCompounds;
        for (size_t i = 0; i < _mavenParameters->allgroups.size(); ++i) {
            if (_mavenParameters->stop)
                return parentCompounds;

            PeakGroup& group = _mavenParameters->allgroups[i];
            Compound* compound = group.getCompound();
            if (compound == nullptr)
                continue;

            if (group.isotope().isParent() && group.adduct()->isParent()) {
                if (parentCompounds.count(compound) == 0)
                    parentCompounds[compound] = {};
                parentCompounds[compound].push_back(i);
            }
        }
        return parentCompounds;
    };

    unordered_map<Compound*, vector<size_t>> parentCompounds =
        compoundParentGroups();

    if (applyGroupFilters) {
        // filter for the N-best groups per compound
        _keepNBestRanked(parentCompounds,
                         _mavenParameters->allgroups,
                         _mavenParameters->eicMaxGroups);

        if (_mavenParameters->stop)
            return;

        // after filtering, the container changed and the original indexes will
        // have to be updated
        parentCompounds = compoundParentGroups();
    }

    // put isotopologues and adducts into separate buckets
    unordered_map<Compound*, vector<size_t>> nonParentIsotopologues;
    unordered_map<Compound*, vector<size_t>> nonParentAdducts;
    for (size_t i = 0; i < _mavenParameters->allgroups.size(); ++i) {
        if (_mavenParameters->stop)
            return;

        PeakGroup& group = _mavenParameters->allgroups[i];
        Compound* compound = group.getCompound();
        if (compound == nullptr)
            continue;

        if (group.isIsotope()) {
            if (nonParentIsotopologues.count(compound) == 0)
                nonParentIsotopologues[compound] = {};
            nonParentIsotopologues[compound].push_back(i);
        } else if (group.isAdduct()) {
            if (nonParentAdducts.count(compound) == 0)
                nonParentAdducts[compound] = {};
            nonParentAdducts[compound].push_back(i);
        }
    }

    // enumerate group IDs for all remaining peak-groups
    int groupId = 1;
    for (auto& group : _mavenParameters->allgroups)
        group.setGroupId(groupId++);

    if (nonParentIsotopologues.empty() && nonParentAdducts.empty())
        return;

    unordered_map<Compound*, unordered_map<size_t, vector<size_t>>> metaGroups;

    // find isotope meta-groups
    for (auto& elem : nonParentIsotopologues) {
        if (_mavenParameters->stop)
            return;

        Compound* compound = elem.first;
        auto& isotopeIndexes = elem.second;
        auto metaIsotopeGroups = _makeMeta(compound,
                                           isotopeIndexes,
                                           parentCompounds,
                                           [](PeakGroup* group) {
                                               return group->isotope().name;
                                           },
                                           _mavenParameters);
        metaGroups[compound] = metaIsotopeGroups;
    }

    // find adduct meta-groups
    for (auto& elem : nonParentAdducts) {
        if (_mavenParameters->stop)
            return;

        Compound* compound = elem.first;
        auto& adductIndexes = elem.second;
        auto metaAdductGroups = _makeMeta(compound,
                                          adductIndexes,
                                          parentCompounds,
                                          [](PeakGroup* group) {
                                              return group->adduct()->getName();
                                          },
                                          _mavenParameters);
        if (metaGroups.count(compound) > 0) {
            auto& existingMetaGroups = metaGroups[compound];
            for (auto& elem : metaAdductGroups) {
                size_t parentIndex = elem.first;
                vector<size_t>& childAdductIndexes = elem.second;
                if (existingMetaGroups.count(parentIndex) > 0) {
                    vector<size_t>& existingChildIndexes =
                        existingMetaGroups[parentIndex];
                    existingChildIndexes.insert(end(existingChildIndexes),
                                                begin(childAdductIndexes),
                                                end(childAdductIndexes));
                } else {
                    existingMetaGroups[parentIndex] = childAdductIndexes;
                }
            }
        } else {
            metaGroups[compound] = metaAdductGroups;
        }
    }

    // perform final meta-grouping and queue children to be erased
    vector<size_t> indexesToErase;
    for (auto& elem : metaGroups) {
        if (_mavenParameters->stop) {
            _mavenParameters->allgroups.clear();
            return;
        }

        auto& compoundMetaGroups = elem.second;
        for (auto& metaElement : compoundMetaGroups) {
            PeakGroup& parent = _mavenParameters->allgroups[metaElement.first];
            auto& childIndexes = metaElement.second;
            for (auto childIndex : childIndexes) {
                PeakGroup& child = _mavenParameters->allgroups[childIndex];
                if (child.isIsotope() && barplotIsotopes) {
                    parent.addIsotopeChildBarPlot(child);
                } else if (child.isIsotope()) {
                    parent.addIsotopeChild(child);
                }
                if (child.isAdduct())
                    parent.addAdductChild(child);
                indexesToErase.push_back(childIndex);
            }
        }
    }

    // the following index-overwrite removal does not preserve order
    mzUtils::eraseIndexes(_mavenParameters->allgroups, indexesToErase);
}

void PeakDetector::linkParentIsotopeRange(PeakGroup& parentGroup,
                                          bool findBarplotIsotopes)
{
    PeakFiltering peakFilter(_mavenParameters, true);
    auto isotopes = findBarplotIsotopes ? parentGroup.childIsotopesBarPlot()
                                        : parentGroup.childIsotopes();
    vector<PeakGroup*> emptyChildren;
    for (auto& child : isotopes) {
        auto eics = pullEICs(&child->getSlice(),
                             _mavenParameters->samples,
                             _mavenParameters);
        for (auto eic : eics) {
            if (eic == nullptr)
                continue;

            mzSample* sample = eic->sample;
            auto parentPeak = parentGroup.getPeak(sample);
            auto childPeak = child->getPeak(sample);
            if (parentPeak == nullptr || childPeak == nullptr)
                continue;

            eic->adjustPeakBounds(*childPeak,
                                  parentPeak->rtmin,
                                  parentPeak->rtmax);
            eic->getPeakDetails(*childPeak);
            if (mzUtils::almostEqual(childPeak->peakArea, 0.0f))
                child->deletePeak(sample);

            if (_mavenParameters->clsf->hasModel())
                _mavenParameters->clsf->scorePeak(*childPeak);
            if (peakFilter.filter(*childPeak))
                child->deletePeak(sample);
        }

        delete_all(eics);
        if (child->peakCount() == 0) {
            emptyChildren.push_back(child.get());
            continue;
        }

        child->updateQuality();
        child->groupStatistics();
    }

    for (auto group : emptyChildren)
        parentGroup.removeChild(group);
}

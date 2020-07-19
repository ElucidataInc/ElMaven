#include <omp.h>

#include <boost/bind.hpp>

#include "adductdetection.h"
#include "datastructures/adduct.h"
#include "classifierNeuralNet.h"
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
#include "isotopeDetection.h"
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
                e->setFilterSignalBaselineDiff(mp->minSignalBaselineDifference);
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

    MassSlicer massSlicer;
    massSlicer.setSamples(_mavenParameters->samples);
    massSlicer.setMavenParameters(_mavenParameters);

    massSlicer.setMaxIntensity(_mavenParameters->maxIntensity);
    massSlicer.setMinIntensity(_mavenParameters->minIntensity);
    massSlicer.setMaxRt	(_mavenParameters->maxRt);
    massSlicer.setMinRt (_mavenParameters->minRt);
    massSlicer.setMaxMz (_mavenParameters->maxMz);
    massSlicer.setMinMz	(_mavenParameters->minMz);
    massSlicer.findFeatureSlices(_mavenParameters->massCutoffMerge,
                                 _mavenParameters->rtStepSize);

    // sort the slices based on their intensities to enurmerate good slices.
    sort(massSlicer.slices.begin(),
         massSlicer.slices.end(),
         mzSlice::compIntensity);

    if (massSlicer.slices.empty())
        return;

    sendBoostSignal("Peak Detection", 0, 1);

    // process good slices
    processSlices(massSlicer.slices, "groups");

    // identify features with known targets
    identifyFeatures(identificationSet);

    // cleanup
    delete_all(massSlicer.slices);
}

void PeakDetector::identifyFeatures(const vector<Compound*>& identificationSet)
{
    if (identificationSet.empty())
        return;

    GroupFiltering groupFiltering(_mavenParameters);
    vector<PeakGroup> toBeMerged;
    auto iter = _mavenParameters->allgroups.begin();
    while(iter != _mavenParameters->allgroups.end()) {
        auto& group = *iter;
        bool matchFound = false;
        for (auto compound : identificationSet) {
            float mz = 0.0f;
            if (compound->formula().length() || compound->neutralMass() != 0.0f) {
                int charge = _mavenParameters->getCharge(compound);
                mz = compound->adjustedMass(charge);
            } else {
                mz = compound->mz();
            }
            if (mzUtils::withinXMassCutoff(mz,
                                           group.meanMz,
                                           _mavenParameters->massCutoffMerge)) {
                PeakGroup groupWithTarget(group);
                groupWithTarget.setCompound(compound);

                // we should filter the annotated group based on its RT, if the
                // user has restricted RT range
                auto rtDiff = groupWithTarget.expectedRtDiff();
                if (_mavenParameters->identificationMatchRt
                    && rtDiff > _mavenParameters->identificationRtWindow) {
                    continue;
                }

                // since we are creating targeted groups, we should ensure they
                // pass MS2 filtering criteria, if enabled
                if (_mavenParameters->matchFragmentationFlag
                    && groupWithTarget.ms2EventCount > 0
                    && groupFiltering.filterByMS2(groupWithTarget)) {
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

    if (!toBeMerged.empty()) {
        _mavenParameters->allgroups.insert(
            _mavenParameters->allgroups.begin(),
            make_move_iterator(toBeMerged.begin()),
            make_move_iterator(toBeMerged.end()));
    }
}

void PeakDetector::pullAllIsotopes() {
    for (unsigned int j = 0; j < _mavenParameters->allgroups.size(); j++) {
        if(_mavenParameters->stop) break;
        PeakGroup& group = _mavenParameters->allgroups[j];

        if (_mavenParameters->pullIsotopesFlag && !group.isIsotope())
        {
            bool C13Flag = _mavenParameters->C13Labeled_BPE;
            bool N15Flag = _mavenParameters->N15Labeled_BPE;
            bool S34Flag = _mavenParameters->S34Labeled_BPE;
            bool D2Flag = _mavenParameters->D2Labeled_BPE;

            IsotopeDetection::IsotopeDetectionType isoType;
            isoType = IsotopeDetection::PeakDetection;

            IsotopeDetection isotopeDetection(
                _mavenParameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
            isotopeDetection.pullIsotopes(&group);
        }


        if (_mavenParameters->showProgressFlag &&
            _mavenParameters->pullIsotopesFlag && j % 10 == 0) {
            sendBoostSignal("Calculating Isotopes", j, _mavenParameters->allgroups.size());
        }
    }
}

void PeakDetector::processCompounds(vector<Compound*> set, std::string setName)
{
    vector<mzSlice*> slices;

    if (set.size() == 0)
        return;

    Adduct* adduct = nullptr;
    for (auto parentAdduct : _mavenParameters->getDefaultAdductList()) {
        if (SIGN(parentAdduct->getCharge())
            == SIGN(_mavenParameters->ionizationMode)) {
            adduct = parentAdduct;
        }
    }

    if (adduct == nullptr)
        return;

    for(auto compound : set) {
        if (_mavenParameters->stop) {
            delete_all(slices);
            break;
        }

        sendBoostSignal("Preparing libraries for search…", 0, 0);

        if (compound == nullptr)
            continue;

        if (compound->type() == Compound::Type::MRM) {
            mzSlice* slice = new mzSlice();
            slice->compound = compound;
            slice->setSRMId();
            slice->calculateRTMinMax(_mavenParameters->matchRtFlag,
                                     _mavenParameters->compoundRTWindow);
            slices.push_back(slice);
        } else {
            mzSlice* slice =
                AdductDetection::createSliceForCompoundAdduct(compound,
                                                              adduct,
                                                              _mavenParameters);
            slices.push_back(slice);
        }
    }

    processSlices(slices, setName);
    delete_all(slices);
}

void PeakDetector::processSlices(vector<mzSlice*> &slices, string setName)
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
        GroupFiltering groupFiltering(_mavenParameters, slice);
        groupFiltering.filter(peakgroups);

        // sort groups according to their rank
        std::sort(peakgroups.begin(), peakgroups.end(),
                  PeakGroup::compRank);

        for (unsigned int j = 0; j < peakgroups.size(); j++) {
            // check for duplicates	and append group
            if (j >= _mavenParameters->eicMaxGroups)
                break;
            _mavenParameters->allgroups.push_back(peakgroups[j]);
        }
    };

    _mavenParameters->allgroups.clear();
    sort(slices.begin(), slices.end(), mzSlice::compIntensity);
    for (unsigned int s = 0; s < slices.size(); s++) {
        if (_mavenParameters->stop)
            break;

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

        if (eicMaxIntensity < _mavenParameters->minGroupIntensity) {
            delete_all(eics);
            continue;
        }

        bool isIsotope = false;
        PeakFiltering peakFiltering(_mavenParameters, isIsotope);
        peakFiltering.filter(eics);

        detectGroupsForSlice(eics, slice);

        // cleanup
        delete_all(eics);

        if (_mavenParameters->allgroups.size() > _mavenParameters->limitGroupCount) {
            cerr << "Group limit exceeded!" << endl;
            break;
        }

        if (_zeroStatus) {
            sendBoostSignal("Status", 0, 1);
            _zeroStatus = false;
        }

        if (_mavenParameters->showProgressFlag && s % 10 == 0) {
            string progressText = "Found "
                                  + to_string(_mavenParameters->allgroups.size())
                                  + " "
                                  + setName;
            sendBoostSignal(progressText,
                            s + 1,
                            std::min((int)slices.size(),
                                     _mavenParameters->limitGroupCount));
        }
    }
}

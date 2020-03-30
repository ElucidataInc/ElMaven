#include "adductdetection.h"
#include "datastructures/adduct.h"
#include "classifierNeuralNet.h"
#include "datastructures/mzSlice.h"
#include "obiwarp.h"
#include "PeakDetector.h"
#include "EIC.h"
#include "mzUtils.h"
#include "Compound.h"
#include "mzSample.h"
#include "mzAligner.h"
#include "constants.h"
#include "classifier.h"
#include "mzMassSlicer.h"
#include "peakFiltering.h"
#include "groupFiltering.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "isotopeDetection.h"
#include "Scan.h"

PeakDetector::PeakDetector() {
    mavenParameters = NULL;
}

PeakDetector::PeakDetector(MavenParameters* mp) {
	mavenParameters = mp;
}

void PeakDetector::resetProgressBar() {
	zeroStatus = true;
}

vector<EIC*> PeakDetector::pullEICs(mzSlice* slice,
                                    std::vector<mzSample*>& samples,
                                    MavenParameters* mp)
{
    vector<EIC*> eics;
    vector<mzSample*> vsamples;
#pragma omp parallel default(shared)
    {
#pragma omp for
        for (unsigned int i = 0; i < samples.size(); i++) {
            if (samples[i] == NULL)
                continue;
            if (samples[i]->isSelected == false)
                continue;
#pragma omp critical
            vsamples.push_back(samples[i]);
        }

        // single threaded version - getting EICs of selected samples.
        // #pragma omp parallel for ordered
#pragma omp for
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
                                   slice->rtmin,
                                   slice->rtmax,
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
                e->setFilterSignalBaselineDiff(mp->minSignalBaselineDifference);
                e->getPeakPositions(mp->eic_smoothingWindow);
                // smoohing over

#pragma omp critical
                // push eic to all eics vector
                eics.push_back(e);
            }
        }
    }
    return eics;
}

void PeakDetector::processSlices() {
        processSlices(mavenParameters->_slices, "sliceset");
}

void PeakDetector::processSlice(mzSlice& slice) {
        vector<mzSlice*> slices;
        slices.push_back(&slice); //add slice to slices vector
        processSlices(slices, "sliceset");
}

void PeakDetector::pullAllIsotopes() {
    for (unsigned int j = 0; j < mavenParameters->allgroups.size(); j++) {
        if(mavenParameters->stop) break;
        PeakGroup& group = mavenParameters->allgroups[j];
        Compound* compound = group.getCompound();

        if (mavenParameters->pullIsotopesFlag && !group.isIsotope())
        {
            bool C13Flag = mavenParameters->C13Labeled_BPE;
            bool N15Flag = mavenParameters->N15Labeled_BPE;
            bool S34Flag = mavenParameters->S34Labeled_BPE;
            bool D2Flag = mavenParameters->D2Labeled_BPE;

            IsotopeDetection::IsotopeDetectionType isoType;
            isoType = IsotopeDetection::PeakDetection;

            IsotopeDetection isotopeDetection(
                mavenParameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
            isotopeDetection.pullIsotopes(&group);
        }


        if (mavenParameters->showProgressFlag &&
            mavenParameters->pullIsotopesFlag && j % 10 == 0) {
			sendBoostSignal("Calculating Isotopes", j, mavenParameters->allgroups.size());
        }
    }
}

void PeakDetector::processMassSlices(const vector<Compound*>& identificationSet) {
    // init
    // TODO: what is this doing?
    // TODO: cant this be in background_peaks_update parameter setting function
    mavenParameters->showProgressFlag = true;
    QTime timer;
    timer.start();

    // TODO: cant this be in background_peaks_update parameter setting function
    mavenParameters->setAverageScanTime();  // find avgScanTime

    MassSlices massSlices;
    massSlices.setSamples(mavenParameters->samples);
    massSlices.setMavenParameters(mavenParameters);

    massSlices.setMaxIntensity(mavenParameters->maxIntensity);
    massSlices.setMinIntensity(mavenParameters->minIntensity);
    massSlices.setMaxRt	(mavenParameters->maxRt);
    massSlices.setMinRt (mavenParameters->minRt);
    massSlices.setMaxMz (mavenParameters->maxMz);
    massSlices.setMinMz	(mavenParameters->minMz);
    massSlices.algorithmB(mavenParameters->massCutoffMerge, mavenParameters->rtStepSize);  // perform algorithmB for samples

    if (massSlices.slices.size() == 0)
        massSlices.algorithmA();  // if no slices present, perform algorithmA
                                  // TODO WHY?!

    // sort the massslices based on their intensities to enurmerate good slices.
    sort(massSlices.slices.begin(), massSlices.slices.end(),
         mzSlice::compIntensity);

    if (massSlices.slices.size() == 0) {
        //	Q_EMIT (updateProgressBar("Quiting! No good mass slices found",
        //1, 1)); TODO: Fix Q_EMIT.
        return;
    }

    sendBoostSignal("Peak Detection",0,1);

    // process goodslices
    processSlices(massSlices.slices, "groups");

    // identify features with known targets
    identifyFeatures(identificationSet);

    // cleanup
    delete_all(massSlices.slices);

    qDebug() << "processMassSlices() Done. ElepsTime=%1 msec"
             << timer.elapsed();
}

/**
 * This function finds the slices for the given compound database
 * Characteristics of a slices are minimum m/z, maximum m/z, minimum RT,
 * maximum RT and its SRM ID TODO: i dont know what is the use of SRM ID.
 */
vector<mzSlice*> PeakDetector::processCompounds(vector<Compound*> set,
                                                std::string setName)
{
    vector<mzSlice*> slices;

    if (set.size() == 0)
        return slices;

    Adduct* adduct = nullptr;
    for (auto parentAdduct : mavenParameters->getDefaultAdductList()) {
        if (SIGN(parentAdduct->getCharge())
            == SIGN(mavenParameters->ionizationMode)) {
            adduct = parentAdduct;
        }
    }

    if (adduct == nullptr)
        return slices;

    for(auto compound : set) {
        if (mavenParameters->stop) {
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
            slice->calculateRTMinMax(mavenParameters->matchRtFlag,
                                     mavenParameters->compoundRTWindow);
            slices.push_back(slice);
        } else {
            mzSlice* slice =
                AdductDetection::createSliceForCompoundAdduct(compound,
                                                              adduct,
                                                              mavenParameters);
            slices.push_back(slice);
        }
    }

    return slices;
}

void PeakDetector::alignSamples(const int& method) {

    // only called from CLI
    if(mavenParameters->samples.size() > 1 ) {

        switch(method) {

        case 1: {
            cerr << "Starting OBI-WARP alignment" << std::endl;
            /*TODO: move the hard coded values in  default_settings.xml and instead of using obi params
            make use mavenParameters to access all the values */
            ObiParams params("cor", false, 2.0, 1.0, 0.20, 3.40, 0.0, 20.0, false, 0.60);
            Aligner mzAligner;
            mzAligner.alignWithObiWarp(mavenParameters->samples, &params, mavenParameters);
        }

        break;


        case 2: {
            mavenParameters->writeCSVFlag = false;
            processMassSlices();
            cerr << "starting POLY-FIT alignment " << std::endl;
            cerr << "Aligner=" << mavenParameters->allgroups.size() << endl;
            vector<PeakGroup*> agroups(mavenParameters->allgroups.size());
            for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++)
                agroups[i] = &mavenParameters->allgroups[i];
            //init aligner
            Aligner aligner;
            aligner.doAlignment(agroups);
            mavenParameters->writeCSVFlag = true;
        }

        break;

        default: break;


        }
    }
}

void PeakDetector::processSlices(vector<mzSlice*> &slices, string setName)
{
    if (slices.empty())
        return;

    // lambda that adds detected groups to mavenparameters
    auto detectGroupsForSlice = [&](vector<EIC*>& eics, mzSlice* slice) {
        vector<PeakGroup> peakgroups =
        EIC::groupPeaks(eics,
                        slice,
                        mavenParameters->eic_smoothingWindow,
                        mavenParameters->grouping_maxRtWindow,
                        mavenParameters->minQuality,
                        mavenParameters->distXWeight,
                        mavenParameters->distYWeight,
                        mavenParameters->overlapWeight,
                        mavenParameters->useOverlap,
                        mavenParameters->minSignalBaselineDifference,
                        mavenParameters->fragmentTolerance,
                        mavenParameters->scoringAlgo);

        GroupFiltering groupFiltering(mavenParameters, slice);
        groupFiltering.filter(peakgroups);

        // sort groups according to their rank
        std::sort(peakgroups.begin(), peakgroups.end(),
                  PeakGroup::compRank);

        for (unsigned int j = 0; j < peakgroups.size(); j++) {
            // check for duplicates	and append group
            if (j >= mavenParameters->eicMaxGroups)
                break;
            mavenParameters->allgroups.push_back(peakgroups[j]);
        }
    };

    mavenParameters->allgroups.clear();
    sort(slices.begin(), slices.end(), mzSlice::compIntensity);
    for (unsigned int s = 0; s < slices.size(); s++) {
        if (mavenParameters->stop)
            break;

        mzSlice* slice = slices[s];
        Compound *compound = slice->compound;

        vector<EIC*> eics = pullEICs(slice,
                                     mavenParameters->samples,
                                     mavenParameters);

        if (mavenParameters->clsf->hasModel())
            mavenParameters->clsf->scoreEICs(eics);

        float eicMaxIntensity = 0;
        for (auto eic : eics) {
            float max = 0;
            switch (static_cast<PeakGroup::QType>(mavenParameters->peakQuantitation))
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

        if (eicMaxIntensity < mavenParameters->minGroupIntensity) {
            delete_all(eics);
            continue;
        }

        bool isIsotope = false;
        PeakFiltering peakFiltering(mavenParameters, isIsotope);
        peakFiltering.filter(eics);

        detectGroupsForSlice(eics, slice);

        // cleanup
        delete_all(eics);

        if (mavenParameters->allgroups.size() > mavenParameters->limitGroupCount) {
            cerr << "Group limit exceeded!" << endl;
            break;
        }

        if (zeroStatus) {
            sendBoostSignal("Status", 0, 1);
            zeroStatus = false;
        }

        if (mavenParameters->showProgressFlag && s % 10 == 0) {
            string progressText = "Found "
                                  + to_string(mavenParameters->allgroups.size())
                                  + " "
                                  + setName;
            sendBoostSignal(progressText,
                            s + 1,
                            std::min((int)slices.size(),
                                     mavenParameters->limitGroupCount));
        }
    }
}

void PeakDetector::identifyFeatures(const vector<Compound*>& identificationSet)
{
    if (identificationSet.empty())
        return;

    GroupFiltering groupFiltering(mavenParameters);
    vector<PeakGroup> toBeMerged;
    auto iter = mavenParameters->allgroups.begin();
    while(iter != mavenParameters->allgroups.end()) {
        auto& group = *iter;
        bool matchFound = false;
        for (auto compound : identificationSet) {
            float mz = 0.0f;
            if (compound->formula().length() || compound->neutralMass() != 0.0f) {
                int charge = mavenParameters->getCharge(compound);
                mz = compound->adjustedMass(charge);
            } else {
                mz = compound->mz();
            }
            if (mzUtils::withinXMassCutoff(mz,
                                           group.meanMz,
                                           mavenParameters->massCutoffMerge)) {
                PeakGroup groupWithTarget(group);
                groupWithTarget.setCompound(compound);

                // we should filter the annotated group based on its RT, if the
                // user has restricted RT range
                auto rtDiff = groupWithTarget.expectedRtDiff();
                if (mavenParameters->identificationMatchRt
                    && rtDiff > mavenParameters->identificationRtWindow) {
                    continue;
                }

                // since we are creating targeted groups, we should ensure they
                // pass MS2 filtering criteria, if enabled
                if (mavenParameters->matchFragmentationFlag
                    && groupFiltering.filterByMS2(groupWithTarget)) {
                    continue;
                }

                matchFound = true;
                toBeMerged.push_back(groupWithTarget);
            }
        }

        if (matchFound) {
            iter = mavenParameters->allgroups.erase(iter);
        } else {
            ++iter;
        }

       sendBoostSignal("Identifying features using the given compound set…",
                       iter - mavenParameters->allgroups.begin(),
                       mavenParameters->allgroups.size());
    }

    if (!toBeMerged.empty()) {
        mavenParameters->allgroups.insert(mavenParameters->allgroups.begin(),
                                          make_move_iterator(toBeMerged.begin()),
                                          make_move_iterator(toBeMerged.end()));
    }
}

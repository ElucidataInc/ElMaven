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
            } else if (c && c->precursorMz > 0 && c->productMz > 0) {
                e = sample->getEIC(c->precursorMz,
                                   c->collisionEnergy,
                                   c->productMz,
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

        if (compound) {
            if (!compound->hasGroup() ||
                group.groupRank < compound->getPeakGroup()->groupRank)
                compound->setPeakGroup(group);
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
    processSlices(massSlices.slices, "allslices");

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
                                                vector<Adduct*> adductList,
                                                string setName)
{
    vector<mzSlice*>slices;
    if (set.size() == 0 )
        return slices;

    MassCalculator massCalc;
    multimap<string, Compound*> stringToCompoundMap = {};
    std::set<string> formulae;
    vector<Compound*> mrmSet;
    for(auto compound : set) {
        if (mavenParameters->stop) {
            delete_all(slices);
            break;
        }

        sendBoostSignal("Preparing libraries for search…", 0, 0);

        if (!compound){
            continue;
        }

        if (compound->type() == Compound::Type::MRM) {
            mrmSet.push_back(compound);
            continue;
        }

        if (compound->formula.empty()) {
            cerr << "Skipping compound \""
                 << compound->name
                 << "\", because it is missing chemical formula."
                 << endl;
            continue;
        }

        string key = compound->formula;
        formulae.insert(key);
        pair<string, Compound*> keyValuePair = make_pair(key, compound);
        stringToCompoundMap.insert(keyValuePair);
    }

    // create slices for MRM compounds separately
    for (auto compound : mrmSet) {
        if (mavenParameters->stop) {
            delete_all(slices);
            break;
        }

        if (compound == nullptr)
            continue;

        mzSlice* slice = new mzSlice();
        slice->compound = compound;
        slice->compoundVector.push_back(compound);
        slice->setSRMId();
        slice->calculateRTMinMax(mavenParameters->matchRtFlag,
                                 mavenParameters->compoundRTWindow);
        slices.push_back(slice);
    }

    // TODO: instead of separating by sample, just organize all samples together
    // into a single vector
    map<mzSample*, vector<Scan*>> allMs2Scans = {};
    if (mavenParameters->matchFragmentationFlag) {
        for (mzSample* sample : mavenParameters->samples) {
            if (mavenParameters->stop) {
                delete_all(slices);
                break;
            }

            vector<Scan*> scanVector;
            for (auto scan : sample->scans) {
                if (scan->mslevel == 2)
                    scanVector.push_back(scan);
            }
            sort(scanVector.begin(), scanVector.end(),
                 [](const Scan* lhs, const Scan* rhs) {
                     return lhs->precursorMz < rhs->precursorMz;
                 });
            allMs2Scans.insert(make_pair(sample, scanVector));
        }
    }

    int counter = 0;
    int allFormulaeCount = static_cast<int>(formulae.size());
    for (const auto& formula : formulae) {
        if (mavenParameters->stop) {
            delete_all(slices);
            break;
        }

        counter++;
        sendBoostSignal("Computing Mass Slices", counter, allFormulaeCount);
        auto compounds = stringToCompoundMap.equal_range(formula);
        Compound *compound = nullptr;
        vector<Compound*> compoundVector;
        for (auto it = compounds.first; it != compounds.second; it++) {
            compound = it->second;
            compoundVector.push_back(compound);
        }

        float neutralMass = massCalc.computeNeutralMass(compound->formula);
        if (neutralMass <= 0) {
            continue;
        }

        for(auto adduct : adductList) {
            if (SIGN(adduct->getCharge()) != SIGN(mavenParameters->ionizationMode))
                continue;

            mzSlice* slice = new mzSlice();
            slice->mz = adduct->computeAdductMass(neutralMass);
            slice->compound = compound; // TODO: this assignment is meaningless.
            slice->compoundVector = compoundVector;
            slice->adduct = adduct;
            bool success =
                slice->calculateMzMinMax(mavenParameters->compoundMassCutoffWindow,
                                         adduct->getCharge());
            if (!success)
                continue;
            slice->calculateRTMinMax(mavenParameters->matchRtFlag,
                                     mavenParameters->compoundRTWindow);
            slice->setSRMId();

            if (mavenParameters->matchFragmentationFlag) {
                bool keepSlice = false;
                for (mzSample* sample : mavenParameters->samples) {
                    auto it = allMs2Scans.find(sample);
                    if (it != allMs2Scans.end()) {
                        auto scanVector = it->second;
                        auto low = lower_bound(scanVector.begin(),
                                               scanVector.end(),
                                               slice->mzmin,
                                               [](const Scan* scan,
                                                   const double &val) {
                                                   return scan->precursorMz < val;
                                               });
                        for (auto scanIter = low;
                             scanIter != scanVector.end();
                             scanIter++) {
                            Scan* scan = *scanIter;
                            if (scan->precursorMz >= slice->mzmin
                                && scan->precursorMz <= slice->mzmax) {
                                keepSlice = true;
                                break;
                            } else if (scan->precursorMz > slice->mzmax) {
                                break;
                            }
                        }
                    }
                    if (keepSlice)
                        break;
                }
                if (!keepSlice) {
                    delete (slice);
                    continue;
                }
            }

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

    mavenParameters->allgroups.clear();
    sort(slices.begin(), slices.end(), mzSlice::compIntensity);
    for (unsigned int s = 0; s < slices.size(); s++) {
        if (mavenParameters->stop)
            break;

        mzSlice* slice = slices[s];
        Compound* compound = slice->compound;
        if (compound != nullptr && compound->hasGroup())
            compound->unlinkGroup();

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

        for (Compound* compound : slice->compoundVector) {
            if (!compound)
                continue;

            if (compound->hasGroup())
                compound->unlinkGroup();

            // assign each compound at least once to the slice, find groups and
            // filter/keep them based on existing mechanisms
            slice->compound = compound;
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
                if (mavenParameters->searchAdducts && slice->adduct)
                    peakgroups[j].tagString = slice->adduct->getName();
                mavenParameters->allgroups.push_back(peakgroups[j]);
            }
        }

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
                                  + " groups";
            sendBoostSignal(progressText,
                            s + 1,
                            std::min((int)slices.size(),
                                     mavenParameters->limitGroupCount));
        }
    }

    // finally, we would like to remove adducts for which no parent ions were
    // detected or if parent ion's RT is too different from adduct's.
    sendBoostSignal("Filtering out false adducts…", 0, 0);
    GroupFiltering groupFilter(mavenParameters);
    groupFilter.filterAdducts(mavenParameters->allgroups);
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
            if (compound->formula().length() || compound->neutralMass != 0.0f) {
                int charge = mavenParameters->getCharge(compound);
                mz = compound->adjustedMass(charge);
            } else {
                mz = compound->mass;
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

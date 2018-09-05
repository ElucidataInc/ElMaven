#include "PeakDetector.h"

PeakDetector::PeakDetector() {
    mavenParameters = NULL;
}

PeakDetector::PeakDetector(MavenParameters* mp) {
	mavenParameters = mp;
}

void PeakDetector::resetProgressBar() {
	zeroStatus = true;
}

//TODO: Refactor this function. Too many parameters - Sahil
vector<EIC*> PeakDetector::pullEICs(mzSlice* slice,
                                    std::vector<mzSample*>&samples,
                                    int peakDetect, 
                                    int smoothingWindow,
                                    int smoothingAlgorithm, 
                                    float amuQ1, 
                                    float amuQ3,
                                    int baseline_smoothingWindow,
                                    int baseline_dropTopX, 
                                    double minSignalBaselineDifference,
                                    int eicType,
                                    string filterline) 
{

        vector<EIC*> eics;
        vector<mzSample*> vsamples;
        #ifndef __APPLE__
        #pragma omp parallel default(shared)
        #endif
        {

                #ifndef __APPLE__
                #pragma omp for
                #endif
                for (unsigned int i = 0; i < samples.size(); i++) {
                        if (samples[i] == NULL)
                                continue;
                        if (samples[i]->isSelected == false)
                                continue;
                        #ifndef __APPLE__
                        #pragma omp critical
                        #endif
                        vsamples.push_back(samples[i]);
                }

                // single threaded version - getting EICs of selected samples.
                // #pragma omp parallel for ordered

                #ifndef __APPLE__
                #pragma omp for
                #endif
                for (unsigned int i = 0; i < vsamples.size(); i++) {
                //Samples been selected
                mzSample* sample = vsamples[i];
                //getting the slice with which EIC has to be pulled
                Compound* c = slice->compound;

                EIC* e = NULL;

                if (!slice->srmId.empty()) {
                    
                    e = sample->getEIC(slice->srmId, eicType);
                }
                else if (c && c->precursorMz > 0 && c->productMz > 0) {

                    e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz, eicType,
                                    filterline, amuQ1, amuQ3);
                } else {

                        e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin,
                                        slice->rtmax, 1, eicType, filterline);
                }
                

                if (e) {
                        //if eic exists, perform smoothing
                        EIC::SmootherType smootherType =
                                (EIC::SmootherType) smoothingAlgorithm;
                        e->setSmootherType(smootherType);
                        e->setBaselineSmoothingWindow(baseline_smoothingWindow);
                        e->setBaselineDropTopX(baseline_dropTopX);
                        e->setFilterSignalBaselineDiff(minSignalBaselineDifference);
                        e->getPeakPositions(smoothingWindow);
                        //smoohing over

                        //push eic to all eics vector
                        #ifndef __APPLE__
                        #pragma omp critical
                        #endif
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
        Compound* compound = group.compound;

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

void PeakDetector::processMassSlices() {
    // init
    // TODO: what is this doing?
    // TODO: cant this be in background_peaks_update parameter setting function
    mavenParameters->showProgressFlag = true;
    mavenParameters->checkConvergance = true;
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
                                                string setName) {

        //While doing a compound database search limitGroupCount is set to
        //INT_MAX because we want all the peaks in the group
        //When doing the peakdetection without database then the amount of
        //peaks that will be there will be huge then its better to take the
        //first n relevent peaks rather than all the peaks
        //mavenParameters->limitGroupCount = INT_MAX;

        //iterating over all compounds in the set
        vector<mzSlice*> slices;

        //Looping over the compounds in the compound database
        for (unsigned int i = 0; i < set.size(); i++) {
                if (mavenParameters->stop) {
                    delete_all(slices);
                    break;
                }

                Compound* c = set[i];
                if (c == NULL)
                        continue;

                mzSlice* slice = new mzSlice();

                //setting the compound information into the slices
                slice->compound = c;

                slice->setSRMId();

                if (c->precursorMz == 0 || c->productMz == 0) {

                    //Calculating the mzmin and mzmax
                    int charge = mavenParameters->getCharge(c);
                    bool success  = \
                    slice->calculateMzMinMax(mavenParameters->compoundMassCutoffWindow, charge);
                    if (!success) continue;
                }

                //calculating the min and max RT
                slice->calculateRTMinMax(mavenParameters->matchRtFlag, \
                                        mavenParameters->compoundRTWindow);
                slices.push_back(slice);
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
            mzAligner.alignWithObiWarp(mavenParameters->samples, &params);
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

void PeakDetector::processSlices(vector<mzSlice *> &slices, string setName)
{

    if (slices.size() == 0)
        return;
    mavenParameters->allgroups.clear();

    sort(slices.begin(), slices.end(), mzSlice::compIntensity);

    int converged = 0;
    int foundGroups = 0;

    int eicCount = 0;
    for (unsigned int s = 0; s < slices.size(); s++)
    {

        if (mavenParameters->stop)
            break;
        mzSlice *slice = slices[s];

        Compound *compound = slice->compound;

        if (compound != NULL && compound->hasGroup())
            compound->unlinkGroup();

        //TODO: what is this for? this is not used
        //mavenParameters->checkConvergance is not always 0
        if (mavenParameters->checkConvergance)
        {
            mavenParameters->allgroups.size() - foundGroups > 0 ? converged =
                                                                      0
                                                                : converged++;
            if (converged > 1000)
            {
                break;
            }
            foundGroups = mavenParameters->allgroups.size();
        }

        vector<EIC *> eics;
        eics = pullEICs(slice,
                        mavenParameters->samples,
                        EicLoader::PeakDetection,
                        mavenParameters->eic_smoothingWindow,
                        mavenParameters->eic_smoothingAlgorithm,
                        mavenParameters->amuQ1,
                        mavenParameters->amuQ3,
                        mavenParameters->baseline_smoothingWindow,
                        mavenParameters->baseline_dropTopX,
                        mavenParameters->minSignalBaselineDifference,
                        mavenParameters->eicType,
                        mavenParameters->filterline);


        if (mavenParameters->clsf->hasModel())
        {
            mavenParameters->clsf->scoreEICs(eics);
        }

        float eicMaxIntensity = 0;
        for (unsigned int j = 0; j < eics.size(); j++)
        {
            eicCount++;
            float max = 0;

            switch ((PeakGroup::QType)mavenParameters->peakQuantitation)
            {
            case PeakGroup::AreaTop:
                max = eics[j]->maxAreaTopIntensity;
                break;
            case PeakGroup::Area:
                max = eics[j]->maxAreaIntensity;
                break;
            case PeakGroup::Height:
                max = eics[j]->maxIntensity;
                break;
            case PeakGroup::AreaNotCorrected:
                max = eics[j]->maxAreaNotCorrectedIntensity;
                break;
            case PeakGroup::AreaTopNotCorrected:
                max = eics[j]->maxAreaTopNotCorrectedIntensity;
                break;
            default:
                max = eics[j]->maxIntensity;
                break;
            }

            if (max > eicMaxIntensity)
                eicMaxIntensity = max;
        }
        if (eicMaxIntensity < mavenParameters->minGroupIntensity)
        {
            delete_all(eics);
            continue;
        }

        bool isIsotope = false;

        PeakFiltering peakFiltering(mavenParameters, isIsotope);
        peakFiltering.filter(eics);

        vector<PeakGroup> peakgroups =
            EIC::groupPeaks(eics,
                            mavenParameters->eic_smoothingWindow,
                            mavenParameters->grouping_maxRtWindow,
                            mavenParameters->minQuality,
                            mavenParameters->distXWeight,
                            mavenParameters->distYWeight,
                            mavenParameters->overlapWeight,
                            mavenParameters->useOverlap,
                            mavenParameters->minSignalBaselineDifference);

        GroupFiltering groupFiltering(mavenParameters, slice);
        groupFiltering.filter(peakgroups);

        //sort groups according to their rank
        std::sort(peakgroups.begin(), peakgroups.end(),
                  PeakGroup::compRank);

        for (unsigned int j = 0; j < peakgroups.size(); j++)
        {
            //check for duplicates	and append group
            if (j >= mavenParameters->eicMaxGroups)
                break;

            PeakGroup group = peakgroups[j];
            addPeakGroup(group);
        }

        //cleanup
        delete_all(eics);

        if (mavenParameters->allgroups.size() > mavenParameters->limitGroupCount)
        {
            cerr << "Group limit exceeded!" << endl;
            break;
        }

        if (zeroStatus)
        {
            sendBoostSignal("Status", 0, 1);
            zeroStatus = false;
        }

        if (mavenParameters->showProgressFlag && s % 10 == 0)
        {

            string progressText = "Found " + to_string(mavenParameters->allgroups.size()) + " groups";
            sendBoostSignal(progressText, s + 1, std::min((int)slices.size(), mavenParameters->limitGroupCount));
        }
    }
}

bool PeakDetector::addPeakGroup(PeakGroup& grup1) {
        bool noOverlap = true;

        for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
                PeakGroup& grup2 = mavenParameters->allgroups[i];
                float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt,
                                                        grup2.minRt, grup2.maxRt);
                if (rtoverlap > 0.9
                    && massCutoffDist(grup2.meanMz, grup1.meanMz,mavenParameters->massCutoffMerge)
                    < mavenParameters->massCutoffMerge->getMassCutoff()) {
                        noOverlap = false;
                break;
                }
        }

        //push the group to the allgroups vector
        mavenParameters->allgroups.push_back(grup1);
        return noOverlap;
}

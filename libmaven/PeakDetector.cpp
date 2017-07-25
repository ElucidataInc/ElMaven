#include "PeakDetector.h"

PeakDetector::PeakDetector() {
    mavenParameters = NULL; //naman: wasn't initialized
}
PeakDetector::PeakDetector(MavenParameters* mp) {
	mavenParameters = mp;
}

/**
 * TODO
 */

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
        #pragma omp parallel default(shared)
        {
                //Selecting only the samples that is been selected by the user
                // double start=omp_get_wtime();
                #pragma omp for
                for (unsigned int i = 0; i < samples.size(); i++) {
                        if (samples[i] == NULL)
                                continue;
                        if (samples[i]->isSelected == false)
                                continue;
                        #pragma omp critical
                        vsamples.push_back(samples[i]);
                }
                // cerr<<"\n Time take by PeakDetector::pullEICs Loop1: "<<omp_get_wtime()-start<<"\n";
                // single threaded version - getting EICs of selected samples.
                // #pragma omp parallel for ordered
                // start=omp_get_wtime();
                #pragma omp for 
                for (unsigned int i = 0; i < vsamples.size(); i++) {
                //Samples been selected
                mzSample* sample = vsamples[i];
                //getting the slice with which EIC has to be pulled
                Compound* c = slice->compound;

                //Init EIC by pointing it to NULL
                EIC* e = NULL;
                //TODO: what is SRM again going here?
                if (!slice->srmId.empty()) {
                        //if srmId of the slice is present, get EICs on the basis of srmId
                        //cout << "computeEIC srm:" << slice->srmId << endl;


                        e = sample->getEIC(slice->srmId, eicType);
                        
                        //TODO this is for MS/MS?
                } else if (c && c->precursorMz > 0 && c->productMz > 0) {
                        //if product and parent ion's m/z of the compound in slice is present, get EICs for QQQ mode

                        e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz, eicType,
                                        filterline, amuQ1, amuQ3);

                } else {
                        //This is the usual case where we are going peakpicking
                        //with DB. This is a general enough senerio
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
                        #pragma omp critical
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
            pullIsotopes(&group);

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
    massSlices.algorithmB(mavenParameters->ppmMerge, mavenParameters->rtStepSize);  // perform algorithmB for samples

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

                // //Calculating the mzmin and mzmax
                int charge = mavenParameters->getCharge(c);
                bool success  = \
                slice->calculateMzMinMax(mavenParameters->compoundPPMWindow, charge);
                if (!success) continue;

                //calculating the min and max RT
                slice->calculateRTMinMax(mavenParameters->matchRtFlag, \
                                        mavenParameters->compoundRTWindow);
                slices.push_back(slice);
        }

        return slices;
}

//TODO: lots of code duplicated between this and PullIsotopes(PeakGroup*). See comments for that function
void PeakDetector::pullIsotopesBarPlot(PeakGroup* parentgroup) {
    // FALSE CONDITIONS
    if (parentgroup == NULL)
        return;
    if (parentgroup->compound == NULL)
        return;
    if (parentgroup->compound->formula.empty() == true)
        return;
    if (mavenParameters->samples.size() == 0)
        return;

    string formula = parentgroup->compound->formula; //parent formula
    int charge = mavenParameters->getCharge(parentgroup->compound);
    //generate isotope list for parent mass
    vector<Isotope> masslist = MassCalculator::computeIsotopes(formula, charge, 
                                                    mavenParameters->isotopeAtom, mavenParameters->noOfIsotopes);

    //iterate over samples to find properties for parent's isotopes.
    map<string, PeakGroup> isotopes;
    map<string, PeakGroup>::iterator itr2;

    //   #pragma omp parallel for ordered
    for (unsigned int s = 0; s < mavenParameters->samples.size(); s++) {
        mzSample* sample = mavenParameters->samples[s];
        for (unsigned int k = 0; k < masslist.size(); k++) {
            //			if (stopped())
            //				break; TODO: stop
            Isotope& x = masslist[k];
            string isotopeName = x.name;
            double isotopeMass = x.mass;
            double expectedAbundance = x.abundance;

            float mzmin = isotopeMass -
                isotopeMass / 1e6 *
                mavenParameters->compoundPPMWindow;
            float mzmax = isotopeMass +
                isotopeMass / 1e6 *
                mavenParameters->compoundPPMWindow;

            float rt = parentgroup->medianRt();
            float rtmin = parentgroup->minRt;
            float rtmax = parentgroup->maxRt;

            Peak* parentPeak = parentgroup->getPeak(sample);
            if (parentPeak)
                rt = parentPeak->rt;
            if (parentPeak)
                rtmin = parentPeak->rtmin;
            if (parentPeak)
                rtmax = parentPeak->rtmax;

            float isotopePeakIntensity = 0;
            float parentPeakIntensity = 0;

            if (parentPeak) {
                parentPeakIntensity = parentPeak->peakIntensity;
                int scannum = parentPeak->getScan()->scannum;
                for (int i = scannum - 3; i < scannum + 3; i++) {
                    Scan* s = sample->getScan(i);

                    //look for isotopic mass in the same spectrum
                    vector<int> matches = s->findMatchingMzs(mzmin, mzmax);

                    for (unsigned int i = 0; i < matches.size(); i++) {
                        int pos = matches[i];
                        if (s->intensity[pos] > isotopePeakIntensity) {
                            isotopePeakIntensity = s->intensity[pos];
                            rt = s->rt;
                        }
                    }
                }

            }
            //if(isotopePeakIntensity==0) continue;

            //natural abundance check
            if ((x.C13 > 0 && mavenParameters->C13Labeled_Barplot == false) //if isotope is not C13Labeled
                    || (x.N15 > 0 && mavenParameters->N15Labeled_Barplot == false) //if isotope is not N15 Labeled
                    || (x.S34 > 0 && mavenParameters->S34Labeled_Barplot == false) //if isotope is not S34 Labeled
                    || (x.H2 > 0 && mavenParameters->D2Labeled_Barplot == false) //if isotope is not D2 Labeled

               ) {
                if (expectedAbundance < 1e-8)
                    continue;
                if (expectedAbundance * parentPeakIntensity < 1)
                    continue;
                float observedAbundance = isotopePeakIntensity
                    / (parentPeakIntensity + isotopePeakIntensity); //find observedAbundance based on isotopePeakIntensity

                float naturalAbundanceError = abs(
                        observedAbundance - expectedAbundance) //if observedAbundance is significant wrt expectedAbundance
                    / expectedAbundance * 100; // compute natural Abundance Error


                if (naturalAbundanceError >
                        mavenParameters->maxNaturalAbundanceErr)
                    continue;
            }

            float w = mavenParameters->maxIsotopeScanDiff
                * mavenParameters->avgScanTime;
            double c = sample->correlation(
                    isotopeMass, parentgroup->meanMz,
                    mavenParameters->compoundPPMWindow, rtmin - w,
                    rtmax + w, mavenParameters->eicType,
                    mavenParameters->filterline);  // find correlation for isotopes
            if (c < mavenParameters->minIsotopicCorrelation)
                continue;

            vector<Peak> allPeaks;

            EIC * eic = sample->getEIC(mzmin, mzmax, sample->minRt,sample->maxRt, 1, mavenParameters->eicType,
                                       mavenParameters->filterline);
            //actually last parameter should probably be deepest MS level?
            //TODO: decide how isotope children should even work in MS mode

            // smooth fond eic TODO: null check for found
            eic->setSmootherType(
                    (EIC::SmootherType)
                    mavenParameters->eic_smoothingAlgorithm);
            eic->setBaselineSmoothingWindow(mavenParameters->baseline_smoothingWindow);
            eic->setBaselineDropTopX(mavenParameters->baseline_dropTopX);
            eic->setFilterSignalBaselineDiff(mavenParameters->isotopicMinSignalBaselineDifference);
            eic->getPeakPositions(mavenParameters->eic_smoothingWindow);
            //TODO: this could be optimized to not bother finding peaks outside of
            //maxIsotopeScanDiff window
            allPeaks = eic->peaks;

            delete(eic);
            // find nearest peak as long as it is within RT window
            float maxRtDiff=mavenParameters->maxIsotopeScanDiff * mavenParameters->avgScanTime;
            //why are we even doing this calculation, why not have the parameter be in units of RT?
            Peak* nearestPeak = NULL;
            float d = FLT_MAX;
            for (unsigned int i = 0; i < allPeaks.size(); i++) {
                Peak& x = allPeaks[i];
                float dist = abs(x.rt - rt);
                if (dist > maxRtDiff)
                    continue;
                if (dist < d) {
                    d = dist;
                    nearestPeak = &x;
                }
            }
            //delete (nearestPeak);
            if (nearestPeak) { //if nearest peak is present
                if (isotopes.count(isotopeName) == 0) { //label the peak of isotope
                    PeakGroup g;
                    g.meanMz = isotopeMass;
                    g.tagString = isotopeName;
                    g.expectedAbundance = expectedAbundance;
                    g.isotopeC13count = x.C13;
                    isotopes[isotopeName] = g;
                }
                isotopes[isotopeName].addPeak(*nearestPeak); //add nearestPeak to isotope peak list
            }
            vector<Peak>().swap(allPeaks);
        }
    }

    //fill peak group list with the compound and its isotopes.
    // peak group list would be filled with the parent group, with its isotopes as children
    // click on + to see children == isotopes
    parentgroup->childrenBarPlot.clear();
    for (itr2 = isotopes.begin(); itr2 != isotopes.end(); ++itr2) {
        string isotopeName = (*itr2).first;
        PeakGroup& child = (*itr2).second;
        child.minQuality = mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        if (mavenParameters->clsf->hasModel()) {
            mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }

        
        if (!getMavenParameters()->C13Labeled_Barplot) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!getMavenParameters()->N15Labeled_Barplot) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }
        
        if (!getMavenParameters()->S34Labeled_Barplot) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!getMavenParameters()->D2Labeled_Barplot) {
            if (isotopeName.find(H2_LABEL) != string::npos)
                continue;
        }
        parentgroup->addChildBarPlot(child);
    }
}

void PeakDetector::pullIsotopes(PeakGroup* parentgroup) {
    // FALSE CONDITIONS
    if (parentgroup == NULL)
        return;
    if (parentgroup->compound == NULL)
        return;
    if (parentgroup->compound->formula.empty() == true)
        return;
    if (mavenParameters->samples.size() == 0)
        return;

    string formula = parentgroup->compound->formula; //parent formula
    int charge = mavenParameters->getCharge(parentgroup->compound);//generate isotope list for parent mass

    vector<Isotope> masslist = MassCalculator::computeIsotopes(formula, charge, 
                                                        mavenParameters->isotopeAtom, mavenParameters->noOfIsotopes);


    //iterate over samples to find properties for parent's isotopes.
    map<string, PeakGroup> isotopes;
    map<string, PeakGroup>::iterator itr2;

    //   #pragma omp parallel for ordered
    for (unsigned int s = 0; s < mavenParameters->samples.size(); s++) {
        mzSample* sample = mavenParameters->samples[s];
        for (unsigned int k = 0; k < masslist.size(); k++) {
            //			if (stopped())
            //				break; TODO: stop
            Isotope& x = masslist[k];
            string isotopeName = x.name;
            double isotopeMass = x.mass;
            double expectedAbundance = x.abundance;

            float mzmin = isotopeMass -
                isotopeMass / 1e6 *
                mavenParameters->compoundPPMWindow;
            float mzmax = isotopeMass +
                isotopeMass / 1e6 *
                mavenParameters->compoundPPMWindow;

            float rt = parentgroup->medianRt();
            float rtmin = parentgroup->minRt;
            float rtmax = parentgroup->maxRt;

            Peak* parentPeak = parentgroup->getPeak(sample);
            if (parentPeak)
                rt = parentPeak->rt;
            if (parentPeak)
                rtmin = parentPeak->rtmin;
            if (parentPeak)
                rtmax = parentPeak->rtmax;

            float isotopePeakIntensity = 0;
            float parentPeakIntensity = 0;

            if (parentPeak) {
                parentPeakIntensity = parentPeak->peakIntensity;
                int scannum = parentPeak->getScan()->scannum;
                for (int i = scannum - 3; i < scannum + 3; i++) {
                    Scan* s = sample->getScan(i);

                    //look for isotopic mass in the same spectrum
                    vector<int> matches = s->findMatchingMzs(mzmin, mzmax);

                    for (unsigned int i = 0; i < matches.size(); i++) {
                        int pos = matches[i];
                        if (s->intensity[pos] > isotopePeakIntensity) {
                            isotopePeakIntensity = s->intensity[pos];
                            rt = s->rt;
                        }
                    }
                }

            }
            //if(isotopePeakIntensity==0) continue;

            //natural abundance check
            //TODO: I think this loop will never run right? Since we're now only pulling the relevant isotopes
            //if x.C13>0 then mavenParameters->C13Labeled_BPE must have been true
            //so we could just eliminate maxNaturalAbundanceErr parameter in this case
            //original idea (see https://github.com/ElucidataInc/ElMaven/issues/43) was to have different checkboxes for "use this element for natural abundance check"
            if ((x.C13 > 0 && mavenParameters->C13Labeled_BPE == false) //if isotope is not C13Labeled
                    || (x.N15 > 0 && mavenParameters->N15Labeled_BPE == false) //if isotope is not N15 Labeled
                    || (x.S34 > 0 && mavenParameters->S34Labeled_BPE == false) //if isotope is not S34 Labeled
                    || (x.H2 > 0 && mavenParameters->D2Labeled_BPE == false) //if isotope is not D2 Labeled

               ) {
                if (expectedAbundance < 1e-8)
                    continue;
                if (expectedAbundance * parentPeakIntensity < 1) //TODO: In practice this is probably fine but in general I don't like these types of intensity checks -- the actual absolute value depends on the type of instrument, etc
                    continue;
                float observedAbundance = isotopePeakIntensity
                    / (parentPeakIntensity + isotopePeakIntensity); //find observedAbundance based on isotopePeakIntensity

                float naturalAbundanceError = abs(
                        observedAbundance - expectedAbundance) //if observedAbundance is significant wrt expectedAbundance
                    / expectedAbundance * 100; // compute natural Abundance Error


                if (naturalAbundanceError >
                        mavenParameters->maxNaturalAbundanceErr)
                    continue;
            }

            //TODO: this is really an abuse of the maxIsotopeScanDiff parameter
            //I can easily imagine you might set maxIsotopeScanDiff to something much less than the peak width
            //here w should really be determined by the minRt and maxRt for the parent and child peaks
            float w = mavenParameters->maxIsotopeScanDiff
                * mavenParameters->avgScanTime;
            double c = sample->correlation(
                    isotopeMass, parentgroup->meanMz,
                    mavenParameters->compoundPPMWindow, rtmin - w,
                    rtmax + w, mavenParameters->eicType,
                    mavenParameters->filterline);  // find correlation for isotopes
            if (c < mavenParameters->minIsotopicCorrelation)
                continue;

            vector<Peak> allPeaks;

            EIC * eic = sample->getEIC(mzmin, mzmax, sample->minRt,sample->maxRt, 1, mavenParameters->eicType,
                                        mavenParameters->filterline);
            //actually last parameter should probably be deepest MS level?
            //TODO: decide how isotope children should even work in MS mode

            // smooth fond eic TODO: null check for found
            eic->setSmootherType(
                    (EIC::SmootherType)
                    mavenParameters->eic_smoothingAlgorithm);
            eic->setBaselineSmoothingWindow(mavenParameters->baseline_smoothingWindow);
            eic->setBaselineDropTopX(mavenParameters->baseline_dropTopX);
            eic->setFilterSignalBaselineDiff(mavenParameters->isotopicMinSignalBaselineDifference);
            eic->getPeakPositions(mavenParameters->eic_smoothingWindow);
            //TODO: this needs be optimized to not bother finding peaks outside of
            //maxIsotopeScanDiff window
            allPeaks = eic->peaks;

            delete(eic);
            // find nearest peak as long as it is within RT window
            float maxRtDiff=mavenParameters->maxIsotopeScanDiff * mavenParameters->avgScanTime;
            //why are we even doing this calculation, why not have the parameter be in units of RT?
            Peak* nearestPeak = NULL;
            float d = FLT_MAX;
            for (unsigned int i = 0; i < allPeaks.size(); i++) {
                Peak& x = allPeaks[i];
                float dist = abs(x.rt - rt);
                if (dist > maxRtDiff)
                    continue;
                if (dist < d) {
                    d = dist;
                    nearestPeak = &x;
                }
            }

            //delete (nearestPeak);
            if (nearestPeak) { //if nearest peak is present
                if (isotopes.count(isotopeName) == 0) { //label the peak of isotope
                    PeakGroup g;
                    g.meanMz = isotopeMass;
                    g.tagString = isotopeName;
                    g.expectedAbundance = expectedAbundance;
                    g.isotopeC13count = x.C13;
                    isotopes[isotopeName] = g;
                }
                isotopes[isotopeName].addPeak(*nearestPeak); //add nearestPeak to isotope peak list
            }
            vector<Peak>().swap(allPeaks);
        }
    }

    //fill peak group list with the compound and its isotopes.
    // peak group list would be filled with the parent group, with its isotopes as children
    // click on + to see children == isotopes
    // parentgroup->children.clear();
    for (itr2 = isotopes.begin(); itr2 != isotopes.end(); ++itr2) {
        string isotopeName = (*itr2).first;
        PeakGroup& child = (*itr2).second;
        child.minQuality = mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        //TODO: isn't this a bug? shouldn't it get a new groupId?
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        if (mavenParameters->clsf->hasModel()) {
            mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }


        float rtDiff = -1;

        if (child.compound != NULL && child.compound->expectedRt > 0)
        {
            rtDiff = abs(child.compound->expectedRt - (child.meanRt));
            child.expectedRtDiff = rtDiff;
        }

        double A = (double) mavenParameters->qualityWeight/10;
        double B = (double) mavenParameters->intensityWeight/10;
        double C = (double) mavenParameters->deltaRTWeight/10;

        if (mavenParameters->deltaRtCheckFlag && child.compound != NULL && child.compound->expectedRt > 0)
        {
            child.groupRank = pow(rtDiff, 2*C) * pow((1.1 - child.maxQuality), A)
                                                  * (1 /( pow(log(child.maxIntensity + 1), B)));
        }
        else
        {
            child.groupRank = pow((1.1 - child.maxQuality), A)
                                                  * (1 /(pow(log(child.maxIntensity + 1), B)));

        }


        if (!getMavenParameters()->C13Labeled_BPE) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!getMavenParameters()->N15Labeled_BPE) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }

        if (!getMavenParameters()->S34Labeled_BPE) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!getMavenParameters()->D2Labeled_BPE) {
            if (isotopeName.find(H2_LABEL) != string::npos)
                continue;
        }

        bool childExist = false;
        for (unsigned int ii = 0; ii < parentgroup->children.size(); ii++) {
            if (parentgroup->children[ii].tagString == isotopeName) {
                childExist = true;
            }
        }

        if (!childExist) parentgroup->addChild(child);
    }

    // parentgroup->childrenIsoWidget.clear();
    for (itr2 = isotopes.begin(); itr2 != isotopes.end(); ++itr2) {
        string isotopeName = (*itr2).first;
        PeakGroup& child = (*itr2).second;
        child.minQuality = mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        
        if (mavenParameters->clsf->hasModel()) {
            mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }
        
        if (!getMavenParameters()->C13Labeled_IsoWidget) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!getMavenParameters()->N15Labeled_IsoWidget) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }
        
        if (!getMavenParameters()->S34Labeled_IsoWidget) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!getMavenParameters()->D2Labeled_IsoWidget) {
            if (isotopeName.find(H2_LABEL) != string::npos)
                continue;
        }

        bool childExist = false;
        for (unsigned int ii = 0; ii < parentgroup->childrenIsoWidget.size(); ii++) {
            if (parentgroup->childrenIsoWidget[ii].tagString == isotopeName) {
                childExist = true;
            }
        }

        if (!childExist) parentgroup->addChildIsoWidget(child);

    }
}

void PeakDetector::alignSamples() {
        //being called in CLI.
        if (mavenParameters->samples.size() > 1
            && mavenParameters->alignSamplesFlag) {
                cerr << "Aligning samples" << endl;

                mavenParameters->writeCSVFlag = false;
                processMassSlices();

                cerr << "Aligner=" << mavenParameters->allgroups.size() << endl;
                vector<PeakGroup*> agroups(mavenParameters->allgroups.size());
                for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++)
                        agroups[i] = &mavenParameters->allgroups[i];
                //init aligner
                Aligner aligner;
                aligner.doAlignment(agroups);
                mavenParameters->writeCSVFlag = true;
        }
}

bool PeakDetector::quantileFilters(PeakGroup *group) {
    if (group->maxIntensity < mavenParameters->minGroupIntensity){
        return true;
    }
    if (group->maxSignalBaselineRatio < mavenParameters->minSignalBaseLineRatio) {
        return true;
    }
    if (mavenParameters->clsf->hasModel() && 
        group->maxQuality < mavenParameters->minQuality) {
            return true;
    }
    if (group->maxIntensity < group->blankMax * mavenParameters->minSignalBlankRatio){
        return true;
    }
    vector<Peak> peaks = group->getPeaks();
    int peaksAboveMinIntensity = 0;
    int peaksAboveBaselineRatio = 0;
    int peaksAboveBlankRatio = 0;
    int peaksAboveMinQuality = 0;
    for (int i = 0; i < peaks.size(); i++) {
        if (peaks[i].peakIntensity > mavenParameters->minGroupIntensity) {
            peaksAboveMinIntensity++;
        }
        if (peaks[i].signalBaselineRatio > mavenParameters->minSignalBaseLineRatio) {
            peaksAboveBaselineRatio++;
        }
        if (peaks[i].peakIntensity > group->blankMax * mavenParameters->minSignalBlankRatio){
            peaksAboveBlankRatio++;
        }
        if (peaks[i].quality > mavenParameters->minQuality) {
            peaksAboveMinQuality++;
        }
    }
    int noVisibleSamples = mavenParameters->getVisibleSamples().size();
    if ((1.0*peaksAboveMinIntensity/noVisibleSamples) * 100 < mavenParameters->quantileIntensity) {
        return true;
    }
    if ((1.0*peaksAboveMinIntensity/noVisibleSamples) * 100 < mavenParameters->quantileQuality) {
        return true;
    }
    if ((1.0*peaksAboveBaselineRatio/noVisibleSamples)*100 < mavenParameters->quantileSignalBaselineRatio){
        return true;
    }
    if ((1.0*peaksAboveBlankRatio/noVisibleSamples)*100 < mavenParameters->quantileSignalBlankRatio){
        return true;
    }
    return false;
}

void PeakDetector::processSlices(vector<mzSlice*>&slices, string setName) {

        if (slices.size() == 0)
                return;
        mavenParameters->allgroups.clear();

	//TODO: All the ion counts are 0 there is no use in sorting
        sort(slices.begin(), slices.end(), mzSlice::compIntensity);

	//process KNOWNS
        QTime timer;
        timer.start();
        // qDebug() << "Proessing slices: setName=" << setName.c_str() << " slices="
        //          << slices.size();

        int converged = 0;
        int foundGroups = 0;

        int eicCount = 0;
        int groupCount = 0;
        // int peakCount = 0; //naman: not being used anywhere TODO comment it out 

        // for all the slies, find EICs and score quality and rank ofpeaks in the EICs
        // using the classifier
        // #pragma omp parallel for ordered
        for (unsigned int s = 0; s < slices.size(); s++) {

                if (mavenParameters->stop) break;
                mzSlice* slice = slices[s];

                Compound* compound = slice->compound;

                if (compound != NULL && compound->hasGroup())
                        compound->unlinkGroup();

                //TODO: what is this for? this is not used
                //mavenParameters->checkConvergance is not always 0
                if (mavenParameters->checkConvergance) {
                        mavenParameters->allgroups.size() - foundGroups > 0 ? converged =
                                0 :
                                converged++;
                        if (converged > 1000) {
                        break;
                        }
                        foundGroups = mavenParameters->allgroups.size();
                }

//		qDebug() << "Pulling EICs";

                vector<EIC*> eics;
                // for all the slies, find EICs
                // #pragma omp ordered
                //TODO: all the settings from this are not connected to the main
                //window parameters which are not connected are they are
                //connected from options settings
                //mavenParameters->baseline_smoothingWindow
                //mavenParameters->baseline_dropTopX
                eics = pullEICs(slice, mavenParameters->samples,
                                EicLoader::PeakDetection, mavenParameters->eic_smoothingWindow,
                                mavenParameters->eic_smoothingAlgorithm, mavenParameters->amuQ1,
                                mavenParameters->amuQ3,
                                mavenParameters->baseline_smoothingWindow,
                                mavenParameters->baseline_dropTopX,
                                mavenParameters->minSignalBaselineDifference,
                                mavenParameters->eicType,
                                mavenParameters->filterline);

                float eicMaxIntensity = 0;

                for (unsigned int j = 0; j < eics.size(); j++) {
                    eicCount++;
                    float max = 0;

                    switch((PeakGroup::QType) mavenParameters->peakQuantitation) {
                        case PeakGroup::AreaTop: max = eics[j]->maxAreaTopIntensity; break;
                        case PeakGroup::Area: max =  eics[j]->maxAreaIntensity; break;
                        case PeakGroup::Height: max = eics[j]->maxIntensity; break;
                        case PeakGroup::AreaNotCorrected: max = eics[j]->maxAreaNotCorrectedIntensity; break;
                        case PeakGroup::AreaTopNotCorrected: max = eics[j]->maxAreaTopNotCorrectedIntensity; break;
                        default: max = eics[j]->maxIntensity; break;
                    }

                    if (max > eicMaxIntensity)
                        eicMaxIntensity = max;
                }
                if (eicMaxIntensity < mavenParameters->minGroupIntensity) {
                        delete_all(eics);
                        continue;
                }

                //for ( unsigned int j=0; j < eics.size(); j++ )	eics[j]->getPeakPositions(eic_smoothingWindow);
                vector<PeakGroup> peakgroups = EIC::groupPeaks(eics,
                                                                mavenParameters->eic_smoothingWindow,
                                                                mavenParameters->grouping_maxRtWindow,
                                                                mavenParameters->minQuality,
                                                                mavenParameters->distXWeight,
                                                                mavenParameters->distYWeight,
                                                                mavenParameters->overlapWeight,
                                                                mavenParameters->useOverlap,
                                                                mavenParameters->minSignalBaselineDifference);


		//score quality of each group using classifier
                vector<PeakGroup*> groupsToAppend;
                for (unsigned int j = 0; j < peakgroups.size(); j++) {

                        PeakGroup& group = peakgroups[j];
                        group.setQuantitationType((PeakGroup::QType) mavenParameters->peakQuantitation);
                        group.minQuality = mavenParameters->minQuality;
                        group.minIntensity = mavenParameters->minGroupIntensity;
                        group.computeAvgBlankArea(eics);
                        group.groupStatistics();
                        groupCount++;
                        // peakCount += group.peakCount(); //naman: not being used anywhere TODO comment it out 

			if (mavenParameters->clsf->hasModel()) {
				mavenParameters->clsf->classify(&group);
                                group.groupStatistics();
                        }
                        if (mavenParameters->clsf->hasModel()
                            && group.goodPeakCount < mavenParameters->minGoodGroupCount)
                                continue;
                        // if (group.blankMean*minBlankRatio > group.sampleMean ) continue;
                        
                        if (group.maxNoNoiseObs < mavenParameters->minNoNoiseObs)
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

                        // Peak Group Rank accoording to given weightage
                        double A = (double) mavenParameters->qualityWeight/10;
                        double B = (double) mavenParameters->intensityWeight/10;
                        double C = (double) mavenParameters->deltaRTWeight/10;

                        if (compound != NULL && compound->expectedRt > 0) {
                            if(mavenParameters->deltaRtCheckFlag) {
                            group.groupRank = pow(rtDiff, 2*C) * pow((1.1 - group.maxQuality), A)
                                                  * (1 /( pow(log(group.maxIntensity + 1), B))); //TODO Formula to rank groups
                            }
                            if (mavenParameters->matchRtFlag && group.expectedRtDiff > mavenParameters->compoundRTWindow) continue;

                        } 
                        
                        if (!mavenParameters->deltaRtCheckFlag || compound == NULL || compound->expectedRt <= 0) {
                            group.groupRank = pow((1.1 - group.maxQuality), A)
                                                  * (1 /(pow(log(group.maxIntensity + 1), B)));
                        }

                        groupsToAppend.push_back(&group);
                }

		//sort groups according to their rank
                std::sort(groupsToAppend.begin(), groupsToAppend.end(),
                          PeakGroup::compRankPtr);

                for (unsigned int j = 0; j < groupsToAppend.size(); j++) {
                        //check for duplicates	and append group
                        if (j >= mavenParameters->eicMaxGroups)
                                break;

                        PeakGroup* group = groupsToAppend[j];
                        // bool ok = addPeakGroup(*group); //naman: unused, maybe used below.
                        addPeakGroup(*group); //naman: unused, maybe used below.

                        //TODO: commended by sabu
                        // //force insert when processing compounds.. even if duplicated
                        // if (ok == false && compound != NULL)
                        //         mavenParameters->allgroups.push_back(*group);
                }

                //cleanup
                delete_all(eics);

                if (mavenParameters->allgroups.size()
                    > mavenParameters->limitGroupCount) {
                        cerr << "Group limit exceeded!" << endl;
//       #pragma omp cancel for
                        break;
                }

                if (zeroStatus){
					sendBoostSignal("Status", 0 , 1);
					zeroStatus = false;
                }

                if (mavenParameters->showProgressFlag && s % 10 == 0) {

                        string progressText = "Found "
                                               + to_string(mavenParameters->allgroups.size())
                                               + " groups";
                sendBoostSignal(progressText,s + 1, std::min((int) slices.size(), mavenParameters->limitGroupCount));

                        /*TODO: Fix Q_EMIT update progress of slices.
                           Q_EMIT(
                           updateProgressBar(progressText, s + 1,
                           std::min((int) slices.size(), limitGroupCount)));
                         */
                }
        }

        //cleanup();
}

bool PeakDetector::addPeakGroup(PeakGroup& grup1) {
        bool noOverlap = true;

//   #pragma omp parallel for
        for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
                PeakGroup& grup2 = mavenParameters->allgroups[i];
                float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt,
                                                        grup2.minRt, grup2.maxRt);
                if (rtoverlap > 0.9
                    && ppmDist(grup2.meanMz, grup1.meanMz)
                    < mavenParameters->ppmMerge) {
                        noOverlap = false;
//       #pragma omp cancel for
                break;
                }
        }

        //push the group to the allgroups vector
        mavenParameters->allgroups.push_back(grup1);
        return noOverlap;
}

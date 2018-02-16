#include "isotopeDetection.h"

IsotopeDetection::IsotopeDetection(MavenParameters *mavenParameters)
{
    _mavenParameters = mavenParameters;
}

//TODO: lots of code duplicated between this and PullIsotopes(PeakGroup*). See comments for that function
void IsotopeDetection::pullIsotopesBarPlot(PeakGroup* parentgroup) {
    // FALSE CONDITIONS
    if (parentgroup == NULL)
        return;
    if (parentgroup->compound == NULL)
        return;
    if (parentgroup->compound->formula.empty() == true)
        return;
    if (_mavenParameters->samples.size() == 0)
        return;

    string formula = parentgroup->compound->formula; //parent formula
    int charge = _mavenParameters->getCharge(parentgroup->compound);
    //generate isotope list for parent mass
    vector<Isotope> masslist = MassCalculator::computeIsotopes(formula, charge, 
                                                    _mavenParameters->isotopeAtom, _mavenParameters->noOfIsotopes);

    //iterate over samples to find properties for parent's isotopes.
    map<string, PeakGroup> isotopes;
    map<string, PeakGroup>::iterator itr2;

    //   #pragma omp parallel for ordered
    for (unsigned int s = 0; s < _mavenParameters->samples.size(); s++) {
        mzSample* sample = _mavenParameters->samples[s];
        for (unsigned int k = 0; k < masslist.size(); k++) {
            //			if (stopped())
            //				break; TODO: stop
            Isotope& x = masslist[k];
            string isotopeName = x.name;
            double isotopeMass = x.mass;
            double expectedAbundance = x.abundance;

            float mzmin = isotopeMass -_mavenParameters->compoundMassCutoffWindow->massCutoffValue(isotopeMass);
            float mzmax = isotopeMass +_mavenParameters->compoundMassCutoffWindow->massCutoffValue(isotopeMass);

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
            if ((x.C13 > 0 && _mavenParameters->C13Labeled_Barplot == false) //if isotope is not C13Labeled
                    || (x.N15 > 0 && _mavenParameters->N15Labeled_Barplot == false) //if isotope is not N15 Labeled
                    || (x.S34 > 0 && _mavenParameters->S34Labeled_Barplot == false) //if isotope is not S34 Labeled
                    || (x.H2 > 0 && _mavenParameters->D2Labeled_Barplot == false) //if isotope is not D2 Labeled

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
                        _mavenParameters->maxNaturalAbundanceErr)
                    continue;
            }

            float w = _mavenParameters->maxIsotopeScanDiff
                * _mavenParameters->avgScanTime;
            double c = sample->correlation(
                    isotopeMass, parentgroup->meanMz,
                    _mavenParameters->compoundMassCutoffWindow, rtmin - w,
                    rtmax + w, _mavenParameters->eicType,
                    _mavenParameters->filterline);  // find correlation for isotopes
            if (c < _mavenParameters->minIsotopicCorrelation)
                continue;

            vector<Peak> allPeaks;

            EIC * eic = sample->getEIC(mzmin, mzmax, sample->minRt,sample->maxRt, 1, _mavenParameters->eicType,
                                       _mavenParameters->filterline);
            //actually last parameter should probably be deepest MS level?
            //TODO: decide how isotope children should even work in MS mode

            // smooth fond eic TODO: null check for found
            eic->setSmootherType(
                    (EIC::SmootherType)
                    _mavenParameters->eic_smoothingAlgorithm);
            eic->setBaselineSmoothingWindow(_mavenParameters->baseline_smoothingWindow);
            eic->setBaselineDropTopX(_mavenParameters->baseline_dropTopX);
            eic->setFilterSignalBaselineDiff(_mavenParameters->isotopicMinSignalBaselineDifference);
            eic->getPeakPositions(_mavenParameters->eic_smoothingWindow);
            //TODO: this could be optimized to not bother finding peaks outside of
            //maxIsotopeScanDiff window
            allPeaks = eic->peaks;

            bool isIsotope = true;
			PeakFiltering peakFiltering(_mavenParameters, isIsotope);
            peakFiltering.filter(allPeaks);

            delete(eic);
            // find nearest peak as long as it is within RT window
            float maxRtDiff=_mavenParameters->maxIsotopeScanDiff * _mavenParameters->avgScanTime;
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
        child.minQuality = _mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        if (_mavenParameters->clsf->hasModel()) {
            _mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }

        
        if (!_mavenParameters->C13Labeled_Barplot) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!_mavenParameters->N15Labeled_Barplot) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }
        
        if (!_mavenParameters->S34Labeled_Barplot) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!_mavenParameters->D2Labeled_Barplot) {
            if (isotopeName.find(H2_LABEL) != string::npos)
                continue;
        }
        parentgroup->addChildBarPlot(child);
    }
}

void IsotopeDetection::pullIsotopes(PeakGroup* parentgroup) {
    // FALSE CONDITIONS
    if (parentgroup == NULL)
        return;
    if (parentgroup->compound == NULL)
        return;
    if (parentgroup->compound->formula.empty() == true)
        return;
    if (_mavenParameters->samples.size() == 0)
        return;

    string formula = parentgroup->compound->formula; //parent formula
    int charge = _mavenParameters->getCharge(parentgroup->compound);//generate isotope list for parent mass

    vector<Isotope> masslist = MassCalculator::computeIsotopes(formula, charge, 
                                                        _mavenParameters->isotopeAtom, _mavenParameters->noOfIsotopes);


    //iterate over samples to find properties for parent's isotopes.
    map<string, PeakGroup> isotopes;

    for (unsigned int s = 0; s < _mavenParameters->samples.size(); s++) {
        mzSample* sample = _mavenParameters->samples[s];
        for (unsigned int k = 0; k < masslist.size(); k++) {
            //			if (stopped())
            //				break; TODO: stop
            Isotope& x = masslist[k];
            string isotopeName = x.name;
            double isotopeMass = x.mass;
            double expectedAbundance = x.abundance;

            float mzmin = isotopeMass -_mavenParameters->compoundMassCutoffWindow->massCutoffValue(isotopeMass);
            float mzmax = isotopeMass +_mavenParameters->compoundMassCutoffWindow->massCutoffValue(isotopeMass);

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
            //if x.C13>0 then _mavenParameters->C13Labeled_BPE must have been true
            //so we could just eliminate maxNaturalAbundanceErr parameter in this case
            //original idea (see https://github.com/ElucidataInc/ElMaven/issues/43) was to have different checkboxes for "use this element for natural abundance check"
            if ((x.C13 > 0 && _mavenParameters->C13Labeled_BPE == false) //if isotope is not C13Labeled
                    || (x.N15 > 0 && _mavenParameters->N15Labeled_BPE == false) //if isotope is not N15 Labeled
                    || (x.S34 > 0 && _mavenParameters->S34Labeled_BPE == false) //if isotope is not S34 Labeled
                    || (x.H2 > 0 && _mavenParameters->D2Labeled_BPE == false) //if isotope is not D2 Labeled

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
                        _mavenParameters->maxNaturalAbundanceErr)
                    continue;
            }

            //TODO: this is really an abuse of the maxIsotopeScanDiff parameter
            //I can easily imagine you might set maxIsotopeScanDiff to something much less than the peak width
            //here w should really be determined by the minRt and maxRt for the parent and child peaks
            float w = _mavenParameters->maxIsotopeScanDiff
                * _mavenParameters->avgScanTime;
            double c = sample->correlation(
                    isotopeMass, parentgroup->meanMz,
                    _mavenParameters->compoundMassCutoffWindow, rtmin - w,
                    rtmax + w, _mavenParameters->eicType,
                    _mavenParameters->filterline);  // find correlation for isotopes
            if (c < _mavenParameters->minIsotopicCorrelation)
                continue;

            vector<Peak> allPeaks;

            EIC * eic = sample->getEIC(mzmin, mzmax, sample->minRt,sample->maxRt, 1, _mavenParameters->eicType,
                                        _mavenParameters->filterline);
            //actually last parameter should probably be deepest MS level?
            //TODO: decide how isotope children should even work in MS mode

            // smooth fond eic TODO: null check for found
            eic->setSmootherType(
                    (EIC::SmootherType)
                    _mavenParameters->eic_smoothingAlgorithm);
            eic->setBaselineSmoothingWindow(_mavenParameters->baseline_smoothingWindow);
            eic->setBaselineDropTopX(_mavenParameters->baseline_dropTopX);
            eic->setFilterSignalBaselineDiff(_mavenParameters->isotopicMinSignalBaselineDifference);
            eic->getPeakPositions(_mavenParameters->eic_smoothingWindow);
            //TODO: this needs be optimized to not bother finding peaks outside of
            //maxIsotopeScanDiff window
            allPeaks = eic->peaks;

            bool isIsotope = true;
			PeakFiltering peakFiltering(_mavenParameters, isIsotope);
            peakFiltering.filter(allPeaks);

            delete(eic);
            // find nearest peak as long as it is within RT window
            float maxRtDiff=_mavenParameters->maxIsotopeScanDiff * _mavenParameters->avgScanTime;
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
                    g.setSelectedSamples(parentgroup->samples);
                    isotopes[isotopeName] = g;
                }
                isotopes[isotopeName].addPeak(*nearestPeak); //add nearestPeak to isotope peak list
            }
            vector<Peak>().swap(allPeaks);
        }
    }

    addIsotopes(parentgroup, isotopes);
}

void IsotopeDetection::addIsotopes(PeakGroup* parentgroup, map<string, PeakGroup> isotopes)
{

    //fill peak group list with the compound and its isotopes.
    // peak group list would be filled with the parent group, with its isotopes as children
    // click on + to see children == isotopes
    // parentgroup->children.clear();
    map<string, PeakGroup>::iterator itr2;
    for (itr2 = isotopes.begin(); itr2 != isotopes.end(); ++itr2) {
        string isotopeName = (*itr2).first;
        PeakGroup& child = (*itr2).second;
        child.minQuality = _mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        //TODO: isn't this a bug? shouldn't it get a new groupId?
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        if (_mavenParameters->clsf->hasModel()) {
            _mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }


        float rtDiff = -1;

        if (child.compound != NULL && child.compound->expectedRt > 0)
        {
            rtDiff = abs(child.compound->expectedRt - (child.meanRt));
            child.expectedRtDiff = rtDiff;
        }

        double A = (double) _mavenParameters->qualityWeight/10;
        double B = (double) _mavenParameters->intensityWeight/10;
        double C = (double) _mavenParameters->deltaRTWeight/10;

        if (_mavenParameters->deltaRtCheckFlag && child.compound != NULL && child.compound->expectedRt > 0)
        {
            child.groupRank = pow(rtDiff, 2*C) * pow((1.1 - child.maxQuality), A)
                                                  * (1 /( pow(log(child.maxIntensity + 1), B)));
        }
        else
        {
            child.groupRank = pow((1.1 - child.maxQuality), A)
                                                  * (1 /(pow(log(child.maxIntensity + 1), B)));

        }


        if (!_mavenParameters->C13Labeled_BPE) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!_mavenParameters->N15Labeled_BPE) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }

        if (!_mavenParameters->S34Labeled_BPE) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }

        if (!_mavenParameters->D2Labeled_BPE) {
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
        child.minQuality = _mavenParameters->minQuality;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        
        if (_mavenParameters->clsf->hasModel()) {
            _mavenParameters->clsf->classify(&child);
            child.groupStatistics();
        }
        
        if (!_mavenParameters->C13Labeled_IsoWidget) {
            if (isotopeName.find(C13_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!_mavenParameters->N15Labeled_IsoWidget) {
            if (isotopeName.find(N15_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13N15_LABEL) != string::npos)
                continue;
        }
        
        if (!_mavenParameters->S34Labeled_IsoWidget) {
            if (isotopeName.find(S34_LABEL) != string::npos)
                continue;
            else if (isotopeName.find(C13S34_LABEL) != string::npos)
                continue;
        }
        
        if (!_mavenParameters->D2Labeled_IsoWidget) {
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
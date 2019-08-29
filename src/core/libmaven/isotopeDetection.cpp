#include "Compound.h"
#include "classifierNeuralNet.h"
#include "constants.h"
#include "datastructures/adduct.h"
#include "EIC.h"
#include "isotopeDetection.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "Peak.h"
#include "peakFiltering.h"
#include "PeakGroup.h"
#include "Scan.h"

IsotopeDetection::IsotopeDetection(
    MavenParameters *mavenParameters,
    IsotopeDetectionType isoType,
    bool C13Flag,
    bool N15Flag,
    bool S34Flag,
    bool D2Flag)
{
    _mavenParameters = mavenParameters;
    _isoType = isoType;
    _C13Flag = C13Flag;
    _N15Flag = N15Flag;
    _S34Flag = S34Flag;
    _D2Flag = D2Flag;

}

void IsotopeDetection::pullIsotopes(PeakGroup* parentgroup)
{
    // FALSE CONDITIONS
    if (parentgroup == NULL)
        return;
    if (parentgroup->getCompound() == NULL)
        return;
    if (parentgroup->getCompound()->formula().empty() == true)
        return;
    if (parentgroup->getAdduct() != nullptr
        && !parentgroup->getAdduct()->isParent()) {
        return;
    }
    if (_mavenParameters->samples.size() == 0)
        return;

    string formula = parentgroup->getCompound()->formula(); //parent formula
    int charge = _mavenParameters->getCharge(parentgroup->getCompound());//generate isotope list for parent mass

    vector<Isotope> masslist =
        MassCalculator::computeIsotopes(formula,
                                        charge,
                                        _C13Flag,
                                        _N15Flag,
                                        _S34Flag,
                                        _D2Flag,
                                        parentgroup->getAdduct());

    map<string, PeakGroup> isotopes = getIsotopes(parentgroup, masslist);

    addIsotopes(parentgroup, isotopes);

}

map<string, PeakGroup> IsotopeDetection::getIsotopes(PeakGroup* parentgroup, vector<Isotope> masslist)
{
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
            if (parentPeak) {
                rt = parentPeak->rt;
                rtmin = parentPeak->rtmin;
                rtmax = parentPeak->rtmax;
            }

            float isotopePeakIntensity = 0;
            float parentPeakIntensity = 0;

            if (parentPeak) {
                parentPeakIntensity = parentPeak->peakIntensity;
                Scan* scan = parentPeak->getScan();
                std::pair<float, float> isotope = getIntensity(scan, mzmin, mzmax);
                isotopePeakIntensity = isotope.first;
                rt = isotope.second;
            }

            if (isotopePeakIntensity == 0 || rt == 0) continue;

            if (filterIsotope(x, isotopePeakIntensity, parentPeakIntensity, sample, parentgroup))
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

            //Set peak quality
            if (_mavenParameters->clsf->hasModel()) {
                for(Peak& peak: allPeaks)
                    peak.quality = _mavenParameters->clsf->scorePeak(peak);
            }

            //filter isotopic peaks
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
                    g.meanMz = isotopeMass; //This get's updated in groupStatistics function
                    g.expectedMz = isotopeMass;
                    g.tagString = isotopeName;
                    g.expectedAbundance = expectedAbundance;
                    g.isotopeC13count = x.C13;
                    g.setSelectedSamples(parentgroup->samples);

                    // create a slice for this group; RT will be updated later
                    mzSlice childSlice(mzmin,
                                       mzmax,
                                       0.0f,
                                       numeric_limits<float>::max());
                    g.setSlice(childSlice);
                    isotopes[isotopeName] = g;
                }
                isotopes[isotopeName].addPeak(*nearestPeak); //add nearestPeak to isotope peak list
            }
            vector<Peak>().swap(allPeaks);
        }
    }
    return isotopes;
}

bool IsotopeDetection::filterIsotope(Isotope x, float isotopePeakIntensity, float parentPeakIntensity, mzSample* sample, PeakGroup* parentGroup)
{    
    //natural abundance check
    //TODO: I think this loop will never run right? Since we're now only pulling the relevant isotopes
    //if x.C13>0 then _mavenParameters->C13Labeled_BPE must have been true
    //so we could just eliminate maxNaturalAbundanceErr parameter in this case
    //original idea (see https://github.com/ElucidataInc/ElMaven/issues/43) was to have different checkboxes for "use this element for natural abundance check"
    if ((x.C13 > 0 && _C13Flag == false) //if isotope is not C13Labeled
            || (x.N15 > 0 && _N15Flag == false) //if isotope is not N15 Labeled
            || (x.S34 > 0 && _S34Flag == false) //if isotope is not S34 Labeled
            || (x.H2 > 0 && _D2Flag == false) //if isotope is not D2 Labeled
        )
    {
        float expectedAbundance = x.abundance;
        if (expectedAbundance < 1e-8)
            return true;
        if (expectedAbundance * parentPeakIntensity < 1) //TODO: In practice this is probably fine but in general I don't like these types of intensity checks -- the actual absolute value depends on the type of instrument, etc
            return true;
        float observedAbundance = isotopePeakIntensity
            / (parentPeakIntensity + isotopePeakIntensity); //find observedAbundance based on isotopePeakIntensity

        float naturalAbundanceError = abs(
                observedAbundance - expectedAbundance) //if observedAbundance is significant wrt expectedAbundance
            / expectedAbundance * 100; // compute natural Abundance Error

        if (naturalAbundanceError >
                _mavenParameters->maxNaturalAbundanceErr)
            return true;
    }

    //TODO: this is really an abuse of the maxIsotopeScanDiff parameter
    //I can easily imagine you might set maxIsotopeScanDiff to something much less than the peak width
    //here w should really be determined by the minRt and maxRt for the parent and child peaks
    if (parentGroup)
    {
        float isotopeMass = x.mass;
        Compound* compound = parentGroup->getCompound();
        float parentMass = MassCalculator::computeMass(compound->formula(),
                                                       _mavenParameters->getCharge(compound));

        Peak* parentPeak = parentGroup->getPeak(sample);
        float rtmin = parentGroup->minRt;
        float rtmax = parentGroup->maxRt;
        if (parentPeak)
        {
            rtmin = parentPeak->rtmin;
            rtmax = parentPeak->rtmax;
        }
        float w = _mavenParameters->maxIsotopeScanDiff
            * _mavenParameters->avgScanTime;
        double c = sample->correlation(
                isotopeMass, parentMass,
                _mavenParameters->compoundMassCutoffWindow, rtmin - w,
                rtmax + w, _mavenParameters->eicType,
            _mavenParameters->filterline);  // find correlation for isotopes
        if (c < _mavenParameters->minIsotopicCorrelation)
            return true;
    }
    return false;
}

std::pair<float, float> IsotopeDetection::getIntensity(Scan* scan, float mzmin, float mzmax)
{
    float highestIntensity = 0;
    float rt = 0;
    mzSample* sample = scan->getSample();

    for (int i = scan->scannum - 2; i < scan->scannum + 2; i++) {
        Scan* s = sample->getScan(i);

        // Filter out MS2 scans when obtaining isotope peak intensities.
        auto pos = i;
        while(s->mslevel > 1 && i < scan->scannum) {
            auto index = i < scan->scannum ? --pos
                                           : ++pos;
            s = sample->getScan(index);
        }

        vector<int> matches = s->findMatchingMzs(mzmin, mzmax);
        for (auto pos : matches) {
            if (s->intensity[pos] > highestIntensity) {
                highestIntensity = s->intensity[pos];
                rt = s->rt;
            }
        }
    }
    return std::make_pair(highestIntensity, rt);
}

void IsotopeDetection::addIsotopes(PeakGroup* parentgroup, map<string, PeakGroup> isotopes)
{

    map<string, PeakGroup>::iterator itrIsotope;
    unsigned int index = 1;
    for (itrIsotope = isotopes.begin(); itrIsotope != isotopes.end(); ++itrIsotope, index++) {
        string isotopeName = (*itrIsotope).first;
        PeakGroup& child = (*itrIsotope).second;
        child.metaGroupId = index;

        childStatistics(parentgroup, child, isotopeName);
        bool isotopeAdded = filterLabel(isotopeName);
        if (!isotopeAdded) continue;
        
        addChild(parentgroup, child, isotopeName);
    }
}

void IsotopeDetection::addChild(PeakGroup *parentgroup, PeakGroup &child, string isotopeName)
{

    bool childExist;

    switch (_isoType)
    {
        case IsotopeDetectionType::PeakDetection:
            childExist = checkChildExist(parentgroup->children, isotopeName); 
            if (!childExist) parentgroup->addChild(child);
            break;
        case IsotopeDetectionType::IsoWidget:
            childExist = checkChildExist(parentgroup->childrenIsoWidget, isotopeName);
            if (!childExist) parentgroup->addChildIsoWidget(child);
            break;
        case IsotopeDetectionType::BarPlot:
            childExist = checkChildExist(parentgroup->childrenBarPlot, isotopeName);
            if (!childExist) parentgroup->addChildBarPlot(child);
            break;
    }
}

bool IsotopeDetection::checkChildExist(vector<PeakGroup> &children, string isotopeName)
{

    bool childExist = false;
    for (unsigned int ii = 0; ii < children.size(); ii++) {
        if (children[ii].tagString == isotopeName) {
            childExist = true;
        }
    }
    
    return childExist;
}

void IsotopeDetection::childStatistics(
                        PeakGroup* parentgroup,
                        PeakGroup &child,
                        string isotopeName)
{

    child.tagString = isotopeName;
    child.groupId = parentgroup->groupId;
    child.parent = parentgroup;
    child.setType(PeakGroup::GroupType::Isotope);
    child.groupStatistics();

    // we now have the RT limits for the isotopic group
    mzSlice currentSlice = child.getSlice();
    currentSlice.rtmin = child.minRt;
    currentSlice.rtmax = child.maxRt;
    child.setSlice(currentSlice);
    child.setCompound(parentgroup->getCompound());

    bool deltaRtCheckFlag = _mavenParameters->deltaRtCheckFlag;
    float compoundRTWindow = _mavenParameters->compoundRTWindow;
    int qualityWeight = _mavenParameters->qualityWeight;
    int intensityWeight = _mavenParameters->intensityWeight;
    int deltaRTWeight = _mavenParameters->deltaRTWeight;

    child.calGroupRank(deltaRtCheckFlag,
                       qualityWeight,
                       intensityWeight,
                       deltaRTWeight);

}

bool IsotopeDetection::filterLabel(string isotopeName)
{

    if (!_C13Flag) {
        if (isotopeName.find(C13_LABEL) != string::npos)
            return false;
        else if (isotopeName.find(C13N15_LABEL) != string::npos)
            return false;
        else if (isotopeName.find(C13S34_LABEL) != string::npos)
            return false;
    }

    if (!_N15Flag) {
        if (isotopeName.find(N15_LABEL) != string::npos)
            return false;
        else if (isotopeName.find(C13N15_LABEL) != string::npos)
            return false;
    }

    if (!_S34Flag) {
        if (isotopeName.find(S34_LABEL) != string::npos)
            return false;
        else if (isotopeName.find(C13S34_LABEL) != string::npos)
            return false;
    }

    if (!_D2Flag) {
        if (isotopeName.find(H2_LABEL) != string::npos)
            return false;
    }

    return true;

}

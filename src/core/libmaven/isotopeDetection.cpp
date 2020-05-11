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

map<string, PeakGroup> IsotopeDetection::getIsotopes(PeakGroup* parentGroup,
                                                     vector<Isotope> masslist)
{
    map<string, PeakGroup> isotopeGroups;
    for (auto sample : _mavenParameters->samples) {
        for (Isotope& isotope : masslist) {
            string isotopeName = isotope.name;
            float expectedAbundance = static_cast<float>(isotope.abundance);

            float isotopeMass = static_cast<float>(isotope.mass);
            float mzDelta = _mavenParameters->compoundMassCutoffWindow->massCutoffValue(isotopeMass);
            float mzmin = isotopeMass - mzDelta;
            float mzmax = isotopeMass + mzDelta;

            Peak* parentPeak = parentGroup->getPeak(sample);
            // we should not attempt to find isotopic peaks for samples in
            // which there is no parent peak
            if (parentPeak == nullptr)
                continue;

            // even though we will do a peak detection on the isotope's slice
            // later on, this is a good fast check to see if there is anything
            // worth searching for; maybe also for adduct & fragment detection?
            Scan* scan = parentPeak->getScan();
            std::pair<float, float> topMatch = getIntensity(scan, mzmin, mzmax);
            float isotopePeakIntensity = topMatch.first;
            float rt = topMatch.second;

            if (isotopePeakIntensity == 0.0f || rt == 0.0f)
                continue;

            float rtmin = parentGroup->getSlice().rtmin;
            float rtmax = parentGroup->getSlice().rtmax;
            EIC* eic = sample->getEIC(mzmin,
                                      mzmax,
                                      rtmin,
                                      rtmax,
                                      1,
                                      _mavenParameters->eicType,
                                      _mavenParameters->filterline);

            if (eic == nullptr || eic->size() == 0)
                continue;

            eic->setSmootherType(static_cast<EIC::SmootherType>(_mavenParameters->eic_smoothingAlgorithm));
            eic->setBaselineSmoothingWindow(_mavenParameters->baseline_smoothingWindow);
            eic->setBaselineDropTopX(_mavenParameters->baseline_dropTopX);
            eic->setFilterSignalBaselineDiff(_mavenParameters->isotopicMinSignalBaselineDifference);
            eic->getPeakPositions(_mavenParameters->eic_smoothingWindow);
            vector<Peak>& allPeaks = eic->peaks;

            // assign peak quality
            if (_mavenParameters->clsf->hasModel()) {
                for (Peak& peak : allPeaks)
                    peak.quality = _mavenParameters->clsf->scorePeak(peak);
            }

            // filter isotopic peaks
            PeakFiltering peakFiltering(_mavenParameters, true);
            peakFiltering.filter(allPeaks);
            filterIsotopePeaks(parentPeak, allPeaks, sample);

            // find nearest peak as long as it is within RT window
            Peak* nearestPeak = nullptr;
            int dist = numeric_limits<int>::max();
            for (auto& peak : allPeaks) {
                int d = 0;
                int minScan = min(peak.scan, parentPeak->scan);
                int maxScan = max(peak.scan, parentPeak->scan);
                while (minScan != maxScan) {
                    auto scanInBetween = sample->getScan(++minScan);
                    if (scanInBetween->mslevel == 1)
                        ++d;
                }
                if (d > _mavenParameters->maxIsotopeScanDiff)
                    continue;
                if (d < dist) {
                    dist = d;
                    nearestPeak = &peak;
                }
            }

            if (nearestPeak == nullptr) {
                delete eic;
                continue;
            }

            if (_mavenParameters->linkIsotopeRtRange) {
                eic->adjustPeakBounds(*nearestPeak,
                                      parentPeak->rtmin,
                                      parentPeak->rtmax);
                eic->getPeakDetails(*nearestPeak);
            }
            Peak isotopePeak = *nearestPeak;
            delete(eic);

            // label the peak of isotope
            if (isotopeGroups.count(isotopeName) == 0) {
                PeakGroup childGroup;
                // meanMz is updated in group statistics later
                childGroup.meanMz = isotopeMass;
                childGroup.expectedMz = isotopeMass;
                childGroup.tagString = isotopeName;
                childGroup.expectedAbundance = expectedAbundance;
                childGroup.isotopeC13count = isotope.C13;
                childGroup.setSelectedSamples(parentGroup->samples);

                // slice for this child group has the same bounds used for
                // pulling the isotope's EIC
                mzSlice childSlice(mzmin, mzmax, rtmin, rtmax);
                childGroup.setSlice(childSlice);
                childGroup.setCompound(parentGroup->getCompound());
                isotopeGroups[isotopeName] = childGroup;
            }
            isotopeGroups[isotopeName].addPeak(isotopePeak);
        }
    }
    return isotopeGroups;
}

void IsotopeDetection::filterIsotopePeaks(const Peak* parentPeak,
                                          vector<Peak>& isotopePeaks,
                                          mzSample* sample)
{
    if (parentPeak == nullptr)
        return;

    auto badCorrelation = [&] (const Peak& peak) {
        auto corr = sample->correlation(parentPeak->mzmin,
                                        parentPeak->mzmax,
                                        peak.mzmin,
                                        peak.mzmax,
                                        parentPeak->rtmin,
                                        parentPeak->rtmax,
                                        1, // ms level
                                        _mavenParameters->eicType,
                                        _mavenParameters->filterline);
        if (corr < _mavenParameters->minIsotopicCorrelation)
            return true;
        return false;
    };
    isotopePeaks.erase(remove_if(begin(isotopePeaks),
                                 end(isotopePeaks),
                                 badCorrelation),
                       end(isotopePeaks));
}

std::pair<float, float> IsotopeDetection::getIntensity(Scan* scan, float mzmin, float mzmax)
{
    float highestIntensity = 0.0f;
    float rt = 0.0f;
    mzSample* sample = scan->getSample();

    for (int i = scan->scannum - _mavenParameters->maxIsotopeScanDiff;
         i < scan->scannum + _mavenParameters->maxIsotopeScanDiff;
         i++) {
        Scan* s = sample->getScan(i);

        // Filter out MS2 scans when obtaining isotope peak intensities.
        auto pos = i;
        while(s->mslevel > 1) {
            auto previousScan = s;
            auto index = i < scan->scannum ? --pos : ++pos;
            s = sample->getScan(index);

            // maybe we reached the last or the first scan - which is still MS2
            if (s == previousScan)
                break;
        }
        if (s->mslevel > 1)
            continue;

        vector<int> matches = s->findMatchingMzs(mzmin, mzmax);
        for (auto match : matches) {
            if (s->intensity[match] > highestIntensity) {
                highestIntensity = s->intensity[match];
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

    bool deltaRtCheckFlag = _mavenParameters->deltaRtCheckFlag;
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

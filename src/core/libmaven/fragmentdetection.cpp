#include "fragmentdetection.h"
#include "Compound.h"
#include "PeakGroup.h"
#include "RealVector.h"
#include "classifierNeuralNet.h"
#include "masscutofftype.h"
#include "massslicer.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "peakdetector.h"

vector<PeakGroup> FragmentDetection::detectFragmentsUntargeted(
    MavenParameters* parameters,
    float precursorMz,
    float precursorRt,
    float rtDeviationLimit)
{
    if (parameters == nullptr)
        return {};

    auto mp = make_shared<MavenParameters>(*parameters);
    MassSlicer slicing(mp.get());
    slicing.disableSignals = true;

    slicing.setMsLevel(2, precursorMz, precursorRt);
    slicing.clearSlices();
    // TODO: step size - MS/MS slice width should be user adjustable
    slicing.findFeatureSlices(true);
    if (slicing.slices.empty())
        return {};

    // sort the slices based on their intensities to enurmerate good slices
    sort(slicing.slices.begin(), slicing.slices.end(), mzSlice::compIntensity);

    PeakDetector detector;
    detector.setMavenParameters(mp.get());
    detector.processSlices(slicing.slices);
    auto msMsGroups = mp->allgroups;

    // cleanup
    delete_all(slicing.slices);
    return msMsGroups;
}

vector<PeakGroup> FragmentDetection::detectFragmentsTargeted(
    MavenParameters* parameters,
    float precursorMz,
    float precursorRt,
    vector<float> targetFragmentMzs)
{
    if (parameters == nullptr)
        return {};

    vector<mzSlice*> slices;
    MassCutoff massCutoff;
    massCutoff.setMassCutoffAndType(parameters->fragmentTolerance, "ppm");
    for (auto fragmentMz : targetFragmentMzs) {
        float mzDelta = massCutoff.massCutoffValue(fragmentMz);
        float rtDelta = 1.0f;  // TODO: another user adjustable param
        mzSlice* slice = new mzSlice(fragmentMz - mzDelta,
                                     fragmentMz + mzDelta,
                                     precursorRt - rtDelta,
                                     precursorRt + rtDelta);
        slice->precursorMz = precursorMz;
        slices.push_back(slice);
    }

    PeakDetector detector;
    detector.setMavenParameters(parameters);
    detector.processSlices(slices);
    auto msMsGroups = parameters->allgroups;

    // lambda: finds the closest m/z value in `targetFragmentMzs`, for a
    // given target m/z
    auto nearestExpectedMz = [targetFragmentMzs](float actualMz) {
        float nearestMz = 0.0f;
        float leastDiff = numeric_limits<float>::max();
        for (float expectedMz : targetFragmentMzs) {
            float diff = abs(expectedMz - actualMz);
            if (diff < leastDiff) {
                nearestMz = expectedMz;
                leastDiff = diff;
            }
        }
        return nearestMz;
    };
    for (auto& group : msMsGroups)
        group.setExpectedMz(nearestExpectedMz(group.meanMz));

    // cleanup
    delete_all(slices);
    return msMsGroups;
}

void FragmentDetection::findFragments(PeakGroup* precursor)
{
    // TODO: should come from the user
    float fiveSeconds = 5.0f / 60.0f;
    vector<PeakGroup> groups;
    // TODO: group's mean mz and RT should work for most peaks but for those
    // that are further than the delta values, will not be captured.
    if (precursor->hasCompoundLink()
        && !precursor->getCompound()->fragmentMzValues().empty()) {
        auto targetFragmentMzs = precursor->getCompound()->fragmentMzValues();
        groups = detectFragmentsTargeted(precursor->parameters().get(),
                                         precursor->meanMz,
                                         precursor->meanRt,
                                         targetFragmentMzs);
    } else {
        groups = detectFragmentsUntargeted(precursor->parameters().get(),
                                           precursor->meanMz,
                                           precursor->meanRt,
                                           fiveSeconds);
    }

    // filter out fragments that are too far or have very bad quality
    auto fragmentGroups = filterFragments(precursor, groups, fiveSeconds);

    // create a mapping of samples to its fragment peaks for this precursor
    map<mzSample*, vector<Peak*>> fragmentPeaks;
    for (auto& fragmentGroup : fragmentGroups) {
        for (auto& peak : fragmentGroup.peaks) {
            auto peakSample = peak.getSample();
            if (fragmentPeaks.count(peakSample) == 0)
                fragmentPeaks[peakSample] = {};
            fragmentPeaks[peakSample].push_back(&peak);
        }
    }

    // generate fragmentation profiles per sample
    Fragment fragment(Fragment::MsType::DIA);
    for (auto& elem : fragmentPeaks) {
        auto& peaks = elem.second;
        NimbleDSP::RealVector<float> rtValues;
        Fragment* sampleFragmentProfile = new Fragment(Fragment::MsType::DIA);
        for (auto peak : peaks) {
            sampleFragmentProfile->insertFragment(
                peak->baseMz, peak->peakArea, {peak->rtmin, peak->rtmax});
            rtValues.vec.push_back(peak->rt);
        }
        sampleFragmentProfile->obscount = vector<int>(peaks.size(), 1);
        sampleFragmentProfile->rt = rtValues.mean();
        fragment.addBrotherFragment(sampleFragmentProfile);

        // TODO: this is wrong, temporarily using this to evade group fitlering
        ++(precursor->ms2EventCount);
    }

    // build a consensus spectrum across all samples
    fragment.buildConsensus(precursor->parameters()->fragmentTolerance);
    fragment.consensus->sortByMz();
    precursor->fragmentationPattern = fragment.consensus;
    precursor->setFragmentGroups(fragmentGroups);
}

vector<PeakGroup> FragmentDetection::filterFragments(
    const PeakGroup* precursor,
    const vector<PeakGroup> candidates,
    const float rtDeviationLimit)
{
    if (precursor == nullptr || precursor->peaks.empty())
        return {};

    int goodPeakCount = 0;
    map<mzSample*, pair<float, float>> precursorRts;
    for (auto& peak : precursor->peaks) {
        precursorRts[peak.getSample()] =
            make_pair(peak.rt - rtDeviationLimit, peak.rt + rtDeviationLimit);
        if (peak.quality > 0.5f)
            ++goodPeakCount;
    }

    vector<PeakGroup> fragments;
    for (auto& candidateFragment : candidates) {
        int goodFragmentCount = 0;
        int proximateFragmentCount = 0;
        for (auto& peak : candidateFragment.peaks) {
            auto peakSample = peak.getSample();
            if (precursorRts.count(peakSample)
                && isBetweenInclusive(peak.rt,
                                      precursorRts[peakSample].first,
                                      precursorRts[peakSample].second)) {
                ++proximateFragmentCount;
            }
            if (peak.quality >= 0.5)
                ++goodFragmentCount;
        }

        // TODO: if [X] percent or more peaks satisfy proximity and quality in
        // comparison to the precursor peaks then we allow this group, because
        // there might be a few low intensity peaks that which might have random
        // peak tops or bad peak quality
        float ratioProximate = static_cast<float>(proximateFragmentCount)
                               / static_cast<float>(precursor->peaks.size());
        float ratioGood = goodPeakCount > 0
                              ? static_cast<float>(goodFragmentCount)
                                    / static_cast<float>(goodPeakCount)
                              : 1.0f;
        if (ratioProximate >= 0.6f && ratioGood >= 0.3f)
            fragments.push_back(candidateFragment);
    }

    return fragments;
}

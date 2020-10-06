#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H

#include <boost/signals2.hpp>

class Adduct;
class Compound;
class EIC;
class MavenParameters;
class mzSample;
class mzSlice;
class PeakGroup;
class ClassifierNeuralNet;

/**
 * @brief The PeakDetector provides function with all the logic for peak
 * detection (calling other independent classes as well).
 */
class PeakDetector
{
public:
    boost::signals2::signal<void(const std::string&, unsigned int, int)>
        boostSignal;

    PeakDetector();
    PeakDetector(MavenParameters* mp);

    // copy constructor
    PeakDetector(const PeakDetector& obj)
    {
        _mavenParameters = obj._mavenParameters;
    }

    // assignment constructor
    PeakDetector& operator=(const PeakDetector& obj)
    {
        _mavenParameters = obj._mavenParameters;
        return *this;
    }

    static std::vector<EIC*> pullEICs(const mzSlice* slice,
                                      const std::vector<mzSample*>& samples,
                                      const MavenParameters* mp,
                                      bool filterUnselectedSamples = true);

    /**
     * @brief Set the RT bounds of the peak, of the given peak-group, for the
     * given sample.
     * @param group Pointer to the peak-group whose peak bounds will be edited.
     * @param peakSample Pointer to an `mzSample` object.
     * @param eics A vector of EICs which used for extracting peak region. This
     * would not be strictly needed since the `peakSample`'s EIC can be pulled.
     * Even so, having this as an argument allows the caller to store all EICs
     * from a potentially parallelized fetch instead of fetching them one at a
     * time (whenever this method is called).
     * @param rtMin Minimum retention time (left bound) to set for the peak.
     * @param rtMax Maximum retention time (right bound) to set for the peak.
     * @param clsf A pointer to a classifier, which will be used for assigning
     * quality score to edited peak.
     */
    static void editPeakRegionForSample(PeakGroup* group,
                                        mzSample* peakSample,
                                        std::vector<EIC*>& eics,
                                        float rtMin,
                                        float rtMax,
                                        ClassifierNeuralNet* clsf);

    /**
     * @brief For a vector of EIC objects, integrate the region between `rtMin`
     * and `rtMax` as a peak-group object.
     * @param eics The EIC from which peak data will be extracted.
     * @param rtMin The lower limit of retention time boundary.
     * @param rtMax The upper limit of retention time boundary.
     * @param slice The slice to be set for integrated group. Should be the
     * slice from which `eics` was also created.
     * @param samples The currently visible set of samples to be set for the
     * integrated peak-group.
     * @param mp A parameters object used for values set for current settings.
     * A frozen copy of parameters will also be added to the integrated
     * peak-group.
     * @param clsf The neural-net classifier used for assigning peak quality to
     * integrated peaks.
     * @param isIsotope Flag used to tell peak-filtering scheme whether it
     * should consider each peak as that of an isotope or a regular peak.
     * @return An newly created `PeakGroup` object for the integrated region.
     */
    static std::shared_ptr<PeakGroup>
    integrateEicRegion(const std::vector<EIC*>& eics,
                       float rtMin,
                       float rtMax,
                       const mzSlice slice,
                       const std::vector<mzSample*>& samples,
                       const MavenParameters* mp,
                       ClassifierNeuralNet* clsf,
                       bool isIsotope);

    void sendBoostSignal(const std::string& progressText,
                         unsigned int completed_slices,
                         int total_slices);

    void resetProgressBar();

    void setFWHMforGroups();

    MavenParameters* mavenParameters() { return _mavenParameters; }
    void setMavenParameters(MavenParameters* mp) { _mavenParameters = mp; }

    /**
     * @brief This method detects features using data slicing techniques.
     * @param identificationSet A collection of target compounds that can be
     * used to identify the untargeted features, once detected.
     */
    void processFeatures(const std::vector<Compound*>& identificationSet = {});

    /**
     * @brief This method can be used to identify features found by performing
     * untargeted detection.
     * @details Untargeted groups, once curated, may have to be identified
     * downstream. If the users suspects that certain compounds already exist
     * within their samples, they can simply replace the corresponding features
     * with targeted groups for these compounds.
     * @param identificationSet A vector of compounds which will be used as
     * known targets for identification.
     */
    void identifyFeatures(const std::vector<Compound*>& identificationSet);

    /**
     * @brief Perform peak-detection for a given set of compounds.
     * @param compounds A vector of compounds for which slices should be found.
     */
    void processCompounds(std::vector<Compound*> compounds,
                          bool applyGroupFilters = true,
                          bool findBarplotIsotopes = false);

    /**
     * @brief For each slice in the given slice vector, perform peak detection.
     * @param slices Reference to a vector of pointers to mzSlice.
     * @param setName A string denoting the type of peaks being detected. These
     * names will be emitted with the detection progress signals.
     * @param applyGroupFilters Whether to apply group-level thresholds on each
     * detected parent group or not.
     */
    void processSlices(std::vector<mzSlice*>& slices,
                       std::string setName,
                       bool applyGroupFilters = true,
                       bool appendNewGroups = false);

    void performMetaGrouping(bool applyGroupFilters = true,
                             bool barplotIsotopes = false);

    /**
     * @brief Given a parent isotopic group (unlabelled metabolite), find all
     * isotopologues according to the current detection scheme.
     * @details For manually integrated parent peak-groups, if "link RT range"
     * among isotopologues is active, no sophisticated detection+grouping will
     * happen. All isotopic peaks for all samples will be integrated using the
     * parent's RT ranges for those respective samples. Filters against parent
     * (if opted for) will still apply regardless of the integration mode.
     * @param parentGroup Peak-group for the unlabelled form of a metabolite,
     * whose labelled forms are desired.
     * @param findBarplotIsotopes If `true`, any isotopes detected will be
     * added to the parent peak-group's `childIsotopesBarPlot` vector.
     */
    void detectIsotopesForParent(PeakGroup& parentGroup,
                                 bool findBarplotIsotopes = false);

    void linkParentIsotopeRange(PeakGroup& parentGroup,
                                bool findBarplotIsotopes = false);

private:
    MavenParameters* _mavenParameters;
    bool _zeroStatus;
};

#endif  // PEAKDETECTOR_H

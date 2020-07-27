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

    void sendBoostSignal(const std::string& progressText,
                         unsigned int completed_slices,
                         int total_slices);

    void resetProgressBar();

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
                          bool findBarplotIsotopes = false);

    /**
     * @brief For each slice in the given slice vector, perform peak detection.
     * @param slices Reference to a vector of pointers to mzSlice.
     * @param setName A string denoting the type of peaks being detected. These
     * names will be emitted with the detection progress signals.
     */
    void processSlices(std::vector<mzSlice*>& slices, std::string setName);

    void performMetaGrouping();
    void linkParentIsotopeRange(PeakGroup& parentGroup);

private:
    MavenParameters* _mavenParameters;
    bool _zeroStatus;
};

#endif  // PEAKDETECTOR_H

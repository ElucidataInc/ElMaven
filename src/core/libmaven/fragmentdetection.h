#ifndef FRAGMENTDETECTION_H
#define FRAGMENTDETECTION_H

#include "standardincludes.h"

class MavenParameters;
class PeakDetector;
class PeakGroup;

namespace FragmentDetection
{
    using namespace std;

    /**
     * @brief Search for features and perform peak detection using DIA MS2 scans
     * for a given precursor m/z and RT, in an untargeted fashion.
     * @param parameters A `MavenParameters` object that will be used for
     * detection settings and populating peak-groups.
     * @param precursorMz The m/z value of the precursor ion for which fragment
     * groups will be detected.
     * @param precursorRt The RT value of the precursor ion for which fragment
     * groups will be detected.
     * @param rtDeviationLimit The maximum limit allowed for difference in
     * fragment tops and their precursor's RT. While filtering will need to be
     * done once fragment groups have been found, this parameter will be used
     * for narrowing down the region used for doing untargeted fragment
     * searching.
     * @return A vector of PeakGroup objects that were detected. Additional
     * filtering might need to be done to get the best matching fragment groups.
     */
    vector<PeakGroup>
    detectFragmentsUntargeted(MavenParameters *parameters,
                              float precursorMz,
                              float precursorRt,
                              float rtDeviationLimit);

    /**
     * @brief Extract and perform peak detection on slices created using DIA MS2
     * scans for a given precursor m/z and RT, in a targeted fashion.
     * @param parameters A `MavenParameters` object that will be used for
     * detection settings and populating peak-groups.
     * @param precursorMz The m/z value of the precursor ion for which fragment
     * groups will be detected.
     * @param precursorRt The RT value of the precursor ion for which fragment
     * groups will be detected.
     * @param targetFragmentMzs A vector of m/z values where fragments for a
     * precursor are expected to be.
     * @return A vector of PeakGroup objects that were detected. Additional
     * filtering might need to be done to get the best matching fragment groups.
     */
    vector<PeakGroup>
    detectFragmentsTargeted(MavenParameters* parameters,
                            float precursorMz,
                            float precursorRt,
                            vector<float> targetFragmentMzs);

    /**
     * @brief Perform peak detection at MS/MS level looking for specific
     * fragments if the group's compound has fragment information, otherwise
     * searching for any fragments in the MS/MS scan space.
     * @param precursor A peak-group whose fragment groups need to be detected.
     * The object's relevant attributes are modified, namely `fragmentGroups`
     * and `fragmentationPattern`.
     */
    void findFragments(PeakGroup* precursor);

    /**
     * @brief Given a precursor and a vector of possible fragments, filters out
     * those fragments that are either too far from the precursor peaks or those
     * that have significantly worse quality compared to precursor peaks.
     * @param precursor Pointer to the precursor `PeakGroup` object.
     * @param candidates A vector of `PeakGroup` objects that need to be
     * filtered for high quality matches.
     * @param rtDeviationLimit The limit allowed on RT drift of fragment peaks
     * from their precursor peaks.
     * @return A filtered form of given fragment group candidates, that can now
     * confidently be called fragment groups of the precursor group.
     */
    vector<PeakGroup> filterFragments(const PeakGroup* precursor,
                                      const vector<PeakGroup> candidates,
                                      const float rtDeviationLimit);
};

#endif // FRAGMENTDETECTION_H

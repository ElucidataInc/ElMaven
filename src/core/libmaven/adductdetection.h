#ifndef ADDUCTDETECTION_H
#define ADDUCTDETECTION_H

#include "standardincludes.h"
#include "PeakGroup.h"

class MavenParameters;
class PeakDetector;

namespace AdductDetection
{
    /**
     * @brief Generates an `mzSlice` that is sized according to the given
     * compound and adduct.
     * @details The compound's neutral mass is adjusted to its adductized form.
     * If the compound's neutral mass is unavailable, an empty slice will be
     * returned.
     * @param compound A compound object.
     * @param adduct An adduct object.
     * @param mp A `MavenParametes` object whose settings will be used for
     * computing bounds of the slice.
     * @return Pointer to an `mzSlice` object.
     */
    mzSlice* createSliceForCompoundAdduct(Compound* compound,
                                          Adduct* adduct,
                                          MavenParameters* mp);

    /**
     * @brief Given a vector of parent-ion peak-groups, find adducts that may
     * have arisen from each group. Adducts of the parent-ion group will be
     * ignored, to avoid duplication.
     * @param parentIons A vector of `PeakGroup` objects, already known to
     * exist.
     * @param adductsList A vector of `Adduct` objects each of which will be
     * used to create slices and detect peaks in them.
     * @param detector A `PeakDetector` object which will be used for detection.
     * @return A vector of peak-groups containing a conjunction of parent-ions
     * provided and any new adduct groups that were found.
     */
    vector<PeakGroup> findAdducts(const vector<PeakGroup>& parentIons,
                                  const vector<Adduct*>& adductsList,
                                  PeakDetector* detector);

    /**
     * @brief Filter any non-parent peakgroups, that do not lie near any parent
     * groups or are not correlated to them.
     * @param groups A reference to a vector of `PeakGroup` objects, which will
     * will be filtered in-place.
     * @param mp A `MavenParameters` object, whose settings will be used while
     * filtering.
     */
    void filterAdducts(vector<PeakGroup>& groups, MavenParameters* mp);
}

#endif // ADDUCTDETECTION_H

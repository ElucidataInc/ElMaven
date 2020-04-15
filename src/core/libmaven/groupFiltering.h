#ifndef GROUPFILTERING_H
#define GROUPFILTERING_H

#include "classifierNeuralNet.h"
#include "standardincludes.h"

class MavenParameters;
class PeakGroup;
class mzSlice;

using namespace std;

class GroupFiltering
{

public:
    enum class FilterType {
       Rank,
       MzRt,
       MsMsScore,
    };

	/**
	 * @brief Constructor of class GroupFiltering
	 * @param mavenParameters Pointer to class MavenParameters
	 * @see MavenParameters
	 * @see PeakGroup
	 */
    GroupFiltering(MavenParameters *mavenParameters);

	/**
	 * @brief Constructor of class GroupFiltering
	 * @param mavenParameters Pointer to class MavenParameters
	 * @param slice Pointer to class mzSlice
	 * @see MavenParameters
	 * @see PeakGroup
	 * @see mzSlice
	 */
    GroupFiltering(MavenParameters *mavenParameters, mzSlice* slice);

    void filter(vector<PeakGroup> &peakgroups);

		bool filterByMS1(PeakGroup &peakgroup);

		bool filterByMS2(PeakGroup& peakgroup);

    /**
     * @brief Filter out peak-groups that have some same property but some of
     * them are better than others, in that aspect.
     * @details The different filter modes do very different things.
     *
     * When the filter is set to `FilterType::Rank`, then all groups are sorted
     * based on their rank and then all but the top `limit` number of groups are
     * filtered out.
     *
     * When the filter is set to `FilterType::MzRt`, all groups that share the
     * same expected values for m/z are clustered. Each cluster might contain
     * groups from different compounds (but having the same m/z value). Then,
     * each cluster is further divided into sub-clusters of groups that fall on
     * the same RT (with an error of ± 2 seconds). Finally, within each
     * sub-cluster expected m/z and RT filtering criteria are re-evaluated to
     * find the top `limit` number of groups that are closest to the expected.
     * The rest are filtered out. In case, expected RT value is missing for some
     * groups but not all then only those having the expected RT values are
     * evaluated and the rest are filtered indiscriminately. If all groups
     * within the sub-clusters are missing expected RT values then only m/z is
     * used for evaluation and filtering. See GitHub issue #1262.
     *
     * When the filter is set to `FilterType::MsMsScore`, then all groups that
     * have the same (original) compound name are clustered. Then, only the top
     * `limit` groups having the best MS/MS fragmentation score are preserved,
     * for each cluster. See GitHub issue #1276.
     * @param groups The vector of `PeakGroup` objects that need to be filtered.
     * @param filter A `FilterType` to be used for elimination.
     * @param limit The number of best groups that should be allowed to pass.
     */
    void filterAllButSome(vector<PeakGroup>& groups,
                          FilterType filter,
                          int limit);

    /**
	 * [apply peak selection filters to group; if x percentage of peaks in the group are above the user input threshold for a parameter, do not reject the group]
	 * @method quantileFilters
	 * @param  group        [pointer to PeakGroup]
	 * @return [True if group has to be rejected, else False]
	 */
	bool quantileFilters(PeakGroup *group);

  private:
		mzSlice *_slice;
    MavenParameters *_mavenParameters;

};

#endif //GROUPFILTERING_H

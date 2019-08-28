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
         * @brief Filter peak groups containing adducts that do not match some
         * necessary criteria for adducts.
         * @param groups A vector of `PeakGroup` objects to be filtered.
         */
        void filterAdducts(vector<PeakGroup>& groups);

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

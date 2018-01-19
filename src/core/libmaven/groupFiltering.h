#ifndef GROUPFILTERING_H
#define GROUPFILTERING_H

#include <iostream>

#include "mavenparameters.h"
#include "PeakGroup.h"

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

    vector<PeakGroup*> groupFiltering(vector<PeakGroup> &peakgroups, mzSlice* slice);

	/**
	 * [apply peak selection filters to group; if x percentage of peaks in the group are above the user input threshold for a parameter, do not reject the group]
	 * @method quantileFilters
	 * @param  group        [pointer to PeakGroup]
	 * @return [True if group has to be rejected, else False]
	 */
	bool quantileFilters(PeakGroup *group);

  private:
    MavenParameters *_mavenParameters;

};

#endif //GROUPFILTERING_H
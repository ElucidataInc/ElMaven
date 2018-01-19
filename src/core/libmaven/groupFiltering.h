#ifndef GROUPFILTERING_H
#define GROUPFILTERING_H

#include <iostream>

#include "mavenparameters.h"

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

  private:
    MavenParameters *_mavenParameters;

};

#endif //GROUPFILTERING_H
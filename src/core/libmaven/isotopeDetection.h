#ifndef ISOTOPEDETECTION_H
#define ISOTOPEDETECTION_H

#include <iostream>

#include "Peak.h"
#include "mzSample.h"
#include "PeakGroup.h"
#include "constants.h"
#include "peakFiltering.h"
#include "mavenparameters.h"

using namespace std;

class IsotopeDetection
{
  public:
	IsotopeDetection(MavenParameters *mavenParameters);

	/**
	 * [pull Isotopes for metabolites]
	 * @method pullIsotopes
	 * @param  group        [group]
	 */
	void pullIsotopes(PeakGroup *group);

	void pullIsotopesBarPlot(PeakGroup *group);

  private:
	MavenParameters *_mavenParameters;
	map<string, PeakGroup> getIsotopes(PeakGroup* parentgroup, vector<Isotope> masslist);
	void addIsotopes(PeakGroup *parentgroup, map<string, PeakGroup> isotopes);
	bool addIsotopes(PeakGroup* parentgroup,
	                  PeakGroup &child,
    				  string isotopeName,
					  bool C13Flag,
					  bool N15Flag,
					  bool S34Flag,
					  bool D2Flag);
};

#endif // ISOTOPEDETECTION_H
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
};

#endif // ISOTOPEDETECTION_H
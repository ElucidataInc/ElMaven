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
	enum IsotopeDetectionType {PeakDetection=0, IsoWidget=1, BarPlot=2};
	IsotopeDetection(
    	MavenParameters *mavenParameters,
    	IsotopeDetectionType isoType);

	/**
	 * [pull Isotopes for metabolites]
	 * @method pullIsotopes
	 * @param  group        [group]
	 */
	void pullIsotopes(PeakGroup *group,
					  bool C13Flag,
					  bool N15Flag,
					  bool S34Flag,
					  bool D2Flag);

	void pullIsotopesBarPlot(PeakGroup *group);

  private:
	MavenParameters *_mavenParameters;
	IsotopeDetectionType _isoType;
	map<string, PeakGroup> getIsotopes(PeakGroup* parentgroup,
									   vector<Isotope> masslist,
									   bool C13Flag,
									   bool N15Flag,
									   bool S34Flag,
									   bool D2Flag);
	void addIsotopes(PeakGroup *parentgroup,
					 map<string, PeakGroup> isotopes,
					bool C13Flag,
					bool N15Flag,
					bool S34Flag,
					bool D2Flag);

	void childStatistics(PeakGroup* parentgroup,
	     				 PeakGroup &child,
						 string isotopeName);

	bool filterLabel(
				string isotopeName,
				bool C13Flag,
				bool N15Flag,
				bool S34Flag,
				bool D2Flag);

	void addChild(PeakGroup *parentgroup, PeakGroup &child);
};

#endif // ISOTOPEDETECTION_H
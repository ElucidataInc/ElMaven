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
    	IsotopeDetectionType isoType,
		bool C13Flag,
		bool N15Flag,
		bool S34Flag,
		bool D2Flag);

	void pullIsotopes(PeakGroup *group);
	bool filterIsotope(Isotope x, bool C13Flag, bool N15Flag, bool S34Flag, bool D2Flag, float parentPeakIntensity, float isotopePeakIntensity, mzSample* sample, PeakGroup* parentGroup = NULL);

  private:
	bool _C13Flag;
	bool _N15Flag;
	bool _S34Flag;
	bool _D2Flag;
	MavenParameters *_mavenParameters;
	IsotopeDetectionType _isoType;

	map<string, PeakGroup> getIsotopes(PeakGroup* parentgroup, vector<Isotope> masslist);
	void addIsotopes(PeakGroup *parentgroup, map<string, PeakGroup> isotopes);
	void childStatistics(PeakGroup* parentgroup, PeakGroup &child, string isotopeName);
	bool filterLabel(string isotopeName);
	void addChild(PeakGroup *parentgroup, PeakGroup &child, string isotopeName);
	bool checkChildExist(vector<PeakGroup> &children, string isotopeName);

};

#endif // ISOTOPEDETECTION_H
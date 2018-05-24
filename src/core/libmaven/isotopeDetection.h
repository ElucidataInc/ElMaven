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
	map<string, PeakGroup> getIsotopes(PeakGroup* parentgroup, vector<Isotope> masslist);

	/**
	* @brief find highest intensity for given m/z and scan ranges
	* @return pair of two values. Intensity and rt at which given intensity was found
	**/
	std::pair<float, float> getIntensity(Scan* scan, float mzmin, float mzmax);
	
	/**
	 * @brief checks for natural abundance error and parent-isotope correlation
	 * @details if parent group is not available, correlation check if skipped
	 * @return bool. true if isotope has to be skipped. false if it passes the checks
	 **/
	bool filterIsotope(Isotope x, float isotopePeakIntensity, float parentPeakIntensity, mzSample* sample, PeakGroup* parentGroup = NULL);

  private:
	bool _C13Flag;
	bool _N15Flag;
	bool _S34Flag;
	bool _D2Flag;
	MavenParameters *_mavenParameters;
	IsotopeDetectionType _isoType;

	void addIsotopes(PeakGroup *parentgroup, map<string, PeakGroup> isotopes);
	void childStatistics(PeakGroup* parentgroup, PeakGroup &child, string isotopeName);
	bool filterLabel(string isotopeName);
	void addChild(PeakGroup *parentgroup, PeakGroup &child, string isotopeName);
	bool checkChildExist(vector<PeakGroup> &children, string isotopeName);
	void sortIsotopes(PeakGroup *group);

};

#endif // ISOTOPEDETECTION_H
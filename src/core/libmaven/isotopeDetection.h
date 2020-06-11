#ifndef ISOTOPEDETECTION_H
#define ISOTOPEDETECTION_H

#include "standardincludes.h"

class Isotope;
class MavenParameters;
class mzSample;
class PeakGroup;
class Scan;
class Peak;

using namespace std;

class IsotopeDetection
{
  public:
    enum IsotopeDetectionType {PeakDetection=0, BarPlot=2};

	IsotopeDetection(
    	MavenParameters *mavenParameters,
    	IsotopeDetectionType isoType,
		bool C13Flag,
		bool N15Flag,
		bool S34Flag,
		bool D2Flag);

	void pullIsotopes(PeakGroup *group);
    map<string, PeakGroup> getIsotopes(PeakGroup* parentGroup, vector<Isotope> masslist);

	/**
	* @brief find highest intensity for given m/z and scan ranges
	* @return pair of two values. Intensity and rt at which given intensity was found
	**/
	std::pair<float, float> getIntensity(Scan* scan, float mzmin, float mzmax);

    /**
     * @brief Checks whether parent-isotope signal correlation is above the
     * minimum threshold.
     * @param parentPeak Pointer to the parent peak whose signal will be used to
     * check for isotope correlation.
     * @param isotopeMzMin The min bound on m/z range for the isotope signal.
     * @param isotopeMzMax The max bound on m/z range for the isotope signal.
     * @param sample The sample whose parent-isotope chromatogram signals will
     * be compared.
     * @return `true` if the calculated correlation is above the expected
     * threshold, `false` otherwise.
     **/
    bool satisfiesCorrelation(const Peak* parentPeak,
                              float isotopeMzMin,
                              float isotopeMzMax,
                              mzSample* sample);

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
    bool checkChildExists(vector<shared_ptr<PeakGroup> > children,
                          string isotopeName);
};

#endif // ISOTOPEDETECTION_H

#ifndef ISOTOPELOGIC_H
#define ISOTOPELOGIC_H

#include <string>
#include <vector>

#include "Compound.h"
#include "mzMassCalculator.h"
#include "mzSample.h"

class IsotopeLogic {
public:
	IsotopeLogic();
	void computeIsotopes(string f, double ppm, double maxNaturalAbundanceErr, bool C13Labeled, bool N15Labeled, 
		bool S34Labeled, bool D2Labeled, map<string, bool> isotopeAtom, int noOfIsotopes);
	void userChangedFormula();

	string _formula;
	float _charge;
	vector<mzLink> links;
	Scan* _scan;
	PeakGroup* _group;
	Compound* _compound;
	MassCalculator mcalc;
	Compound* tempCompound;

private:
	float getIsotopeIntensity(float mz, double ppm);
};

#endif // ISOTOPELOGIC_H

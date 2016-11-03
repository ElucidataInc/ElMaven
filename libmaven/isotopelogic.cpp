#include "isotopelogic.h"

IsotopeLogic::IsotopeLogic() {
	_scan = NULL;
	_group = NULL;
	_compound = NULL;
	_charge = -1;
	tempCompound = new Compound("Unknown", "Unknown", string(), 0);	//temp compound
}
void IsotopeLogic::userChangedFormula() {

	_group = NULL;

	tempCompound->formula = _formula;
	tempCompound->name = "Unknown_" + _formula;
	tempCompound->id = "unknown";
	_compound = tempCompound;
}

float IsotopeLogic::getIsotopeIntensity(float mz, double ppm) {
	float highestIntensity = 0;

	if (_scan == NULL)
		return 0;
	mzSample* sample = _scan->getSample();
	if (sample == NULL)
		return 0;

	for (int i = _scan->scannum - 2; i < _scan->scannum + 2; i++) {
		Scan* s = sample->getScan(i);
		vector<int> matches = s->findMatchingMzs(mz - mz / 1e6 * ppm,
				mz + mz / 1e6 * ppm);
		for (int i = 0; i < matches.size(); i++) {
			int pos = matches[i];
			if (s->intensity[pos] > highestIntensity)
				highestIntensity = s->intensity[pos];
		}
	}
	return highestIntensity;
}

void IsotopeLogic::computeIsotopes(string f, double ppm,
		double maxNaturalAbundanceErr, bool C13Labeled, bool N15Labeled,
		bool S34Labeled, bool D2Labeled) {

	double parentMass = MassCalculator::computeMass(f, _charge);
	float parentPeakIntensity = getIsotopeIntensity(parentMass, ppm);

	vector<Isotope> isotopes = MassCalculator::computeIsotopes(f, _charge);
	for (int i = 0; i < isotopes.size(); i++) {
		Isotope& x = isotopes[i];

		float expectedAbundance = x.abundance;

		mzLink link;
		if ((x.C13 > 0 && C13Labeled == false)
				|| (x.N15 > 0 && N15Labeled == false)
				|| (x.S34 > 0 && S34Labeled == false)
				|| (x.H2 > 0 && D2Labeled == false)) {

			if (expectedAbundance < 1e-8)
				continue;
			// if (expectedAbundance * parentPeakIntensity < 500) continue;
			float isotopePeakIntensity = getIsotopeIntensity(x.mass, ppm);

			float observedAbundance = isotopePeakIntensity
					/ (parentPeakIntensity + isotopePeakIntensity);
			float naturalAbundanceError = abs(
					observedAbundance - expectedAbundance) / expectedAbundance
					* 100;

			if (naturalAbundanceError > maxNaturalAbundanceErr)
				continue;
		}
		link.mz1 = parentMass;
		link.mz2 = x.mass;
		link.note = x.name;
		link.value1 = x.abundance;
		link.value2 = getIsotopeIntensity(x.mass, ppm);
		links.push_back(link);
	}

	sort(links.begin(), links.end(), mzLink::compMz);
	_scan = NULL;
}

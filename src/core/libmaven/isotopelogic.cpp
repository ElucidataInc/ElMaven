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

float IsotopeLogic::getIsotopeIntensity(float mz, MassCutoff *massCutoff) {
	float highestIntensity = 0;

	if (_scan == NULL)
		return 0;
	mzSample* sample = _scan->getSample();
	if (sample == NULL)
		return 0;

	for (int i = _scan->scannum - 2; i < _scan->scannum + 2; i++) {
		Scan* s = sample->getScan(i);
		vector<int> matches = s->findMatchingMzs(mz - massCutoff->massCutoffValue(mz),
				mz + massCutoff->massCutoffValue(mz));
		for (unsigned int i = 0; i < matches.size(); i++) {
			int pos = matches[i];
			if (s->intensity[pos] > highestIntensity)
				highestIntensity = s->intensity[pos];
		}
	}
	return highestIntensity;
}

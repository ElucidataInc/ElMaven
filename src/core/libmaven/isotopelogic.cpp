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


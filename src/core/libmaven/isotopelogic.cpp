#include "Compound.h"
#include "isotopelogic.h"
#include "mzMassCalculator.h"
#include "mzSample.h"

IsotopeLogic::IsotopeLogic() {
	_scan = NULL;
	_group = NULL;
	_compound = NULL;
	_charge = -1;
	tempCompound = new Compound("Unknown", "Unknown", string(), 0);	//temp compound
}
void IsotopeLogic::userChangedFormula() {

	_group = NULL;

    tempCompound->setFormula  (_formula);
    tempCompound->setName  ("Unknown_" + _formula);
    tempCompound->setId  ("unknown");
	_compound = tempCompound;
}


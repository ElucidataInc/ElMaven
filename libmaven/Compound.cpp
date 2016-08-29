#include "mzSample.h"

Compound::Compound(string id, string name, string formula, int charge ) {
		this->id = id;
		this->name = name;
		this->formula = formula;
		this->charge = charge;
        this->mass =  MassCalculator::computeNeutralMass(formula);
		this->expectedRt = -1;

		precursorMz=0;
		productMz=0;
		collisionEnergy=0;
		_groupUnlinked=false;
}

float Compound::ajustedMass(int charge) { 
	return MassCalculator::computeMass(formula,charge); 
}


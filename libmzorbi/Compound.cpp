#include "mzSample.h"

Compound::Compound(string id, string name, string formula, int charge ) {
	this->id = id;
	this->name = name;
	this->formula = formula;
	this->charge = charge;
	this->mass =  mcalc->computeMass(formula);
	this->massDelta = FLT_MAX;
	this->expectedRt = -1;

	precursorMz = 0;
	productMz = 0;
	collisionEnergy = 0;
	_groupUnlinked = false;
}

MassCalculator* Compound::mcalc = new MassCalculator();

float Compound::ajustedMass(int charge) {
	return Compound::mcalc->computeMass(formula, charge);
}


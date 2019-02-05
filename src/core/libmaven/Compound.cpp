#include "Compound.h"
#include "mzMassCalculator.h"

Compound::Compound(string id, string name, string formula, int charge ) {
    this->id = id;
    this->name = name;
    this->formula = formula;
    this->charge = charge;
    /**
    *@brief  -   calculate mass of compound by its formula and assign it to mass
    *@see  - double MassCalculator::computeNeutralMass(string formula) in mzMassCalculator.cpp
    */
    this->mass =  MassCalculator::computeNeutralMass(formula);
    this->expectedRt = -1;
    this->logP = 0;

    precursorMz=0;
    productMz=0;
    collisionEnergy=0;
    _groupUnlinked=false;

    virtualFragmentation = false;
    isDecoy = false;
    ionizationMode = 0;
}

float Compound::adjustedMass(int charge) { 
     /**   
    *@return    -    total mass by formula minus loss of electrons' mass 
    *@see  -  double MassCalculator::computeMass(string formula, int charge) in mzMassCalculator.cpp
    */
    return MassCalculator::computeMass(formula,charge);
}

Compound::Type Compound::type() {
    bool hasFragMzs = fragmentMzValues.size() > 0;
    bool hasFragInts = fragmentIntensities.size() == fragmentMzValues.size();
    if (hasFragMzs && hasFragInts)
        return Type::PRM;

    bool hasPrecursorMz = precursorMz > 0;
    bool hasProductMz = productMz > 0;
    if (hasPrecursorMz && hasProductMz)
        return Type::MRM;

    // Is this the only requirement for being usable as an MS1 compound?
    if (mass)
        return Type::MS1;

    return Type::UNKNOWN;
}

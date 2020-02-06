#include "Compound.h"
#include "constants.h"
#include "mzMassCalculator.h"
#include "mzUtils.h"

using namespace mzUtils;

Compound::Compound(string id, string name, string formula, int charge ) {
    this->id = id;
    this->name = name;
    this->setFormula(formula);
    this->charge = charge;
    /**
    *@brief  -   calculate mass of compound by its formula and assign it to mass
    *@see  - double MassCalculator::computeNeutralMass(string formula) in mzMassCalculator.cpp
    */
    this->mass =  MassCalculator::computeNeutralMass(_formula);
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

bool Compound::operator == (const Compound& rhs) const
{
    return (id == rhs.id
            && name == rhs.name
            && formula() == rhs.formula()
            && kegg_id == rhs.kegg_id
            && pubchem_id == rhs.pubchem_id
            && hmdb_id == rhs.hmdb_id
            && alias == rhs.alias
            && smileString == rhs.smileString
            && adductString == rhs.adductString
            && srmId == rhs.srmId
            && almostEqual(expectedRt, rhs.expectedRt)
            && charge == rhs.charge
            && almostEqual(mass, rhs.mass)
            && method_id == rhs.method_id
            && almostEqual(precursorMz, rhs.precursorMz)
            && almostEqual(productMz, rhs.productMz)
            && almostEqual(collisionEnergy, rhs.collisionEnergy)
            && almostEqual(logP, rhs.logP)
            && virtualFragmentation == virtualFragmentation
            && isDecoy == rhs.isDecoy
            && ionizationMode == rhs.ionizationMode
            && db == rhs.db
            && fragmentMzValues.size() == rhs.fragmentMzValues.size()
            && equal(begin(fragmentMzValues),
                     end(fragmentMzValues),
                     begin(rhs.fragmentMzValues),
                     [](float a, float b) {
                        return almostEqual(a, b);
                     })
            && fragmentIntensities.size() == rhs.fragmentIntensities.size()
            && equal(begin(fragmentIntensities),
                     end(fragmentIntensities),
                     begin(rhs.fragmentIntensities),
                     [](float a, float b) {
                        return almostEqual(a, b);
                     })
            && fragmentIonTypes == rhs.fragmentIonTypes
            && category == rhs.category
            && type() == rhs.type());
}

float Compound::adjustedMass(int charge) { 
     /**   
    *@return    -    total mass by formula minus loss of electrons' mass 
    *@see  -  double MassCalculator::computeMass(string formula, int charge) in mzMassCalculator.cpp
    */
     return MassCalculator::computeMass(_formula, charge);
}

Compound::Type Compound::type() const {
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

FragmentationMatchScore Compound::scoreCompoundHit(Fragment* expFrag,
                                                   float productPpmTolr,
                                                   bool searchProton)
{
    FragmentationMatchScore s;

    if (fragmentMzValues.size() == 0) return s;

    Fragment libFrag;
    libFrag.precursorMz = precursorMz;
    libFrag.mzValues = fragmentMzValues;
    libFrag.intensityValues = fragmentIntensities;
    libFrag.annotations = fragmentIonTypes;
    if (searchProton)  { //special case, check for loss or gain of protons
        int N = libFrag.mzValues.size();
        for(int i = 0; i < N; i++) {
            libFrag.mzValues.push_back(libFrag.mzValues[i] + PROTON_MASS);
            libFrag.intensityValues.push_back(libFrag.intensityValues[i]);
            libFrag.mzValues.push_back( libFrag.mzValues[i] - PROTON_MASS);
            libFrag.intensityValues.push_back(libFrag.intensityValues[i]);
        }
    }
    //theory fragmentation or library fragmentation = libFrag
    //experimental data = expFrag
    libFrag.sortByIntensity();
    s = libFrag.scoreMatch(expFrag, productPpmTolr);
    return s;
}

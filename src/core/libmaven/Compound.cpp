#include "doctest.h"
#include "Compound.h"
#include "constants.h"
#include "mzMassCalculator.h"
#include "mzUtils.h"
#include "Scan.h"
#include "testUtils.h"


using namespace mzUtils;

Compound::Compound(string id, string name, string formula, int charge,
                   float expectedRt, float mass, string db, float precursorMz,
                   float productMz, float collisionEnergy, string note) {
    this->_id = id;
    this->_name = name;
    this->_formula = formula;
    this->_charge = charge;
    this->_expectedRt = expectedRt;
    this->_db = db;

    /**
    *@brief  -   calculate mass of compound by its formula and assign it to mass
    *@see  - double MassCalculator::computeNeutralMass(string formula) in mzMassCalculator.cpp
    */
    this->_mz =  MassCalculator::computeNeutralMass(formula);
    this->_neutralMass = MassCalculator::computeNeutralMass(_formula);
    this->_logP = 0;
    this->_srmId = "";
    this->_precursorMz = precursorMz;
    this->_productMz = productMz;
    this->_collisionEnergy = collisionEnergy;
    _virtualFragmentation = false;
    _isDecoy = false;
    ionizationMode = Compound::IonizationMode::Neutral;

}

bool Compound::operator == (const Compound& rhs) const
{
    return (_id == rhs._id
            && _name == rhs._name
            && _formula == rhs._formula
            && _kegg_id == rhs._kegg_id
            && _pubchem_id == rhs._pubchem_id
            && _hmdb_id == rhs._hmdb_id
            && _alias == rhs._alias
            && _smileString == rhs._smileString
            && _srmId == rhs._srmId
            && almostEqual(_expectedRt, rhs._expectedRt)
            && _charge == rhs._charge
            && almostEqual(_mz, rhs._mz)
            && _method_id == rhs._method_id
            && almostEqual(_precursorMz, rhs._precursorMz)
            && almostEqual(_productMz, rhs._productMz)
            && almostEqual(_collisionEnergy, rhs._collisionEnergy)
            && almostEqual(_logP, rhs._logP)
            && _virtualFragmentation == rhs._virtualFragmentation
            && _isDecoy == rhs._isDecoy
            && ionizationMode == rhs.ionizationMode
            && _db == rhs._db
            && _fragmentMzValues.size() == rhs._fragmentMzValues.size()
            && equal(begin(_fragmentMzValues),
                     end(_fragmentMzValues),
                     begin(rhs._fragmentMzValues),
                     [](float a, float b) {
                        return almostEqual(a, b);
                     })
            && _fragmentIntensities.size() == rhs._fragmentIntensities.size()
            && equal(begin(_fragmentIntensities),
                     end(_fragmentIntensities),
                     begin(rhs._fragmentIntensities),
                     [](float a, float b) {
                        return almostEqual(a, b);
                     })
            && _fragmentIonTypes == rhs._fragmentIonTypes
            && _category == rhs._category
            && type() == rhs.type());
}


Compound* Compound::operator = (const Compound& rhs)
{
    _id = rhs._id;
    _name = rhs._name;
    _formula = rhs._formula;
    _kegg_id = rhs._kegg_id;
    _pubchem_id = rhs._pubchem_id;
    _hmdb_id = rhs._hmdb_id;
    _alias = rhs._alias;
    _smileString = rhs._smileString;
    _srmId = rhs._srmId;
    _expectedRt = rhs._expectedRt;
    _charge = rhs._charge;
    _mz = rhs._mz;
    _method_id = rhs._method_id;
    _precursorMz = rhs._precursorMz;
    _productMz = rhs._productMz;
    _collisionEnergy = rhs._collisionEnergy;
    _logP = rhs._logP;
    _virtualFragmentation = rhs._virtualFragmentation;
    _isDecoy = rhs._isDecoy;
    ionizationMode = rhs.ionizationMode;
    _db = rhs._db;
    _fragmentMzValues = rhs._fragmentMzValues;
    _fragmentIntensities = rhs._fragmentIntensities;
    _fragmentIonTypes = rhs._fragmentIonTypes;
    _category = rhs._category;
    return this;
}



float Compound::adjustedMass(int charge) {
     /**
    *@return    -    total mass by formula minus loss of electrons' mass
    *@see  -  double MassCalculator::computeMass(string formula, int charge) in mzMassCalculator.cpp
    */
     if (!_formula.empty()) {
        return MassCalculator::computeMass(_formula, charge);
     } else if (_neutralMass != 0.0f) {
         return MassCalculator::adjustMass(_neutralMass, charge);
     }
     return _mz;
}

Compound::Type Compound::type() const {
    bool hasFragMzs = _fragmentMzValues.size() > 0;
    bool hasFragInts = _fragmentIntensities.size() == _fragmentMzValues.size();
    if (hasFragMzs && hasFragInts)
        return Type::MS2;

    bool hasPrecursorMz = _precursorMz > 0;
    bool hasProductMz = _productMz > 0;
    if (hasPrecursorMz && hasProductMz)
        return Type::MRM;

    // Is this the only requirement for being usable as an MS1 compound?
    if (_mz)
        return Type::MS1;

    return Type::UNKNOWN;
}

FragmentationMatchScore Compound::scoreCompoundHit(Fragment* expFrag,
                                                   float productPpmTolr,
                                                   bool searchProton)
{
    FragmentationMatchScore s;

    if (_fragmentMzValues.size() == 0) return s;

    Fragment libFrag;
    libFrag.precursorMz = _precursorMz;
    libFrag.mzValues = _fragmentMzValues;
    libFrag.intensityValues = _fragmentIntensities;
    libFrag.annotations = _fragmentIonTypes;
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


string Compound::id()
{
    return this->_id;
}


void Compound::setId(string id){
    this->_id = id;
}

string Compound::name()
{
    return this->_name;
}

void Compound::setName(string name)
{
    this->_name = name;
}

void Compound::setFormula(string formula) {
    _formula = filterFormula(formula);
}

string Compound::filterFormula(string formulaString)
{
    string validChars = CHE_FORMULA_ALPHA_UPP
                        + CHE_FORMULA_ALPHA_LOW
                        + CHE_FORMULA_COFF;
    formulaString.erase(remove_if(formulaString.begin(),
                                  formulaString.end(),
                                  [validChars] (const char& c) {
                                      return (validChars.find(c)
                                              == string::npos);
                                  }),
                        formulaString.end());
    return formulaString;
}

string Compound::formula()
{
    return this->_formula;
}


void Compound::setAlias(string alias)
{
    this->_alias = alias;
}

string Compound::alias()
{
    return this->_alias;
}


void Compound::setExpectedRt(float rt)
{
    this->_expectedRt = rt;
}

float Compound::expectedRt()
{
    return this->_expectedRt;
}


void Compound::setCharge(int charge)
{
    this->_charge = charge;
}

int Compound::charge()
{
    return this->_charge;
}


void Compound::setMz(float mass)
{
    this->_mz = mass;
}

float Compound::mz()
{
    return this->_mz;
}

void Compound::setProductMz(float productMz)
{
    this->_productMz = productMz;
}

float Compound::productMz()
{
    return this->_productMz;
}

void Compound::setPrecursorMz(float precursorMz)
{
    this->_precursorMz = precursorMz;
}

float Compound::precursorMz()
{
    return this->_precursorMz;
}

void Compound::setCollisionEnergy(float collisionEnergy)
{
    this->_collisionEnergy = collisionEnergy;
}

float Compound::collisionEnergy()
{
    return this->_collisionEnergy;
}


void Compound::setDb(string db)
{
    this->_db = db;
}

string Compound::db()
{
    return this->_db;
}

string Compound::srmId()
{
    return this->_srmId;
}

void Compound::setSrmId(string srmId)
{
    this->_srmId = srmId;
}

void Compound::setNeutralMass(float mass)
{
    _neutralMass = mass;
}

float Compound::neutralMass()
{
    return _neutralMass;
}

void Compound::setCategory(vector<string> category){
    _category = category;
}

vector<string> Compound::category(){
    return _category;
}

void Compound::setFragmentMzValues(vector<float> mzValues){
    _fragmentMzValues = mzValues;
}

vector<float> Compound::fragmentMzValues(){
    return _fragmentMzValues;
}

void Compound::setFragmentIntensities(vector<float> intensities){
    _fragmentIntensities = intensities;
}

vector<float> Compound::fragmentIntensities(){
    return _fragmentIntensities;
}

void Compound::setFragmentIonTypes(map<int, string> types){
    _fragmentIonTypes = types;
}

map<int, string> Compound::fragmentIonTypes(){
    return _fragmentIonTypes;
}

void Compound::setSmileString(string smileString){
    _smileString = smileString;
}

string Compound::smileString(){
    return _smileString;
}

void Compound::setLogP(float logP){
    _logP = logP;
}

float Compound:: logP(){
    return _logP;
}

void Compound::setVirtualFragmentation(bool isVirtual){
    _virtualFragmentation = isVirtual;
}

bool Compound:: virtualFragmentation(){
    return _virtualFragmentation;
}

void Compound:: setNote(string note){
    _note = note;
}

string Compound:: note(){
    return _note;
}

void Compound::setIsDecoy(bool isDecoy){
    _isDecoy = isDecoy;
}

bool Compound::isDecoy(){
    return _isDecoy;
}

void Compound::setMethod_id(string id){
    _method_id = id;
}

string Compound::method_id(){
    return _method_id;
}

void Compound::setTransition_id(int id){
    _transition_id = id;
}

int Compound::transition_id(){
    return _transition_id;
}

void Compound::setKegg_id(string id){
    _kegg_id = id;
}

string Compound::kegg_id(){
    return _kegg_id;
}

void Compound::setPubchem_id(string id){
    _pubchem_id = id;
}

string Compound::pubchem_id(){
    return _pubchem_id;
}

void Compound::setHmdb_id(string id){
    _hmdb_id = id;
}

string Compound::hmdb_id(){
    return  _hmdb_id;
}


class Test_CompoundFixture: public SampleLoadingFixture{

    private:
            void _initializeCompoundName(){
                compoundName.push_back("dTTP");
                compoundName.push_back("NADPH");
                compoundName.push_back("CTP");
                compoundName.push_back("dCTP");
                compoundName.push_back("methionine");
                compoundName.push_back("proline");
                compoundName.push_back("glutamine");
                compoundName.push_back("serine");
                compoundName.push_back("alanine");
                compoundName.push_back("lysine");
            }

            void _initializeCompoundFormula(){
                compoundFormula.push_back("C10H17N2O14P3");
                compoundFormula.push_back("C21H30N7O17P3");
                compoundFormula.push_back("C9H16N3O14P3");
                compoundFormula.push_back("C5H11NO2S");
                compoundFormula.push_back("C5H9NO2");
                compoundFormula.push_back("C5H10N2O3");
                compoundFormula.push_back("C3H7NO3");
                compoundFormula.push_back("C3H7NO2");
                compoundFormula.push_back("C6H14N2O2");
                compoundFormula.push_back("C9H16N3O13P3");

            }

            void _initializeCopoundId(){
                compoundId.push_back("C00459");
                compoundId.push_back("C00005");
                compoundId.push_back("C00063");
                compoundId.push_back("C00458");
                compoundId.push_back("C00073");
                compoundId.push_back("C00148");
                compoundId.push_back("C00064");
                compoundId.push_back("C00065");
                compoundId.push_back("C00041");
                compoundId.push_back("C00047");
            }

            void _initializeCompoundExpectedRt(){
                compoundExpectedRt.push_back(14.94);
                compoundExpectedRt.push_back(14.87);
                compoundExpectedRt.push_back(14.9);
                compoundExpectedRt.push_back(14.8);
                compoundExpectedRt.push_back(1.5);
                compoundExpectedRt.push_back(1.12);
                compoundExpectedRt.push_back(1.1);
                compoundExpectedRt.push_back(1);
                compoundExpectedRt.push_back(1);
                compoundExpectedRt.push_back(0.8);
            }

            void _initializeCompoundCharge(){

                for(int i=0; i<10; i++){
                    int charge = mzUtils::randInt(0, 2);
                    compoundCharge.push_back(charge);
                }
            }

    protected:
                vector<string> compoundName;
                vector<string> compoundFormula;
                vector<string> compoundId;
                vector<float> compoundExpectedRt;
                vector<int> compoundCharge;
    public:
            Test_CompoundFixture()
            {
                _initializeCompoundName();
                _initializeCompoundFormula();
                _initializeCopoundId();
                _initializeCompoundExpectedRt();
                _initializeCompoundCharge();
            }

};


TEST_CASE_FIXTURE( Test_CompoundFixture ,"Testing Compound_Class"){

        SUBCASE("Testing GetterFunctions"){
        for(int i=0; i<100; i++){

            int name = mzUtils::randInt(0, 9);
            int formula = mzUtils::randInt(0, 9);
            int id = mzUtils::randInt(0, 9);
            int rt = mzUtils::randInt(0, 9);
            int charge = mzUtils::randInt(0, 9);

            Compound* compound = new Compound(compoundId[id],
                                              compoundName[name],
                                              compoundFormula[formula],
                                              compoundCharge[charge],
                                              compoundExpectedRt[rt]);

            string c_id = compound->id();
            string randId = compoundId[id];
            REQUIRE(c_id == randId);

            string c_name = compound->name();
            string randName = compoundName[name];
            REQUIRE(c_name == randName);

            string c_formula = compound->formula();
            string randFormula = compoundFormula[formula];
            REQUIRE(c_formula == randFormula);

            float c_rt = compound->expectedRt();
            float randRt = compoundExpectedRt[rt];
            REQUIRE(randRt == doctest::Approx(c_rt));

            compound->setMethod_id("test_id");
            REQUIRE(compound->method_id() == "test_id");

            compound->setTransition_id(1);
            REQUIRE(compound->transition_id() == 1);

            compound->setKegg_id("test_id");
            REQUIRE(compound->kegg_id() == "test_id");

            compound->setPubchem_id("test_id");
            REQUIRE(compound->pubchem_id() == "test_id");

            compound->setHmdb_id("test_id");
            REQUIRE(compound->hmdb_id() == "test_id");

            compound->setNote("test_note");
            REQUIRE(compound->note() == "test_note");

            compound->setSmileString("test_smileString");
            REQUIRE(compound->smileString() == "test_smileString");

            bool decoy = i % 2 == 0 ? true : false;
            compound->setIsDecoy(decoy);
            REQUIRE(compound->isDecoy() == decoy);

            bool virtualFrag = i % 2 == 0 ? true : false;
            compound->setVirtualFragmentation(virtualFrag);
            REQUIRE(compound->virtualFragmentation() == virtualFrag);

            auto logP = mzUtils::randFloat(0,50);
            compound->setLogP(logP);
            REQUIRE(compound->logP() == logP);

            vector<string> category;
            category.push_back("one");
            category.push_back("two");
            compound->setCategory(category);
            auto getCategory = compound->category();
            REQUIRE(getCategory[0] == "one");
            REQUIRE(getCategory[1] == "two");

            vector<float> mzValues;
            for(int i = 0; i < 5; i++)
                mzValues.push_back(mzUtils::randFloat(0,50));
            compound->setFragmentMzValues(mzValues);
            auto getMzValues = compound->fragmentMzValues();
            for(int i = 0; i < 5; i++)
                REQUIRE(getMzValues[i] == mzValues[i]);

            vector<float> intensities;
            for(int i = 0; i < 5; i++)
                intensities.push_back(mzUtils::randFloat(0,50));
            compound->setFragmentIntensities(intensities);
            auto getIntensities = compound->fragmentMzValues();
            for(int i = 0; i < 5; i++)
                REQUIRE(getIntensities[i] == intensities[i]);

            map<int, string> ionTypes;
            ionTypes[1] = "positive";
            ionTypes[2] = "negative";
            compound->setFragmentIonTypes(ionTypes);
            REQUIRE(compound->fragmentIonTypes() == ionTypes);
        }
    }

    SUBCASE("Testing compare functions"){

        Compound* a = new Compound("C00166", "UTP" ,
                       "C9H15N2O14P3", 1, 14.81);

        Compound* b = new Compound("C00175", "ZTP" ,
                       "C9H15N2O15P3", 1, 14.91);

        Compound* c = new Compound("C00166", "UTP" ,
                       "C9H15N2O14P3", 1, 14.81);

        REQUIRE(a->compName(a, b));
        REQUIRE(a->compMass(a, b));
        REQUIRE(a->compFormula(a, b));
    }

    SUBCASE("Testing Adjusted Mass"){
        Compound a("C00166", "UTP" ,
                       "C9H15N2O14P3", 1, 14.81);

        float mass = a.adjustedMass(1);
        REQUIRE(468.981 == doctest::Approx(mass));
    }


    SUBCASE("Testing Assignment Operator"){
        for(int i=0; i<100; i++){
            int name = mzUtils::randInt(0, 9);
            int formula = mzUtils::randInt(0, 9);
            int id = mzUtils::randInt(0, 9);
            int rt = mzUtils::randInt(0, 9);
            int charge = mzUtils::randInt(0, 9);

            Compound a(compoundId[id], compoundName[name],
                           compoundFormula[formula], compoundCharge[charge],
                           compoundExpectedRt[rt]);

            vector<string> category;
            category.push_back("amino acid");
            a.setCategory(category);

            Compound b = a;

            REQUIRE(b.id() == a.id());
            REQUIRE(b.name() == a.name());
            REQUIRE(b.formula() == a.formula());
            REQUIRE(b.expectedRt() == doctest::Approx(a.expectedRt()));
        }
    }


    SUBCASE("Testing ScoreCompoundHit"){
        Compound a("C00166", "UTP" ,
                   "C9H15N2O14P3", 1, 14.81);
        auto mzSamples = samples();
        Scan* scan = new Scan(mzSamples[0], 2, 0, 0.782, 0, 0);
        Fragment fragment(scan, 0.12, 0.15, 3);
        FragmentationMatchScore f = a.scoreCompoundHit(&fragment, 5, false);
        REQUIRE(f.fractionMatched == 0);
        REQUIRE(f.ppmError == 1000);
        REQUIRE(f.mzFragError == 1000);
    }

    SUBCASE("Testing type"){
        Compound a("C00166", "UTP" , "C9H15N2O14P3",
                         1, 14.81);
        Compound::Type T = a.type();
        REQUIRE(T == Compound::Type::MS1);
    }

    SUBCASE("Testing equalsTo operator"){
        Compound a("C00166", "UTP" ,
                       "C9H15N2O14P3", 1, 14.81);

        Compound b("C00166", "UTP" ,
                       "C9H15N2O14P3", 1, 14.81);
        REQUIRE( a == b);
    }
}

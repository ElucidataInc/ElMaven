#include "testCharge.h"

TestCharge::TestCharge() {

}

void TestCharge::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestCharge::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestCharge::init() {
    // This function is executed before each test
}

void TestCharge::cleanup() {
    // This function is executed after each test
}

void TestCharge::testChargeWithFormulaAndCompound(){
    Compound* compound = new Compound("1","one","C6H12O6",2);
    MavenParameters* mp = new MavenParameters();
    mp->charge = 3;
    mp->ionizationMode = 1;
    mp->formulaFlag = true;
    int charge = mp->getCharge(compound);
    QVERIFY(charge == 3);
}

void TestCharge::testChargeWithCompoundNoFormula(){
    Compound* compound = new Compound("1","one","C6H12O6",2);
    MavenParameters* mp = new MavenParameters();
    mp->charge = 3;
    mp->ionizationMode = 1;
    mp->formulaFlag = false;
    int charge = mp->getCharge(compound);
    QVERIFY(charge == 2);
}

void TestCharge::testChargeWithCompoundNoCharge(){
    Compound* compound = new Compound("1","one","C6H12O6",0);
    MavenParameters* mp = new MavenParameters();
    mp->charge = 3;
    mp->ionizationMode = 1;
    mp->formulaFlag = false;
    int charge = mp->getCharge(compound);
    QVERIFY(charge == 3);
}

void TestCharge::testChargeWithNoCompoundNoFormula(){
    MavenParameters* mp = new MavenParameters();
    mp->charge = 3;
    mp->ionizationMode = -1;
    mp->formulaFlag = false;
    int charge = mp->getCharge();
    QVERIFY(charge == -3);
}

void TestCharge::testChargeWithFormulaNoCompound(){
    MavenParameters* mp = new MavenParameters();
    mp->charge = 3;
    mp->ionizationMode = -1;
    mp->formulaFlag = true;
    int charge = mp->getCharge();
    QVERIFY(charge == -3);
}
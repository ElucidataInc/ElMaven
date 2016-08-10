#include "testMassCalculator.h"
#include "../libmaven/mzMassCalculator.h"

TestMassCalculator::TestMassCalculator() {

}

void TestMassCalculator::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestMassCalculator::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestMassCalculator::init() {
    // This function is executed before each test
}

void TestMassCalculator::cleanup() {
    // This function is executed after each test
}

void TestMassCalculator::testGetComposition() {
    map<string,int> massCompoH20 = MassCalculator::getComposition("H2O");
    map<string,int> massCompoH20Man;
    massCompoH20Man["H"] = 2;
    massCompoH20Man["O"] = 1;
    QVERIFY(common::compareMaps(massCompoH20, massCompoH20Man));

    map<string,int> massCompoC2H6O = MassCalculator::getComposition("C2H6O");
    map<string,int> massCompoC2H6OMan;
    massCompoC2H6OMan["C"] = 2;
    massCompoC2H6OMan["H"] = 6;
    massCompoC2H6OMan["O"] = 1;
    QVERIFY(common::compareMaps(massCompoC2H6O, massCompoC2H6OMan));

    map<string,int> massCompoC2H5OH = MassCalculator::getComposition("C2H5OH");
    map<string,int> massCompoC2H5OHMan;
    massCompoC2H5OHMan["C"] = 2;
    massCompoC2H5OHMan["H"] = 6;
    massCompoC2H5OHMan["O"] = 1;
    QVERIFY(common::compareMaps(massCompoC2H5OH,massCompoC2H5OHMan));

    map<string,int> massCompoHCl = MassCalculator::getComposition("HCl");
    map<string,int> massCompoHClMan;
    massCompoHClMan["H"] = 1;
    massCompoHClMan["Cl"] = 1;
    QVERIFY(common::compareMaps(massCompoHCl,massCompoHClMan));

}

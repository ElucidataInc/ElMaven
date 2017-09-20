#include "testMassCalculator.h"
#include "mzMassCalculator.h"
#include "mzSample.h"

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

void TestMassCalculator::testNeutralMass() {
    double alcoholMonoIsotopicMass = 46.041866;

    QVERIFY(common::floatCompare(MassCalculator::computeNeutralMass("C2H6O"), alcoholMonoIsotopicMass));

    QVERIFY(common::floatCompare(MassCalculator::computeNeutralMass("C2H5OH"), alcoholMonoIsotopicMass));

}

void TestMassCalculator::testComputeMass() {
    string XanthosineChe = "C10H12N4O6";
    double XanthosineMonoIso = 284.075684;
    double HydrogenMass =  1.007;

    QVERIFY(common::floatCompare(MassCalculator::computeMass(XanthosineChe, 0), XanthosineMonoIso));

    QVERIFY(common::floatCompare(MassCalculator::computeMass(XanthosineChe, -1), XanthosineMonoIso - HydrogenMass ));

    QVERIFY(common::floatCompare(MassCalculator::computeMass(XanthosineChe, +1), XanthosineMonoIso + HydrogenMass ));

}

void TestMassCalculator::testComputeIsotopes() {
    string XanthosineChe = "C10H12N4O6";

    map <string, bool> isotopeAtom;
    isotopeAtom["D2Labeled_BPE"] = true;
    isotopeAtom["C13Labeled_BPE"] = true;
    isotopeAtom["N15Labeled_BPE"] = true;
    isotopeAtom["S34Labeled_BPE"] = true;

    vector<Isotope> isotopesXanthosine = MassCalculator::computeIsotopes(XanthosineChe, +1, isotopeAtom, 2);

    //TODO: have to add a test case here
    for(vector<Isotope>::iterator it = isotopesXanthosine.begin(); it != isotopesXanthosine.end(); ++it) {
         //cerr << it->name << endl;
    }

    QVERIFY(true);
}

void TestMassCalculator::testenumerateMasses() {
    //TODO: have to add a test case for ennumurate mass
    // MassCalculator masCal;
    // masCal.enumerateMasses()

    QVERIFY(true);
}

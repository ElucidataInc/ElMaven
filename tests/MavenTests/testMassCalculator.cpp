#include "testMassCalculator.h"
#include "database.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "utilities.h"

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
    QVERIFY(TestUtils::compareMaps(massCompoH20, massCompoH20Man));

    map<string,int> massCompoC2H6O = MassCalculator::getComposition("C2H6O");
    map<string,int> massCompoC2H6OMan;
    massCompoC2H6OMan["C"] = 2;
    massCompoC2H6OMan["H"] = 6;
    massCompoC2H6OMan["O"] = 1;
    QVERIFY(TestUtils::compareMaps(massCompoC2H6O, massCompoC2H6OMan));

    map<string,int> massCompoC2H5OH = MassCalculator::getComposition("C2H5OH");
    map<string,int> massCompoC2H5OHMan;
    massCompoC2H5OHMan["C"] = 2;
    massCompoC2H5OHMan["H"] = 6;
    massCompoC2H5OHMan["O"] = 1;
    QVERIFY(TestUtils::compareMaps(massCompoC2H5OH,massCompoC2H5OHMan));

    map<string,int> massCompoHCl = MassCalculator::getComposition("HCl");
    map<string,int> massCompoHClMan;
    massCompoHClMan["H"] = 1;
    massCompoHClMan["Cl"] = 1;
    QVERIFY(TestUtils::compareMaps(massCompoHCl,massCompoHClMan));

}

void TestMassCalculator::testNeutralMass() {
    double alcoholMonoIsotopicMass = 46.041866;

    QVERIFY(TestUtils::floatCompare(MassCalculator::computeNeutralMass("C2H6O"), alcoholMonoIsotopicMass));

    QVERIFY(TestUtils::floatCompare(MassCalculator::computeNeutralMass("C2H5OH"), alcoholMonoIsotopicMass));

}

void TestMassCalculator::testComputeMass() {
    string XanthosineChe = "C10H12N4O6";
    double XanthosineMonoIso = 284.075684;
    double HydrogenMass =  1.007;

    QVERIFY(TestUtils::floatCompare(MassCalculator::computeMass(XanthosineChe, 0), XanthosineMonoIso));

    QVERIFY(TestUtils::floatCompare(MassCalculator::computeMass(XanthosineChe, -1), XanthosineMonoIso - HydrogenMass ));

    QVERIFY(TestUtils::floatCompare(MassCalculator::computeMass(XanthosineChe, +1), XanthosineMonoIso + HydrogenMass ));

}

void TestMassCalculator::testComputeIsotopes() {
    string formula = "C12H18N4O4PS";

    bool C13Labeled_BPE = true;
    bool N15Labeled_BPE = true;
    bool S34Labeled_BPE = true;
    bool D2Labeled_BPE = true;

    vector<Isotope> isotopes = MassCalculator::computeIsotopes(
        formula,
        +1,
        C13Labeled_BPE,
        N15Labeled_BPE,
        S34Labeled_BPE,
        D2Labeled_BPE
    );

    //verify number of isotopes
    QVERIFY(isotopes.size() == 312);
    //verify C12 parent mass
    QVERIFY(floor(isotopes[0].mass) == 346);
    //verify C13 mass
    QVERIFY(floor(isotopes[277].mass) == 349);
    //verify N15 mass
    QVERIFY(floor(isotopes[290].mass) == 348);
    //verify D2 mass
    QVERIFY(floor(isotopes[294].mass) == 352);
    //verify S34 mass
    QVERIFY(floor(isotopes[293].mass) == 351);
    //verify C13N15 dual label mass
    QVERIFY(floor(isotopes[1].mass) == 347);
    //verify C13D2 dual label mass
    QVERIFY(floor(isotopes[61].mass) == 362);
    //verify C13S34 dual label mass
    QVERIFY(floor(isotopes[49].mass) == 350);
    //verify abundance calculation
    QVERIFY(isotopes[3].abundance > 0.0002 && isotopes[3].abundance < 0.0003);

    C13Labeled_BPE = true;
    N15Labeled_BPE = false;
    S34Labeled_BPE = true;
    D2Labeled_BPE = false;

    vector<Isotope> isotopes1 = MassCalculator::computeIsotopes(
        formula,
        +1,
        C13Labeled_BPE,
        N15Labeled_BPE,
        S34Labeled_BPE,
        D2Labeled_BPE
    );

    //verify number of isotopes
    QVERIFY(isotopes1.size() == 26);

}

void TestMassCalculator::testenumerateMasses() {
    //TODO: have to add a test case for ennumurate mass
    // MassCalculator masCal;
    // masCal.enumerateMasses()

    QVERIFY(true);
}

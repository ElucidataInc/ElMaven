#include "testPeakDetection.h"

TestPeakDetection::TestPeakDetection() {
    loadCompoundDB = "/home/rajat/elucidata/maven/maven_opensource/bin/methods/qe3_v11_2016_04_29.csv";

}

void TestPeakDetection::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestPeakDetection::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestPeakDetection::init() {
    // This function is executed before each test
}

void TestPeakDetection::cleanup() {
    // This function is executed after each test
}

void TestPeakDetection::testProcessCompound() {
    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("qe3_v11_2016_04_29");

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundPPMWindow = 10;
    mavenparameters->ionizationMode = +1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds, "compounds");

    QVERIFY(true);

}


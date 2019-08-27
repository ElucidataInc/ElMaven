#include "testPeakDetection.h"
#include "datastructures/mzSlice.h"
#include "masscutofftype.h"
#include "PeakGroup.h"
#include "EIC.h"
#include "utilities.h"
#include "mzSample.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "isotopeDetection.h"
#include "classifierNeuralNet.h"

TestPeakDetection::TestPeakDetection() {
    loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    loadCompoundDB1 = "bin/methods/KNOWNS.csv";
    files << "bin/methods/testsample_2.mzxml" << "bin/methods/testsample_3.mzxml";

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
    maventests::database.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = maventests::database.getCompoundsSubset("qe3_v11_2016_04_29");

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    mavenparameters->ionizationMode = +1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds,
                                                            mavenparameters->getDefaultAdductList(),
                                                            "compounds");

    QVERIFY(true);

}

void TestPeakDetection::testPullEICs() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (int i = 0; i <  files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[2];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->aslsBaselineMode = false;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    vector<EIC*> eics = PeakDetector::pullEICs(slice,
                                               mavenparameters->samples,
                                               mavenparameters);
    QVERIFY(eics.size() == 2);
}

void TestPeakDetection::testprocessSlices() {

    vector<PeakGroup> allgroups = TestUtils::getGroupsFromProcessCompounds();
    QVERIFY(allgroups.size() > 0);

}

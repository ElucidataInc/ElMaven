#include "testMzSlice.h"
#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "mavenparameters.h"
#include "utilities.h"

TestMzSlice::TestMzSlice() {

}

void TestMzSlice::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestMzSlice::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestMzSlice::init() {
    // This function is executed before each test
}

void TestMzSlice::cleanup() {
    // This function is executed after each test
}

void TestMzSlice::testCalculateMzMaxMinWithCF() {
    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    int ionizationMode = +1;

    float mass = 663.109131 + 1.007;
    float mzMin = mass - mavenparameters->compoundMassCutoffWindow->massCutoffValue(mass);
    float mzMax = mass + mavenparameters->compoundMassCutoffWindow->massCutoffValue(mass);

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    QVERIFY(TestUtils::floatCompare(slice->mzmin,mzMin) && \
    TestUtils::floatCompare(slice->mzmax,mzMax));
}

void TestMzSlice::testCalculateMzMaxMinWithNOCF() {
    vector<Compound*> compounds = TestUtils::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    int ionizationMode = +1;
    float ppmScale = 1e6;

    float mzMin = compounds[0]->mz() - mavenparameters->compoundMassCutoffWindow->massCutoffValue(compounds[0]->mz());
    float mzMax = compounds[0]->mz() + mavenparameters->compoundMassCutoffWindow->massCutoffValue(compounds[0]->mz());

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    QVERIFY(TestUtils::floatCompare(slice->mzmin,mzMin) && \
    TestUtils::floatCompare(slice->mzmax,mzMax));
}


void TestMzSlice::testCalculateMzMaxMinWithNOCFNOMass() {
    vector<Compound*> compounds = TestUtils::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[1];
    compounds[1]->setMz(0);
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    int ionizationMode = +1;

    QVERIFY(!slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode));
}

void TestMzSlice::testcalculateRTMinMaxWithRTandEnabled() {
    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    
    float rtmin = compounds[0]->expectedRt() - compoundRTWindow;
    float rtmax = compounds[0]->expectedRt() + compoundRTWindow;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(TestUtils::floatCompare(slice->rtmin,rtmin) && \
    TestUtils::floatCompare(slice->rtmax,rtmax));


}

void TestMzSlice::testcalculateRTMinMaxWithNORTandEnabled() {
    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithNORT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = true;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(TestUtils::floatCompare(slice->rtmin,0) && \
    TestUtils::floatCompare(slice->rtmax,1e9));
}

void TestMzSlice::testcalculateRTMinMaxWithNORTandDisabled() {
    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithNORT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = false;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(TestUtils::floatCompare(slice->rtmin,0)  && \
    TestUtils::floatCompare(slice->rtmax,1e9));
}


void TestMzSlice::testcalculateRTMinMaxWithRTandDisabled() {

    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = false;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(TestUtils::floatCompare(slice->rtmin,0) && \
    TestUtils::floatCompare(slice->rtmax,1e9));
}




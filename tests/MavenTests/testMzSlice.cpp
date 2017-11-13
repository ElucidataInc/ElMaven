#include "testMzSlice.h"

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
    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow=new MassCutoff();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    int ionizationMode = +1;

    float mass = 663.109131 + 1.007;
    float mzMin = mass - mavenparameters->compoundMassCutoffWindow->massCutoffValue(mass);
    float mzMax = mass + mavenparameters->compoundMassCutoffWindow->massCutoffValue(mass);

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    QVERIFY(common::floatCompare(slice->mzmin,mzMin) && \
    common::floatCompare(slice->mzmax,mzMax));
}

void TestMzSlice::testCalculateMzMaxMinWithNOCF() {
    vector<Compound*> compounds = common::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow=new MassCutoff();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    int ionizationMode = +1;
    float ppmScale = 1e6;

    float mzMin = compounds[0]->mass - mavenparameters->compoundMassCutoffWindow->massCutoffValue(compounds[0]->mass);
    float mzMax = compounds[0]->mass + mavenparameters->compoundMassCutoffWindow->massCutoffValue(compounds[0]->mass);

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    QVERIFY(common::floatCompare(slice->mzmin,mzMin) && \
    common::floatCompare(slice->mzmax,mzMax));
}


void TestMzSlice::testCalculateMzMaxMinWithNOCFNOMass() {
    vector<Compound*> compounds = common::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[1];
    compounds[1]->mass = 0;
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow=new MassCutoff();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    int ionizationMode = +1;

    QVERIFY(!slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode));
}

void TestMzSlice::testcalculateRTMinMaxWithRTandEnabled() {
    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    
    float rtmin = compounds[0]->expectedRt - compoundRTWindow;
    float rtmax = compounds[0]->expectedRt + compoundRTWindow;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(common::floatCompare(slice->rtmin,rtmin) && \
    common::floatCompare(slice->rtmax,rtmax));


}

void TestMzSlice::testcalculateRTMinMaxWithNORTandEnabled() {
    vector<Compound*> compounds = common::getCompoudDataBaseWithNORT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = true;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(common::floatCompare(slice->rtmin,0) && \
    common::floatCompare(slice->rtmax,1e9));
}

void TestMzSlice::testcalculateRTMinMaxWithNORTandDisabled() {
    vector<Compound*> compounds = common::getCompoudDataBaseWithNORT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = false;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(common::floatCompare(slice->rtmin,0)  && \
    common::floatCompare(slice->rtmax,1e9));
}


void TestMzSlice::testcalculateRTMinMaxWithRTandDisabled() {

    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    bool matchRtFlag = false;
    float compoundRTWindow = 2;

    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);

    QVERIFY(common::floatCompare(slice->rtmin,0) && \
    common::floatCompare(slice->rtmax,1e9));
}




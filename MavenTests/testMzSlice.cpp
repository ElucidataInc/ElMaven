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
    
    pair<massAccType,double> pr = make_pair(ppm,10);
    int ionizationMode = +1;

    float mass = 663.109131 + 1.007;
    float massAcc = mzUtils::getMassAcc(pr,mass);
    float mzMin = mass - massAcc;
    float mzMax = mass + massAcc;

    slice->calculateMzMinMax(pr, ionizationMode);

    QVERIFY(common::floatCompare(slice->mzmin,mzMin) && \
    common::floatCompare(slice->mzmax,mzMax));
}

void TestMzSlice::testCalculateMzMaxMinWithNOCF() {
    vector<Compound*> compounds = common::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[0];
    pair<massAccType,double> pr = make_pair(ppm,10);
    int ionizationMode = +1;

    float massAcc = mzUtils::getMassAcc(pr,compounds[0]->mass);
    float mzMin = compounds[0]->mass - massAcc;
    float mzMax = compounds[0]->mass + massAcc;

    slice->calculateMzMinMax(pr, ionizationMode);

    slice->calculateMzMinMax(pr, ionizationMode);

    QVERIFY(common::floatCompare(slice->mzmin,mzMin) && \
    common::floatCompare(slice->mzmax,mzMax));
}


void TestMzSlice::testCalculateMzMaxMinWithNOCFNOMass() {
    vector<Compound*> compounds = common::getFaltyCompoudDataBase();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[1];
    compounds[1]->mass = 0;
    pair<massAccType,double> pr = make_pair(ppm,10);
    int ionizationMode = +1;

    QVERIFY(!slice->calculateMzMinMax(pr, ionizationMode));
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




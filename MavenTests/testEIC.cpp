#include "testEIC.h"
#include "../libmaven/mzMassCalculator.h"
#include "../libmaven/mzSample.h"

TestEIC::TestEIC() {
    loadFile = "bin/methods/bk_#sucyxpe_1_9.mzxml";
    loadGoodSample = "bin/methods/sample_#sucyxpe_2_5.mzxml";

}

void TestEIC::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printettings();
}

void TestEIC::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestEIC::init() {
    // This function is executed before each test
}

void TestEIC::cleanup() {
    // This function is executed after each test
}

void TestEIC::testgetEIC() {
    unsigned int numberOfScans = 445;
    mzSample* mzsample = new mzSample();
    mzsample->loadSample(loadFile);
    EIC e;

    bool status = e.makeEICSlice(mzsample, 180.002,180.004, 0, 2, 1);
    QVERIFY(e.intensity.size() == numberOfScans && status);
}

void TestEIC::testcomputeSpline() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12, 16, 1);
    
    //if eic exists, perform smoothing
    EIC::SmootherType smootherType = 
        (EIC::SmootherType) 1;

    e->setSmootherType(smootherType);
    e->computeSpline(10);
    QVERIFY(true);
}

void TestEIC::testgetPeakPositions() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12, 16, 1);
    
    EIC::SmootherType smootherType =
            (EIC::SmootherType) 1;
    e->setSmootherType(smootherType);
    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(80);
    e->getPeakPositions(10);
    QVERIFY(true);
}

void TestEIC::testcomputeBaseLine() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;
    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12, 16, 1);
    
    EIC::SmootherType smootherType =
            (EIC::SmootherType) 1;
    e->setSmootherType(smootherType);
    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(60);
    e->computeBaseLine(5, 60);
    QVERIFY(true);
}


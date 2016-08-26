#include "testMzAligner.h"

TestMzAligner::TestMzAligner() {

    files << "bin/methods/sample_#sucyxpe_2_5.mzxml"
          << "bin/methods/sample_#sucyxpe_2_6.mzxml";
}

void TestMzAligner::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
}

void TestMzAligner::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestMzAligner::init() {
    // This function is executed before each test
}

void TestMzAligner::cleanup() {
    // This function is executed after each test
}

void TestMzAligner::testSamplesFromGroups() {

    vector<mzSample*> samplesToLoad;
    for (int i = 0; i <  files.size(); ++i) {
        mzSample* sample = new mzSample();
        sample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(sample);
    }

    MavenParameters* mavenparameters = new MavenParameters();

    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    string loadmodel = "bin/methods/default.model";
    clsf->loadModel(loadmodel);
    mavenparameters->clsf = clsf;
    mavenparameters->alignSamplesFlag = true;
    mavenparameters->processAllSlices = true;
    mavenparameters->showProgressFlag = false;
    mavenparameters->ionizationMode = +1;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->baseline_dropTopX = 80;
    mavenparameters->minGroupIntensity = 100000;
    mavenparameters->samples = samplesToLoad;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    peakDetector.processMassSlices();
    QVERIFY(true);
}

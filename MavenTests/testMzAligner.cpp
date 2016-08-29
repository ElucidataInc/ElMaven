#include "testMzAligner.h"

TestMzAligner::TestMzAligner() {

    files << "bin/methods/Alignment/SAMPLE_#SPGDYAF_3_3.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SPGDYAF_3_6.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SPGDYAF_4_7.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SQLDY72_3_7.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SQY3FB3_2_7.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SQY3FB3_3_5.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SQY3FB3_3_7.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SQY3FB3_4_6.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SRCLG3V_4_3.mzxml"
          << "bin/methods/Alignment/SAMPLE_#SSSLMX8_4_6.mzxml";
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

void TestMzAligner::testInputGroups() {

    vector<mzSample*> samplesToLoad;
    for (int i = 0; i <  files.size(); ++i) {
        mzSample* sample = new mzSample();
        sample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(sample);
    }

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
    QVERIFY(mavenparameters->allgroups.size());
}

void TestMzAligner::testDoAlignment() {

    vector<PeakGroup*> peakgroups(mavenparameters->allgroups.size());
    for (unsigned int i = 0; i < mavenparameters->allgroups.size(); i++)
            peakgroups[i] = &mavenparameters->allgroups[i];

    Aligner aligner;
    aligner.doAlignment(peakgroups);
    QVERIFY(true);
}

void TestMzAligner::testSaveFit(){

    vector<mzSample*> samplesToLoad;
    for (int i = 0; i <  files.size(); ++i) {
        mzSample* sample = new mzSample();
        sample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(sample);
    }

    Aligner aligner;
    aligner.samples = samplesToLoad;
    aligner.saveFit();
    cerr << aligner.fit.size();
    QVERIFY(aligner.fit.size());

}

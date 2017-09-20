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

void TestMzAligner::testDoAlignment() {

    vector<PeakGroup> allgroups = common::getGroupsFromProcessCompounds();

    vector<PeakGroup*> peakgroups(allgroups.size());
    for (unsigned int i = 0; i < allgroups.size(); i++)
            peakgroups[i] = &allgroups[i];

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

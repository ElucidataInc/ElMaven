#include "testCLI.h"

TestCLI::TestCLI() {

    clsfPath = "bin/default.model";
    dbPath = "bin/methods/KNOWNS.csv";
    normalSample = "bin/methods/bk_#sucyxpe_1_9.mzxml";
    blankSample = "bin/methods/blan_#sucyxpe_2_4.mzxml";

}

TestCLI::~TestCLI() {

}

void TestCLI::testLoadClassificationModel() {
    
    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();
    peakdetectorCLI->loadClassificationModel(clsfPath);

    QVERIFY(peakdetectorCLI->mavenParameters->clsf->hasModel());

}

void TestCLI::testLoadCompoundsFile() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();
    peakdetectorCLI->mavenParameters->ligandDbFilename = dbPath;
    peakdetectorCLI->loadCompoundsFile();

    QVERIFY(peakdetectorCLI->mavenParameters->compounds.size() == 257);

}

void TestCLI::testLoadSamples() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();
    peakdetectorCLI->filenames.push_back(normalSample);
    peakdetectorCLI->filenames.push_back(blankSample);

    peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);

    QVERIFY(peakdetectorCLI->mavenParameters->samples.size() == 2);
    QVERIFY(peakdetectorCLI->mavenParameters->samples[0]->getSampleName().compare("bk_#sucyxpe_1_9.mzxml"));
    QVERIFY(peakdetectorCLI->mavenParameters->samples[1]->getSampleName().compare("blan_#sucyxpe_2_4.mzxml"));

}

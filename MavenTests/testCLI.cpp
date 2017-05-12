#include "testCLI.h"

TestCLI::TestCLI() {

    xmlPath = "bin/methods/test.xml";
    createXmlPath = "bin/methods/createTest.xml";
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

void TestCLI::testProcessXml() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();
    peakdetectorCLI->processXML((char*)xmlPath);

    QVERIFY(peakdetectorCLI->mavenParameters->ionizationMode == 1);
    QVERIFY(peakdetectorCLI->mavenParameters->charge == 3);
    QVERIFY(peakdetectorCLI->mavenParameters->minGoodGroupCount == 5);
    QVERIFY(peakdetectorCLI->mavenParameters->matchRtFlag == 1);
    QVERIFY(peakdetectorCLI->mavenParameters->processAllSlices == 0);
    QVERIFY(peakdetectorCLI->mavenParameters->C13Labeled_BPE == true);
    QVERIFY(peakdetectorCLI->mavenParameters->N15Labeled_BPE == true);
    QVERIFY(peakdetectorCLI->mavenParameters->S34Labeled_BPE == false);
    QVERIFY(peakdetectorCLI->mavenParameters->D2Labeled_BPE == false);
    QVERIFY(peakdetectorCLI->mavenParameters->grouping_maxRtWindow == 2.5);
    QVERIFY(peakdetectorCLI->mavenParameters->minGroupIntensity == 400000);
    QVERIFY(peakdetectorCLI->mavenParameters->quantileIntensity == 50);
    QVERIFY(peakdetectorCLI->mavenParameters->eicMaxGroups == 4);
    QVERIFY(peakdetectorCLI->mavenParameters->ppmMerge == 32);

    QVERIFY(peakdetectorCLI->mavenParameters->quantileQuality == 32);
    QVERIFY(peakdetectorCLI->mavenParameters->rtStepSize == 25);
    QVERIFY(peakdetectorCLI->mavenParameters->minNoNoiseObs == 3);
    QVERIFY(peakdetectorCLI->mavenParameters->eic_smoothingWindow == 20);
    QVERIFY(peakdetectorCLI->mavenParameters->minSignalBaseLineRatio == 4);
    QVERIFY(peakdetectorCLI->mavenParameters->alignSamplesFlag == 0);
    QVERIFY(peakdetectorCLI->saveJsonEIC == 1);
    QVERIFY(peakdetectorCLI->saveMzrollFile == 1);
    QVERIFY(peakdetectorCLI->mavenParameters->peakQuantitation == 2);
    QVERIFY(peakdetectorCLI->quantitationType == 2);

    peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);
    QVERIFY(peakdetectorCLI->mavenParameters->samples[0]->getSampleName().compare("bk_#sucyxpe_1_9.mzxml"));
    QVERIFY(peakdetectorCLI->mavenParameters->samples[1]->getSampleName().compare("blan_#sucyxpe_2_4.mzxml"));

	peakdetectorCLI->loadClassificationModel(peakdetectorCLI->clsfModelFilename);
    QVERIFY(peakdetectorCLI->mavenParameters->clsf->hasModel());

	peakdetectorCLI->loadCompoundsFile();
    QVERIFY(peakdetectorCLI->mavenParameters->compounds.size() == 257);

}

void TestCLI::testCreateXMLFile() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();
    peakdetectorCLI->createXMLFile((char*)createXmlPath);


    int size = 0;
    QFile myFile(createXmlPath);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size(); 
        myFile.close();
    } 

    QVERIFY(size >= 1268);

}

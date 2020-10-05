#include "testCLI.h"
#include "database.h"
#include "classifierNeuralNet.h"
#include "common/analytics.h"
#include "common/logger.h"
#include "csvreports.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "peakdetector.h"
#include "peakdetectorcli.h"
#include "utilities.h"

TestCLI::TestCLI()
{
    QString parentFolder = "ElMaven";
    QString logFile = QString::fromStdString(Logger::constant_time()
                                             + "_peakdetector_cli.log");
    QString fpath = QStandardPaths::writableLocation(
                        QStandardPaths::GenericConfigLocation)
                    + QDir::separator()
                    + parentFolder
                    + QDir::separator()
                    + logFile;
    _log = new Logger(fpath.toStdString(), true);
    _analytics = new Analytics();

    xmlPath = "bin/methods/test.xml";
    createXmlPath = "bin/methods/createTest.xml";
    clsfPath = "bin/default.model";
    dbPath = "bin/methods/KNOWNS.csv";
    normalSample = "bin/methods/testsample_1.mzxml";
    blankSample = "bin/methods/blank_1.mzxml";

}

TestCLI::~TestCLI() {

}


void TestCLI::testLoadClassificationModel() {
    
    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);
    peakdetectorCLI->loadClassificationModel(clsfPath);

    QVERIFY(peakdetectorCLI->mavenParameters->clsf->hasModel());

}

void TestCLI::testLoadCompoundsFile() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);
    peakdetectorCLI->mavenParameters->ligandDbFilename = dbPath;
    peakdetectorCLI->loadCompoundsFile();

    QVERIFY(peakdetectorCLI->mavenParameters->compounds.size() == 266);
}

void TestCLI::testLoadSamples() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);
    peakdetectorCLI->filenames.push_back(normalSample);
    peakdetectorCLI->filenames.push_back(blankSample);

    peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);

    QVERIFY(peakdetectorCLI->mavenParameters->samples.size() == 2);
    QVERIFY(peakdetectorCLI->mavenParameters->samples[0]->getSampleName().compare("testsample_1.mzxml"));
    QVERIFY(peakdetectorCLI->mavenParameters->samples[1]->getSampleName().compare("blank_1.mzxml"));

}

void TestCLI::testProcessXml() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);
    peakdetectorCLI->processXML((char*)xmlPath);

    QVERIFY(peakdetectorCLI->mavenParameters->ionizationMode == -1);
    QVERIFY(peakdetectorCLI->mavenParameters->charge == 1);
    QVERIFY(peakdetectorCLI->mavenParameters->matchRtFlag == 0);
    QVERIFY(peakdetectorCLI->mavenParameters->processAllSlices == 0);
    QVERIFY(peakdetectorCLI->mavenParameters->C13Labeled_BPE == true);
    QVERIFY(peakdetectorCLI->mavenParameters->N15Labeled_BPE == true);
    QVERIFY(peakdetectorCLI->mavenParameters->S34Labeled_BPE == false);
    QVERIFY(peakdetectorCLI->mavenParameters->D2Labeled_BPE == false);
    QVERIFY(peakdetectorCLI->mavenParameters->grouping_maxRtWindow == 2.5);
    QVERIFY(peakdetectorCLI->mavenParameters->minGroupIntensity == 500);
    QVERIFY(peakdetectorCLI->mavenParameters->quantileIntensity == 50);
    QVERIFY(peakdetectorCLI->mavenParameters->eicMaxGroups == 4);
    QVERIFY(peakdetectorCLI->mavenParameters->massCutoffMerge->getMassCutoff() == 32);

    QVERIFY(peakdetectorCLI->mavenParameters->quantileQuality == 32);
    QVERIFY(peakdetectorCLI->mavenParameters->rtStepSize == 25);
    QVERIFY(peakdetectorCLI->mavenParameters->minNoNoiseObs == 3);
    QVERIFY(peakdetectorCLI->mavenParameters->eic_smoothingWindow == 20);
    QVERIFY(peakdetectorCLI->mavenParameters->minSignalBaseLineRatio == 4);
    QVERIFY(peakdetectorCLI->mavenParameters->alignSamplesFlag == 0);
    QVERIFY(peakdetectorCLI->saveJsonEIC == 1);
    QVERIFY(peakdetectorCLI->mavenParameters->peakQuantitation == 2);
    QVERIFY(peakdetectorCLI->quantitationType == 2);
    QVERIFY(peakdetectorCLI->mavenParameters->minMz == (float)10.51);
    QVERIFY(peakdetectorCLI->mavenParameters->maxMz == (float)99.97);
    QVERIFY(peakdetectorCLI->mavenParameters->minRt == (float)5.4);
    QVERIFY(peakdetectorCLI->mavenParameters->maxRt == (float)24.6);
    QVERIFY(peakdetectorCLI->mavenParameters->minIntensity == (float)5000);
    QVERIFY(peakdetectorCLI->mavenParameters->maxIntensity  == (float)1000000);


    peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);
    QVERIFY(peakdetectorCLI->mavenParameters->samples[0]->getSampleName().compare("testsample_1.mzxml"));
    QVERIFY(peakdetectorCLI->mavenParameters->samples[1]->getSampleName().compare("blank_1.mzxml"));

	peakdetectorCLI->loadClassificationModel(peakdetectorCLI->clsfModelFilename);
    QVERIFY(peakdetectorCLI->mavenParameters->clsf->hasModel());

	peakdetectorCLI->loadCompoundsFile();
    QVERIFY(peakdetectorCLI->mavenParameters->compounds.size() == 266);
}

void TestCLI::testCreateXMLFile() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);
    peakdetectorCLI->createXMLFile((char*)createXmlPath);


    int size = 0;
    QFile myFile(createXmlPath);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size(); 
        myFile.close();
    } 

    QVERIFY(size >= 1528);

}

void TestCLI::testReduceGroups() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);

	peakdetectorCLI->processXML((char*)xmlPath);

	if (!peakdetectorCLI->status) {
		cerr << peakdetectorCLI->textStatus;
		return;
    }
    
	peakdetectorCLI->loadClassificationModel(peakdetectorCLI->clsfModelFilename);
	peakdetectorCLI->peakDetector->setMavenParameters(peakdetectorCLI->mavenParameters);
	peakdetectorCLI->loadCompoundsFile();
	peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);
	peakdetectorCLI->mavenParameters->setAverageScanTime();
	peakdetectorCLI->mavenParameters->setIonizationMode(MavenParameters::AutoDetect);

	if (peakdetectorCLI->mavenParameters->compounds.size()) {
        peakdetectorCLI->peakDetector->processCompounds(
            peakdetectorCLI->mavenParameters->compounds);
        QCOMPARE(static_cast<int>(peakdetectorCLI->mavenParameters->allgroups.size()), 23);
        peakdetectorCLI->reduceGroups();
        QCOMPARE(static_cast<int>(peakdetectorCLI->mavenParameters->allgroups.size()), 21);
	}

	delete_all(peakdetectorCLI->mavenParameters->samples);
	peakdetectorCLI->mavenParameters->samples.clear();
	peakdetectorCLI->mavenParameters->allgroups.clear();

}

void TestCLI::testWriteReport() {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(_log, _analytics);

	peakdetectorCLI->processXML((char*)xmlPath);

	if (!peakdetectorCLI->status) {
		cerr << peakdetectorCLI->textStatus;
		return;
	}

	peakdetectorCLI->loadClassificationModel(peakdetectorCLI->clsfModelFilename);
	peakdetectorCLI->peakDetector->setMavenParameters(peakdetectorCLI->mavenParameters);
	peakdetectorCLI->loadCompoundsFile();
	peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);
	peakdetectorCLI->mavenParameters->setAverageScanTime();
	peakdetectorCLI->mavenParameters->setIonizationMode(MavenParameters::AutoDetect);

	if (peakdetectorCLI->mavenParameters->compounds.size()) {
        peakdetectorCLI->peakDetector->processCompounds(
            peakdetectorCLI->mavenParameters->compounds);

        peakdetectorCLI->saveCSV(peakdetectorCLI->mavenParameters->outputdir + "testcsv", 
                                false, CSVReports::ReportType::GroupReport);
        QFileInfo csvFile(QString::fromStdString(peakdetectorCLI->mavenParameters->outputdir + "testcsv" + ".csv"));
        QVERIFY(csvFile.exists() && csvFile.isFile());
	}

	delete_all(peakdetectorCLI->mavenParameters->samples);
	peakdetectorCLI->mavenParameters->samples.clear();
	peakdetectorCLI->mavenParameters->allgroups.clear();

}

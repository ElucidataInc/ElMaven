#include "testCSVReports.h"
#include "EIC.h"
#include "mzSample.h"
#include "csvreports.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "isotopeDetection.h"
#include "classifierNeuralNet.h"
#include "utilities.h"

TestCSVReports::TestCSVReports() {
    outputfile = "output.csv";
    mzsample1 = new mzSample();
    mzsample1->sampleName = "testsample_1";
    mzsample1->_sampleOrder = 1;
    mzsample2 = new mzSample();
    mzsample2->sampleName = "bk_#sucyxpe_1_10";
    mzsample2->_sampleOrder = 2;
    mzsamples.resize(2);
    mzsamples[0] = mzsample1;
    mzsamples[1] = mzsample2;
    
}

void TestCSVReports::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestCSVReports::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestCSVReports::init() {
    // This function is executed before each test
}

void TestCSVReports::cleanup() {
    // This function is executed after each test
}

void TestCSVReports::testopenGroupReport() {



    CSVReports* csvreports =  new CSVReports(mzsamples);

    csvreports->openGroupReport(outputfile, false, true);

    ifstream ifile(outputfile.c_str());
    string temp;
    getline(ifile, temp);
    
    

    QStringList colnames;
    colnames << "label"
             << "metaGroupId"
             << "groupId"
             << "goodPeakCount"
             << "medMz"
             << "medRt"
             << "maxQuality"
             << "isotopeLabel"
             << "compound"
             << "compoundId"
             << "formula"
             << "expectedRtDiff"
             << "ppmDiff"
             << "parent"
             << "testsample_1"
             << "bk_#sucyxpe_1_10";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);

    colnames.clear();
    getline(ifile, temp);
    remove(outputfile.c_str());
    for(unsigned int i=0; i < 15; i++) { colnames << ","; }
    header = colnames.join("");
    QVERIFY(header.toStdString()==temp);
}

void TestCSVReports::testopenPeakReport() {

    CSVReports* csvreports =  new CSVReports(mzsamples);

    csvreports->openPeakReport(outputfile);

    ifstream ifile(outputfile.c_str());
    string temp;
    getline(ifile, temp);
    remove(outputfile.c_str());


    QStringList colnames;
    colnames << "groupId"
             << "compound"
             << "compoundId"
             << "formula"
             << "isotopeLabel"
             << "sample"
             << "peakMz"
             << "mzmin"
             << "mzmax"
             << "rt"
             << "rtmin"
             << "rtmax"
             << "quality"
             << "peakIntensity"
             << "peakArea"
             << "peakSplineArea"
             << "peakAreaTop"
             << "peakAreaCorrected"
             << "peakAreaTopCorrected"
             << "noNoiseObs"
             << "signalBaseLineRatio"
             << "fromBlankSample";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);
}

void TestCSVReports::testaddGroup()
{
    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();
    MavenParameters* mavenparameters = new MavenParameters();
    vector<mzSample*> samplesToLoad;
    TestUtils::loadSamplesAndParameters(samplesToLoad, mavenparameters);

    PeakDetector targetedPeakDetector;
    targetedPeakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = targetedPeakDetector.processCompounds(compounds, "compounds");
    targetedPeakDetector.processSlices(slices, "compounds");

    verifyTargetedGroupReport(samplesToLoad, mavenparameters);
    verifyTargetedPeakReport(samplesToLoad, mavenparameters);

    PeakDetector untargetedPeakDetector;
    untargetedPeakDetector.setMavenParameters(mavenparameters);
    untargetedPeakDetector.processMassSlices();

    verifyUntargetedGroupReport(samplesToLoad, mavenparameters);
    verifyUntargetedPeakReport(samplesToLoad, mavenparameters);
}

void TestCSVReports::verifyTargetedGroupReport(vector<mzSample*>& samplesToLoad,
                                               MavenParameters* mavenparameters)
{
    // we only test the write correctness for the first group
    PeakGroup& parent = mavenparameters->allgroups[0];

    bool C13Flag = mavenparameters->C13Labeled_BPE;
    bool N15Flag = mavenparameters->N15Labeled_BPE;
    bool S34Flag = mavenparameters->S34Labeled_BPE;
    bool D2Flag = mavenparameters->D2Labeled_BPE;

    IsotopeDetection::IsotopeDetectionType isoType;
    isoType = IsotopeDetection::PeakDetection;

    IsotopeDetection isotopeDetection(mavenparameters,
                                      isoType,
                                      C13Flag,
                                      N15Flag,
                                      S34Flag,
                                      D2Flag);
    isotopeDetection.pullIsotopes(&parent);

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->setMavenParameters(mavenparameters);
    csvreports->openGroupReport(outputfile, false, true);
    csvreports->addGroup(&(parent));

    string headersString;
    string cohortString;
    string parentString;
    string labelString;

    ifstream ifile(outputfile.c_str());

    getline(ifile, headersString);
    getline(ifile, cohortString);
    getline(ifile, parentString);
    getline(ifile, labelString);

    remove(outputfile.c_str());

    //check if number of columns is correct
    vector<std::string> header;
    mzUtils::splitNew(headersString, "," , header);
    QVERIFY(header.size() == 16);

    // check if parent group values are correctly written
    vector<std::string> parentValues;
    mzUtils::splitNew(parentString, "," , parentValues);
    QVERIFY(parentValues.size() == 16);
    QVERIFY(parentValues[0] == "");
    QVERIFY(parentValues[1] == "1");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QCOMPARE(stof(parentValues[4]), 665.215942f);
    QCOMPARE(stof(parentValues[5]), TestUtils::roundTo(1.463882f, 3));
    QCOMPARE(stof(parentValues[6]), 0.847695f);
    QVERIFY(parentValues[7] == "C12 PARENT");
    QVERIFY(parentValues[8] == "Stachyose");
    QVERIFY(parentValues[9] == "HMDB03553");
    QVERIFY(parentValues[10] == "C24H42O21");
    QCOMPARE(stof(parentValues[11]), TestUtils::roundTo(0.646118f, 3));
    QCOMPARE(stof(parentValues[12]), 2.018557f);
    QCOMPARE(stof(parentValues[13]), 665.215942f);
    QCOMPARE(stof(parentValues[14]), 58567.76f);
    QCOMPARE(stof(parentValues[15]), 38766.77f);

    // check if labelled child values are correctly written
    vector<std::string> childValues;
    mzUtils::splitNew(labelString, "," , childValues);
    QVERIFY(childValues.size() == 16);
    QVERIFY(childValues[0] == "");
    QVERIFY(childValues[1] == "1");
    QVERIFY(childValues[2] == "2");
    QVERIFY(childValues[3] == "0");
    QCOMPARE(stof(childValues[4]), 666.219666f);
    QCOMPARE(stof(childValues[5]), TestUtils::roundTo(1.475023f, 3));
    QCOMPARE(stof(childValues[6]), 0.130955f);
    QVERIFY(childValues[7] == "C13-label-1");
    QVERIFY(childValues[8] == "Stachyose");
    QVERIFY(childValues[9] == "HMDB03553");
    QVERIFY(childValues[10] == "C24H42O21");
    QCOMPARE(stof(childValues[11]), TestUtils::roundTo(0.634977f, 3));
    QCOMPARE(stof(childValues[12]), 2.565203f);
    QCOMPARE(stof(childValues[13]), 665.215942f);
    QCOMPARE(stof(childValues[14]), 6103.33f);
    QCOMPARE(stof(childValues[15]), 0.00f);
}

void TestCSVReports::verifyUntargetedGroupReport(vector<mzSample*>& samplesToLoad,
                                                 MavenParameters* mavenparameters)
{
    // we only test the write correctness for the first group
    PeakGroup& parent = mavenparameters->allgroups[0];

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->setMavenParameters(mavenparameters);
    csvreports->openGroupReport(outputfile, false, true);
    csvreports->addGroup(&(parent));

    string headersString;
    string cohortString;
    string parentString;

    ifstream ifile(outputfile.c_str());

    getline(ifile, headersString);
    getline(ifile, cohortString);
    getline(ifile, parentString);

    remove(outputfile.c_str());

    //check if number of columns is correct
    vector<std::string> header;
    mzUtils::splitNew(headersString, "," , header);
    QVERIFY(header.size() == 16);

    // check if group values are correctly written
    vector<std::string> parentValues;
    mzUtils::splitNew(parentString, "," , parentValues);
    QVERIFY(parentValues.size() == 16);
    QVERIFY(parentValues[0] == "");
    QVERIFY(parentValues[1] == "2");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QCOMPARE(stof(parentValues[4]), 210.150269f);
    QCOMPARE(stof(parentValues[5]), TestUtils::roundTo(16.714417f, 3));
    QCOMPARE(stof(parentValues[6]), 0.803054f);
    QVERIFY(parentValues[7] == "");
    QVERIFY(parentValues[8] == "210.150269@16.714417");
    QVERIFY(parentValues[9] == "210.150269@16.714417");
    QVERIFY(parentValues[10] == "");
    QCOMPARE(stof(parentValues[11]), TestUtils::roundTo(0.000f, 3));
    QCOMPARE(stof(parentValues[12]), 0.000000f);
    QCOMPARE(stof(parentValues[13]), 210.150269f);
    QCOMPARE(stof(parentValues[14]), 1234094464.00f);
    QCOMPARE(stof(parentValues[15]), 1199781760.00f);
}

void TestCSVReports::verifyTargetedPeakReport(vector<mzSample*>& samplesToLoad,
                                              MavenParameters* mavenparameters)
{
    // we only test the write correctness for the first group
    PeakGroup& parent = mavenparameters->allgroups[0];

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->setMavenParameters(mavenparameters);
    csvreports->openPeakReport(outputfile);
    csvreports->addGroup(&(parent));

    string headersString;
    string peakString1;
    string peakString2;

    ifstream ifile(outputfile.c_str());

    getline(ifile, headersString);
    getline(ifile, peakString1);
    getline(ifile, peakString2);

    remove(outputfile.c_str());

    //check if number of columns is correct
    vector<std::string> header;
    mzUtils::splitNew(headersString, "," , header);
    QVERIFY(header.size() == 22);

    // check if parent group values are correctly written
    vector<std::string> peakValues1;
    mzUtils::splitNew(peakString1, "," , peakValues1);
    QVERIFY(peakValues1.size() == 22);
    QVERIFY(peakValues1[0] == "1");
    QVERIFY(peakValues1[1] == "Stachyose");
    QVERIFY(peakValues1[2] == "HMDB03553");
    QVERIFY(peakValues1[3] == "C24H42O21");
    QVERIFY(peakValues1[4] == "C12 PARENT");
#ifndef WIN32
    // TODO: fix this separately, sample names are empty on Windows
    QVERIFY(peakValues1[5] == "testsample_2");
#endif
    QCOMPARE(stof(peakValues1[6]), 665.216309f);
    QCOMPARE(stof(peakValues1[7]), 665.216431f);
    QCOMPARE(stof(peakValues1[8]), 665.216736f);
    QCOMPARE(stof(peakValues1[9]), 1.466f);
    QCOMPARE(stof(peakValues1[10]), 1.426f);
    QCOMPARE(stof(peakValues1[11]), 1.520f);
    QCOMPARE(stof(peakValues1[12]), 0.666f);
    QCOMPARE(stof(peakValues1[13]), 79580.98f);
    QCOMPARE(stof(peakValues1[14]), 595991.69f);
    QCOMPARE(stof(peakValues1[15]), 635897.38f);
    QCOMPARE(stof(peakValues1[16]), 58567.76f);
    QCOMPARE(stof(peakValues1[17]), 595991.69f);
    QCOMPARE(stof(peakValues1[18]), 58567.76f);
#ifndef WIN32
    QVERIFY(peakValues1[19] == "18");
#endif
    QCOMPARE(stof(peakValues1[20]), 9.12f);
    QVERIFY(peakValues1[21] == "0");

    // check if labelled child values are correctly written
    vector<std::string> peakValues2;
    mzUtils::splitNew(peakString2, "," , peakValues2);
    QVERIFY(peakValues2.size() == 22);
    QVERIFY(peakValues2[0] == "1");
    QVERIFY(peakValues2[1] == "Stachyose");
    QVERIFY(peakValues2[2] == "HMDB03553");
    QVERIFY(peakValues2[3] == "C24H42O21");
    QVERIFY(peakValues2[4] == "C12 PARENT");
#ifndef WIN32
    QVERIFY(peakValues2[5] == "testsample_3");
#endif
    QCOMPARE(stof(peakValues2[6]), 665.214966f);
    QCOMPARE(stof(peakValues2[7]), 665.215210f);
    QCOMPARE(stof(peakValues2[8]), 665.215088f);
    QCOMPARE(stof(peakValues2[9]), 1.462f);
    QCOMPARE(stof(peakValues2[10]), 1.377f);
    QCOMPARE(stof(peakValues2[11]), 1.502f);
    QCOMPARE(stof(peakValues2[12]), 0.848f);
    QCOMPARE(stof(peakValues2[13]), 54565.93f);
    QCOMPARE(stof(peakValues2[14]), 359001.78f);
    QCOMPARE(stof(peakValues2[15]), 355344.88f);
    QCOMPARE(stof(peakValues2[16]), 38766.77f);
    QCOMPARE(stof(peakValues2[17]), 359001.78f);
    QCOMPARE(stof(peakValues2[18]), 38766.77f);
    #ifndef WIN32
    QVERIFY(peakValues2[19] == "16");
    #endif
    QCOMPARE(stof(peakValues2[20]), 5456.59f);
    QVERIFY(peakValues2[21] == "0");
}

void TestCSVReports::verifyUntargetedPeakReport(vector<mzSample*>& samplesToLoad,
                                                MavenParameters* mavenparameters)
{
    // we only test the write correctness for the first group
    PeakGroup& parent = mavenparameters->allgroups[0];

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->setMavenParameters(mavenparameters);
    csvreports->openPeakReport(outputfile);
    csvreports->addGroup(&(parent));

    string headersString;
    string peakString1;
    string peakString2;

    ifstream ifile(outputfile.c_str());

    getline(ifile, headersString);
    getline(ifile, peakString1);
    getline(ifile, peakString2);

    remove(outputfile.c_str());

    //check if number of columns is correct
    vector<std::string> header;
    mzUtils::splitNew(headersString, "," , header);
    QVERIFY(header.size() == 22);

    // check if parent group values are correctly written
    vector<std::string> peakValues1;
    mzUtils::splitNew(peakString1, "," , peakValues1);
    QVERIFY(peakValues1.size() == 22);
    QVERIFY(peakValues1[0] == "2");
    QVERIFY(peakValues1[1] == "210.150269@16.714417");
    QVERIFY(peakValues1[2] == "210.150269@16.714417");
    QVERIFY(peakValues1[3] == "");
    QVERIFY(peakValues1[4] == "");
#ifndef WIN32
    QVERIFY(peakValues1[5] == "testsample_2");
#endif
    QCOMPARE(stof(peakValues1[6]), 210.150375f);
    QCOMPARE(stof(peakValues1[7]), 210.150452f);
    QCOMPARE(stof(peakValues1[8]), 210.150452f);
    QCOMPARE(stof(peakValues1[9]), 16.710f);
    QCOMPARE(stof(peakValues1[10]), 16.603f);
    QCOMPARE(stof(peakValues1[11]), 17.392f);
    QCOMPARE(stof(peakValues1[12]), 0.801f);
    QCOMPARE(stof(peakValues1[13]), 1255329664.00f);
    QCOMPARE(stof(peakValues1[14]), 30111416320.00f);
    QCOMPARE(stof(peakValues1[15]), 30103683072.00f);
    QCOMPARE(stof(peakValues1[16]), 1234160640.00f);
    QCOMPARE(stof(peakValues1[17]), 30099625984.00f);
    QCOMPARE(stof(peakValues1[18]), 1234094464.00f);
    QVERIFY(peakValues1[19] == "178");
    QCOMPARE(stof(peakValues1[20]), 116.51f);
    QVERIFY(peakValues1[21] == "0");
}

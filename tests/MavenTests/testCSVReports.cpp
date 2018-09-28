#include "testCSVReports.h"


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

    csvreports->openGroupReport(outputfile,true);

    ifstream ifile(outputfile.c_str());
    string temp;
    getline(ifile, temp);
    
    

    QStringList colnames;
    colnames << "label" << "metaGroupId" << "groupId" << "goodPeakCount"
                << "medMz" << "medRt" << "maxQuality" << "isotopeLabel" << "compound"
                << "compoundId" << "formula" << "expectedRtDiff" << "ppmDiff" 
                << "parent" << "testsample_1" << "bk_#sucyxpe_1_10";

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
    colnames << "groupId" << "compound" << "compoundId" << "formula" << "sample" << "peakMz"
             << "medianMz" << "baseMz" << "rt" << "rtmin" << "rtmax" << "quality"
             << "peakIntensity" << "peakArea" << "peakSplineArea" << "peakAreaTop"
             << "peakAreaCorrected" << "peakAreaTopCorrected" << "noNoiseObs" << "signalBaseLineRatio"
             << "fromBlankSample";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);
}

void TestCSVReports::testaddGroup()
{
    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    MavenParameters* mavenparameters = new MavenParameters();
    vector<mzSample*> samplesToLoad;
    common::loadSamplesAndParameters(samplesToLoad, mavenparameters);

    PeakDetector targetedPeakDetector;
    targetedPeakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = targetedPeakDetector.processCompounds(compounds, "compounds");
    targetedPeakDetector.processSlices(slices, "compounds");

    verifyTargetedGroupReport(samplesToLoad, mavenparameters);

    PeakDetector untargetedPeakDetector;
    untargetedPeakDetector.setMavenParameters(mavenparameters);
    untargetedPeakDetector.processMassSlices();

    verifyUntargetedGroupReport(samplesToLoad, mavenparameters);
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
    csvreports->openGroupReport(outputfile,true);
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
    QVERIFY(parentValues[1] == "1");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QVERIFY(parentValues[4] == "665.2159");
    QVERIFY(parentValues[5] == "1.463882");
    QVERIFY(parentValues[6] == "0.8476948");
    QVERIFY(parentValues[7] == "C12 PARENT");
    QVERIFY(parentValues[8] == "Stachyose");
    QVERIFY(parentValues[9] == "HMDB03553");
    QVERIFY(parentValues[10] == "C24H42O21");
    QVERIFY(parentValues[11] == "0.6461183");
    QVERIFY(parentValues[12] == "2.018557");
    QVERIFY(parentValues[13] == "665.2159");
    QVERIFY(parentValues[14] == "58567.76");
    QVERIFY(parentValues[15] == "38766.77");

    // check if labelled child values are correctly written
    vector<std::string> childValues;
    mzUtils::splitNew(labelString, "," , childValues);
    QVERIFY(childValues[1] == "1");
    QVERIFY(childValues[2] == "2");
    QVERIFY(childValues[3] == "0");
    QVERIFY(childValues[4] == "666.2197");
    QVERIFY(childValues[5] == "1.475023");
    QVERIFY(childValues[6] == "0.1309547");
    QVERIFY(childValues[7] == "C13-label-1");
    QVERIFY(childValues[8] == "Stachyose");
    QVERIFY(childValues[9] == "HMDB03553");
    QVERIFY(childValues[10] == "C24H42O21");
    QVERIFY(childValues[11] == "0.6349765");
    QVERIFY(childValues[12] == "2.565203");
    QVERIFY(childValues[13] == "665.2159");
    QVERIFY(childValues[14] == "6103.332");
    QVERIFY(childValues[15] == "0");
}

void TestCSVReports::verifyUntargetedGroupReport(vector<mzSample*>& samplesToLoad,
                                                 MavenParameters* mavenparameters)
{
    // we only test the write correctness for the first group
    PeakGroup& parent = mavenparameters->allgroups[0];

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->setMavenParameters(mavenparameters);
    csvreports->openGroupReport(outputfile,true);
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
    QVERIFY(parentValues[1] == "15");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QVERIFY(parentValues[4] == "210.1503");
    QVERIFY(parentValues[5] == "16.71442");
    QVERIFY(parentValues[6] == "0.8033339");
    QVERIFY(parentValues[7] == "");
    QVERIFY(parentValues[8] == "210.150269@16.714417");
    QVERIFY(parentValues[9] == "210.150269@16.714417");
    QVERIFY(parentValues[10] == "");
    QVERIFY(parentValues[11] == "0");
    QVERIFY(parentValues[12] == "0");
    QVERIFY(parentValues[13] == "210.1503");
    QVERIFY(parentValues[14] == "1.234094e+09");
    QVERIFY(parentValues[15] == "1.199782e+09");
}

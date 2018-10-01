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
    QVERIFY(parentValues[0] == "");
    QVERIFY(parentValues[1] == "1");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QVERIFY(parentValues[4] == "665.215942");
    QVERIFY(parentValues[5] == "1.463882");
    QVERIFY(parentValues[6] == "0.847695");
    QVERIFY(parentValues[7] == "C12 PARENT");
    QVERIFY(parentValues[8] == "Stachyose");
    QVERIFY(parentValues[9] == "HMDB03553");
    QVERIFY(parentValues[10] == "C24H42O21");
    QVERIFY(parentValues[11] == "0.646118");
    QVERIFY(parentValues[12] == "2.018557");
    QVERIFY(parentValues[13] == "665.215942");
    QVERIFY(parentValues[14] == "58567.76");
    QVERIFY(parentValues[15] == "38766.77");

    // check if labelled child values are correctly written
    vector<std::string> childValues;
    mzUtils::splitNew(labelString, "," , childValues);
    QVERIFY(childValues[0] == "");
    QVERIFY(childValues[1] == "1");
    QVERIFY(childValues[2] == "2");
    QVERIFY(childValues[3] == "0");
    QVERIFY(childValues[4] == "666.219666");
    QVERIFY(childValues[5] == "1.475023");
    QVERIFY(childValues[6] == "0.130955");
    QVERIFY(childValues[7] == "C13-label-1");
    QVERIFY(childValues[8] == "Stachyose");
    QVERIFY(childValues[9] == "HMDB03553");
    QVERIFY(childValues[10] == "C24H42O21");
    QVERIFY(childValues[11] == "0.634977");
    QVERIFY(childValues[12] == "2.565203");
    QVERIFY(childValues[13] == "665.215942");
    QVERIFY(childValues[14] == "6103.33");
    QVERIFY(childValues[15] == "0.00");
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
    QVERIFY(parentValues[0] == "");
    QVERIFY(parentValues[1] == "15");
    QVERIFY(parentValues[2] == "1");
    QVERIFY(parentValues[3] == "2");
    QVERIFY(parentValues[4] == "210.150269");
    QVERIFY(parentValues[5] == "16.714417");
    QVERIFY(parentValues[6] == "0.803334");
    QVERIFY(parentValues[7] == "");
    QVERIFY(parentValues[8] == "210.150269@16.714417");
    QVERIFY(parentValues[9] == "210.150269@16.714417");
    QVERIFY(parentValues[10] == "");
    QVERIFY(parentValues[11] == "0.000000");
    QVERIFY(parentValues[12] == "0.000000");
    QVERIFY(parentValues[13] == "210.150269");
    QVERIFY(parentValues[14] == "1234094464.00");
    QVERIFY(parentValues[15] == "1199781760.00");
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
    QVERIFY(header.size() == 21);

    // check if parent group values are correctly written
    vector<std::string> peakValues1;
    mzUtils::splitNew(peakString1, "," , peakValues1);
    QVERIFY(peakValues1[0] == "1");
    QVERIFY(peakValues1[1] == "Stachyose");
    QVERIFY(peakValues1[2] == "HMDB03553");
    QVERIFY(peakValues1[3] == "C24H42O21");
    QVERIFY(peakValues1[4] == "testsample_2");
    QVERIFY(peakValues1[5] == "665.216309");
    QVERIFY(peakValues1[6] == "665.216431");
    QVERIFY(peakValues1[7] == "665.216736");
    QVERIFY(peakValues1[8] == "1.466");
    QVERIFY(peakValues1[9] == "1.426");
    QVERIFY(peakValues1[10] == "1.520");
    QVERIFY(peakValues1[11] == "0.667");
    QVERIFY(peakValues1[12] == "79580.98");
    QVERIFY(peakValues1[13] == "595991.69");
    QVERIFY(peakValues1[14] == "635897.38");
    QVERIFY(peakValues1[15] == "58567.76");
    QVERIFY(peakValues1[16] == "595991.69");
    QVERIFY(peakValues1[17] == "58567.76");
    QVERIFY(peakValues1[18] == "18");
    QVERIFY(peakValues1[19] == "9.12");
    QVERIFY(peakValues1[20] == "0");

    // check if labelled child values are correctly written
    vector<std::string> peakValues2;
    mzUtils::splitNew(peakString2, "," , peakValues2);
    QVERIFY(peakValues2[0] == "1");
    QVERIFY(peakValues2[1] == "Stachyose");
    QVERIFY(peakValues2[2] == "HMDB03553");
    QVERIFY(peakValues2[3] == "C24H42O21");
    QVERIFY(peakValues2[4] == "testsample_3");
    QVERIFY(peakValues2[5] == "665.214966");
    QVERIFY(peakValues2[6] == "665.215210");
    QVERIFY(peakValues2[7] == "665.215088");
    QVERIFY(peakValues2[8] == "1.462");
    QVERIFY(peakValues2[9] == "1.377");
    QVERIFY(peakValues2[10] == "1.502");
    QVERIFY(peakValues2[11] == "0.848");
    QVERIFY(peakValues2[12] == "54565.93");
    QVERIFY(peakValues2[13] == "359001.78");
    QVERIFY(peakValues2[14] == "355344.88");
    QVERIFY(peakValues2[15] == "38766.77");
    QVERIFY(peakValues2[16] == "359001.78");
    QVERIFY(peakValues2[17] == "38766.77");
    QVERIFY(peakValues2[18] == "16");
    QVERIFY(peakValues2[19] == "5456.59");
    QVERIFY(peakValues2[20] == "0");
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
    QVERIFY(header.size() == 21);

    // check if parent group values are correctly written
    vector<std::string> peakValues1;
    mzUtils::splitNew(peakString1, "," , peakValues1);
    QVERIFY(peakValues1[0] == "15");
    QVERIFY(peakValues1[1] == "210.150269@16.714417");
    QVERIFY(peakValues1[2] == "210.150269@16.714417");
    QVERIFY(peakValues1[3] == "");
    QVERIFY(peakValues1[4] == "testsample_2");
    QVERIFY(peakValues1[5] == "210.150375");
    QVERIFY(peakValues1[6] == "210.150452");
    QVERIFY(peakValues1[7] == "210.150452");
    QVERIFY(peakValues1[8] == "16.710");
    QVERIFY(peakValues1[9] == "16.603");
    QVERIFY(peakValues1[10] == "17.392");
    QVERIFY(peakValues1[11] == "0.801");
    QVERIFY(peakValues1[12] == "1255329664.00");
    QVERIFY(peakValues1[13] == "30111416320.00");
    QVERIFY(peakValues1[14] == "30103683072.00");
    QVERIFY(peakValues1[15] == "1234160640.00");
    QVERIFY(peakValues1[16] == "30099625984.00");
    QVERIFY(peakValues1[17] == "1234094464.00");
    QVERIFY(peakValues1[18] == "178");
    QVERIFY(peakValues1[19] == "116.51");
    QVERIFY(peakValues1[20] == "0");

    // check if labelled child values are correctly written
    vector<std::string> peakValues2;
    mzUtils::splitNew(peakString2, "," , peakValues2);
    QVERIFY(peakValues2[0] == "15");
    QVERIFY(peakValues2[1] == "210.150269@16.714417");
    QVERIFY(peakValues2[2] == "210.150269@16.714417");
    QVERIFY(peakValues2[3] == "");
    QVERIFY(peakValues2[4] == "testsample_3");
    QVERIFY(peakValues2[5] == "210.150055");
    QVERIFY(peakValues2[6] == "210.150101");
    QVERIFY(peakValues2[7] == "210.150101");
    QVERIFY(peakValues2[8] == "16.719");
    QVERIFY(peakValues2[9] == "16.612");
    QVERIFY(peakValues2[10] == "17.401");
    QVERIFY(peakValues2[11] == "0.803");
    QVERIFY(peakValues2[12] == "1221961088.00");
    QVERIFY(peakValues2[13] == "29895890944.00");
    QVERIFY(peakValues2[14] == "29900089344.00");
    QVERIFY(peakValues2[15] == "1199850368.00");
    QVERIFY(peakValues2[16] == "29883682816.00");
    QVERIFY(peakValues2[17] == "1199781760.00");
    QVERIFY(peakValues2[18] == "178");
    QVERIFY(peakValues2[19] == "114.35");
    QVERIFY(peakValues2[20] == "0");
}

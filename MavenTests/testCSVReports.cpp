#include "testCSVReports.h"


TestCSVReports::TestCSVReports() {
    outputfile = "output.csv";
    mzsample1 = new mzSample();
    mzsample1->sampleName = "bk_#sucyxpe_1_9";
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

    csvreports->openGroupReport(outputfile);

    ifstream ifile(outputfile.c_str());
    string temp;
    getline(ifile, temp);
    remove(outputfile.c_str());
    

    QStringList colnames;
    colnames << "label" << "metaGroupId" << "groupId" << "goodPeakCount"
                << "medMz" << "medRt" << "maxQuality" << "note" << "compound"
                << "compoundId" << "expectedRtDiff" << "ppmDiff" << "parent"
                <<"bk_#sucyxpe_1_9"<<"bk_#sucyxpe_1_10";

    QString header = colnames.join(",");
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
    colnames << "groupId" << "compound" << "compoundId" << "sample" << "peakMz"
             << "medianMz" << "baseMz" << "rt" << "rtmin" << "rtmax" << "quality"
             << "peakIntensity" << "peakArea" << "peakAreaTop"
             << "peakAreaCorrected" << "noNoiseObs" << "signalBaseLineRatio"
             << "fromBlankSample";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);
}

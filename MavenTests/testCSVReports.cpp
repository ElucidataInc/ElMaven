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
                << "medMz" << "medRt" << "maxQuality" << "note" << "compound"
                << "compoundId" << "formula" << "expectedRtDiff" << "ppmDiff" 
                << "parent" << "testsample_1" << "bk_#sucyxpe_1_10";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);

    colnames.clear();
    getline(ifile, temp);
    remove(outputfile.c_str());
    for(unsigned int i=0; i < 14; i++) { colnames << ","; }
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
             << "peakIntensity" << "peakArea" << "peakAreaTop"
             << "peakAreaCorrected" << "peakAreaTopCorrected" << "noNoiseObs" << "signalBaseLineRatio"
             << "fromBlankSample";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);    
}

void TestCSVReports::testaddGroups() {

    const char* loadCompoundDB;
    QStringList files;
    loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    files << "bin/methods/testsample_2.mzxml" << "bin/methods/testsample_3.mzxml";

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds =
        DBS.getCopoundsSubset("qe3_v11_2016_04_29");
    vector<mzSample*> samplesToLoad;

    for (int i = 0; i < files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    MavenParameters* mavenparameters = new MavenParameters();
    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    string loadmodel = "bin/default.model";
    clsf->loadModel(loadmodel);
    mavenparameters->clsf = clsf;
    mavenparameters->compoundPPMWindow = 10;
    mavenparameters->ionizationMode = +1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices =
        peakDetector.processCompounds(compounds, "compounds");
    peakDetector.processSlices(slices, "compounds");
    PeakGroup& parent = mavenparameters->allgroups[0];
    peakDetector.pullIsotopes(&parent);

    CSVReports* csvreports =  new CSVReports(samplesToLoad);
    csvreports->openGroupReport(outputfile,true);
    csvreports->addGroup(&(parent));

    ifstream ifile(outputfile.c_str());
    string temp;
    getline(ifile, temp);
    getline(ifile, temp);
    getline(ifile, temp);
    remove(outputfile.c_str());

    QStringList colnames;
    colnames << "" << "0" << "1" << "1" << "786.1589"
    <<"14.93828"<<"0.6369841"<<"C12 PARENT"<<"FAD"
    <<"HMDB01248"<<"C27H33N9O15P2" << "1.491715"<<"2562.053"<<"786.1589"
    <<"NA"<<"NA";

    QString header = colnames.join(",");
    QVERIFY(header.toStdString()==temp);

}

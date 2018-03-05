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

void TestCSVReports::testExport() {

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
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    mavenparameters->massCutoffMerge->setMassCutoffAndType(10,"ppm");
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

    bool C13Flag = mavenparameters->C13Labeled_BPE;
    bool N15Flag = mavenparameters->N15Labeled_BPE;
    bool S34Flag = mavenparameters->S34Labeled_BPE;
    bool D2Flag = mavenparameters->D2Labeled_BPE;

    IsotopeDetection::IsotopeDetectionType isoType;
    isoType = IsotopeDetection::PeakDetection;

	IsotopeDetection isotopeDetection(
                mavenparameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
    isotopeDetection.pullIsotopes(&parent);

    PeakGroup::QType quantitationType = PeakGroup::AreaTop;
    CSVReports::ExportType exportType = CSVReports::GroupExport;
    int selectionFlag = -1;
    bool includeSetNamesLine = false;

    CSVReports* csvExport = new CSVReports(samplesToLoad, mavenparameters, quantitationType,
                                     outputfile, exportType, selectionFlag, includeSetNamesLine);

    csvExport->addItem(&(parent));
    if(csvExport->exportGroup() == false){
        cerr << "error " << csvExport->getErrorReport() << endl;
    }


    ifstream ifile(outputfile.c_str());
    string temp;

    getline(ifile, temp);
    vector<std::string> header;
    mzUtils::splitNew(temp, "," , header);
    QVERIFY(header.size() == 16);

    getline(ifile, temp);
    std::size_t found = temp.find("HMDB01248");
    QVERIFY(found != std::string::npos);

}

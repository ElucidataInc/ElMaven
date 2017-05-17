#include "testPeakDetection.h"

TestPeakDetection::TestPeakDetection() {
    loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    files << "bin/methods/sample_#sucyxpe_2_5.mzxml" << "bin/methods/sample_#sucyxpe_2_6.mzxml";

}

void TestPeakDetection::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestPeakDetection::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestPeakDetection::init() {
    // This function is executed before each test
}

void TestPeakDetection::cleanup() {
    // This function is executed after each test
}

void TestPeakDetection::testProcessCompound() {
    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("qe3_v11_2016_04_29");

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->cmpdMassAccValue = 10;
    mavenparameters->cmpdMassAccType = 0;
    mavenparameters->ionizationMode = +1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds, "compounds");

    QVERIFY(true);

}

void TestPeakDetection::testPullEICs() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    pair<string,double> pr = make_pair("ppm",10);
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (int i = 0; i <  files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[2];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(pr, ionizationMode);

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    vector<EIC*> eics = PeakDetector::pullEICs(slice, mavenparameters->samples,
                                    1, mavenparameters->eic_smoothingWindow,
                                    mavenparameters->eic_smoothingAlgorithm, mavenparameters->amuQ1,
                                    mavenparameters->amuQ3,
                                    mavenparameters->baseline_smoothingWindow,
                                    mavenparameters->baseline_dropTopX);
    QVERIFY(eics.size() == 2);
}

void TestPeakDetection::testprocessSlices() {

    vector<PeakGroup> allgroups = common::getGroupsFromProcessCompounds();
    QVERIFY(allgroups.size() > 0);

}

void TestPeakDetection::testpullIsotopes() {
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
    mavenparameters->cmpdMassAccValue = 10;
    mavenparameters->cmpdMassAccType = 0;
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
    QVERIFY(parent.childCount() > 0);
}

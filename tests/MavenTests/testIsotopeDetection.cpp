#include "testIsotopeDetection.h"

TestIsotopeDetection::TestIsotopeDetection() {
    loadCompoundDB = "bin/methods/KNOWNS.csv";
    files << "bin/methods/testsample_2.mzxml" << "bin/methods/testsample_3.mzxml";
}

void TestIsotopeDetection::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
}

void TestIsotopeDetection::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestIsotopeDetection::init() {
    // This function is executed before each test
}

void TestIsotopeDetection::cleanup() {
    // This function is executed after each test
}

void TestIsotopeDetection::testpullIsotopes() {
    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("KNOWNS");
    vector<mzSample*> samplesToLoad;

    for (int i = 0; i < files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    clsf->loadModel("bin/default.model");
    mavenparameters->clsf = clsf;
    mavenparameters->ionizationMode = +1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds, "compounds");
    peakDetector.processSlices(slices, "compounds");
    
    PeakGroup& parent = mavenparameters->allgroups[0];

    IsotopeDetection isotopeDetection1(
        mavenparameters,
        IsotopeDetection::PeakDetection,
        mavenparameters->C13Labeled_BPE,
        mavenparameters->N15Labeled_BPE,
        mavenparameters->S34Labeled_BPE,
        mavenparameters->D2Labeled_BPE
    );

    isotopeDetection1.pullIsotopes(&parent);
    
    //verify number of isotopes
    QVERIFY(parent.childCount() == 7);

    //verify if isotopic correlation filter works
    mavenparameters->minIsotopicCorrelation = 1;
    parent = mavenparameters->allgroups[1];

    IsotopeDetection isotopeDetection2(
        mavenparameters,
        IsotopeDetection::PeakDetection,
        mavenparameters->C13Labeled_BPE,
        mavenparameters->N15Labeled_BPE,
        mavenparameters->S34Labeled_BPE,
        mavenparameters->D2Labeled_BPE
    );

    isotopeDetection2.pullIsotopes(&parent);

    //childCount for this group is 3 for minIsotopicCorrelation = 0.2
    QVERIFY(parent.childCount() == 1);

    //verify if peaks are within specified rt distance
    mavenparameters->minIsotopicCorrelation = 0.2;
    mavenparameters->maxIsotopeScanDiff = 2;
    mavenparameters->avgScanTime = 0.5;
    parent = mavenparameters->allgroups[3];
    mzSample* sample = mavenparameters->samples[0];
    Peak* parentPeak = parent.getPeak(sample);
    float parentRt = parentPeak->rt;
    float maxRtDiff = mavenparameters->maxIsotopeScanDiff*mavenparameters->avgScanTime;

    IsotopeDetection isotopeDetection3(
        mavenparameters,
        IsotopeDetection::PeakDetection,
        mavenparameters->C13Labeled_BPE,
        mavenparameters->N15Labeled_BPE,
        mavenparameters->S34Labeled_BPE,
        mavenparameters->D2Labeled_BPE
    );

    isotopeDetection3.pullIsotopes(&parent);
    
    int outlier = 0;
    for (int i = 0; i < parent.children.size(); i++) 
    {
        PeakGroup& child = parent.children[i];
        Peak* childPeak = child.getPeak(sample);
        if (!childPeak) continue;
        float rtDiff = abs(parentRt - childPeak->rt);
        if (rtDiff > maxRtDiff) outlier++;
    }
    QVERIFY(outlier == 0);
    
    //test for different labels
    mavenparameters->C13Labeled_BPE = true;
    mavenparameters->N15Labeled_BPE = false;
    mavenparameters->D2Labeled_BPE = false;
    mavenparameters->S34Labeled_BPE = true;
    mavenparameters->minIsotopicCorrelation = 0.2;
    mavenparameters->maxIsotopeScanDiff = 5;
    mavenparameters->avgScanTime = 0.2;
    parent = mavenparameters->allgroups[4];

    IsotopeDetection isotopeDetection4(
        mavenparameters,
        IsotopeDetection::PeakDetection,
        mavenparameters->C13Labeled_BPE,
        mavenparameters->N15Labeled_BPE,
        mavenparameters->S34Labeled_BPE,
        mavenparameters->D2Labeled_BPE
    );

    isotopeDetection4.pullIsotopes(&parent);

    int N15_BPE = 0;
    int D2_BPE = 0;
    int C13_BPE = 0;
    for (int i = 0; i < parent.children.size(); i++)
    {
        PeakGroup& child = parent.children[i];
        string isotopeName = child.tagString;
        if (isotopeName.find(N15_LABEL) != string::npos || isotopeName.find(C13N15_LABEL) != string::npos)
            N15_BPE++;
        if (isotopeName.find(H2_LABEL) != string::npos || isotopeName.find(C13H2_LABEL) != string::npos)
            D2_BPE++;
        if (isotopeName.find(C13_LABEL) != string::npos)
            C13_BPE++;
    }
    QVERIFY(N15_BPE == 0);
    QVERIFY(D2_BPE == 0);
    QVERIFY(C13_BPE > 0);
}
#include "testPeakDetection.h"

TestPeakDetection::TestPeakDetection() {
    loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    loadCompoundDB1 = "bin/methods/KNOWNS.csv";
    files << "bin/methods/testsample_2.mzxml" << "bin/methods/testsample_3.mzxml";

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
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
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
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (int i = 0; i <  files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");

    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[2];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

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
                                    mavenparameters->baseline_dropTopX,
                                    mavenparameters->minSignalBaselineDifference,
                                    mavenparameters->eicType,
                                    mavenparameters->filterline);
    QVERIFY(eics.size() == 2);
}

void TestPeakDetection::testprocessSlices() {

    vector<PeakGroup> allgroups = common::getGroupsFromProcessCompounds();
    QVERIFY(allgroups.size() > 0);

}

void TestPeakDetection::testpullIsotopes() {
    DBS.loadCompoundCSVFile(loadCompoundDB1);
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
    mavenparameters->isotopeAtom["ShowIsotopes"] = true;
    mavenparameters->isotopeAtom["C13Labeled_BPE"] = true;
    mavenparameters->isotopeAtom["N15Labeled_BPE"] = true;
    mavenparameters->isotopeAtom["D2Labeled_BPE"] = true;
    mavenparameters->isotopeAtom["S34Labeled_BPE"] = true;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds, "compounds");
    peakDetector.processSlices(slices, "compounds");
    
    PeakGroup& parent = mavenparameters->allgroups[0];
    peakDetector.pullIsotopes(&parent);
    
    //verify number of isotopes
    QVERIFY(parent.childCount() == 7);

    //verify if isotopic correlation filter works
    mavenparameters->minIsotopicCorrelation = 1;
    parent = mavenparameters->allgroups[1];
    peakDetector.pullIsotopes(&parent);

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
    
    peakDetector.pullIsotopes(&parent);
    
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

    peakDetector.pullIsotopes(&parent);

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

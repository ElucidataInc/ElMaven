#include "testEIC.h"

TestEIC::TestEIC() {
    loadFile = "bin/methods/testsample_1.mzxml";
    loadGoodSample = "bin/methods/testsample_2.mzxml";

    files << "bin/methods/testsample_2.mzxml" << "bin/methods/testsample_3.mzxml";

}

void TestEIC::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printettings();
}

void TestEIC::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestEIC::init() {
    // This function is executed before each test
}

void TestEIC::cleanup() {
    // This function is executed after each test
}

void TestEIC::testgetEIC() {
    unsigned int numberOfScans = 445;
    mzSample* mzsample = new mzSample();
    mzsample->loadSample(loadFile);
    EIC e;

    bool status = e.makeEICSlice(mzsample, 180.002,180.004, 0, 2, 1, 0, "");
    QVERIFY(e.intensity.size() == numberOfScans && status);
}

void TestEIC::testcomputeSpline() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12.0, 16.0, 1, 0, "");
    
    //if eic exists, perform smoothing
    EIC::SmootherType smootherType = 
        (EIC::SmootherType) 1;

    e->setSmootherType(smootherType);
    e->computeSpline(10);
    QVERIFY(true);
}

void TestEIC::testgetPeakPositions() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12.0, 16.0, 1, 0, "");
    
    EIC::SmootherType smootherType =
            (EIC::SmootherType) 1;
    e->setSmootherType(smootherType);
    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(80);
    e->setFilterSignalBaselineDiff(0);
    e->getPeakPositions(10);
    QVERIFY(true);
}

void TestEIC::testcomputeBaseLine() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;
    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12.0, 16.0, 1, 0, "");
    
    EIC::SmootherType smootherType =
            (EIC::SmootherType) 1;
    e->setSmootherType(smootherType);
    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(60);
    e->computeBaseLine(5, 60);
    QVERIFY(true);
}

void TestEIC::testfindPeakBounds() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12.0, 16.0, 1, 0, "");
    
    //if eic exists, perform smoothing
    EIC::SmootherType smootherType = 
        (EIC::SmootherType) 1;

    e->setSmootherType(smootherType);
    e->computeSpline(10);
    e->findPeaks();
    e->computeBaseLine(5, 80);
    e->findPeakBounds(e->peaks[10]);

    vector<float> intensity;
    for(unsigned int i =  e->peaks[10].minpos ; i <= e->peaks[10].maxpos; i++) {
         intensity.push_back(e->intensity[i]);
    }

    QVERIFY(common::floatCompare(*max_element(intensity.begin(), intensity.end()), e->intensity[e->peaks[10].pos]));
   
}


void TestEIC:: testGetPeakDetails() {
    mzSample* mzsample = new mzSample();
    EIC* e = NULL;

    mzsample->loadSample(loadGoodSample);
    e = mzsample->getEIC(402.9929, 402.9969, 12.0, 16.0, 1, 0, "");
    
    //if eic exists, perform smoothing
    EIC::SmootherType smootherType = 
        (EIC::SmootherType) 1;

    e->setSmootherType(smootherType);
    e->computeSpline(10);
    e->findPeaks();
    e->computeBaseLine(5, 80);
    e->findPeakBounds(e->peaks[10]);
    e->getPeakDetails(e->peaks[10]);

    vector<float> intensity;
    float peakArea = 0.;
    unsigned int noNoiseObs = 0;
    float baselineArea = 0.;
    for(unsigned int i =  e->peaks[10].minpos ; i <= e->peaks[10].maxpos; i++) {
         intensity.push_back(e->intensity[i]);
         peakArea += e->intensity[i];
         baselineArea += e->baseline[i];
         if (e->intensity[i] > e->baseline[i]) noNoiseObs ++;
    }

    float peakAreaCorrected = peakArea - baselineArea;

    QVERIFY(common::floatCompare(*max_element(intensity.begin(), intensity.end()), e->peaks[10].peakIntensity));

    QVERIFY(e->peaks[10].noNoiseObs == noNoiseObs);

    QVERIFY(common::floatCompare(peakArea, e->peaks[10].peakArea));

    QVERIFY(common::floatCompare(peakAreaCorrected, e->peaks[10].peakAreaCorrected));

    QVERIFY(common::floatCompare(peakAreaCorrected / (e->totalIntensity + 1), e->peaks[10].peakAreaFractional));
    
    QVERIFY(e->peaks[10].gaussFitSigma > 0);
    
    QVERIFY(e->peaks[10].gaussFitR2 > 0);
}

void TestEIC:: testgroupPeaks() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    float compoundPPMWindow = 10;
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (int i = 0; i <  files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();

    // for(std::vector<Compound*>::iterator it = compounds.begin(); it != compounds.end(); ++it) {
    //     cerr << (*it)->name << endl; 
    // }
    
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[4];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(compoundPPMWindow, ionizationMode);

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;
    mavenparameters->grouping_maxRtWindow = 0.5;
    mavenparameters->distXWeight = 1;
    mavenparameters->distYWeight = 5;
    mavenparameters->minPeakRtDiff = 0;
    mavenparameters->useOverlap = 0;


    vector<EIC*> eics = PeakDetector::pullEICs(slice, mavenparameters->samples,
                                    1, mavenparameters->eic_smoothingWindow,
                                    mavenparameters->eic_smoothingAlgorithm, mavenparameters->amuQ1,
                                    mavenparameters->amuQ3,
                                    mavenparameters->baseline_smoothingWindow,
                                    mavenparameters->baseline_dropTopX,
                                    mavenparameters->minSignalBaselineDifference,
                                    mavenparameters->eicType,
                                    mavenparameters->filterline);

    vector<PeakGroup> peakgroups = EIC::groupPeaks(eics,
                                                    mavenparameters->eic_smoothingWindow,
                                                    mavenparameters->grouping_maxRtWindow,
                                                    mavenparameters->minQuality,
                                                    mavenparameters->distXWeight,
                                                    mavenparameters->distYWeight,
                                                    mavenparameters->minPeakRtDiff,
                                                    mavenparameters->useOverlap,
                                                    mavenparameters->minSignalBaselineDifference);



    QVERIFY(peakgroups.size() == 3);
    QVERIFY(13.2378 < peakgroups[0].meanRt < 13.238);
    QVERIFY(744.075 < peakgroups[0].meanMz < 744.077);
    QVERIFY(13.7593 < peakgroups[1].meanRt < 13.7595);
    QVERIFY(744.0749 <peakgroups[1].meanMz < 744.0751);
    QVERIFY(14.8468 < peakgroups[2].meanRt < 14.847);
    QVERIFY(744.0849 < peakgroups[2].meanMz < 744.0851);

    unsigned int maxPeakNum = 0;
    for(unsigned int j = 0; j < peakgroups.size(); j++) {   
        if (maxPeakNum < peakgroups[j].peaks.size())
            maxPeakNum = peakgroups[j].peaks.size();
    }

    QVERIFY(maxPeakNum == 2);    
}


void TestEIC:: testeicMerge() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    float compoundPPMWindow = 10;
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (int i = 0; i <  files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    vector<Compound*> compounds = common::getCompoudDataBaseWithRT();

    mzSlice* slice = new mzSlice();
    slice->compound = compounds[4];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(compoundPPMWindow, ionizationMode);

    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;
    //mavenparameters->grouping_maxRtWindow = 0.5;

    vector<EIC*> eics = PeakDetector::pullEICs(slice, mavenparameters->samples,
                                    1, mavenparameters->eic_smoothingWindow,
                                    mavenparameters->eic_smoothingAlgorithm, mavenparameters->amuQ1,
                                    mavenparameters->amuQ3,
                                    mavenparameters->baseline_smoothingWindow,
                                    mavenparameters->baseline_dropTopX,
                                    mavenparameters->minSignalBaselineDifference,
                                    mavenparameters->eicType,
                                    mavenparameters->filterline);

    EIC* m = EIC::eicMerge(eics);

    unsigned int maxEICsize = 0;
    for(unsigned int j = 0; j < eics.size(); j++) {
        if (maxEICsize < eics[j]->size())
            maxEICsize = eics[j]->size();
    }
    
    QVERIFY(maxEICsize == m->size());
    QVERIFY(maxEICsize == m->intensity.size());
    QVERIFY(maxEICsize == m->rt.size());
    QVERIFY(maxEICsize == m->scannum.size());
    QVERIFY(maxEICsize == m->mz.size());
    QVERIFY(m->rtmin > 0);
    QVERIFY(m->rtmax > 0);
}


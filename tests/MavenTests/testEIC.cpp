#include "testEIC.h"
#include "datastructures/mzSlice.h"
#include "EIC.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "PeakGroup.h"
#include "PeakDetector.h"
#include "utilities.h"

TestEIC::TestEIC() {}

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
    mzSample* mzsample = maventests::samples.smallSample;
    EIC e;

    bool status = e.makeEICSlice(mzsample, 180.002,180.004, 0, 2, 1, 0, "");
    QVERIFY(e.intensity.size() == numberOfScans && status);
}

void TestEIC::testgetEICms2() {
    mzSample* mzsample = maventests::samples.ms2TestSamples[1];
    mzSample* mzsample_2 = maventests::samples.ms2TestSamples[0];
    
    EIC* e = NULL;
    e = mzsample->getEIC(195,0,70,0,"",0.5,0.5); //precursorMz,collisionEnergy,productMz,eicType,filterline,amuQ1,amuQ3
    QVERIFY(e->rt.size() == 305);
    QVERIFY(e->scannum[e->scannum.size()-1] == 3351);
    QVERIFY(e->maxIntensity == 20200);

    EIC* e2 = NULL;
    e2 = mzsample_2->getEIC(195,0,69,1,"",0.5,0.5);
    QVERIFY(e2->rt.size() == 305);
    QVERIFY(e2->scannum[e2->scannum.size()-1] == 3041);
    QVERIFY(e2->maxIntensity == 10600);

    EIC* e3 = NULL;
    e3 = mzsample_2->getEIC(195,0,69,1,"",2,2);
    QVERIFY(e3->rt.size() == 305);
    QVERIFY(e3->scannum[e3->scannum.size()-1] == 3040);
    QVERIFY(e3->maxIntensity == 49400);
}

void TestEIC::testcomputeSpline()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");

    // if eic exists, perform smoothing
    EIC::SmootherType smootherType = static_cast<EIC::SmootherType>(1);
    e->setSmootherType(smootherType);

    e->computeSpline(10);
    QVERIFY(true);
}

void TestEIC::testgetPeakPositions()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");

    // if eic exists, perform smoothing
    EIC::SmootherType smootherType = static_cast<EIC::SmootherType>(1);
    e->setSmootherType(smootherType);

    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(80);

    e->setFilterSignalBaselineDiff(0);
    e->getPeakPositions(10);
    QVERIFY(true);
}

void TestEIC::testcomputeBaselineThreshold()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");

    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(60);
    e->computeBaseline();
    QVERIFY(true);

    // deallocate
    delete e;
}

void TestEIC::testcomputeBaselineAsLSSmoothing()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");

    // change default baseline mode
    e->setBaselineMode(EIC::BaselineMode::AsLSSmoothing);
    e->setAsLSSmoothness(2);
    e->setAsLSAsymmetry(8);

    // TODO: for lack of better floating point comparators to test the
    // consistency of the output, we can at least test whether the operation
    // completes successfully
    e->computeBaseline();

    // test whether all values are greater than zero, otherwise EIC widget
    // behaves erratically
    auto allGreaterThanZero = true;
    if(std::all_of(e->intensity.begin(),
                   e->intensity.end(),
                   [](float intensity) { return intensity < 0.0f; }))
        allGreaterThanZero = false;
    QVERIFY(allGreaterThanZero);

    // deallocate
    delete e;
}

void TestEIC::testcomputeBaselineZeroIntensity()
{
    // obtain a zero intensity EIC (all entries in intensity vector are zero)
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(381.123744f,
                                                381.123754f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");

    // change default baseline mode
    e->setBaselineMode(EIC::BaselineMode::AsLSSmoothing);
    e->setAsLSSmoothness(2);
    e->setAsLSAsymmetry(8);

    e->computeBaseline();

    // find the max of baseline and test if its an extremely small value
    // (there's no reliable way of comparing with zero in floating point)
    auto maxVal = std::max_element(e->baseline,
                                   e->baseline + e->intensity.size());
    QVERIFY(fabs(*maxVal) <= 1e-10f);

    // deallocate
    delete e;
}

void TestEIC::testcomputeBaselineEmptyEIC()
{
    // create an empty EIC, i.e., its intensity vector is empty
    EIC e;

    // change default baseline mode
    e.setBaselineMode(EIC::BaselineMode::AsLSSmoothing);
    e.setAsLSSmoothness(2);
    e.setAsLSAsymmetry(8);

    e.computeBaseline();
    QVERIFY(e.baseline == nullptr);
}

void TestEIC::testfindPeakBounds()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");
    
    // if eic exists, perform smoothing
    EIC::SmootherType smootherType = static_cast<EIC::SmootherType>(1);
    e->setSmootherType(smootherType);

    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(80);

    e->computeSpline(10);
    e->findPeaks();
    e->computeBaseline();
    e->findPeakBounds(e->peaks[10]);

    vector<float> intensity;
    for(unsigned int i =  e->peaks[10].minpos ; i <= e->peaks[10].maxpos; i++) {
         intensity.push_back(e->intensity[i]);
    }

    QVERIFY(TestUtils::floatCompare(*max_element(intensity.begin(), intensity.end()), e->intensity[e->peaks[10].pos]));
   
}

void TestEIC:: testGetPeakDetails()
{
    EIC* e = maventests::samples.ms1TestSamples[0]->getEIC(402.9929f,
                                                402.9969f,
                                                12.0,
                                                16.0,
                                                1,
                                                0,
                                                "");
    
    // if eic exists, perform smoothing
    EIC::SmootherType smootherType = static_cast<EIC::SmootherType>(1);
    e->setSmootherType(smootherType);

    e->setBaselineSmoothingWindow(5);
    e->setBaselineDropTopX(80);

    e->computeSpline(10);
    e->findPeaks();
    e->computeBaseline();
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

    float peakSplineArea = 0.0;
    for (unsigned int j = e->peaks[10].splineminpos; j <= e->peaks[10].splinemaxpos; j++)
    {
        peakSplineArea += e->spline[j];
    }

    float peakAreaCorrected = peakArea - baselineArea;

    QVERIFY(TestUtils::floatCompare(*max_element(intensity.begin(), intensity.end()), e->peaks[10].peakIntensity));

    QVERIFY(e->peaks[10].noNoiseObs == noNoiseObs);

    QVERIFY(TestUtils::floatCompare(peakArea, e->peaks[10].peakArea));

    QVERIFY(TestUtils::floatCompare(peakSplineArea, e->peaks[10].peakSplineArea));

    QVERIFY(TestUtils::floatCompare(peakAreaCorrected, e->peaks[10].peakAreaCorrected));

    QVERIFY(TestUtils::floatCompare(peakAreaCorrected / (e->totalIntensity + 1), e->peaks[10].peakAreaFractional));
    
    QVERIFY(e->peaks[10].gaussFitSigma > 0);
    
    QVERIFY(e->peaks[10].gaussFitR2 > 0);
}

void TestEIC:: testgroupPeaks() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (auto mzsample: maventests::samples.ms1TestSamples) {
        samplesToLoad.push_back(mzsample);
    }

    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();

    // for(std::vector<Compound*>::iterator it = compounds.begin(); it != compounds.end(); ++it) {
    //     cerr << (*it)->name << endl; 
    // }
    
    mzSlice* slice = new mzSlice();
    slice->compound = compounds[4];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->aslsBaselineMode = false;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;
    mavenparameters->grouping_maxRtWindow = 0.5;
    mavenparameters->distXWeight = 1;
    mavenparameters->distYWeight = 5;
    mavenparameters->overlapWeight = 2;
    mavenparameters->useOverlap = 0;

    vector<EIC*> eics = PeakDetector::pullEICs(slice,
                                               mavenparameters->samples,
                                               mavenparameters);

    vector<PeakGroup> peakgroups = EIC::groupPeaks(eics,
                                                   slice,
                                                   mavenparameters->eic_smoothingWindow,
                                                   mavenparameters->grouping_maxRtWindow,
                                                   mavenparameters->minQuality,
                                                   mavenparameters->distXWeight,
                                                   mavenparameters->distYWeight,
                                                   mavenparameters->overlapWeight,
                                                   mavenparameters->useOverlap,
                                                   mavenparameters->minSignalBaselineDifference,
                                                   mavenparameters->fragmentTolerance,
                                                   mavenparameters->scoringAlgo);

    QVERIFY(peakgroups.size() == 3);
    QVERIFY(13.2378 < peakgroups[0].meanRt < 13.238);
    QVERIFY(744.075 < peakgroups[0].meanMz < 744.077);
    QVERIFY(13.7593 < peakgroups[1].meanRt < 13.7595);
    QVERIFY(744.0749 <peakgroups[1].meanMz < 744.0751);
    QVERIFY(14.8468 < peakgroups[2].meanRt < 14.847);
    QVERIFY(744.0849 < peakgroups[2].meanMz < 744.0851);

    QVERIFY(peakgroups[0].peaks.size() == 2);
    QVERIFY(peakgroups[1].peaks.size() == 1);
    QVERIFY(peakgroups[2].peaks.size() == 1);
}


void TestEIC:: testeicMerge() {
    bool matchRtFlag = true;
    float compoundRTWindow = 2;
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,"ppm");
    int ionizationMode = +1;

    vector<mzSample*> samplesToLoad;

    for (auto mzsample: maventests::samples.ms1TestSamples) {
        samplesToLoad.push_back(mzsample);
    }

    vector<Compound*> compounds = TestUtils::getCompoudDataBaseWithRT();

    mzSlice* slice = new mzSlice();
    slice->compound = compounds[4];
    slice->calculateRTMinMax(matchRtFlag, compoundRTWindow);
    slice->calculateMzMinMax(mavenparameters->compoundMassCutoffWindow, ionizationMode);

    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->aslsBaselineMode = false;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    vector<EIC*> eics = PeakDetector::pullEICs(slice,
                                               mavenparameters->samples,
                                               mavenparameters);

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
    QVERIFY(13.041 < m->rtmin < 13.042);
    QVERIFY(17.039 < m->rtmax < 17.040);
}


#include "testMzAligner.h"
#include "classifierNeuralNet.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "obiwarp.h"
#include "PeakDetector.h"
#include "PeakGroup.h"
#include "Scan.h"
#include "utilities.h"

TestMzAligner::TestMzAligner() {}

void TestMzAligner::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
}

void TestMzAligner::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestMzAligner::init() {
    // This function is executed before each test
}

void TestMzAligner::cleanup() {
    // This function is executed after each test
}

void TestMzAligner::testDoAlignment() {

    vector<PeakGroup> allgroups = TestUtils::getGroupsFromProcessCompounds();

    vector<PeakGroup*> peakgroups(allgroups.size());
    for (unsigned int i = 0; i < allgroups.size(); i++)
            peakgroups[i] = &allgroups[i];

    Aligner aligner;
    aligner.doAlignment(peakgroups);
    QVERIFY(true);
}

void TestMzAligner::testObiWarp()
{

    MavenParameters* mavenparameters = new MavenParameters;
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(5,"ppm");
    mavenparameters->minQuality = 0.8;
    mavenparameters->minIntensity = 10000;
    mavenparameters->matchRtFlag = false;
    mavenparameters->samples = maventests::samples.alignmentSamples;
    mavenparameters->clsf = new ClassifierNeuralNet;
    mavenparameters->clsf->loadModel("bin/default.model");

    ObiParams params("cor", false, 2.0, 1.0, 0.20, 3.40, 0.0, 20.0, false, 0.60);
    Aligner aligner;
    aligner.alignWithObiWarp(mavenparameters->samples, &params, mavenparameters);

    maventests::database.loadCompoundCSVFile("bin/methods/KNOWNS.csv");
    vector<Compound*> compounds = maventests::database.getCompoundsSubset("KNOWNS");


    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds,
                                                            "compounds");
    peakDetector.processSlices(slices, "compounds");

    for(PeakGroup grp: mavenparameters->allgroups) {

        if(grp.getName() == "NAD+" || grp.getName() == "methionine" || grp.getName() == "dTMP") {
            float refSampleOriginalRt;
            vector<float> sampleOriginalRt;
            vector<float> sampleNewRt;
            for(Peak peak: grp.peaks) {
                if(peak.getSample()->sampleName == aligner.refSample->sampleName) {
                    refSampleOriginalRt = peak.getScan()->originalRt;
                }
                else {
                    sampleOriginalRt.push_back(peak.getScan()->originalRt);
                    sampleNewRt.push_back(peak.getScan()->rt);
                }
            }
            for(int rtCount=0; rtCount<sampleOriginalRt.size(); rtCount++) {
                 float newDiff = std::abs(refSampleOriginalRt - sampleNewRt[rtCount]);
                 float originalDiff = std::abs(refSampleOriginalRt - sampleOriginalRt[rtCount]);
                 QVERIFY((int)(newDiff) <= (int)(originalDiff));
             }
        }
    }

}

void TestMzAligner::testSaveFit(){

    vector<mzSample*> samplesToLoad  = maventests::samples.alignmentSamples;

    Aligner aligner;
    aligner.samples = samplesToLoad;
    aligner.saveFit();
    cerr << aligner.fit.size();
    QVERIFY(aligner.fit.size());

}

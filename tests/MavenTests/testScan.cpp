#include "testScan.h"


TestScan::TestScan() {

    sample = new mzSample();
}

void TestScan::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestScan::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestScan::init() {
    // This function is executed before each test
}

void TestScan::cleanup() {
    // This function is executed after each test
}

void TestScan::initScan(Scan* scan) {

    scan->productMz=5.5;
    scan->collisionEnergy=5.6;
    scan->centroided=1;
    //scan->intensity.resize(4);

    float arr[8]={9.9,4.4,5.5,8.8,2.07,2.08,2.09,8.8};
    scan->intensity.assign(arr,arr+4);
    scan->mz.assign(arr+4,arr+8);
    scan->scanType="testString1";
    scan->filterLine="testString2";

}

void TestScan::testdeepcopy() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);

    mzSample* sample1 = new mzSample();
    Scan* scan1 = new Scan(sample1,3,4,5.0,6.0,-1);
    scan1->deepcopy(scan);

    QVERIFY(scan->sample = scan1->sample);
    QVERIFY(scan->rt = scan1->rt);
    QVERIFY(scan->scannum = scan1->scannum);
    QVERIFY(scan->precursorMz = scan1->precursorMz);
    QVERIFY(scan->mslevel = scan1->mslevel);
    QVERIFY(scan->polarity = scan1->polarity);
    QVERIFY(scan->productMz= scan1->productMz);
    QVERIFY(scan->collisionEnergy= scan1->collisionEnergy);
    QVERIFY(scan->centroided= scan1->centroided);
    QVERIFY(scan->intensity[0] == scan1->intensity[0]);
    QVERIFY(scan->intensity[1] == scan1->intensity[1]);
    QVERIFY(scan->intensity[2] == scan1->intensity[2]);
    QVERIFY(scan->intensity[3] == scan1->intensity[3]);
    QVERIFY(scan->mz[0] == scan1->mz[0]);
    QVERIFY(scan->mz[1] == scan1->mz[1]);
    QVERIFY(scan->mz[2] == scan1->mz[2]);
    QVERIFY(scan->mz[3] == scan1->mz[3]);

    bool scanType=false;
    if (scan->scanType.compare(scan1->scanType)==0)
        scanType=true;
    QVERIFY(scanType);

    bool filterLine=false;
    if (scan->filterLine.compare(scan1->filterLine)==0)
        filterLine=true;
    QVERIFY(filterLine);

}



void TestScan::testfindHighestIntensityPos() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->massCutoffMerge->setMassCutoffAndType(100000,"ppm");
    int bestpos=scan->findHighestIntensityPos(2.1, mavenparameters->massCutoffMerge);
    QVERIFY(bestpos==2);

}

void TestScan::testfindMatchingMzs() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);

    vector<int>matches=scan->findMatchingMzs(2.079,2.1);
    QVERIFY(matches.size()==2);
    QVERIFY(matches[0]==1);
    QVERIFY(matches[1]==2);
}

void TestScan::testquantileFilter() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);

    scan->quantileFilter(25);
    QVERIFY(scan->mz.size()==2);
    QVERIFY(common::floatCompare(scan->mz[0],2.07));
    QVERIFY(common::floatCompare(scan->mz[1],8.8));
    QVERIFY(scan->intensity.size()==2);
    QVERIFY(common::floatCompare(scan->intensity[0],9.9));
    QVERIFY(common::floatCompare(scan->intensity[1],8.8));
}

void TestScan::testintensityFilter() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);

    scan->intensityFilter(6);
    QVERIFY(scan->mz.size()==2);
    QVERIFY(common::floatCompare(scan->mz[0],2.07));
    QVERIFY(common::floatCompare(scan->mz[1],8.8));
    QVERIFY(common::floatCompare(scan->intensity.size(),2));
    QVERIFY(common::floatCompare(scan->intensity[0],9.9));
    QVERIFY(common::floatCompare(scan->intensity[1],8.8));
}

void TestScan::testsimpleCentroid() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);

    float intensityarr[34]={131825.3,1902060,183417.5,0,93979.5,62351.94,65614.81,0,0,238641,
                            529399.9,337128.4,188587.9,42605.88,128301.7,89723.02,145668.700,
                            145668.700,145668.700,134054.1,920588.9,151300.9,10949580,86719.1,
                            0,0,0,0,0,134054.1,920588.9,151300.9,10949580,86719.1};
    scan->intensity.assign(intensityarr,intensityarr+34);
    float mzarr[34]={86.06158,86.06161,86.06158,86.06159,86.06158,86.06161,86.06163,
                     86.06163,87.00926,87.00927,87.00927,87.00926,87.00926,87.00926,
                     87.04557,87.04557,87.04557,88.04099,88.04099,88.04099,88.04099,
                     88.04099,88.04096,88.04095,88.04093,88.04098,88.04099,88.04099,
                     88.04099,88.04099,88.04099,88.04099,88.04096,88.04095};
    scan->mz.assign(mzarr,mzarr+34);
    scan->simpleCentroid();

    QVERIFY(scan->mz.size()==6);
    QVERIFY(common::floatCompare(scan->mz[0],86.0616));
    QVERIFY(common::floatCompare(scan->mz[1],86.0616));
    QVERIFY(common::floatCompare(scan->mz[2],87.0093));
    QVERIFY(common::floatCompare(scan->mz[3],87.0456));
    QVERIFY(common::floatCompare(scan->mz[4],88.041));
    QVERIFY(common::floatCompare(scan->mz[5],88.041));
    QVERIFY(scan->intensity.size()==6);
    QVERIFY(common::floatCompare(scan->intensity[0],1.90206e+06));
    QVERIFY(common::floatCompare(scan->intensity[1],93979.5));
    QVERIFY(common::floatCompare(scan->intensity[2],238641.0));
    QVERIFY(common::floatCompare(scan->intensity[3],145668.703125));
    QVERIFY(common::floatCompare(scan->intensity[4],151300.90625));
    QVERIFY(common::floatCompare(scan->intensity[5],151300.90625));

    scan->~Scan();
}

void TestScan::testhasMz() {

    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->massCutoffMerge->setMassCutoffAndType(10000,"ppm");

    QVERIFY(scan->hasMz(2.1,mavenparameters->massCutoffMerge));
    QVERIFY(!scan->hasMz(1.0,mavenparameters->massCutoffMerge));
    QVERIFY(!scan->hasMz(9,mavenparameters->massCutoffMerge));
}

void TestScan::testchargeSeries() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);;
    initScan (scan);
    vector<float> chargeStates=scan->chargeSeries(4.1,3);
    QVERIFY(common::floatCompare(chargeStates[0],0));
    QVERIFY(common::floatCompare(chargeStates[1],10.2999992370605));
    QVERIFY(common::floatCompare(chargeStates[2],5.64999961853027));
    QVERIFY(common::floatCompare(chargeStates[3],4.09999990463257));
    QVERIFY(common::floatCompare(chargeStates[4],3.32499980926514));
    QVERIFY(common::floatCompare(chargeStates[5],2.85999989509583));
    QVERIFY(common::floatCompare(chargeStates[6],2.54999995231628));
    QVERIFY(common::floatCompare(chargeStates[7],2.32857131958008));
    QVERIFY(common::floatCompare(chargeStates[8],2.16249990463257));
    QVERIFY(common::floatCompare(chargeStates[9],2.03333330154419));
    QVERIFY(common::floatCompare(chargeStates[10],1.92999994754791));
    QVERIFY(common::floatCompare(chargeStates[11],1.845454454422));
    QVERIFY(common::floatCompare(chargeStates[12],1.77499997615814));
    QVERIFY(common::floatCompare(chargeStates[13],1.71538460254669));
    QVERIFY(common::floatCompare(chargeStates[14],1.66428565979004));
    QVERIFY(common::floatCompare(chargeStates[15],1.62000000476837));
    QVERIFY(common::floatCompare(chargeStates[16],1.58124995231628));
    QVERIFY(common::floatCompare(chargeStates[17],1.54705882072449));
    QVERIFY(common::floatCompare(chargeStates[18],1.51666665077209));
    QVERIFY(common::floatCompare(chargeStates[19],1.48947370052338));
    QVERIFY(common::floatCompare(chargeStates[20],1.46499991416931));
    QVERIFY(common::floatCompare(chargeStates[21],1.44285714626312));
    QVERIFY(common::floatCompare(chargeStates[22],1.422727227211));
}

void TestScan::testdeconvolute() {

    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);
    initScan (scan);

    float intensityarr[34]={131.825,190.206,183.417,0,93.979,62.354,65.6181,0,0,23.841,
                            52.939,33.7124,18.8579,42.6058,12.807,89.702,14.8700,
                            14.56700,14.58700,13.4041,9.2089,15.139,10.980,86.791,
                            0,0,0,0,0,13.401,92.589,15.139,10.950,86.11};
    scan->intensity.assign(intensityarr,intensityarr+34);
    float mzarr[34]={86.06158,86.06161,86.06158,86.06159,86.06158,86.06161,86.06163,
                     86.06163,87.00926,87.00927,87.00927,87.00926,87.00926,87.00926,
                     87.04557,87.04557,87.04557,88.04099,88.04099,88.04099,88.04099,
                     88.04099,88.04096,88.04095,88.04093,88.04098,88.04099,88.04099,
                     88.04099,88.04099,88.04099,88.04099,88.04096,88.04095};
    scan->mz.assign(mzarr,mzarr+34);
    MavenParameters* mavenparameters = new MavenParameters();
    mavenparameters->massCutoffMerge->setMassCutoffAndType(100000,"ppm");
    ChargedSpecies* x=scan->deconvolute(87,4,mavenparameters->massCutoffMerge,2,3,100,500,2e5,3);
    ChargedSpecies* x1=scan->deconvolute(87,1,mavenparameters->massCutoffMerge,2,3,100,500,2e5,3);

    QVERIFY(common::floatCompare(x->totalIntensity, 3423.70849609375));
    QVERIFY(x->countMatches==18);
    QVERIFY(x->minZ==82);
    QVERIFY(x->maxZ==99);
    QVERIFY(x->upCount==11);
    QVERIFY(x->downCount==7);
    QVERIFY(common::floatCompare(x->qscore, 515.369812011719));
    QVERIFY(x->observedCharges.size()==18);
    QVERIFY(x->observedMzs.size()==18);
    QVERIFY(x->observedIntensities.size()==18);
    QVERIFY(common::floatCompare(x->observedCharges[0], 82));
    QVERIFY(common::floatCompare(x->observedCharges[1], 83));
    QVERIFY(common::floatCompare(x->observedCharges[2], 84));
    QVERIFY(common::floatCompare(x->observedCharges[3], 85));
    QVERIFY(common::floatCompare(x->observedCharges[4], 86));
    QVERIFY(common::floatCompare(x->observedCharges[5], 87));
    QVERIFY(common::floatCompare(x->observedCharges[6], 88));
    QVERIFY(common::floatCompare(x->observedCharges[7], 89));
    QVERIFY(common::floatCompare(x->observedCharges[8], 90));
    QVERIFY(common::floatCompare(x->observedCharges[9], 91));
    QVERIFY(common::floatCompare(x->observedCharges[10], 92));
    QVERIFY(common::floatCompare(x->observedCharges[11], 93));
    QVERIFY(common::floatCompare(x->observedCharges[12], 94));
    QVERIFY(common::floatCompare(x->observedCharges[13], 95));
    QVERIFY(common::floatCompare(x->observedCharges[14], 96));
    QVERIFY(common::floatCompare(x->observedCharges[15], 97));
    QVERIFY(common::floatCompare(x->observedCharges[16], 98));
    QVERIFY(common::floatCompare(x->observedCharges[17], 99));
    QVERIFY(common::floatCompare(x->observedMzs[0], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[1], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[2], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[3], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[4], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[5], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[6], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[7], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[8], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[9], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[10], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[11], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[12], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[13], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[14], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[15], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[16], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedMzs[17], 86.0616073608398));
    QVERIFY(common::floatCompare(x->observedIntensities[0], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[1], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[2], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[3], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[4], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[5], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[6], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[7], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[8], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[9], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[10], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[11], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[12], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[13], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[14], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[15], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[16], 190.205993652344));
    QVERIFY(common::floatCompare(x->observedIntensities[17], 190.205993652344));
    QVERIFY(common::floatCompare(x->error, 443.505981445312));
    QVERIFY(x1==NULL);



}

void TestScan::testgetTopPeaks() {
    Scan* scan=new Scan (sample,1,2,3.3,4.4,1);
    initScan (scan);

    vector<pair<float, float> > selected = scan->getTopPeaks(0.05, 1.5,.9);

    QVERIFY(selected.size()==2);
    QVERIFY(common::floatCompare(selected[0].first,(float) 9.8999996));
    QVERIFY(common::floatCompare(selected[1].first,(float) 8.8000001907));
    QVERIFY(common::floatCompare(selected[0].second,(float) 2.06999993));
    QVERIFY(common::floatCompare(selected[1].second,(float) 8.8000001));
}

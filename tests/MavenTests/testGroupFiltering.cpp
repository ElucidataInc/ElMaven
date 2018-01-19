#include "testGroupFiltering.h"

TestGroupFiltering::TestGroupFiltering()
{
    //constructor
}

void TestGroupFiltering::testquantileFilters() {
    vector<PeakGroup> allgroups = common::getGroupsFromProcessCompounds();
    PeakGroup group = allgroups[0];
    MavenParameters* mavenparameters = new MavenParameters();
    GroupFiltering groupFiltering(mavenparameters);
    mavenparameters->minGroupIntensity = group.maxIntensity + 1;
    QVERIFY(groupFiltering.quantileFilters(&group) == true);

    mavenparameters->minGroupIntensity = -1;
    mavenparameters->minSignalBaseLineRatio = group.maxSignalBaselineRatio + 1;
    QVERIFY(groupFiltering.quantileFilters(&group) == true);

    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    string loadmodel = "bin/default.model";
    clsf->loadModel(loadmodel);
    mavenparameters->clsf = clsf;
    mavenparameters->minGroupIntensity = -1;
    mavenparameters->minSignalBaseLineRatio = -1;
    mavenparameters->minQuality = group.maxQuality + 1;
    QVERIFY(groupFiltering.quantileFilters(&group) == true);

    mavenparameters->minGroupIntensity = -1;
    mavenparameters->minSignalBaseLineRatio = -1;
    mavenparameters->minQuality = -1;
    group.blankMax = 1;
    mavenparameters->minSignalBlankRatio = group.maxIntensity + 1;
    QVERIFY(groupFiltering.quantileFilters(&group) == true);

    mavenparameters->minGroupIntensity = -1;
    mavenparameters->minSignalBaseLineRatio = -1;
    mavenparameters->minQuality = -1;
    group.blankMax = 1;
    mavenparameters->minSignalBlankRatio = -1;
    mavenparameters->quantileIntensity = 0;
    mavenparameters->quantileQuality = 0;
    mavenparameters->quantileSignalBaselineRatio = 0;
    mavenparameters->quantileSignalBlankRatio = 0;
    QVERIFY(groupFiltering.quantileFilters(&group) == false);
}

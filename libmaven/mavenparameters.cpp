#include "mavenparameters.h"

MavenParameters::MavenParameters() {

        clsf = NULL; //initially classifier is not loaded

        alignSamplesFlag = false;
        processAllSlices = false;
        pullIsotopesFlag = false;
        matchRtFlag = false;
        checkConvergance = false;

        outputdir = "reports" + string(DIR_SEPARATOR_STR);

        writeCSVFlag = false;
        ionizationMode = -1;
        keepFoundGroups = true;
        showProgressFlag = true;

        mzBinStep = 0.01;
        rtStepSize = 20;
        ppmMerge = 30;
        avgScanTime = 0.2;

        limitGroupCount = INT_MAX;

        //peak detection
        eic_smoothingWindow = 10;
        eic_smoothingAlgorithm = 0;
        baseline_smoothingWindow = 5;
        baseline_dropTopX = 40;

        //peak grouping across samples
        grouping_maxRtWindow = 0.5;

        //peak filtering criteria
        minGoodGroupCount = 1;
        minSignalBlankRatio = 2;
        minNoNoiseObs = 1;
        minSignalBaseLineRatio = 2;
        minGroupIntensity = 500;
        minQuality = 0.5;

        compoundPPMWindow = 10;
        compoundRTWindow = 2;
        eicMaxGroups = INT_MAX;

        amuQ1 = 0.25;
        amuQ3 = 0.3;

        maxIsotopeScanDiff = 10;
        maxNaturalAbundanceErr = 100;
        minIsotopicCorrelation = 0;
        C13Labeled = false;
        N15Labeled = false;
        S34Labeled = false;
        D2Labeled = false;

}

void MavenParameters::setAverageScanTime() {
        if (samples.size() > 0)
                avgScanTime = samples[0]->getAverageFullScanTime();
}

/**
 * MavenParameters::setIonizationMode In this the mode is selected my looking
 * at the first sample polarity
 */
void MavenParameters::setIonizationMode() {
        if (samples.size() > 0 && samples[0]->getPolarity() > 0)
                ionizationMode = +1;
        else
                ionizationMode = -1;  //set ionization mode for compound matching
}

void MavenParameters::setSamples(vector<mzSample*>&set) {
        samples = set;
        setAverageScanTime();
}

void MavenParameters::cleanup() {
        allgroups.clear();
}

void MavenParameters::printSettings() {
        cerr << "#Output folder=" << outputdir << endl;
        cerr << "#ionizationMode=" << ionizationMode << endl;
        cerr << "#keepFoundGroups=" << keepFoundGroups << endl;
        cerr << "#showProgressFlag=" << showProgressFlag << endl;

        cerr << "#rtStepSize=" << rtStepSize << endl;
        cerr << "#ppmMerge=" << ppmMerge << endl;
        cerr << "#avgScanTime=" << avgScanTime << endl;

//peak detection
        cerr << "#eic_smoothingWindow=" << eic_smoothingWindow << endl;

//peak grouping across samples
        cerr << "#grouping_maxRtWindow=" << grouping_maxRtWindow << endl;

//peak filtering criteria
        cerr << "#minGoodGroupCount=" << minGoodGroupCount << endl;
        cerr << "#minSignalBlankRatio=" << minSignalBlankRatio << endl;
        cerr << "#minNoNoiseObs=" << minNoNoiseObs << endl;
        cerr << "#minSignalBaseLineRatio=" << minSignalBaseLineRatio << endl;
        cerr << "#minGroupIntensity=" << minGroupIntensity << endl;


//compound detection setting
        cerr << "#compoundPPMWindow=" << compoundPPMWindow << endl;
        cerr << "#compoundRTWindow=" << compoundRTWindow << endl;
}

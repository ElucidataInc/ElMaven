#include "mavenparameters.h"
#include "settings.h"

#include <pugixml.hpp>

#include <cstdlib>


MavenParameters::MavenParameters()
{
    /* default_settings_xml is a  character array generated using Default_Settings.xml file
     * It's present in settings.h header file
     */
    defaultSettingsData = (char*)default_settings_xml;
    loadSettings(defaultSettingsData);


	clsf = NULL;
	alignSamplesFlag = false;
        processAllSlices = false;
        pullIsotopesFlag = false;
        matchRtFlag = false;
        checkConvergance = false;
        stop = false;

        outputdir = "reports" + string(DIR_SEPARATOR_STR);

        writeCSVFlag = false;
        ionizationMode = -1;
        charge = 1;
        keepFoundGroups = true;
        showProgressFlag = true;

        alignButton = 0;
        /*
        * Whenever we create an instance of this class, massCutoffType must be set for fragmentMatchMassCutoffTolr
        */
        fragmentMatchMassCutoffTolr = new MassCutoff();
        fragmentMatchMassCutoffTolr->setMassCutoff(1000);
        mzBinStep = 0.01;
        rtStepSize = 20;
        /*
        * Whenever we create an instance of this class, massCutoffType must be set for massCutoffMerge
        */
        massCutoffMerge = new MassCutoff();
        massCutoffMerge->setMassCutoff(30);
        avgScanTime = 0.2;

        limitGroupCount = INT_MAX;

        // peak detection
        eic_smoothingWindow = 10;
        eic_smoothingAlgorithm = 0;
        baseline_smoothingWindow = 5;
        baseline_dropTopX = 80;
        minSignalBaselineDifference = 0;
        isotopicMinSignalBaselineDifference=0;

        eicType = 0;

        // peak grouping across samples
        grouping_maxRtWindow = 0.5;

        // peak filtering criteria
        minGoodGroupCount = 1;
        minSignalBlankRatio = 2;
        minNoNoiseObs = 1;
        minSignalBaseLineRatio = 2;
        minGroupIntensity = 500;
        peakQuantitation = 0;
        minQuality = 0.5;

        qualityWeight=10;
	intensityWeight=10;
	deltaRTWeight=10;
        deltaRtCheckFlag = false;

        /*
        * Whenever we create an instance of this class, massCutoffType must be set for compoundMassCutoffWindow
        */
        compoundMassCutoffWindow=new MassCutoff();
        compoundMassCutoffWindow->setMassCutoff(10);
        compoundRTWindow = 2;
        eicMaxGroups = INT_MAX;

        amuQ1 = 0.5;
        amuQ3 = 0.5;
        filterline = "";

        maxIsotopeScanDiff = 10;
        maxNaturalAbundanceErr = 100;
        minIsotopicCorrelation = 0;
        noOfIsotopes = 1000000;

	C13Labeled_BPE = false;
	N15Labeled_BPE = false;
	S34Labeled_BPE = false;
	D2Labeled_BPE = false;
	
	C13Labeled_Barplot = false;
	N15Labeled_Barplot = false;
	S34Labeled_Barplot = false;
	D2Labeled_Barplot = false;
    
	C13Labeled_IsoWidget = false;
	N15Labeled_IsoWidget = false;
	S34Labeled_IsoWidget = false;
	D2Labeled_IsoWidget = false;

        alignMaxItterations = 10;  //TODO: Sahil - Kiran, Added while merging mainwindow
        alignPolynomialDegree = 5; //TODO: Sahil - Kiran, Added while merging mainwindow
        
        quantileQuality = 0.0;
        quantileIntensity = 0.0;
        quantileSignalBaselineRatio = 0.0;
        quantileSignalBlankRatio = 0.0;

        //options dialog::peak grouping tab-widget
	distXWeight = 1.0;
	distYWeight = 1.0;
	overlapWeight = 1.0;
	bool useOverlap = true;

}

std::map<string, string>& MavenParameters::getSettings()
{
    return mavenSettings;
}

void  MavenParameters::setPeakDetectionSettings(const char* key, const char* value)
{
    mavenSettings[const_cast<char*>(key)] = const_cast<char*>(value);


    if(strcmp(key, "automatedDetection") == 0 )
        processAllSlices = atof(value);

//     TODO: replace with massCutoffMerge. remove ppmMerge from default_settings_xml as well. and regenerate settings.h 
//     if(strcmp(key, "ppmStep") == 0 )
//         ppmMerge = atof(value);

    if(strcmp(key,"rtStep") == 0)
        rtStepSize = atof(value);

    if(strcmp(key,"mzMin") == 0)
        minMz = atof(value);

    if(strcmp(key,"mzMax") == 0)
        maxMz = atof(value);

    if(strcmp(key,"rtMin") == 0)
        minRt = atof(value);

    if(strcmp(key,"rtMax") == 0)
        maxRt = atof(value);

    if(strcmp(key,"minIntensity") == 0)
        minIntensity = atof(value);

    if(strcmp(key,"maxIntensity") == 0)
        maxIntensity = atof(value);

    if(strcmp(key,"chargeMin") == 0)
        minCharge = atof(value);

    if(strcmp(key,"chargeMax") == 0)
        maxCharge = atof(value);

    if(strcmp(key, "dbDetection") == 0 )
        //TODO

        //TODO: replace with compoundMassCutoffWindow. replace compoundPPMWindow in default_settings_xml. regenerate settings.h
//     if(strcmp(key, "compoundPPMWindow") == 0 )
//         compoundPPMWindow = atof(value);

    if(strcmp(key, "compoundRTWindow") == 0 )
        compoundRTWindow = atof(value);

    if(strcmp(key, "matchRt") == 0 )
        matchRtFlag = atof(value);

    if(strcmp(key, "eicMaxGroups") == 0 )
        eicMaxGroups = atof(value);

    if(strcmp(key, "matchFragmentationOptions") == 0 )
        matchFragmentation = atof(value);

    if(strcmp(key, "reportIsotopesOptions") == 0 )
        pullIsotopesFlag = atof(value);

    if(strcmp(key, "minGroupIntensity") == 0 )
        minGroupIntensity = atof(value);

    if(strcmp(key, "peakQuantitation") == 0 )
        peakQuantitation = atof(value);


    if(strcmp(key, "quantileIntensity") == 0 )
        quantileIntensity = atof(value);

    if(strcmp(key, "minQuality") == 0 )
        minQuality = atof(value);

    if(strcmp(key, "quantileQuality") == 0 )
        quantileQuality = atof(value);


    if(strcmp(key, "sigBlankRatio") == 0 )
        minSignalBlankRatio = atof(value);

    if(strcmp(key, "quantileSignalBlankRatio") == 0 )
        quantileSignalBlankRatio = atof(value);

    if(strcmp(key, "sigBaselineRatio") == 0 )
        minSignalBaseLineRatio = atof(value);

    if(strcmp(key, "quantileSignalBaselineRatio") == 0 )
        quantileSignalBaselineRatio = atof(value);


    if(strcmp(key, "minNoNoiseObs") == 0 )
        minNoNoiseObs = atof(value);

    if(strcmp(key, "minGoodGroupCount") == 0 )
        minGoodGroupCount = atof(value);

}

void MavenParameters::setOptionsDialogSettings(const char* key, const char* value)
{
    mavenSettings[const_cast<char*>(key)] = const_cast<char*>(value);

    if(strcmp(key, "ionizationMode") == 0)
        ionizationMode = atof(value);

    if(strcmp(key, "amuQ1") == 0)
        amuQ1 = atof(value);

    if(strcmp(key, "amuQ3") == 0)
        amuQ3 = atof(value);

    //TODO
//     if(strcmp(key, "filterline") == 0)
//         filterline = atof(value);

    if(strcmp(key, "eic_smoothingAlgorithm") == 0)
        eic_smoothingAlgorithm = atof(value);

    if(strcmp(key, "eic_smoothingWindow") == 0)
        eic_smoothingWindow = atof(value);

    if(strcmp(key, "grouping_maxRtWindow") == 0)
        grouping_maxRtWindow = atof(value);

    if(strcmp(key, "baseline_quantile") == 0)
        baseline_dropTopX = atof(value);

    if(strcmp(key, "baseline_smoothing") == 0)
        baseline_smoothingWindow = atof(value);

    if(strcmp(key, "minSignalBaselineDifference") == 0)
        minSignalBaselineDifference = atof(value);

    if(strcmp(key, "D2Labeled_BPE") == 0)
        D2Labeled_BPE = atof(value);

    if(strcmp(key, "C13Labeled_BPE") == 0)
        C13Labeled_BPE = atof(value);

    if(strcmp(key, "N15Labeled_BPE") == 0)
        N15Labeled_BPE = atof(value);

    if(strcmp(key, "S34Labeled_BPE") == 0)
        S34Labeled_BPE = atof(value);

    if(strcmp(key, "D2Labeled_Barplot") == 0)
        D2Labeled_Barplot = atof(value);

    if(strcmp(key, "C13Labeled_Barplot") == 0)
        C13Labeled_Barplot = atof(value);

    if(strcmp(key, "N15Labeled_Barplot") == 0)
        N15Labeled_Barplot = atof(value);

    if(strcmp(key, "S34Labeled_Barplot") == 0)
        S34Labeled_Barplot = atof(value);

    if(strcmp(key, "D2Labeled_IsoWidget") == 0)
        D2Labeled_IsoWidget = atof(value);

    if(strcmp(key, "C13Labeled_IsoWidget") == 0)
        C13Labeled_IsoWidget = atof(value);

    if(strcmp(key, "N15Labeled_IsoWidget") == 0)
        N15Labeled_IsoWidget = atof(value);

    if(strcmp(key, "S34Labeled_IsoWidget") == 0)
        S34Labeled_IsoWidget = atof(value);

    if(strcmp(key, "noOfIsotopes") == 0)
        noOfIsotopes = atof(value);

    if(strcmp(key, "minIsotopicCorrelation") == 0)
        minIsotopicCorrelation = atof(value);

    if(strcmp(key, "maxIsotopeScanDiff") == 0)
        maxIsotopeScanDiff = atof(value);

    if(strcmp(key, "maxNaturalAbundanceErr") == 0)
        maxNaturalAbundanceErr = atof(value);

    if(strcmp(key, "isotopicMinSignalBaselineDifference") == 0)
        isotopicMinSignalBaselineDifference = atof(value);

    if(strcmp(key, "eicType") == 0)
        eicType = atof(value);

    if(strcmp(key, "useOverlap") == 0)
        useOverlap = atof(value);

    if(strcmp(key, "distXWeight") == 0)
        distXWeight = atof(value);

    if(strcmp(key, "distYWeight") == 0)
        distYWeight = atof(value);

    if(strcmp(key, "overlapWeight") == 0)
        overlapWeight = atof(value);

    if(strcmp(key, "qualityWeight") == 0)
        qualityWeight = atof(value);

    if(strcmp(key, "intensityWeight") == 0)
        intensityWeight = atof(value);

    if(strcmp(key, "deltaRTWeight") == 0)
        deltaRTWeight = atof(value);

    if(strcmp(key, "deltaRTCheck") == 0)
        deltaRtCheckFlag = atof(value);

}

bool MavenParameters::loadSettings(const char* data)
{

    pugi::xml_document xmlDoc;
    pugi::xml_parse_result parseResult = xmlDoc.load_string(data);

    // return if parsing the xml failed
    if(parseResult.status != pugi::xml_parse_status::status_ok) {

        std::cout << parseResult.description() << endl;
        return false;
    }

    pugi::xml_node pnode = xmlDoc.child("Settings");

    for(pugi::xml_node& node: pnode.children()) {


        if(strcmp(node.name(), "Peak_Detection")== 0) {

            for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it) {

                setPeakDetectionSettings(it->name(), it->text().get());

            }

        }

        if(strcmp(node.name(), "Settings_Form") == 0 ) {

            for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it) {

                setOptionsDialogSettings(it->name(), it->text().get());

            }
        }
    }

}

vector<mzSample*> MavenParameters::getVisibleSamples() {

	vector<mzSample*> vsamples;
	for (int i = 0; i < samples.size(); i++) {
		if (samples[i] && samples[i]->isSelected) {
			vsamples.push_back(samples[i]);
		}
	}
	return vsamples;
}

void MavenParameters::setAverageScanTime() {
        if (samples.size() > 0)
                avgScanTime = samples[0]->getAverageFullScanTime();
}

int MavenParameters::getCharge(Compound* compound){
	int charge;
        if(compound != nullptr && !this->formulaFlag){
                if(compound->charge !=0){
                        charge = compound->charge;
                } else {
                        charge = this->ionizationMode*this->charge;
                }
        }
        else{
                charge = this->ionizationMode*this->charge;
        }
	return charge;
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
        cerr << "#massCutoffMerge=" << massCutoffMerge->getMassCutoff() << endl;
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
        cerr << "#compoundMassCutoffWindow=" << compoundMassCutoffWindow->getMassCutoff() << endl;
        cerr << "#compoundRTWindow=" << compoundRTWindow << endl;

        //peak grouping score
        cerr << "#distXWeight=" << distXWeight << endl;
        cerr << "#distYWeight=" << distYWeight << endl; 
        cerr << "#overlapWeight=" << overlapWeight << endl; 
        cerr << "#useOverlap=" << useOverlap << endl;              
}

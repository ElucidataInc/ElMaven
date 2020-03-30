#include <pugixml.hpp>

#include "mavenparameters.h"
#include "settings.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "Compound.h"
#include "masscutofftype.h"
#include "classifierNeuralNet.h"

MavenParameters::MavenParameters(string settingsPath):lastUsedSettingsPath(settingsPath)
{

    clsf = NULL;
    alignSamplesFlag = false;
        processAllSlices = false;
        pullIsotopesFlag = false;
        matchRtFlag = false;
        stop = false;

        outputdir = "reports" + string(DIR_SEPARATOR_STR);

        writeCSVFlag = false;
        ionizationMode = 1;
        charge = 1;
        keepFoundGroups = true;
        showProgressFlag = true;

        alignButton = 0;

        fragmentTolerance = 20;
        minFragMatchScore = 0;
        minFragMatch = 3;
        scoringAlgo = "Hypergeometric Score";
        
        
        /*
        * Whenever we create an instance of this class, massCutoffType must be set for massCutoffMerge
        */
        massCutoffMerge = new MassCutoff();
        massCutoffMerge->setMassCutoffAndType(30, "ppm");
        mzBinStep = 0.01;
        rtStepSize = 20;
        avgScanTime = 0.2;

        limitGroupCount = INT_MAX;

        // to allow adduct matching
        searchAdducts = false;
        adductSearchWindow = 0.1f;
        adductPercentCorrelation = 90.0f;

        // peak detection
        eic_smoothingWindow = 10;
        eic_smoothingAlgorithm = 0;

        // baseline estimation
        aslsBaselineMode = false;
        baseline_smoothingWindow = 5;
        baseline_dropTopX = 80;
        aslsSmoothness = 2;
        aslsAsymmetry = 80;

        isIsotopeEqualPeakFilter = false;
        minSignalBaselineDifference = 0;
        isotopicMinSignalBaselineDifference=0;
        minPeakQuality = 0.0;
        minIsotopicPeakQuality = 0.0;

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
        compoundMassCutoffWindow->setMassCutoffAndType(10, "ppm");
        compoundRTWindow = 1;
        eicMaxGroups = INT_MAX;

        amuQ1 = 0.5;
        amuQ3 = 0.5;
        filterline = "";

        maxIsotopeScanDiff = 10;
        maxNaturalAbundanceErr = 100;
        minIsotopicCorrelation = 0;
        isotopeC13Correction = 0;

	C13Labeled_BPE = false;
	N15Labeled_BPE = false;
	S34Labeled_BPE = false;
	D2Labeled_BPE = false;
	
	C13Labeled_Barplot = true;
	N15Labeled_Barplot = false;
	S34Labeled_Barplot = false;
	D2Labeled_Barplot = false;

        alignMaxIterations = 10;  //TODO: Sahil - Kiran, Added while merging mainwindow
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

    /* default_settings_xml is a  character array generated using Default_Settings.xml file
     * It's present in settings.h header file
     * TODO: stop assigning values in constructor. just use loadSettings
     */
    defaultSettingsData = (char*)default_settings_xml;


    /*TODO: find a clean way to implement this. just check if
    * the 'lastUsedSettingsPath'is not empty and the file
    * actually exits. If not, then load defaultSettingsData
    */


    if(!lastUsedSettingsPath.empty()) {
        ifstream ifs(lastUsedSettingsPath, std::ios_base::in);
        if(ifs.is_open()) {
            stringstream ss;
            ss << ifs.rdbuf();
            if(!ss.str().empty()){
                loadSettings(ss.str().c_str());
            }
            ifs.close();
        }
        else {
            loadSettings(defaultSettingsData);
        }
     }

    else
        loadSettings(defaultSettingsData);
}

MavenParameters::~MavenParameters()
{
    saveSettings(lastUsedSettingsPath.c_str());
}

void MavenParameters::setOutputDir(QString outdir) {
    outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
}

std::map<string, string>& MavenParameters::getSettings()
{
    return mavenSettings;
}

void  MavenParameters::setIsotopeDialogSettings(const char* key, const char* value)
{
    if(key[0] == '\0' || value[0] == '\0')
        return;

    mavenSettings[const_cast<char*>(key)] = const_cast<char*>(value);

    if(strcmp(key, "reportIsotopes") == 0 )
        pullIsotopesFlag = atof(value);
    
    if(strcmp(key, "C13LabelBPE") == 0)
        C13Labeled_BPE = atof(value);

    if(strcmp(key, "D2LabelBPE") == 0)
        D2Labeled_BPE = atof(value);

    if(strcmp(key, "N15LabelBPE") == 0)
        N15Labeled_BPE = atof(value);

    if(strcmp(key, "S34LabelBPE") == 0)
        S34Labeled_BPE = atof(value);

    if(strcmp(key, "correctC13IsotopeAbundance") == 0)
        isotopeC13Correction = atof(value);

    if(strcmp(key, "minIsotopeParentCorrelation") == 0)
        minIsotopicCorrelation = atof(value);

    if(strcmp(key, "maxIsotopeScanDiff") == 0)
        maxIsotopeScanDiff = atof(value);

    if(strcmp(key, "maxNaturalAbundanceError") == 0)
        maxNaturalAbundanceErr = atof(value);
}

std::vector<Adduct*> MavenParameters::getDefaultAdductList()
{
    vector<Adduct*> adductList;
    if (ionizationMode > 0) {
        adductList.push_back(MassCalculator::PlusHAdduct);
    } else if (ionizationMode < 0) {
        adductList.push_back(MassCalculator::MinusHAdduct);
    } else {
        adductList.push_back(MassCalculator::ZeroMassAdduct);
    }
    return adductList;
}

void  MavenParameters::setPeakDetectionSettings(const char* key, const char* value)
{
    if(key[0] == '\0' || value[0] == '\0')
        return;

    mavenSettings[const_cast<char*>(key)] = const_cast<char*>(value);

    if(strcmp(key, "automatedDetection") == 0 )
        processAllSlices = atof(value);

    if(strcmp(key, "massCutoffType") == 0 ) {
        if(massCutoffMerge != nullptr)
            massCutoffMerge->setMassCutoffType(value);

        if(compoundMassCutoffWindow != nullptr)
            compoundMassCutoffWindow->setMassCutoffType(value);
    }

    if(strcmp(key, "massDomainResolution") == 0
        && massCutoffMerge != nullptr) {
        massCutoffMerge->setMassCutoff(atof(value));
    }

    if(strcmp(key, "fragmentTolerance") == 0)
        fragmentTolerance = atof(value);

    if (strcmp(key, "minFragMatch") == 0)
        minFragMatch = atof(value);

    if (strcmp(key, "minFragMatchScore") == 0)
        minFragMatchScore = atof(value);

    if(strcmp(key,"timeDomainResolution") == 0)
        rtStepSize = atof(value);

    if(strcmp(key,"minMz") == 0)
        minMz = atof(value);

    if(strcmp(key,"maxMz") == 0)
        maxMz = atof(value);

    if(strcmp(key,"minRt") == 0)
        minRt = atof(value);

    if(strcmp(key,"maxRt") == 0)
        maxRt = atof(value);

    if(strcmp(key,"minIntensity") == 0)
        minIntensity = atof(value);

    if(strcmp(key,"maxIntensity") == 0)
        maxIntensity = atof(value);

    if(strcmp(key,"chargeMin") == 0)
        minCharge = atof(value);

    if(strcmp(key,"chargeMax") == 0)
        maxCharge = atof(value);

    if(strcmp(key, "identificationMatchRt") == 0)
        identificationMatchRt = static_cast<bool>(atoi(value));

    if(strcmp(key, "identificationRtWindow") == 0)
        identificationRtWindow = atof(value);

    if(strcmp(key, "databaseSearch") == 0 );
        //TODO

    if(strcmp(key, "compoundExtractionWindow") == 0
        && compoundMassCutoffWindow != nullptr) {
        compoundMassCutoffWindow->setMassCutoff(atof(value));
    }

    if(strcmp(key, "compoundRtWindow") == 0 )
        compoundRTWindow = atof(value);

    if(strcmp(key, "matchRt") == 0 )
        matchRtFlag = atof(value);

    if(strcmp(key, "limitGroupsPerCompound") == 0 )
        eicMaxGroups = atof(value);

    if(strcmp(key, "searchAdducts") == 0 )
        searchAdducts = atof(value);

    if(strcmp(key, "adductSearchWindow") == 0 )
        adductSearchWindow = atof(value);

    if(strcmp(key, "adductPercentCorrelation") == 0 )
        adductPercentCorrelation = atof(value);

    if(strcmp(key, "matchFragmentation") == 0 )
        matchFragmentationFlag = atof(value);

    if(strcmp(key, "minGroupIntensity") == 0 )
        minGroupIntensity = atof(value);

    if(strcmp(key, "peakQuantitation") == 0 )
        peakQuantitation = atof(value);

    if(strcmp(key, "intensityQuantile") == 0 )
        quantileIntensity = atof(value);

    if(strcmp(key, "minGroupQuality") == 0 )
        minQuality = atof(value);

    if(strcmp(key, "qualityQuantile") == 0 )
        quantileQuality = atof(value);

    if(strcmp(key, "minSignalBlankRatio") == 0 )
        minSignalBlankRatio = atof(value);

    if(strcmp(key, "signalBlankRatioQuantile") == 0 )
        quantileSignalBlankRatio = atof(value);

    if(strcmp(key, "minSignalBaselineRatio") == 0 )
        minSignalBaseLineRatio = atof(value);

    if(strcmp(key, "signalBaselineRatioQuantile") == 0 )
        quantileSignalBaselineRatio = atof(value);

    if(strcmp(key, "minPeakWidth") == 0 )
        minNoNoiseObs = atof(value);

    if(strcmp(key, "minGoodPeakCount") == 0 )
        minGoodGroupCount = atof(value);
}

void MavenParameters::setOptionsDialogSettings(const char* key, const char* value)
{
    if(key[0] == '\0' || value[0] == '\0')
        return;

    mavenSettings[const_cast<char*>(key)] = const_cast<char*>(value);

    if(strcmp(key, "ionizationMode") == 0) {
        int polarity=atoi(value);
        setIonizationMode((Polarity)polarity);
    }

    if (strcmp(key, "ionizationType") == 0 && stoi(value) == 1) {
        MassCalculator::ionizationType = MassCalculator::EI;
    } else {
        MassCalculator::ionizationType = MassCalculator::ESI;
    }

    if(strcmp(key, "q1Accuracy") == 0)
        amuQ1 = atof(value);

    if(strcmp(key, "q3Accuracy") == 0)
        amuQ3 = atof(value);

    // TODO
    // if(strcmp(key, "filterline") == 0)
    //     filterline = atof(value);

    if (strcmp(key, "centroidScans") == 0 && stoi(value) == 1) {
        mzSample::setFilter_centroidScans(true);
    } else {
        mzSample::setFilter_centroidScans(false);
    }

    if (strcmp(key, "scanFilterMinIntensity") == 0) {
        mzSample::setFilter_minIntensity(stof(value));
    }

    if (strcmp(key, "scanFilterMinQuantile") == 0) {
        mzSample::setFilter_intensityQuantile(stoi(value));
    }

    if (strcmp(key, "scanFilterPolarity") == 0) {
        if (stoi(value) == 0) {
            mzSample::setFilter_polarity(0);
        } else if (stoi(value) == 1) {
            mzSample::setFilter_polarity(+1);
        } else {
            mzSample::setFilter_polarity(-1);
        }
    }

    if (strcmp(key, "scanFilterMsLevel") == 0) {
        if (stoi(value) == 0) {
            mzSample::setFilter_mslevel(0);
        } else if (stoi(value) == 1) {
            mzSample::setFilter_mslevel(1);
        } else {
            mzSample::setFilter_mslevel(2);
        }
    }

    if(strcmp(key, "eicSmoothingAlgorithm") == 0)
        eic_smoothingAlgorithm = atof(value);

    if(strcmp(key, "eicSmoothingWindow") == 0)
        eic_smoothingWindow = atof(value);

    if(strcmp(key, "maxRtDiffBetweenPeaks") == 0)
        grouping_maxRtWindow = atof(value);

    if(strcmp(key, "aslsBaselineMode") == 0)
        aslsBaselineMode = static_cast<bool>(atof(value));

    if(strcmp(key, "baselineQuantile") == 0)
        baseline_dropTopX = atof(value);

    if(strcmp(key, "baselineSmoothing") == 0)
        baseline_smoothingWindow = atof(value);

    if(strcmp(key, "aslsSmoothness") == 0)
        aslsSmoothness = atoi(value);

    if(strcmp(key, "aslsAsymmetry") == 0)
        aslsAsymmetry = atoi(value);

    if(strcmp(key, "isotopeFilterEqualPeak") == 0)
        isIsotopeEqualPeakFilter = atof(value);    

    if(strcmp(key, "minSignalBaselineDifference") == 0)
        minSignalBaselineDifference = atof(value);

    if(strcmp(key, "isotopeMinSignalBaselineDifference") == 0)
        isotopicMinSignalBaselineDifference = atof(value);

    if(strcmp(key, "minPeakQuality") == 0)
        minPeakQuality = atof(value);

    if(strcmp(key, "minIsotopePeakQuality") == 0)
        minIsotopicPeakQuality = atof(value);

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

    if(strcmp(key, "considerDeltaRT") == 0)
        deltaRtCheckFlag = atof(value);
}

bool MavenParameters::saveSettings(const char* path)
{
    cerr << "\n\n saving the settings at  " << path << "\n";
    pugi::xml_document xmlDoc;

    pugi::xml_node pNode = xmlDoc.append_child("Settings");



    for(std::map<std::string, std::string>::iterator  it = mavenSettings.begin(); it != mavenSettings.end(); it++) {

        pugi::xml_node cNode = pNode.append_child(it->first.c_str());
        cNode.append_child(pugi::node_pcdata).set_value(it->second.c_str());

    }


    if(!xmlDoc.save_file(path,"\t", format_default, pugi::xml_encoding::encoding_utf8))
        return false;

    return true;
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

    for(pugi::xml_node_iterator it = pnode.begin(); it != pnode.end(); ++it) {

                setIsotopeDialogSettings(it->name(), it->text().get());
                setPeakDetectionSettings(it->name(), it->text().get());
                setOptionsDialogSettings(it->name(), it->text().get());
    }

    return true;
}


void MavenParameters::reset(const std::list<string>& keys)
{

    pugi::xml_document xmlDoc;
    pugi::xml_parse_result parseResult = xmlDoc.load_string(defaultSettingsData);

    if(parseResult.status != pugi::xml_parse_status::status_ok)
        std::cerr << "parsing error  : " << parseResult.status ;

    pugi::xml_node pnode = xmlDoc.child("Settings");

    for(pugi::xml_node_iterator it = pnode.begin(); it != pnode.end(); ++it) {

        if( std::find(keys.begin(), keys.end(), it->name()) != keys.end() ) {
            setOptionsDialogSettings(it->name(), it->text().get());
            setPeakDetectionSettings(it->name(), it->text().get());
            setIsotopeDialogSettings(it->name(), it->text().get());
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

//TODO: a member of the class is called 'charge'. Rename the function
//to avoid mistaking it as a getter function for that variable
int MavenParameters::getCharge(Compound* compound){
	int charge;
        if(compound != nullptr && !this->formulaFlag){
                if(compound->charge() !=0){
                        charge = compound->charge();
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
void MavenParameters::setIonizationMode(Polarity polarity) {
    switch(polarity){
        case Neutral:
            ionizationMode=0;
            break;
        case Positive:
            ionizationMode=1;
            break;
        case Negative:
            ionizationMode=-1;
            break;
        default:
            if (samples.size() > 0)
                ionizationMode=samples[0]->getPolarity();
            else
                ionizationMode = 1;
            break;
    }

}

void MavenParameters::setSamples(vector<mzSample*>&set) {
        samples = set;
        setAverageScanTime();
}

void MavenParameters::cleanup() {
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

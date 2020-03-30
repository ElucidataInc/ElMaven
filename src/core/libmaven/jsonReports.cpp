#include "json.hpp"
#include "doctest.h"
#include "testUtils.h"
#include "jsonReports.h"
#include "Compound.h"
#include "EIC.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"
#include "datastructures/mzSlice.h"
#include "databases.h"
#include "classifierNeuralNet.h"
#include "PeakDetector.h"

using json = nlohmann::json;


JSONReports::JSONReports(MavenParameters* mp, bool pollyUpload):
    _uploadToPolly(pollyUpload), _mavenParameters(mp){}

void JSONReports::_writeGroup(PeakGroup& grp, ofstream& filename)
{
    //add labels to json file
    char label = grp.label;
    PeakGroup* parentGroup = grp.getParent();
    if (parentGroup) {
        if (grp.label == '\0') {
            label = parentGroup->label;
        }
    }

    filename << setprecision(10);
    filename << "{\n";
    filename << "\"groupId\": " << grp.groupId ;

    filename << ",\n" << "\"label\": ";
    filename << "\"";
    if (label == 'g' || label == 'b') filename << label;
    filename << "\"";

    if(_uploadToPolly) {
        int mlLabel =  (grp.markedGoodByCloudModel) ? 1 : (grp.markedBadByCloudModel) ? -1 : 0;
        filename  << ",\n" << "\"ml-label\": " << mlLabel;
    }

    filename << ",\n" << "\"metaGroupId\": " << grp.metaGroupId ;
    filename << ",\n" << "\"meanMz\": " << grp.meanMz  ;
    filename << ",\n" << "\"meanRt\": " << grp.meanRt ;
    filename << ",\n" << "\"rtmin\": " << grp.minRt ;
    filename << ",\n" << "\"rtmax\": " << grp.maxRt ;
    filename << ",\n" << "\"maxQuality\": " << grp.maxQuality ;
}

void JSONReports::_writeCompoundLink(PeakGroup& grp, ofstream& filename)
{
    filename << setprecision(10);
    double mz = 0.0f;
    int charge = _mavenParameters->getCharge(grp.getCompound());
    mz = grp.getExpectedMz(charge);

    if (mz == -1)
        mz = grp.meanMz;

    filename << ",\n" << "\"compound\": { " ;

    string compoundID = grp.getCompound()->id();
    filename << "\"compoundId\": "<< _sanitizeJSONstring(compoundID) ;
    string compoundName = grp.getCompound()->name();
    filename << ",\n" << "\"compoundName\": "<< _sanitizeJSONstring(compoundName);
    string formula = grp.getCompound()->formula();
    filename << ",\n" << "\"formula\": "<< _sanitizeJSONstring(formula);
    filename << ",\n" << "\"expectedRt\": " << grp.getCompound()->expectedRt();
    filename << ",\n" << "\"expectedMz\": " << mz ;
    filename << ",\n" << "\"srmID\": " << _sanitizeJSONstring(grp.srmId) ;
    filename << ",\n" << "\"tagString\": " << _sanitizeJSONstring(grp.tagString) ;

    string fullTag = grp.srmId + grp.tagString;
    string fullName = compoundName;
    string fullID = compoundID;
    if (fullTag.length()) {
        fullName = compoundName + " [" + fullTag + "]";
        fullID = compoundID + " [" + fullTag + "]";
    }
    filename << ",\n" << "\"fullCompoundName\": "<< _sanitizeJSONstring(fullName);
    filename << ",\n" << "\"fullCompoundID\": "<< _sanitizeJSONstring(fullID) ;
    filename << "}" ; // compound
}

void JSONReports::_writePeak(PeakGroup& grp, ofstream& filename, vector<mzSample*> vsamples)
{

    filename << setprecision(10);

    filename << ",\n"<< "\"peaks\": [ " ;

    for(auto it = vsamples.begin(); it != vsamples.end(); ++it) {
        if (it != vsamples.begin()) {
            filename << ",\n";
        }

        mzSample* sample = *it;
        Peak* peak = grp.getPeak(sample);
        if(peak) {
            //TODO: add slice information here: e.g. what ppm was used
            filename << "{\n";
            filename << "\"sampleName\": " << _sanitizeJSONstring((*it)->sampleName) ;
            filename << ",\n" << "\"peakMz\": " << peak->peakMz ;
            filename << ",\n" << "\"medianMz\": " << peak->medianMz ;
            filename << ",\n" << "\"baseMz\": " << peak->baseMz ;
            filename << ",\n" << "\"mzmin\": " << peak->mzmin ;
            filename << ",\n" << "\"mzmax\": " << peak->mzmax ;
            filename << ",\n" << "\"rt\": " << peak->rt ;
            filename << ",\n" << "\"rtmin\": " << peak->rtmin ;
            filename << ",\n" << "\"rtmax\": " << peak->rtmax ;
            filename << ",\n" << "\"quality\": " << peak->quality ;
            filename << ",\n" << "\"peakIntensity\": " << peak->peakIntensity ;
            filename << ",\n" << "\"peakBaseLineLevel\": " << peak->peakBaseLineLevel ;
            filename << ",\n" << "\"peakArea\": " << peak->peakAreaCorrected ;
            filename << ",\n" << "\"peakSplineArea\": " << peak->peakSplineArea ;
            filename << ",\n" << "\"peakAreaTop\": " << peak->peakAreaTopCorrected;
            filename << ",\n" << "\"peakAreaNotCorrected\": " << peak->peakArea;
            filename << ",\n" << "\"peakAreaTopNotCorrected\": " << peak->peakAreaTop;
            filename << ",\n" << "\"noNoiseObs\": " << peak->noNoiseObs ;
            filename << ",\n" << "\"signalBaselineRatio\": " << peak->signalBaselineRatio ;
            filename << ",\n" << "\"fromBlankSample\": " << peak->fromBlankSample ;
            filename << ",\n" << "\"peakAreaFractional\": " << peak->peakAreaFractional ;
            filename << ",\n" << "\"symmetry\": " << peak->symmetry ;
            filename << ",\n" << "\"noNoiseFraction\": " << peak->noNoiseFraction ;
            filename << ",\n" << "\"groupOverlap\": " << peak->groupOverlap ;
            filename << ",\n" << "\"groupOverlapFrac\": " << peak->groupOverlapFrac ;
            filename << ",\n" << "\"gaussFitR2\": " << peak->gaussFitR2 ;
            filename << ",\n" << "\"peakRank\": " << peak->peakRank ;
            filename << ",\n" << "\"peakWidth\": " << peak->width ;
        } else {
            filename << "{\n";
            filename << "\"sampleName\": " << _sanitizeJSONstring((*it)->sampleName) ;
            filename << ",\n" << "\"peakMz\": " << "\"NA\"" ;
            filename << ",\n" << "\"medianMz\": " << "\"NA\"" ;
            filename << ",\n" << "\"baseMz\": " << "\"NA\"" ;
            filename << ",\n" << "\"mzmin\": " << "\"NA\"" ;
            filename << ",\n" << "\"mzmax\": " << "\"NA\"" ;
            filename << ",\n" << "\"rt\": " << "\"NA\"" ;
            filename << ",\n" << "\"rtmin\": " << "\"NA\"" ;
            filename << ",\n" << "\"rtmax\": " << "\"NA\"" ;
            filename << ",\n" << "\"quality\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakIntensity\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakBaseLineLevel\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakArea\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakSplineArea\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakAreaTop\": " << "\"NA\"";
            filename << ",\n" << "\"peakAreaNotCorrected\": " << "\"NA\"";
            filename << ",\n" << "\"peakAreaTopNotCorrected\": " << "\"NA\"";
            filename << ",\n" << "\"noNoiseObs\": " << "\"NA\"" ;
            filename << ",\n" << "\"signalBaselineRatio\": " << "\"NA\"" ;
            filename << ",\n" << "\"fromBlankSample\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakAreaFractional\": " << "\"NA\"" ;
            filename << ",\n" << "\"symmetry\": " << "\"NA\"" ;
            filename << ",\n" << "\"noNoiseFraction\": " << "\"NA\"" ;
            filename << ",\n" << "\"groupOverlap\": " << "\"NA\"";
            filename << ",\n" << "\"groupOverlapFrac\": " << "\"NA\"" ;
            filename << ",\n" << "\"gaussFitR2\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakRank\": " << "\"NA\"" ;
            filename << ",\n" << "\"peakWidth\": " << "\"NA\"" ;
        }
        _writeEIC(grp, filename, sample);
    }

    filename << "\n]" ; //peaks

    filename << "}" ; //group
}


void JSONReports::_writeEIC(PeakGroup& grp, ofstream& filename, mzSample* sample)
{

    filename << setprecision(10);

    mzSlice* slice= new mzSlice();

    int charge = _mavenParameters->getCharge(grp.getCompound());
    slice->mz = grp.getExpectedMz(charge);

    if (slice->mz == -1)
        slice->mz = grp.meanMz;

    EIC* eic = nullptr;

    //TODO: Refactor the code :Sahil
    if (grp.hasCompoundLink()) {
        if ( !grp.srmId.empty() ) { //MS-MS case 1
            eic = (sample)->getEIC(grp.srmId, _mavenParameters->eicType);
        }
        else if((grp.getCompound()->precursorMz() > 0) & (grp.getCompound()->productMz() > 0)) { //MS-MS case 2
            //TODO: this is a problem -- amuQ1 and amuQ3 that were used to generate the peakgroup are not stored anywhere
            //will use mainWindow->MavenParameters for now but those values may have changed between generation and export
            eic =(sample)->getEIC(grp.getCompound()->precursorMz(), grp.getCompound()->collisionEnergy(), grp.getCompound()->productMz(),_mavenParameters->eicType,
                               _mavenParameters->filterline, _mavenParameters->amuQ1, _mavenParameters->amuQ3);
        }
       else {//MS1 case
            //TODO: same problem here: need the ppm that was used, or the slice object
            //for mz could rely on same computation being done way above
            //redoing it only for code clarity
            MassCutoff *massCutoff=_mavenParameters->compoundMassCutoffWindow;
            slice->mzmin = slice->mz - massCutoff->massCutoffValue(slice->mz);
            slice->mzmax = slice->mz + massCutoff->massCutoffValue(slice->mz);
            slice->rtmin = grp.minRt - _outputRtWindow;
            slice->rtmax = grp.maxRt + _outputRtWindow;
            eic = (sample)->getEIC(slice->mzmin,slice->mzmax,slice->rtmin,slice->rtmax,1,
                                   _mavenParameters->eicType,
                                   _mavenParameters->filterline);
            }
    }else {
        //no compound information
        //TODO: same problem here: need the ppm that was used, or the slice object
        slice->mz=grp.meanMz;
        MassCutoff *massCutoff=_mavenParameters->compoundMassCutoffWindow;
        slice->mzmin = slice->mz - massCutoff->massCutoffValue(slice->mz);
        slice->mzmax = slice->mz + massCutoff->massCutoffValue(slice->mz);
        slice->rtmin = grp.minRt - _outputRtWindow;
        slice->rtmax = grp.maxRt + _outputRtWindow;
        eic = (sample)->getEIC(slice->mzmin, slice->mzmax, slice->rtmin, slice->rtmax,1,
                               _mavenParameters->eicType,
                               _mavenParameters->filterline);
    }

    //TODO: for MS1 we've already limited RT range, but for MS/MS the entire RT range of the SRM will be output
    //either check here or edit getEIC functionality
    if(eic) {
        int N = eic->rt.size();
        filename << ",\n" << "\"eic\": {" ;
        filename << "\"rt\": [";

        for(int i = 0; i < N; i++){
            if ( eic->rt[i] > 0) {
                filename << eic->rt[i];
                if (i < N - 1) filename << ",";
            }
        }

        filename << "],\n" ; //rt
        filename << "\"intensity\": [";
        for(int i= 0; i < N; i++){
            if ( eic->rt[i] > 0) {
                filename << eic->intensity[i];
                if (i < N - 1) filename << ",";
            }
        }

        filename << "]" ; //intensity
        filename << "\n}" ;//eic
        filename << "\n}" ; //peak
        delete(eic);
    }

}



void JSONReports::save(string filename, vector<PeakGroup> allgroups, vector<mzSample*> samples)
{
    ofstream file(filename.c_str());
    file << setprecision(10);

    file << "{\"groups\": [" <<endl;

    int groupId = 0;
    int metaGroupId = 0;

    for(size_t i=0; i < allgroups.size() ; i++ ) {
        PeakGroup& grp = allgroups[i];

        //if compound is unknown, output only the unlabeled form information
        if(grp.getCompound() == NULL || grp.childCount() == 0) {
            grp.groupId = ++groupId;
            grp.metaGroupId = ++metaGroupId;
            if(groupId > 1) file<< "\n,";
            _writeGroup(grp,file);
            if(grp.hasCompoundLink())
                _writeCompoundLink(grp, file);
            _writePeak(grp, file, samples);

        } else {
            //output all relevant isotope info otherwise
            //does this work? is children[0] always the same as grp (parent)?
            grp.metaGroupId = ++ metaGroupId;
            for (unsigned int k = 0; k < grp.children.size(); k++) {
                grp.children[k].metaGroupId = grp.metaGroupId;
                grp.children[k].groupId = ++groupId;
                if(groupId > 1) file << "\n,";
                _writeGroup(grp.children[k], file);
                if( grp.children[k].hasCompoundLink() )
                    _writeCompoundLink(grp, file);
                _writePeak(grp.children[k], file, samples);
            }
        }
    }
    file << "]}"; //groups
    file.close();
}

string JSONReports::_sanitizeJSONstring(string s)
{
    boost::replace_all(s, "\"","\\\"");
    s = "\""+s+"\"";    //quote the whole string
    return s;
}

////////////////////////////////////////TestCASES////////////////////////////////////////////

/**
 *@brief Defines the test cases to test JSONReports class.
 * @details Generates the json file by calling the correspoding
 * functions of the Json class. Compare it against the already
 * saved Jsonfile in "tests/test-libmaven" directory.
 */
TEST_CASE_FIXTURE(SampleLoadingFixture,"Test writing to the JSON file")
{
    targetedGroup();
    string jsonFilename = "test.json";
    JSONReports* jsonReports = new JSONReports(mavenparameters(), false);
    auto samplesUsed = samples();
    sort(begin(samplesUsed), end(samplesUsed), mzSample::compSampleSort);
    jsonReports->save(jsonFilename, allgroups(), samplesUsed);

    ifstream fileInput("test.json");
    ifstream fileSaved("tests/test-libmaven/test_jsonReports.json");

    json rootInput = json::parse(fileInput);
    json rootSaved = json::parse(fileSaved);
    REQUIRE_MESSAGE(rootSaved["groups"].size() == rootInput["groups"].size(),
                    "number of groups in the two reports do not match");

    // samples are loaded in different order at different insatance.
    // correct group must be found in the json files to compare.
    for(size_t input = 0; input < rootInput["groups"].size(); input++) {
        size_t saved = -1;
        for(size_t s = 0; s < rootSaved["groups"].size(); s++ ) {
            if (rootInput["groups"][input]["meanMz"].get<double>() ==
                    doctest::Approx(rootSaved["groups"][s]["meanMz"].get<double>())
                && rootInput["groups"][input]["meanRt"].get<double>() ==
                       doctest::Approx(rootSaved["groups"][s]["meanRt"].get<double>())
                && rootInput["groups"][input]["compound"]["compoundName"].get<string>() ==
                       rootSaved["groups"][s]["compound"]["compoundName"].get<string>()) {
                saved = s;
                break;
            }
        }

        auto compoundName = rootInput["groups"][input]["compound"]["compoundName"].get<string>();
        REQUIRE_MESSAGE(saved != -1, "compound \""<< compoundName << "\" not found");

        REQUIRE(saved <= rootSaved["groups"].size());

        string labelInput = "";
        string labelSaved = "";
        if(!rootInput["groups"][input]["label"].is_null())
            labelInput = rootInput["groups"][input]["label"].get<string>();

        if(!rootSaved["groups"][saved]["label"].is_null())
            labelSaved = rootSaved["groups"][saved]["label"].get<string>();

        double meanMzInput = rootInput["groups"][input]["meanMz"].get<double>();
        double meanMzSaved = rootSaved["groups"][saved]["meanMz"].get<double>();

        double meanRtInput = rootInput["groups"][input]["meanRt"].get<double>();
        double meanRtSaved = rootSaved["groups"][saved]["meanRt"].get<double>();

        double rtminInput = rootInput["groups"][input]["rtmin"].get<double>();
        double rtminSaved = rootSaved["groups"][saved]["rtmin"].get<double>();

        double rtmaxInput = rootInput["groups"][input]["rtmax"].get<double>();
        double rtmaxSaved = rootSaved["groups"][saved]["rtmax"].get<double>();

        double maxQualityInput = rootInput["groups"][input]["maxQuality"].get<double>();
        double maxQualitySaved = rootSaved["groups"][saved]["maxQuality"].get<double>();

        REQUIRE(labelInput == labelSaved);
        REQUIRE(meanMzInput == doctest::Approx(meanMzSaved).epsilon(0.05));
        REQUIRE(meanRtInput == doctest::Approx(meanRtSaved).epsilon(0.05));
        REQUIRE(rtminInput == doctest::Approx(rtminSaved).epsilon(0.05));
        REQUIRE(rtmaxInput == doctest::Approx(rtmaxSaved).epsilon(0.05));
        REQUIRE(maxQualityInput == doctest::Approx(maxQualitySaved).epsilon(0.05));

        string compoundIdInput = rootInput["groups"][input]["compound"]["compoundId"].get<string>();
        string compoundIdSaved = rootSaved["groups"][saved]["compound"]["compoundId"].get<string>();

        string compoundNameInput = rootInput["groups"][input]["compound"]["compoundName"].get<string>();
        string compoundNameSaved = rootSaved["groups"][saved]["compound"]["compoundName"].get<string>();

        string formulaInput = rootInput["groups"][input]["compound"]["formula"].get<string>();
        string formulaSaved = rootSaved["groups"][saved]["compound"]["formula"].get<string>();

        double expectedRtInput = rootInput["groups"][input]["compound"]["expectedRt"].get<double>();
        double expectedRtSaved = rootSaved["groups"][saved]["compound"]["expectedRt"].get<double>();

        double expectedMzInput = rootInput["groups"][input]["compound"]["expectedMz"].get<double>();
        double expectedMzSaved = rootSaved["groups"][saved]["compound"]["expectedMz"].get<double>();

        string srmIdInput = "";
        string srmIdSaved = "";

        if(!rootInput["groups"][input]["compound"]["srmId"].is_null())
            srmIdInput = rootInput["groups"][input]["compound"]["srmId"].get<string>();

        if(!rootSaved["groups"][saved]["compound"]["srmId"].is_null())
            srmIdSaved= rootSaved["groups"][saved]["compound"]["srmId"].get<string>();

        string tagStringInput = "";
        string tagStringSaved = "";

        if(!rootInput["groups"][input]["compound"]["tagString"].is_null())
            tagStringInput = rootInput["groups"][input]["compound"]["tagString"].get<string>();

        if(!rootInput["groups"][saved]["compound"]["tagString"].is_null())
            tagStringSaved = rootSaved["groups"][saved]["compound"]["tagString"].get<string>();

        string fullCompoundNameInput = rootInput["groups"][input]["compound"]["fullCompoundName"].get<string>();
        string fullCompoundNameSaved = rootSaved["groups"][saved]["compound"]["fullCompoundName"].get<string>();

        string fullCompoundIDInput = rootInput["groups"][input]["compound"]["fullCompoundID"].get<string>();
        string fullCompoundIDSaved = rootSaved["groups"][saved]["compound"]["fullCompoundID"].get<string>();

        REQUIRE( compoundIdInput == compoundIdSaved );
        REQUIRE( compoundNameInput == compoundNameSaved );
        REQUIRE( formulaInput == formulaSaved );
        REQUIRE( expectedRtInput == doctest::Approx(expectedRtSaved).epsilon(0.05) );
        REQUIRE( expectedMzInput == doctest::Approx(expectedMzSaved).epsilon(0.05) );
        REQUIRE( srmIdInput == srmIdSaved );
        REQUIRE( tagStringInput == tagStringSaved );
        REQUIRE( fullCompoundNameInput == fullCompoundNameSaved );
        REQUIRE( fullCompoundIDInput == fullCompoundIDSaved );

        for(size_t i = 0; i < rootInput["groups"][input]["peaks"].size(); i++) {
            string sampleNameInput;
            string sampleNameSaved;

            REQUIRE(!rootInput["groups"][input]["peaks"][i]["sampleName"].is_null());
            sampleNameInput = rootInput["groups"][input]["peaks"][i]["sampleName"].get<string>();

            REQUIRE(!rootInput["groups"][input]["peaks"][i]["sampleName"].is_null());
            sampleNameSaved = rootSaved["groups"][saved]["peaks"][i]["sampleName"].get<string>();

            double pMzInput = rootInput["groups"][input]["peaks"][i]["peakMz"].get<double>();
            double pMzSaved = rootSaved["groups"][saved]["peaks"][i]["peakMz"].get<double>();

            double mMzInput = rootInput["groups"][input]["peaks"][i]["medianMz"].get<double>();
            double mMzSaved = rootSaved["groups"][saved]["peaks"][i]["medianMz"].get<double>();

            double bMzInput = rootInput["groups"][input]["peaks"][i]["baseMz"].get<double>();
            double bMzSaved = rootSaved["groups"][saved]["peaks"][i]["baseMz"].get<double>();

            double mzminInput = rootInput["groups"][input]["peaks"][i]["mzmin"].get<double>();
            double mzminSaved = rootSaved["groups"][saved]["peaks"][i]["mzmin"].get<double>();

            double mzmaxInput = rootInput["groups"][input]["peaks"][i]["mzmax"].get<double>();
            double mzmaxSaved = rootSaved["groups"][saved]["peaks"][i]["mzmax"].get<double>();

            double rtInput = rootInput["groups"][input]["peaks"][i]["rt"].get<double>();
            double rtSaved = rootSaved["groups"][saved]["peaks"][i]["rt"].get<double>();

            double rtminInput = rootInput["groups"][input]["peaks"][i]["rtmin"].get<double>();
            double rtminSaved = rootSaved["groups"][saved]["peaks"][i]["rtmin"].get<double>();

            double rtmaxInput = rootInput["groups"][input]["peaks"][i]["rtmax"].get<double>();
            double rtmaxSaved = rootSaved["groups"][saved]["peaks"][i]["rtmax"].get<double>();

            double qualityInput = rootInput["groups"][input]["peaks"][i]["quality"].get<double>();
            double qualitySaved = rootSaved["groups"][saved]["peaks"][i]["quality"].get<double>();

            double pIInput = rootInput["groups"][input]["peaks"][i]["peakIntensity"].get<double>();
            double pISaved = rootSaved["groups"][saved]["peaks"][i]["peakIntensity"].get<double>();

            double pBLLInput = rootInput["groups"][input]["peaks"][i]["peakBaseLineLevel"].get<double>();
            double pBLLSaved = rootSaved["groups"][saved]["peaks"][i]["peakBaseLineLevel"].get<double>();

            double pAreaInput = rootInput["groups"][input]["peaks"][i]["peakArea"].get<double>();
            double pAreaSaved = rootSaved["groups"][saved]["peaks"][i]["peakArea"].get<double>();

            double pSAreaInput = rootInput["groups"][input]["peaks"][i]["peakSplineArea"].get<double>();
            double pSAreaSaved = rootSaved["groups"][saved]["peaks"][i]["peakSplineArea"].get<double>();

            double pATopInput = rootInput["groups"][input]["peaks"][i]["peakAreaTop"].get<double>();
            double pATopSaved = rootSaved["groups"][saved]["peaks"][i]["peakAreaTop"].get<double>();

            double pANotCorInput = rootInput["groups"][input]["peaks"][i]["peakAreaNotCorrected"].get<double>();
            double pANotCorSaved = rootSaved["groups"][saved]["peaks"][i]["peakAreaNotCorrected"].get<double>();

            double pATNotInput = rootInput["groups"][input]["peaks"][i]["peakAreaTopNotCorrected"].get<double>();
            double pATNOtSaved = rootSaved["groups"][saved]["peaks"][i]["peakAreaTopNotCorrected"].get<double>();

            double noNoiseInput = rootInput["groups"][input]["peaks"][i]["noNoiseObs"].get<double>();
            double noNoiseSaved = rootSaved["groups"][saved]["peaks"][i]["noNoiseObs"].get<double>();

            double sBRatioInput = rootInput["groups"][input]["peaks"][i]["signalBaselineRatio"].get<double>();
            double sBRatioSaved = rootSaved["groups"][saved]["peaks"][i]["signalBaselineRatio"].get<double>();

            double fBankSInput = rootInput["groups"][input]["peaks"][i]["fromBlankSample"].get<double>();
            double fBankSSaved = rootSaved["groups"][saved]["peaks"][i]["fromBlankSample"].get<double>();

            double pAFInput = rootInput["groups"][input]["peaks"][i]["peakAreaFractional"].get<double>();
            double pAFSaved = rootSaved["groups"][saved]["peaks"][i]["peakAreaFractional"].get<double>();

            double symmetryInput = rootInput["groups"][input]["peaks"][i]["symmetry"].get<double>();
            double symmetrySaved = rootSaved["groups"][saved]["peaks"][i]["symmetry"].get<double>();

            double noNFracInput = rootInput["groups"][input]["peaks"][i]["noNoiseFraction"].get<double>();
            double noNFracSaved = rootSaved["groups"][saved]["peaks"][i]["noNoiseFraction"].get<double>();

            double gOverlapInput = rootInput["groups"][input]["peaks"][i]["groupOverlap"].get<double>();
            double gOverlapSaved = rootSaved["groups"][saved]["peaks"][i]["groupOverlap"].get<double>();

            double gOFracInput = rootInput["groups"][input]["peaks"][i]["groupOverlapFrac"].get<double>();
            double gOFracSaved = rootSaved["groups"][saved]["peaks"][i]["groupOverlapFrac"].get<double>();

            double gFitInput = rootInput["groups"][input]["peaks"][i]["gaussFitR2"].get<double>();
            double gFitSaved = rootSaved["groups"][saved]["peaks"][i]["gaussFitR2"].get<double>();

            double pRankInput = rootInput["groups"][input]["peaks"][i]["peakRank"].get<double>();
            double pRankSaved = rootSaved["groups"][saved]["peaks"][i]["peakRank"].get<double>();

            double pWidthInput = rootInput["groups"][input]["peaks"][i]["peakWidth"].get<double>();
            double pWidthSaved = rootSaved["groups"][saved]["peaks"][i]["peakWidth"].get<double>();

            REQUIRE( pMzInput == doctest::Approx( pMzSaved ).epsilon(0.05));
            REQUIRE( mMzInput == doctest::Approx( mMzSaved ).epsilon(0.05));
            REQUIRE( bMzInput == doctest::Approx( bMzSaved ).epsilon(0.05));
            REQUIRE( mzminInput == doctest::Approx( mzminSaved ).epsilon(0.05));
            REQUIRE( mzmaxInput == doctest::Approx( mzmaxSaved ).epsilon(0.05));
            REQUIRE( rtInput == doctest::Approx( rtSaved ).epsilon(0.05));
            REQUIRE( rtminInput == doctest::Approx( rtminSaved ).epsilon(0.05));
            REQUIRE( rtmaxInput == doctest::Approx( rtmaxSaved ).epsilon(0.05));
            REQUIRE( qualityInput == doctest::Approx( qualitySaved ).epsilon(0.05));
            REQUIRE( pIInput == doctest::Approx( pISaved ).epsilon(0.05));
            REQUIRE( pBLLInput == doctest::Approx( pBLLSaved ).epsilon(0.05));
            REQUIRE( pAreaInput == doctest::Approx( pAreaSaved ).epsilon(0.05));
            REQUIRE( pSAreaInput == doctest::Approx( pSAreaSaved ).epsilon(0.05));
            REQUIRE( pATopInput == doctest::Approx( pATopSaved ).epsilon(0.05));
            REQUIRE( pANotCorInput == doctest::Approx( pANotCorSaved ).epsilon(0.05));
            REQUIRE( pATNOtSaved == doctest::Approx( pATNotInput ).epsilon(0.05));
            REQUIRE( noNoiseInput == doctest::Approx( noNoiseSaved ).epsilon(0.05));
            REQUIRE( sBRatioInput == doctest::Approx( sBRatioSaved ).epsilon(0.05));
            REQUIRE( fBankSInput == doctest::Approx( fBankSSaved ).epsilon(0.05));
            REQUIRE( pAFInput == doctest::Approx( pAFSaved ).epsilon(0.05));
            REQUIRE( symmetryInput == doctest::Approx( symmetrySaved ).epsilon(0.05));
            REQUIRE( noNFracInput == doctest::Approx( noNFracSaved ).epsilon(0.05));
            REQUIRE( gOFracInput == doctest::Approx( gOFracSaved ).epsilon(0.05));
            REQUIRE( gOverlapInput == doctest::Approx( gOverlapSaved ).epsilon(0.05));
            REQUIRE( gFitInput == doctest::Approx( gFitSaved ).epsilon(0.05));
            REQUIRE( pRankInput == doctest::Approx( pRankSaved ).epsilon(0.05));
            REQUIRE( pWidthInput == doctest::Approx( pWidthSaved ).epsilon(0.05));

            auto eicNodeInput = rootInput["groups"][input]["peaks"][i]["eic"];
            auto eicNodeSaved = rootSaved["groups"][saved]["peaks"][i]["eic"];

            REQUIRE (eicNodeInput["rt"].size() == eicNodeSaved["rt"].size());
            for(size_t l = 0; l < eicNodeInput["rt"].size(); l++) {
                auto input = eicNodeInput["rt"][l].get<double>();
                auto saved = eicNodeSaved["rt"][l].get<double>();
                REQUIRE(input == doctest::Approx( saved ).epsilon(0.05));
            }

            REQUIRE (eicNodeInput["intensity"].size() == eicNodeSaved["intensity"].size());
            for(size_t l = 0; l < eicNodeInput["intensity"].size(); l++) {
                auto input = eicNodeInput["intensity"][l].get<double>();
                auto saved = eicNodeSaved["intensity"][l].get<double>();
                REQUIRE(input == doctest::Approx( saved ).epsilon(0.05));
            }
        }
    }
    remove("test.json");
}

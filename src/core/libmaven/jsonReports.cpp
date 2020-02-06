#include "json.hpp"
#include "doctest.h"
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

    string compoundID = grp.getCompound()->id;
    filename << "\"compoundId\": "<< _sanitizeJSONstring(compoundID) ;
    string compoundName = grp.getCompound()->name;
    filename << ",\n" << "\"compoundName\": "<< _sanitizeJSONstring(compoundName);
    string formula = grp.getCompound()->formula();
    filename << ",\n" << "\"formula\": "<< _sanitizeJSONstring(formula);
    filename << ",\n" << "\"expectedRt\": " << grp.getCompound()->expectedRt;
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
        if (!grp.srmId.empty()) {
            //MS-MS case 1
            eic = (sample)->getEIC(grp.srmId, _mavenParameters->eicType);
        } else if((grp.getCompound()->precursorMz > 0) & (grp.getCompound()->productMz > 0)) {
            //MS-MS case 2
            //TODO: this is a problem -- amuQ1 and amuQ3 that were used to generate the peakgroup are not stored anywhere
            //will use mainWindow->MavenParameters for now but those values may have changed between generation and export
            eic =(sample)->getEIC(grp.getCompound()->precursorMz, grp.getCompound()->collisionEnergy, grp.getCompound()->productMz, _mavenParameters->eicType,
                                  _mavenParameters->filterline, _mavenParameters->amuQ1, _mavenParameters->amuQ3);
        } else {
            //MS1 case
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

class JsonReportsFixture{

    private:
            vector<mzSample*> _samples;
            vector <PeakGroup> _allgroups;
            MavenParameters* _mavenparameters;
            Databases _database;

            /**
             * @brief _makeSampleList Initialises the vector of the samples
             * to be loaded.
             */
            void _makeSampleList()
            {
                auto sample1 = new mzSample();
                auto sample2 = new mzSample();
                auto sample3 = new mzSample();
                auto sample4 = new mzSample();
                sample1->loadSample("bin/methods/091215_120i.mzXML");
                sample2->loadSample("bin/methods/091215_120M.mzXML");
                sample3->loadSample("bin/methods/091215_240i.mzXML");
                sample4->loadSample("bin/methods/091215_240M.mzXML");
                _samples.push_back(sample1);
                _samples.push_back(sample2);
                _samples.push_back(sample3);
                _samples.push_back(sample4);
                  
            }

            /**
             * @brief Loads sample and initialises the maven parameters.
             * @param samplesToLoad Vector of sample datasets to be loaded.
             * @param mavenparameters Object of MavenParameter class, stores
             * various parameters needed.
             */
            void _loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                         MavenParameters* mavenparameters)
            {
          
                ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
                string loadmodel = "bin/default.model";
                clsf->loadModel(loadmodel);
                mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10, "ppm");
                mavenparameters->clsf = clsf;
                mavenparameters->ionizationMode = -1;
                mavenparameters->matchRtFlag = true;
                mavenparameters->compoundRTWindow = 1;
                mavenparameters->samples = samplesToLoad;
                mavenparameters->eic_smoothingWindow = 10;
                mavenparameters->eic_smoothingAlgorithm = 1;
                mavenparameters->amuQ1 = 0.25;
                mavenparameters->amuQ3 = 0.30;
                mavenparameters->baseline_smoothingWindow = 5;
                mavenparameters->baseline_dropTopX = 80;
            }

            /**
             * @brief _getGroupsFromProcessCompounds Process the compounds
             * to give the peakgroups.
             * @return Vector of groups thus formed.
             */
            vector<PeakGroup> _getGroupsFromProcessCompounds()
            {
                const char* loadCompoundDB = "bin/methods/KNOWNS.csv";
                _database.loadCompoundCSVFile(loadCompoundDB);
                vector<Compound*> compounds =
                    _database.getCompoundsSubset("KNOWNS");

                _loadSamplesAndParameters(_samples, _mavenparameters);

                PeakDetector peakDetector;
                peakDetector.setMavenParameters(_mavenparameters);
                vector<mzSlice*> slices =
                    peakDetector.processCompounds(compounds, "compounds");
                peakDetector.processSlices(slices, "compounds");

                return _mavenparameters->allgroups;
            }

    public:
            /**
             * @brief JsonReportsFixture No parameter constructor.
             * @details The constructor calls other member functions to
             * initialise the vector of sample, sets the mavenparametrs
             * and form the peakgroups.
             */
            JsonReportsFixture()
            {
                _mavenparameters = new MavenParameters();
                _makeSampleList();
                _allgroups = _getGroupsFromProcessCompounds();
                 
            }


            /**
             * @brief Destructor deletes the initialised data members.
             */
            ~JsonReportsFixture()
            {
                delete _samples[0];
                delete _samples[1];
                delete _samples[2];
                delete _samples[3];
                delete _mavenparameters;
            }

            /**
             * @brief samples Returns vector of samples loaded.
             * @return
             */
            vector<mzSample*> samples()
            {
                return _samples;
            }

            /**
             * @brief allgroups Returns all peakgroups of the mavenparameters.
             * @return
             */
            vector<PeakGroup> allgroups()
            {
                return _allgroups;
            }
            MavenParameters* mavenparameters()
            {
                return _mavenparameters;
            }

};

/**
 *@brief Defines the test cases to test JSONReports class.
 * @details Generates the json file by calling the correspoding
 * functions of the Json class. Compare it against the already
 * saved Jsonfile in "tests/test-libmaven" directory.
 */
TEST_CASE_FIXTURE(JsonReportsFixture,"Test writing to the JSON file")
{
    string jsonFilename = "test.json";
    JSONReports* jsonReports = new JSONReports(mavenparameters(), false);
    jsonReports->save(jsonFilename, allgroups(), samples());

    ifstream fileInput("test.json");
    ifstream fileSaved("tests/test-libmaven/test_jsonReports.json");
                
    json rootInput = json::parse(fileInput);
    json rootSaved = json::parse(fileSaved);
                              

    size_t saved = 0;

    // samples are loaded in different order at different insatance.
    // correct group must be found in the json files to compare.
    for(size_t input=0 ; input < rootInput["groups"].size(); input++){
        size_t s = 0;
        for(s = 0; s < rootSaved["groups"].size(); s++ ){
            if(rootInput["groups"][input]["meanMz"].get<double>() ==
               doctest::Approx(rootSaved["groups"][s]["meanMz"].get<double>()) &&
               rootInput["groups"][input]["meanRt"].get<double>() ==
               doctest::Approx(rootSaved["groups"][s]["meanRt"].get<double>()) &&
               rootInput["groups"][input]["compound"]["compoundName"].get<string>() ==
               rootSaved["groups"][s]["compound"]["compoundName"].get<string>()){
                    saved = s;
                    break;
            }
        }

        REQUIRE(s <= rootSaved["groups"].size());

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

        for(size_t i = 0 ; i < rootInput["groups"][input]["peak"].size(); i++ ){
                                    
            string SampleNameInput = "";
            string SampleNameSaved = "";

            REQUIRE( !rootInput["groups"][input]["peak"][i]["sampleName"].is_null() );
            SampleNameInput = rootInput["groups"][input]["peak"][i]["sampleName"].get<string>();
                                    
            REQUIRE( !rootInput["groups"][input]["peak"][i]["sampleName"].is_null() );
            SampleNameSaved = rootSaved["groups"][saved]["peak"][i]["sampleName"].get<string>();
                                    
            double pMzInput = rootInput["groups"][input]["peak"][i]["peakMz"].get<double>();
            double pMzSaved = rootSaved["groups"][saved]["peak"][i]["peakMz"].get<double>();
                                    
            double mMzInput = rootInput["groups"][input]["peak"][i]["medianMz"].get<double>();
            double mMzSaved = rootSaved["groups"][saved]["peak"][i]["medianMz"].get<double>();
                                    
            double bMzInput = rootInput["groups"][input]["peak"][i]["baseMz"].get<double>();
            double bMzSaved = rootSaved["groups"][saved]["peak"][i]["baseMz"].get<double>();
                                    
            double mzminInput = rootInput["groups"][input]["peak"][i]["mzmin"].get<double>();
            double mzminSaved = rootSaved["groups"][saved]["peak"][i]["mzmin"].get<double>();
                                    
            double mzmaxInput = rootInput["groups"][input]["peak"][i]["mzmax"].get<double>();
            double mzmaxSaved = rootSaved["groups"][saved]["peak"][i]["mzmax"].get<double>();
                                    
            double rtInput = rootInput["groups"][input]["peak"][i]["rt"].get<double>();
            double rtSaved = rootSaved["groups"][saved]["peak"][i]["rt"].get<double>();
                                    
            double rtminInput = rootInput["groups"][input]["peak"][i]["rtmin"].get<double>();
            double rtminSaved = rootSaved["groups"][saved]["peak"][i]["rtmin"].get<double>();
                                    
            double rtmaxInput = rootInput["groups"][input]["peak"][i]["rtmax"].get<double>();
            double rtmaxSaved = rootSaved["groups"][saved]["peak"][i]["rtmax"].get<double>();
                                    
            double qualityInput = rootInput["groups"][input]["peak"][i]["quality"].get<double>();
            double qualitySaved = rootSaved["groups"][saved]["peak"][i]["quality"].get<double>();
            
            
            double pIInput = rootInput["groups"][input]["peak"][i]["peakIntensity"].get<double>();
            double pISaved = rootSaved["groups"][saved]["peak"][i]["peakIntensity"].get<double>();
                                            
            double pBLLInput = rootInput["groups"][input]["peak"][i]["peakBaseLineLevel"].get<double>();
            double pBLLSaved = rootSaved["groups"][saved]["peak"][i]["peakBaseLineLevel"].get<double>();
                                            
            double pAreaInput = rootInput["groups"][input]["peak"][i]["peakArea"].get<double>();
            double pAreaSaved = rootSaved["groups"][saved]["peak"][i]["peakArea"].get<double>();
                                            
            double pSAreaInput = rootInput["groups"][input]["peak"][i]["peakSplineArea"].get<double>();
            double pSAreaSaved = rootSaved["groups"][saved]["peak"][i]["peakSplineArea"].get<double>();
                                            
            double pATopInput = rootInput["groups"][input]["peak"][i]["peakAreaTop"].get<double>();
            double pATopSaved = rootSaved["groups"][saved]["peak"][i]["peakAreaTop"].get<double>();
                                            
            double pANotCorInput = rootInput["groups"][input]["peak"][i]["peakAreaNotCorrected"].get<double>();
            double pANotCorSaved = rootSaved["groups"][saved]["peak"][i]["peakAreaNotCorrected"].get<double>();
                                            
            double pATNotInput = rootInput["groups"][input]["peak"][i]["peakAreaTopNotCorrected"].get<double>();
            double pATNOtSaved = rootSaved["groups"][saved]["peak"][i]["peakAreaTopNotCorrected"].get<double>();
                                            
            double NoNoiseInput = rootInput["groups"][input]["peak"][i]["noNoiseObs"].get<double>();
            double NoNoiseSaved = rootSaved["groups"][saved]["peak"][i]["noNoiseObs"].get<double>();
                                            
            double SBRatioInput = rootInput["groups"][input]["peak"][i]["SignalBaselineRatio"].get<double>();
            double SBRatioSaved = rootSaved["groups"][saved]["peak"][i]["SignalBaselineRatio"].get<double>();
                                            
            double fBankSInput = rootInput["groups"][input]["peak"][i]["fromBankSample"].get<double>();
            double fBankSSaved = rootSaved["groups"][saved]["peak"][i]["peakIntensity"].get<double>();
                                            
            double pAFInput = rootInput["groups"][input]["peak"][i]["peakAreaFractional"].get<double>();
            double pAFSaved = rootSaved["groups"][saved]["peak"][i]["peakAreaFractional"].get<double>();
            
            double symmetryInput = rootInput["groups"][input]["peak"][i]["symmetry"].get<double>();
            double symmetrySaved = rootSaved["groups"][saved]["peak"][i]["symmetry"].get<double>();

            double noNFracInput = rootInput["groups"][input]["peak"][i]["noNoiseFraction"].get<double>();
            double noNFracSaved = rootSaved["groups"][saved]["peak"][i]["noNoiseFraction"].get<double>();
                                                
                                                        
            double gOverlapInput = rootInput["groups"][input]["peak"][i]["groupOverlap"].get<double>();
            double gOverlapSaved = rootSaved["groups"][saved]["peak"][i]["groupOverlap"].get<double>();
                                                
            double gOFracInput = rootInput["groups"][input]["peak"][i]["groupOverlapFrac"].get<double>();
            double gOFracSaved = rootSaved["groups"][saved]["peak"][i]["groupOverlapFrac"].get<double>();
                                                
            double gFitInput = rootInput["groups"][input]["peak"][i]["gaussFitR2"].get<double>();
            double gFitSaved = rootSaved["groups"][saved]["peak"][i]["gaussFitR2"].get<double>();

            double pRankInput = rootInput["groups"][input]["peak"][i]["peakRank"].get<double>();
            double pRankSaved = rootSaved["groups"][saved]["peak"][i]["peakRank"].get<double>();
                                                
            double pWidthInput = rootInput["groups"][input]["peak"][i]["peakWidth"].get<double>();
            double pWidthSaved = rootSaved["groups"][saved]["peak"][i]["peakWidth"].get<double>();

            REQUIRE( SampleNameSaved == SampleNameInput );
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
            REQUIRE( NoNoiseInput == doctest::Approx( NoNoiseSaved ).epsilon(0.05));
            REQUIRE( SBRatioInput == doctest::Approx( SBRatioSaved ).epsilon(0.05));
            REQUIRE( fBankSInput == doctest::Approx( fBankSSaved ).epsilon(0.05));
            REQUIRE( pAFInput == doctest::Approx( pAFSaved ).epsilon(0.05));
            REQUIRE( symmetryInput == doctest::Approx( symmetrySaved ).epsilon(0.05));
            REQUIRE( noNFracInput == doctest::Approx( noNFracSaved ).epsilon(0.05));
            REQUIRE( gOFracInput == doctest::Approx( gOFracSaved ).epsilon(0.05));
            REQUIRE( gOverlapInput == doctest::Approx( gOverlapSaved ).epsilon(0.05));
            REQUIRE( gFitInput == doctest::Approx( gFitSaved ).epsilon(0.05));
            REQUIRE( pRankInput == doctest::Approx( pRankSaved ).epsilon(0.05));
            REQUIRE( pWidthInput == doctest::Approx( pWidthSaved ).epsilon(0.05));
            
            for(size_t l = 0 ; l < rootInput["groups"][input]["peak"][i]["eic"]["rt"].size() ; l++)
                REQUIRE(rootInput["groups"][input]["peak"][i]["eic"]["rt"][l] ==
                                                                rootSaved["groups"][saved]["peak"][i]["eic"]["rt"][l]);
                                                                                              
            for(size_t l = 0; i < rootInput["groups"][input]["peak"][i]["eic"]["intensity"].size() ; l++)
                REQUIRE(rootInput["groups"][input]["peak"][i]["eic"]["intensity"][l] == 
                                                                rootSaved["groups"][saved]["peak"][i]["eic"]["intensity"][l]);
                                
        }
    }
    remove("test.json");
}

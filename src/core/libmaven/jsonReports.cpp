#include "doctest.h"
#include "json.hpp"

using json = nlohmann::json;

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


JSONReports::JSONReports(MavenParameters* _mp, bool pollyUpload):
    _uploadToPolly(pollyUpload)
{
    _mavenParameters=_mp;
}


void JSONReports::_writeGroup(PeakGroup& grp, ofstream& myfile) {
    
    //add labels to json file
    char lab = grp.label;

    PeakGroup* parentGroup = grp.getParent();
    if (parentGroup) {
        if (grp.label == '\0') {
            lab = parentGroup->label;
        }
    }

    myfile << setprecision(10);
    myfile << "{\n";
    myfile << "\"groupId\": " << grp.groupId ;

    myfile << ",\n" << "\"label\": ";
    myfile << "\"";
    if (lab == 'g' || lab == 'b') myfile << lab;
    myfile << "\"";

    if(_uploadToPolly) {

        int mlLabel =  (grp.markedGoodByCloudModel) ? 1 : (grp.markedBadByCloudModel) ? -1 : 0;
        myfile  << ",\n" << "\"ml-label\": " << mlLabel;
    }

    myfile << ",\n" << "\"metaGroupId\": " << grp.metaGroupId ;
    myfile << ",\n" << "\"meanMz\": " << grp.meanMz  ;
    myfile << ",\n" << "\"meanRt\": " << grp.meanRt ;
    myfile << ",\n" << "\"rtmin\": " << grp.minRt ;
    myfile << ",\n" << "\"rtmax\": " << grp.maxRt ;
    myfile << ",\n" << "\"maxQuality\": " << grp.maxQuality ;
}


void JSONReports::_writeCompoundLink(PeakGroup& grp,ofstream& myfile){
    
    myfile << setprecision(10);
    double mz;
    
    int charge = _mavenParameters->getCharge(grp.getCompound());
    mz = grp.getExpectedMz(charge);

    if (mz == -1) {
        mz = grp.meanMz;
    }
    
    myfile << ",\n" << "\"compound\": { " ;

    string compoundID = grp.getCompound()->id;
    myfile << "\"compoundId\": "<< _sanitizeJSONstring(compoundID) ;
    string compoundName = grp.getCompound()->name;
    myfile << ",\n" << "\"compoundName\": "<< _sanitizeJSONstring(compoundName);
    string formula = grp.getCompound()->formula;
    myfile << ",\n" << "\"formula\": "<< _sanitizeJSONstring(formula);
    myfile << ",\n" << "\"expectedRt\": " << grp.getCompound()->expectedRt;
    myfile << ",\n" << "\"expectedMz\": " << mz ;
    myfile << ",\n" << "\"srmID\": " << _sanitizeJSONstring(grp.srmId) ;
    myfile << ",\n" << "\"tagString\": " << _sanitizeJSONstring(grp.tagString) ;

    string fullTag = grp.srmId + grp.tagString;
    string fullName=compoundName;
    string fullID=compoundID;
    if (fullTag.length()) {
        fullName = compoundName + " [" + fullTag + "]";
        fullID = compoundID + " [" + fullTag + "]";
    }
    myfile << ",\n" << "\"fullCompoundName\": "<< _sanitizeJSONstring(fullName);
    myfile << ",\n" << "\"fullCompoundID\": "<< _sanitizeJSONstring(fullID) ;
    myfile << "}" ; // compound
}


void JSONReports::_writePeak(PeakGroup& grp, ofstream& myfile, vector<mzSample*> vsamples){

    myfile << setprecision(10);
    
    myfile << ",\n"<< "\"peaks\": [ " ;

    for(std::vector<mzSample*>::iterator it = vsamples.begin(); it != vsamples.end(); ++it) {
        if (it!=vsamples.begin()) {
            myfile << ",\n";
        }
    
        Peak* peak = grp.getPeak(*it);
        if(peak) {
            //TODO: add slice information here: e.g. what ppm was used
            myfile << "{\n";
            myfile << "\"sampleName\": " << _sanitizeJSONstring((*it)->sampleName) ;
            myfile << ",\n" << "\"peakMz\": " << peak->peakMz ;
            myfile << ",\n" << "\"medianMz\": " << peak->medianMz ;
            myfile << ",\n" << "\"baseMz\": " << peak->baseMz ;
            myfile << ",\n" << "\"mzmin\": " << peak->mzmin ;
            myfile << ",\n" << "\"mzmax\": " << peak->mzmax ;
            myfile << ",\n" << "\"rt\": " << peak->rt ;
            myfile << ",\n" << "\"rtmin\": " << peak->rtmin ;
            myfile << ",\n" << "\"rtmax\": " << peak->rtmax ;
            myfile << ",\n" << "\"quality\": " << peak->quality ;
            myfile << ",\n" << "\"peakIntensity\": " << peak->peakIntensity ;
            myfile << ",\n" << "\"peakBaseLineLevel\": " << peak->peakBaseLineLevel ;
            myfile << ",\n" << "\"peakArea\": " << peak->peakAreaCorrected ;
            myfile << ",\n" << "\"peakSplineArea\": " << peak->peakSplineArea ;
            myfile << ",\n" << "\"peakAreaTop\": " << peak->peakAreaTopCorrected;
            myfile << ",\n" << "\"peakAreaNotCorrected\": " << peak->peakArea;
            myfile << ",\n" << "\"peakAreaTopNotCorrected\": " << peak->peakAreaTop;
            myfile << ",\n" << "\"noNoiseObs\": " << peak->noNoiseObs ;
            myfile << ",\n" << "\"signalBaselineRatio\": " << peak->signalBaselineRatio ;
            myfile << ",\n" << "\"fromBlankSample\": " << peak->fromBlankSample ;
            myfile << ",\n" << "\"peakAreaFractional\": " << peak->peakAreaFractional ;
            myfile << ",\n" << "\"symmetry\": " << peak->symmetry ;
            myfile << ",\n" << "\"noNoiseFraction\": " << peak->noNoiseFraction ;
            myfile << ",\n" << "\"groupOverlap\": " << peak->groupOverlap ;
            myfile << ",\n" << "\"groupOverlapFrac\": " << peak->groupOverlapFrac ;
            myfile << ",\n" << "\"gaussFitR2\": " << peak->gaussFitR2 ;
            myfile << ",\n" << "\"peakRank\": " << peak->peakRank ;
            myfile << ",\n" << "\"peakWidth\": " << peak->width ;
        }
        else {
            myfile << "{\n";
            myfile << "\"sampleName\": " << _sanitizeJSONstring((*it)->sampleName) ;
            myfile << ",\n" << "\"peakMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"medianMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"baseMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"mzmin\": " << "\"NA\"" ;
            myfile << ",\n" << "\"mzmax\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rt\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rtmin\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rtmax\": " << "\"NA\"" ;
            myfile << ",\n" << "\"quality\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakIntensity\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakBaseLineLevel\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakArea\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakSplineArea\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakAreaTop\": " << "\"NA\"";
            myfile << ",\n" << "\"peakAreaNotCorrected\": " << "\"NA\"";
            myfile << ",\n" << "\"peakAreaTopNotCorrected\": " << "\"NA\"";
            myfile << ",\n" << "\"noNoiseObs\": " << "\"NA\"" ;
            myfile << ",\n" << "\"signalBaselineRatio\": " << "\"NA\"" ;
            myfile << ",\n" << "\"fromBlankSample\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakAreaFractional\": " << "\"NA\"" ;
            myfile << ",\n" << "\"symmetry\": " << "\"NA\"" ;
            myfile << ",\n" << "\"noNoiseFraction\": " << "\"NA\"" ;
            myfile << ",\n" << "\"groupOverlap\": " << "\"NA\"";
            myfile << ",\n" << "\"groupOverlapFrac\": " << "\"NA\"" ;
            myfile << ",\n" << "\"gaussFitR2\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakRank\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakWidth\": " << "\"NA\"" ;
        }
        
        _writeEIC(grp,myfile,*it);
    }
    
    myfile << "\n]" ; //peaks
    
    myfile << "}" ; //group
}


void JSONReports::_writeEIC(PeakGroup& grp, ofstream& myfile, mzSample* it){
    
    myfile << setprecision(10);
    
    mzSlice* slice= new mzSlice();
    
    int charge = _mavenParameters->getCharge(grp.getCompound());
    slice->mz = grp.getExpectedMz(charge);

    if (slice->mz == -1) {
        slice->mz = grp.meanMz;
    }
    
    
    EIC* eic=NULL;
                
    //TODO: Refactor the code :Sahil
        
    if (grp.hasCompoundLink()) {
        if ( !grp.srmId.empty() ) { //MS-MS case 1
            eic = (it)->getEIC(grp.srmId, _mavenParameters->eicType);
        }
        else if((grp.getCompound()->precursorMz > 0) & (grp.getCompound()->productMz > 0)) { //MS-MS case 2
            //TODO: this is a problem -- amuQ1 and amuQ3 that were used to generate the peakgroup are not stored anywhere
            //will use mainWindow->MavenParameters for now but those values may have changed between generation and export
            eic =(it)->getEIC(grp.getCompound()->precursorMz, grp.getCompound()->collisionEnergy, grp.getCompound()->productMz,_mavenParameters->eicType,
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
            eic = (it)->getEIC(slice->mzmin,slice->mzmax,slice->rtmin,slice->rtmax,1,
                                _mavenParameters->eicType,
                                _mavenParameters->filterline);
            }
    }

    else {//no compound information
          //TODO: same problem here: need the ppm that was used, or the slice object
        slice->mz=grp.meanMz;
        MassCutoff *massCutoff=_mavenParameters->compoundMassCutoffWindow;
        slice->mzmin = slice->mz - massCutoff->massCutoffValue(slice->mz);
        slice->mzmax = slice->mz + massCutoff->massCutoffValue(slice->mz);
        slice->rtmin = grp.minRt - _outputRtWindow;
        slice->rtmax = grp.maxRt + _outputRtWindow;
        eic = (it)->getEIC(slice->mzmin,slice->mzmax,slice->rtmin,slice->rtmax,1,
                            _mavenParameters->eicType,
                            _mavenParameters->filterline);
    }

    //TODO: for MS1 we've already limited RT range, but for MS/MS the entire RT range of the SRM will be output
    //either check here or edit getEIC functionality
    if(eic) {
        
        int N = eic->rt.size();
        myfile << ",\n" << "\"eic\": {" ;
        myfile << "\"rt\": [";

        for(int i=0;i<N;i++){
            if ( eic->rt[i] > 0) {
                myfile << eic->rt[i];
                if (i < N - 1) myfile << ",";
            }
        }
            
        myfile << "],\n" ; //rt
        myfile << "\"intensity\": [";
        for(int i=0;i<N;i++){
            if ( eic->rt[i] > 0) {
                myfile << eic->intensity[i];
                if (i < N - 1) myfile << ",";
            }
        }
            
        myfile << "]" ; //intensity
        myfile << "\n}" ;//eic

        myfile << "\n}" ; //peak
        delete(eic);
    }
   
}



void JSONReports::save(string filename,vector<PeakGroup> allgroups,vector<mzSample*> samples) {
    ofstream myfile(filename.c_str());
    myfile << setprecision(10);

    myfile << "{\"groups\": [" <<endl;

    int groupId=0;
    int metaGroupId=0;
   
    for(int i=0; i < static_cast<int>(allgroups.size()) ; i++ ) {
        PeakGroup& grp = allgroups[i];

        //if compound is unknown, output only the unlabeled form information
        if( grp.getCompound() == NULL || grp.childCount() == 0 ) {
            grp.groupId= ++groupId;
            grp.metaGroupId= ++metaGroupId;
            if(groupId>1) myfile << "\n,";

            _writeGroup(grp,myfile);  
            if( grp.hasCompoundLink() )
                _writeCompoundLink(grp,myfile);
            _writePeak(grp,myfile,samples);
            //writeEIC(grp,myfile,samples);
        }
        else { //output all relevant isotope info otherwise
            //does this work? is children[0] always the same as grp (parent)?
            grp.metaGroupId = ++ metaGroupId;
            for (unsigned int k=0; k < grp.children.size(); k++) {
                grp.children[k].metaGroupId = grp.metaGroupId;
                grp.children[k].groupId= ++groupId;
                if(groupId>1) myfile << "\n,";
            
                _writeGroup(grp.children[k],myfile);
                if( grp.children[k].hasCompoundLink() )
                    _writeCompoundLink(grp,myfile);
                _writePeak(grp.children[k],myfile,samples);
                //writeEIC(grp.children[k],myfile,samples);
            }
        }
        //Q_EMIT(updateProgressBar("Writing to json file. Please wait...", i, allgroups.size() - 1));
    }
    myfile << "]}"; //groups
    myfile.close();
    //Q_EMIT(updateProgressBar("Writing to json complete.", 1, 1));
}

string JSONReports::_sanitizeJSONstring(string s) {
    boost::replace_all(s, "\"","\\\"");
    //replace(s.begin(),s.end(),"\"","\\\""); //escape quotes
    s="\""+s+"\""; //quote the whole string
    return s;
}





class JsonReportsFixture{
    
    
    protected:
            
            vector<mzSample*> samples;
               
            vector <PeakGroup> allgroups;
            
            MavenParameters* mavenparameters;

    private:
            

            Databases database;
            
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
                
                samples.push_back(sample1);
                samples.push_back(sample2);
                samples.push_back(sample3);
                samples.push_back(sample4);
                  
            }
            
            
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

            vector<PeakGroup> _getGroupsFromProcessCompounds()
            {
                const char* loadCompoundDB = "bin/methods/KNOWNS.csv";
                database.loadCompoundCSVFile(loadCompoundDB);
                vector<Compound*> compounds =
                    database.getCompoundsSubset("KNOWNS");

          
                _loadSamplesAndParameters(samples,mavenparameters);

                PeakDetector peakDetector;
                peakDetector.setMavenParameters(mavenparameters);
                vector<mzSlice*> slices =
                    peakDetector.processCompounds(compounds, "compounds");
                peakDetector.processSlices(slices, "compounds");

                return mavenparameters->allgroups;
            }

            
            
    public:
            JsonReportsFixture(){
                mavenparameters= new MavenParameters();
                                
                _makeSampleList();
          
                allgroups=_getGroupsFromProcessCompounds();
                 
            }
                
            ~JsonReportsFixture(){
                delete samples[0];
                delete samples[1];
                delete samples[2];
                delete samples[3];
                delete mavenparameters;
            }            
            
};
    
    
TEST_CASE_FIXTURE(JsonReportsFixture,"Test writing to the JSON file")
{
    string jsonFilename = "test.json";
    JSONReports* jsonReports = new JSONReports(mavenparameters, false);
    jsonReports->save(jsonFilename, allgroups, samples);
    
    ifstream fileInput("test.json");
    ifstream fileSaved("C:\\msys64\\home\\kashika\\ElMaven\\tests\\doctest\\test_jsonReports.json");
                
    json rootInput = json::parse(fileInput);
    json rootSaved = json::parse(fileSaved);
                              
    int input;
    int saved; 
        
    for(input=0 , saved=0 ; input < static_cast<int>( rootInput["groups"].size() )&& 
                                        saved < static_cast<int>( rootSaved["groups"].size() ) ; input++, saved++){
                    
        int groupIdInput = rootInput["groups"][input]["groupId"].get<int>();
        int groupIdSaved = rootSaved["groups"][saved]["groupId"].get<int>();
                    
        string labelInput="";
        string labelSaved="";
        if( ! rootInput["groups"][input]["label"].is_null() ) 
            labelInput = rootInput["groups"][input]["label"].get<string>();
                    
        if( ! rootSaved["groups"][saved]["label"].is_null() )
            labelSaved = rootSaved["groups"][saved]["label"].get<string>();
                    
        int metaGroupIdInput = rootInput["groups"][input]["metaGroupId"].get<int>();
        int metaGroupIdSaved = rootSaved["groups"][saved]["metaGroupId"].get<int>();
                    
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
    
        REQUIRE( groupIdInput == groupIdSaved );
        REQUIRE( labelInput == labelSaved );
        REQUIRE( metaGroupIdInput == metaGroupIdSaved );
        REQUIRE( meanMzInput == doctest::Approx(meanMzSaved) );
        REQUIRE( meanRtInput == doctest::Approx(meanRtSaved) );
        REQUIRE( rtminInput == doctest::Approx(rtminSaved) );
        REQUIRE( rtmaxInput == doctest::Approx(rtmaxSaved) );
        REQUIRE( maxQualityInput == doctest::Approx(maxQualitySaved) );
        
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
                                
        string srmIdInput=""; 
        string srmIdSaved="";
                            
        if( ! rootInput["groups"][input]["compound"]["srmId"].is_null() )
            srmIdInput = rootInput["groups"][input]["compound"]["srmId"].get<string>();
                            
        if( ! rootSaved["groups"][saved]["compound"]["srmId"].is_null() )
            srmIdSaved= rootSaved["groups"][saved]["compound"]["srmId"].get<string>();
                            
                            
        string tagStringInput=""; 
        string tagStringSaved="";
                            
        if( ! rootInput["groups"][input]["compound"]["tagString"].is_null() )
            tagStringInput = rootInput["groups"][input]["compound"]["tagString"].get<string>();
                            
        if( ! rootInput["groups"][saved]["compound"]["tagString"].is_null() )
            tagStringSaved = rootSaved["groups"][saved]["compound"]["tagString"].get<string>();
                            
        string fullCompoundNameInput = rootInput["groups"][input]["compound"]["fullCompoundName"].get<string>();
        string fullCompoundNameSaved = rootSaved["groups"][saved]["compound"]["fullCompoundName"].get<string>();
                            
        string fullCompoundIDInput = rootInput["groups"][input]["compound"]["fullCompoundID"].get<string>();
        string fullCompoundIDSaved = rootSaved["groups"][saved]["compound"]["fullCompoundID"].get<string>();
   
            
        REQUIRE( compoundIdInput == compoundIdSaved );
        REQUIRE( compoundNameInput == compoundNameSaved );
        REQUIRE( formulaInput == formulaSaved );
        REQUIRE( expectedRtInput == doctest::Approx(expectedRtSaved) );
        REQUIRE( expectedMzInput == doctest::Approx(expectedMzSaved) );
        REQUIRE( srmIdInput == srmIdSaved );
        REQUIRE( tagStringInput == tagStringSaved );
        REQUIRE( fullCompoundNameInput == fullCompoundNameSaved );
        REQUIRE( fullCompoundIDInput == fullCompoundIDSaved );
        
        int i;          //Iterator for input file
        int j;          //Iterator for saved file
        for(i = 0, j = 0 ; i < static_cast<int>(rootInput["groups"][input]["peak"].size()) && 
                                            j < static_cast<int>(rootInput["groups"][saved]["peak"].size()) ; i++, j++ ){
                                    
            string SNameInput="";
            string SNameSaved="";

            REQUIRE( !rootInput["groups"][input]["peak"][i]["sampleName"].is_null() );
                SNameInput = rootInput["groups"][input]["peak"][i]["sampleName"].get<string>();
                                    
            REQUIRE( !rootInput["groups"][input]["peak"][i]["sampleName"].is_null() );
                SNameSaved = rootSaved["groups"][saved]["peak"][j]["sampleName"].get<string>();
                                    
            double pMzInput = rootInput["groups"][input]["peak"][i]["peakMz"].get<double>();
            double pMzSaved = rootSaved["groups"][saved]["peak"][j]["peakMz"].get<double>();
                                    
            double mMzInput = rootInput["groups"][input]["peak"][i]["medianMz"].get<double>();
            double mMzSaved = rootSaved["groups"][saved]["peak"][j]["medianMz"].get<double>();
                                    
            double bMzInput = rootInput["groups"][input]["peak"][i]["baseMz"].get<double>();
            double bMzSaved = rootSaved["groups"][saved]["peak"][j]["baseMz"].get<double>();
                                    
            double mzminInput = rootInput["groups"][input]["peak"][i]["mzmin"].get<double>();
            double mzminSaved = rootSaved["groups"][saved]["peak"][j]["mzmin"].get<double>();
                                    
            double mzmaxInput = rootInput["groups"][input]["peak"][i]["mzmax"].get<double>();
            double mzmaxSaved = rootSaved["groups"][saved]["peak"][j]["mzmax"].get<double>();
                                    
            double rtInput = rootInput["groups"][input]["peak"][i]["rt"].get<double>();
            double rtSaved = rootSaved["groups"][saved]["peak"][j]["rt"].get<double>();
                                    
            double rtminInput = rootInput["groups"][input]["peak"][i]["rtmin"].get<double>();
            double rtminSaved = rootSaved["groups"][saved]["peak"][j]["rtmin"].get<double>();
                                    
            double rtmaxInput = rootInput["groups"][input]["peak"][i]["rtmax"].get<double>();
            double rtmaxSaved = rootSaved["groups"][saved]["peak"][j]["rtmax"].get<double>();
                                    
            double qualityInput = rootInput["groups"][input]["peak"][i]["quality"].get<double>();
            double qualitySaved = rootSaved["groups"][saved]["peak"][j]["quality"].get<double>();
            
            
            double pIInput = rootInput["groups"][input]["peak"][i]["peakIntensity"].get<double>();
            double pISaved = rootSaved["groups"][saved]["peak"][j]["peakIntensity"].get<double>();
                                            
            double pBLLInput = rootInput["groups"][input]["peak"][i]["peakBaseLineLevel"].get<double>();
            double pBLLSaved = rootSaved["groups"][saved]["peak"][j]["peakBaseLineLevel"].get<double>();
                                            
            double pAreaInput = rootInput["groups"][input]["peak"][i]["peakArea"].get<double>();
            double pAreaSaved = rootSaved["groups"][saved]["peak"][j]["peakArea"].get<double>();
                                            
            double pSAreaInput = rootInput["groups"][input]["peak"][i]["peakSplineArea"].get<double>();
            double pSAreaSaved = rootSaved["groups"][saved]["peak"][j]["peakSplineArea"].get<double>();
                                            
            double pATopInput = rootInput["groups"][input]["peak"][i]["peakAreaTop"].get<double>();
            double pATopSaved = rootSaved["groups"][saved]["peak"][j]["peakAreaTop"].get<double>();
                                            
            double pANotCorInput = rootInput["groups"][input]["peak"][i]["peakAreaNotCorrected"].get<double>();
            double pANotCorSaved = rootSaved["groups"][saved]["peak"][j]["peakAreaNotCorrected"].get<double>();
                                            
            double pATNotInput = rootInput["groups"][input]["peak"][i]["peakAreaTopNotCorrected"].get<double>();
            double pATNOtSaved = rootSaved["groups"][saved]["peak"][j]["peakAreaTopNotCorrected"].get<double>();
                                            
            double NoNoiseInput = rootInput["groups"][input]["peak"][i]["noNoiseObs"].get<double>();
            double NoNoiseSaved = rootSaved["groups"][saved]["peak"][j]["noNoiseObs"].get<double>();
                                            
            double SBRatioInput = rootInput["groups"][input]["peak"][i]["SignalBaselineRatio"].get<double>();
            double SBRatioSaved = rootSaved["groups"][saved]["peak"][j]["SignalBaselineRatio"].get<double>();
                                            
            double fBankSInput = rootInput["groups"][input]["peak"][i]["fromBankSample"].get<double>();
            double fBankSSaved = rootSaved["groups"][saved]["peak"][j]["peakIntensity"].get<double>();
                                            
            double pAFInput = rootInput["groups"][input]["peak"][i]["peakAreaFractional"].get<double>();
            double pAFSaved = rootSaved["groups"][saved]["peak"][j]["peakAreaFractional"].get<double>();
            
            double symmetryInput = rootInput["groups"][input]["peak"][i]["symmetry"].get<double>();
            double symmetrySaved = rootSaved["groups"][saved]["peak"][j]["symmetry"].get<double>();

            double noNFracInput = rootInput["groups"][input]["peak"][i]["noNoiseFraction"].get<double>();
            double noNFracSaved = rootSaved["groups"][saved]["peak"][j]["noNoiseFraction"].get<double>(); 
                                                
                                                        
            double gOverlapInput = rootInput["groups"][input]["peak"][i]["groupOverlap"].get<double>();
            double gOverlapSaved = rootSaved["groups"][saved]["peak"][j]["groupOverlap"].get<double>();
                                                
            double gOFracInput = rootInput["groups"][input]["peak"][i]["groupOverlapFrac"].get<double>();
            double gOFracSaved = rootSaved["groups"][saved]["peak"][j]["groupOverlapFrac"].get<double>(); 
                                                
            double gFitInput = rootInput["groups"][input]["peak"][i]["gaussFitR2"].get<double>();
            double gFitSaved = rootSaved["groups"][saved]["peak"][j]["gaussFitR2"].get<double>();

            double pRankInput = rootInput["groups"][input]["peak"][i]["peakRank"].get<double>();
            double pRankSaved = rootSaved["groups"][saved]["peak"][j]["peakRank"].get<double>(); 
                                                
            double pWidthInput = rootInput["groups"][input]["peak"][i]["peakWidth"].get<double>();
            double pWidthSaved = rootSaved["groups"][saved]["peak"][j]["peakWidth"].get<double>();

    
            REQUIRE( SNameSaved == SNameInput );
            REQUIRE( pMzInput == doctest::Approx( pMzSaved ));
            REQUIRE( mMzInput == doctest::Approx( mMzSaved ));
            REQUIRE( bMzInput == doctest::Approx( bMzSaved ));
            REQUIRE( mzminInput == doctest::Approx( mzminSaved ));
            REQUIRE( mzmaxInput == doctest::Approx( mzmaxSaved ));
            REQUIRE( rtInput == doctest::Approx( rtSaved ));
            REQUIRE( rtminInput == doctest::Approx( rtminSaved ));
            REQUIRE( rtmaxInput == doctest::Approx( rtmaxSaved ));
            REQUIRE( qualityInput == doctest::Approx( qualitySaved ));
            REQUIRE( pIInput == doctest::Approx( pISaved ));
            REQUIRE( pBLLInput == doctest::Approx( pBLLSaved ));
            REQUIRE( pAreaInput == doctest::Approx( pAreaSaved ));
            REQUIRE( pSAreaInput == doctest::Approx( pSAreaSaved ));
            REQUIRE( pATopInput == doctest::Approx( pATopSaved ));
            REQUIRE( pANotCorInput == doctest::Approx( pANotCorSaved ));
            REQUIRE( pATNOtSaved == doctest::Approx( pATNotInput ));
            REQUIRE( NoNoiseInput == doctest::Approx( NoNoiseSaved ));
            REQUIRE( SBRatioInput == doctest::Approx( SBRatioSaved ));
            REQUIRE( fBankSInput == doctest::Approx( fBankSSaved ));
            REQUIRE( pAFInput == doctest::Approx( pAFSaved ));
            REQUIRE( symmetryInput == doctest::Approx( symmetrySaved ));
            REQUIRE( noNFracInput == doctest::Approx( noNFracSaved ));
            REQUIRE( gOFracInput == doctest::Approx( gOFracSaved ));
            REQUIRE( gOverlapInput == doctest::Approx( gOverlapSaved ));
            REQUIRE( gFitInput == doctest::Approx( gFitSaved ));
            REQUIRE( pRankInput == doctest::Approx( pRankSaved ));
            REQUIRE( pWidthInput == doctest::Approx( pWidthSaved ));
            
            int l=0; 
            int m=0; 
                                                    
            for(l = 0, m = 0 ; l < static_cast<int>(rootInput["groups"][input]["peak"][i]["eic"]["rt"].size()) ; l++, m++)
                REQUIRE(rootInput["groups"][input]["peak"][i]["eic"]["rt"][l] == rootSaved["groups"][saved]["peak"][j]["eic"]["rt"][m]);
                                                                                              
            for(l = 0, m = 0; i < static_cast<int>(rootInput["groups"][input]["peak"][i]["eic"]["intensity"].size()) ; l++, m++)
                REQUIRE(rootInput["groups"][input]["peak"][i]["eic"]["intensity"][l] == 
                                                                rootSaved["groups"][saved]["peak"][j]["eic"]["intensity"][m]);
                                
        }
    }
}

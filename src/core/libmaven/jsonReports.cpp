#include"jsonReports.h"

JSONReports::JSONReports(MavenParameters* _mp){
    mavenParameters=_mp;
}


//TODO: Refactor this function : Sahil (Keeping in mind multiprocessing)
void JSONReports::writeGroupMzEICJson(PeakGroup& grp,ofstream& myfile, vector<mzSample*> vsamples) {

    double mz,mzmin,mzmax,rtmin,rtmax;

    int charge = mavenParameters->getCharge(grp.compound);
    mz = grp.getExpectedMz(charge);

    if (mz == -1) {
        mz = grp.meanMz;
    }

    myfile << setprecision(10);
    myfile << "{\n";
    myfile << "\"groupId\": " << grp.groupId ;
    myfile << ",\n" << "\"metaGroupId\": " << grp.metaGroupId ;
    myfile << ",\n" << "\"meanMz\": " << grp.meanMz  ;
    myfile << ",\n" << "\"meanRt\": " << grp.meanRt ;
    myfile << ",\n" << "\"rtmin\": " << grp.minRt ;
    myfile << ",\n" << "\"rtmax\": " << grp.maxRt ;
    myfile << ",\n" << "\"maxQuality\": " << grp.maxQuality ;

    if ( grp.hasCompoundLink() ) {

        myfile << ",\n" << "\"compound\": { " ;

        string compoundID = grp.compound->id;
        myfile << "\"compoundId\": "<< sanitizeJSONstring(compoundID) ;
        string compoundName = grp.compound->name;
        myfile << ",\n" << "\"compoundName\": "<< sanitizeJSONstring(compoundName);
        string formula = grp.compound->formula;
        myfile << ",\n" << "\"formula\": "<< sanitizeJSONstring(formula);

        myfile << ",\n" << "\"expectedRt\": " << grp.compound->expectedRt;

        myfile << ",\n" << "\"expectedMz\": " << mz ;

        myfile << ",\n" << "\"srmID\": " << sanitizeJSONstring(grp.srmId) ;
        myfile << ",\n" << "\"tagString\": " << sanitizeJSONstring(grp.tagString) ;

        string fullTag = grp.srmId + grp.tagString;
        string fullName=compoundName;
        string fullID=compoundID;
        if (fullTag.length()) {
            fullName = compoundName + " [" + fullTag + "]";
            fullID = compoundID + " [" + fullTag + "]";
        }
        myfile << ",\n" << "\"fullCompoundName\": "<< sanitizeJSONstring(fullName);
        myfile << ",\n" << "\"fullCompoundID\": "<< sanitizeJSONstring(fullID) ;

        myfile << "}" ; // compound
    }
    myfile << ",\n"<< "\"peaks\": [ " ;

    for(std::vector<mzSample*>::iterator it = vsamples.begin(); it != vsamples.end(); ++it) {
        if (it!=vsamples.begin()) {
            myfile << ",\n";
        }
        //TODO: Use getPeak()
        Peak* peak = grp.getSamplePeak(*it);
        if(peak) {
            //TODO: add slice information here: e.g. what ppm was used
            myfile << "{\n";
            myfile << "\"sampleName\": " << sanitizeJSONstring((*it)->sampleName) ;
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
            myfile << "\"sampleName\": " << sanitizeJSONstring((*it)->sampleName) ;
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

        EIC* eic=NULL;
        //TODO: replace this by putting mzSlice pointer in peakgroup and using that
        //TODO: Refactor the code :Sahil
        if (grp.hasCompoundLink()) {
            if ( !grp.srmId.empty() ) { //MS-MS case 1
                eic = (*it)->getEIC(grp.srmId, mavenParameters->eicType);
            }
            else if((grp.compound->precursorMz > 0) & (grp.compound->productMz > 0)) { //MS-MS case 2
                //TODO: this is a problem -- amuQ1 and amuQ3 that were used to generate the peakgroup are not stored anywhere
                //will use mainWindow->MavenParameters for now but those values may have changed between generation and export
                eic =(*it)->getEIC(grp.compound->precursorMz, grp.compound->collisionEnergy, grp.compound->productMz,mavenParameters->eicType,
                                   mavenParameters->filterline, mavenParameters->amuQ1, mavenParameters->amuQ3);
            }
            else {//MS1 case
                //TODO: same problem here: need the ppm that was used, or the slice object
                //for mz could rely on same computation being done way above
                //redoing it only for code clarity

                MassCutoff *massCutoff=mavenParameters->compoundMassCutoffWindow;
                mzmin = mz - massCutoff->massCutoffValue(mz);
                mzmax = mz + massCutoff->massCutoffValue(mz);
                rtmin = grp.minRt - outputRtWindow;
                rtmax = grp.maxRt + outputRtWindow;
                eic = (*it)->getEIC(mzmin,mzmax,rtmin,rtmax,1,
                                    mavenParameters->eicType,
                                    mavenParameters->filterline);
            }
        }

        else {//no compound information
            //TODO: same problem here: need the ppm that was used, or the slice object
            mz=grp.meanMz;
            MassCutoff *massCutoff=mavenParameters->compoundMassCutoffWindow;
            mzmin = mz - massCutoff->massCutoffValue(mz);
            mzmax = mz + massCutoff->massCutoffValue(mz);
            rtmin = grp.minRt - outputRtWindow;
            rtmax = grp.maxRt + outputRtWindow;
            eic = (*it)->getEIC(mzmin,mzmax,rtmin,rtmax,1,
                                mavenParameters->eicType,
                                mavenParameters->filterline);
        }

        //TODO: for MS1 we've already limited RT range, but for MS/MS the entire RT range of the SRM will be output
        //either check here or edit getEIC functionality
        if(eic) {
            int N = eic->rt.size();

            //myfile << setprecision(4);

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


    myfile << "\n]" ; //peaks

    myfile << "}" ; //group
}


void JSONReports::saveMzEICJson(string filename,vector<PeakGroup> allgroups,vector<mzSample*> samples) {
    ofstream myfile(filename.c_str());
    myfile << setprecision(10);

    myfile << "{\"groups\": [" <<endl;

    int groupId=0;
    int metaGroupId=0;
    vector<mzSample*> vsamples = samples;

    for(int i=0; i < allgroups.size(); i++ ) {
        PeakGroup& grp = allgroups[i];

        //if compound is unknown, output only the unlabeled form information
        if( grp.compound == NULL || grp.childCount() == 0 ) {
            grp.groupId= ++groupId;
            grp.metaGroupId= ++metaGroupId;
            if(groupId>1) myfile << "\n,";
            writeGroupMzEICJson(grp,myfile, vsamples);
        }
        else { //output all relevant isotope info otherwise
            //does this work? is children[0] always the same as grp (parent)?
            grp.metaGroupId = ++ metaGroupId;
            for (unsigned int k=0; k < grp.children.size(); k++) {
                grp.children[k].metaGroupId = grp.metaGroupId;
                grp.children[k].groupId= ++groupId;
                if(groupId>1) myfile << "\n,";
                writeGroupMzEICJson(grp.children[k], myfile, vsamples);
            }
        }
        //Q_EMIT(updateProgressBar("Writing to json file. Please wait...", i, allgroups.size() - 1));
    }
    myfile << "]}"; //groups
    myfile.close();
    //Q_EMIT(updateProgressBar("Writing to json complete.", 1, 1));
}

string JSONReports::sanitizeJSONstring(string s) {
    boost::replace_all(s, "\"","\\\"");
    //replace(s.begin(),s.end(),"\"","\\\""); //escape quotes
    s="\""+s+"\""; //quote the whole string
    return s;
}
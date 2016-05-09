#include "csvreports.h"

CSVReports::CSVReports(vector<mzSample*>&insamples) {
    samples = insamples;
    groupId = 0;
    setUserQuantType(PeakGroup::AreaTop);
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
}


CSVReports::~CSVReports() { 
    closeFiles();
}

void CSVReports::openGroupReport(string outputfile) {
    if (samples.size()==0) return;

    groupReport.open(outputfile.c_str());
    if (groupReport.is_open()) {
        groupReport << "label,metaGroupId,groupId,goodPeakCount,medMz,medRt,maxQuality,note,compound,compoundId,expectedRtDiff,ppmDiff,parent";
        for(unsigned int i=0; i< samples.size(); i++) { groupReport << "," << samples[i]->sampleName; }
        groupReport << endl;
    }
}

void CSVReports::openPeakReport(string outputfile) {
    if (samples.size()==0) return;

    peakReport.open(outputfile.c_str());
    if (peakReport.is_open()) {
        peakReport << "groupId,compound,compoundId,sample,peakMz,medianMz,baseMz,rt,rtmin,rtmax,quality,peakIntensity,peakArea,peakAreaTop,peakAreaCorrected,noNoiseObs,signalBaseLineRatio,fromBlankSample" << endl;
    }
}

//Feng note: CSVReports::addGroup modified to (1) output only C12 data without labeling or wen compound is unknown, (2) output all related isotopic forms with labeling, 
//even when peak height is zero
void CSVReports::addGroup(PeakGroup* group) { 
    writePeakInfo( group );

    //get ionization mode
    int ionizationMode; 
    if( samples.size()>0 && samples[0]->getPolarity()>0 ) ionizationMode=+1;
    else ionizationMode = -1;

    //if compound is unknown, output only the unlabeled form information
    if( group->compound == NULL || group->childCount() == 0 ) writeGroupInfo(group); 
    //output all relevant isotope info otherwise
    else { 
	writeGroupInfo( &group->children[0] ); //C12 info
    	MassCalculator *masscalc;
    	string formula = group->compound->formula;
    	vector<Isotope> masslist = masscalc->computeIsotopes(formula, ionizationMode);
    	for( int i=0; i<masslist.size(); i++ ) {
	    Isotope& x = masslist[i];
	    string isotopeName = x.name;
	    if( ( isotopeName.find("C13-label")!=string::npos && samples[0]->_C13Labeled==true ) ||
		( isotopeName.find("N15-label")!=string::npos && samples[0]->_N15Labeled==true ) ||
		( isotopeName.find("S34-label")!=string::npos && samples[0]->_S34Labeled==true ) ||
		( isotopeName.find("D-label")!=string::npos && samples[0]->_D2Labeled==true ) ) {
	    		int counter=0;
    	    		for (unsigned int k=0; k<group->children.size() && counter==0; k++) { //output non-zero-intensity peaks
		    		PeakGroup* subgroup = &group->children[k];
		    		if( subgroup->tagString == isotopeName ) { 
        				subgroup->metaGroupId = group->metaGroupId;
        				writeGroupInfo(subgroup);
					counter=1;
		    		}
   	    		}
	    		if( counter==0 ) { //output zero-intensity peaks
		    		groupReport << "No peak" << "," << setprecision(7) << group->metaGroupId << "," << "N/A" << "," << "N/A" << "," << "N/A" << "," << "N/A" << "," << "N/A" << "," << isotopeName << "," << group->compound->name << "," << group->compound->id << "," << "N/A" << "," << "N/A"; 
		    		if ( group->parent != NULL ) groupReport << "," << group->parent->meanMz; 
		    		else groupReport << "," << group->meanMz; 
		    		for( unsigned int j=0; j < samples.size(); j++) groupReport << "," << 0; 
		    		groupReport << endl;
	    		}
		}
    	}
    }
}

/*
void CSVReports::addGroup(PeakGroup* group) { 
    writeGroupInfo(group);
    writePeakInfo(group);
}
*/

void CSVReports::closeFiles() { 
    if(groupReport.is_open()) groupReport.close();
    if(peakReport.is_open() ) peakReport.close();
}

void CSVReports::writeGroupInfo(PeakGroup* group) {
    if(! groupReport.is_open()) return;
    groupId++;
    vector<float> yvalues = group->getOrderedIntensityVector(samples,qtype);
    //if ( group->metaGroupId == 0 ) { group->metaGroupId=groupId; }

    string tagString = group->srmId + group->tagString;
    tagString = mzUtils::substituteInQuotedString(tagString,"\",'","---");
    char label[2];
    sprintf(label,"%c",group->label);

    groupReport << label << "," << setprecision(7) << group->metaGroupId << "," << groupId << "," << group->goodPeakCount << ", "
            << group->meanMz << "," << group->meanRt << "," << group->maxQuality << "," << tagString;


    string compoundName;
    string compoundID;
    float  expectedRtDiff=0;
    float  ppmDist=0;

    if ( group->compound != NULL ) {
        compoundName = mzUtils::substituteInQuotedString(group->compound->name,"\",'","---");
        compoundID =  group->compound->id;
        ppmDist = mzUtils::ppmDist((double) group->compound->mass,(double) group->meanMz);
        expectedRtDiff = group->expectedRtDiff;
    }

    groupReport << "," << compoundName;
    groupReport << "," << compoundID;
    groupReport << "," << expectedRtDiff;
    groupReport << "," << ppmDist;

    if ( group->parent != NULL ) {
        groupReport << "," << group->parent->meanMz;
    } else {
        groupReport << "," << group->meanMz;
    }

    for( unsigned int j=0; j < samples.size(); j++) groupReport << "," <<  yvalues[j];
    groupReport << endl;

    for (unsigned int k=0; k < group->children.size(); k++) {
        group->children[k].metaGroupId = group->metaGroupId;
        writeGroupInfo(&group->children[k]);
        //writePeakInfo(&group->children[k]);
    }

}

void CSVReports::writePeakInfo(PeakGroup* group) {
    if(! peakReport.is_open()) return;
    string compoundName = "";
    string compoundID = "";

    if (group->compound != NULL) {
        compoundName = mzUtils::substituteInQuotedString(group->compound->name,"\",'","---");
        compoundID   = group->compound->id;
    }


    for(unsigned int j=0; j<group->peaks.size(); j++ ) {
        Peak& peak = group->peaks[j];
        mzSample* sample = peak.getSample();
        string sampleName;
        if ( sample !=NULL) sampleName= sample->sampleName;

        peakReport << setprecision(8)
                << groupId << ","
                << compoundName << ","
                << compoundID << ","
                << sampleName << ","
                << peak.peakMz <<  ","
                << peak.medianMz <<  ","
                << peak.baseMz <<  ","
                << setprecision(3)
                << peak.rt <<  ","
                << peak.rtmin <<  ","
                << peak.rtmax <<  ","
                << peak.quality << ","
                << peak.peakIntensity <<  ","
                << peak.peakArea <<  ","
                << peak.peakAreaTop <<  ","
                << peak.peakAreaCorrected <<  ","
                << peak.noNoiseObs <<  ","
                << peak.signalBaselineRatio <<  ","
                << peak.fromBlankSample << ","
                << endl;
    }
}


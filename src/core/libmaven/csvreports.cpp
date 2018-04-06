#include "csvreports.h"

CSVReports::CSVReports(vector<mzSample*>&insamples) {
    /*
    *@detail -   constructor for instantiating class by all samples uploaded,
    *different from samples vector of PeakGroup which will hold
    *samples used for that particular group. it will be used to export group info
    *only for samples used by a group and for other group, fields will be marked NA.
    *Note that these samples are represented by pointers which will change their state
    *even after group has been determine and detected. Only way to get those samples
    *used for particular group by comparing these sample and samples from PeakGroup
    */
    samples = insamples;
    groupId = 0;
    /**@brief-  set user quant type-  generally represent intensity but not always check QType enum in PeaKGroup.h  */
    setUserQuantType(PeakGroup::AreaTop);
    setTabDelimited();      /**@brief-  set output file separator as tab*/
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
    errorReport = "";
}

CSVReports::~CSVReports() {
    /**
    *@details-    close all open output files opened for writing csv or tab file
    */
    closeFiles();
}

QString CSVReports::sanitizeString(const char* s) {
    QString out=s;
    out.replace(QString("\""),QString("\"\""));
    if(out.contains(SEP.c_str())){
        out="\""+out+"\"";
    }
    return out;
}

void CSVReports::openGroupReport(string outputfile,bool includeSetNamesLine) {

    initialCheck(outputfile);                                                                                               /**@brief-  if number of sample is zero, output file will not open*/
    openGroupReportCSVFile(outputfile);                                                                        /**@brief-  after checking initial check, open output file*/
    insertGroupReportColumnNamesintoCSVFile(outputfile, includeSetNamesLine);   /**@brief-  write name of column  if output file is open */

}

void CSVReports::openPeakReport(string outputfile) {

    initialCheck(outputfile);                                           /**@brief-  if number of sample is zero, output file will not open*/
    openPeakReportCSVFile(outputfile);                      /**@brief-  after checking initial check, open output file*/
    insertPeakReportColumnNamesintoCSVFile();      /**@brief-  write name of column  if output file is open */

}

void CSVReports::initialCheck(string outputfile) {

    if (samples.size() == 0)
        return;
    if (QString(outputfile.c_str()).endsWith(".csv", Qt::CaseInsensitive))
        setCommaDelimited();
}

void CSVReports::openGroupReportCSVFile(string outputfile) {

     groupReport.open(outputfile.c_str());
}

void CSVReports::openPeakReportCSVFile(string outputfile) {

     peakReport.open(outputfile.c_str());
}

void CSVReports::insertGroupReportColumnNamesintoCSVFile(string outputfile,bool includeSetNamesLine){
    if (groupReport.is_open()) {
        QStringList groupReportcolnames;
        groupReportcolnames << "label" << "metaGroupId" << "groupId" << "goodPeakCount"
                << "medMz" << "medRt" << "maxQuality" << "isotopeLabel" << "compound"
                << "compoundId" << "formula" << "expectedRtDiff" << "ppmDiff" << "parent";
        int cohort_offset = groupReportcolnames.size() - 1;
        QString header = groupReportcolnames.join(SEP.c_str());
        groupReport << header.toStdString();
        for (unsigned int i = 0; i < samples.size(); i++) {
            groupReport << SEP << sanitizeString(samples[i]->sampleName.c_str()).toStdString();
        }
        groupReport << endl;
        //TODO: Remove this to remove row in csv reports --@Giridhari
        if (includeSetNamesLine){
             for(unsigned int i = 0; i < cohort_offset; i++) { groupReport << SEP; }
             for(unsigned int i = 0; i < samples.size(); i++) { groupReport << SEP << sanitizeString(samples[i]->getSetName().c_str()).toStdString(); }
             groupReport << endl;
         }
    }
    else {
        errorReport = "Unable to write to file \"" + QString::fromStdString(outputfile) + "\"\n";
        errorReport += "Please check if you have permission to write to the specified location or the file is not in use";
    }
}

void CSVReports::insertPeakReportColumnNamesintoCSVFile(){

    if (peakReport.is_open()) {
        QStringList peakReportcolnames;
        peakReportcolnames << "groupId" << "compound" << "compoundId" << "formula" << "sample" << "peakMz"
                << "medianMz" << "baseMz" << "rt" << "rtmin" << "rtmax" << "quality"
                << "peakIntensity" << "peakArea" << "peakSplineArea" << "peakAreaTop"
                << "peakAreaCorrected" << "peakAreaTopCorrected"
                << "noNoiseObs" << "signalBaseLineRatio"
                << "fromBlankSample";
        QString header = peakReportcolnames.join(SEP.c_str());
        peakReport << header.toStdString() << endl;
    }
}

//Feng note: CSVReports::addGroup modified to (1) output only C12 data without labeling or wen compound is unknown, (2) output all related isotopic forms with labeling,
//even when peak height is zero

void CSVReports::addGroup (PeakGroup* group) {

    if(peakReport.is_open())
        writePeakInfo(group);

    if(groupReport.is_open()){
        if( group->children.size() == 0 )
            writeGroupInfo(group);

        for( int i = 0 ; i < group->children.size() ; ++i ){
            group->children[i].metaGroupId = group->metaGroupId;
            writeGroupInfo(&group->children[i]);
        }        
    }
}

void CSVReports::insertPeakInformationIntoCSVFile(PeakGroup* group) {

      writePeakInfo(group);

}

void CSVReports::insertGroupInformationIntoCSVFile (PeakGroup* group) {

    if(group->compound == NULL || group->childCount() == 0) {

        writeGroupInfo(group);

    }

    else {

        string formula = group->compound->formula;
        int charge = getMavenParameters()->getCharge(group->compound);
        bool C13Flag = getMavenParameters()->C13Labeled_BPE;
        bool N15Flag = getMavenParameters()->N15Labeled_BPE;
        bool S34Flag = getMavenParameters()->S34Labeled_BPE;
        bool D2Flag = getMavenParameters()->D2Labeled_BPE;

        vector<Isotope> masslist = MassCalculator::computeIsotopes(
            formula,
            charge,
            C13Flag,
            N15Flag,
            S34Flag,
            D2Flag
        );
        insertIsotopes(group,masslist);

    }
}

int CSVReports::getIonisationMode() {

      int ionizationMode;

      if (samples.size() > 0 && samples[0]->getPolarity() > 0)
          ionizationMode = +1;
      else
          ionizationMode = -1;

      return ionizationMode;
}

void CSVReports::insertIsotopes (PeakGroup* group, vector<Isotope> masslist) {

      for (unsigned int i = 0; i < masslist.size(); i++) {
                Isotope& x = masslist[i];
                string isotopeName = x.name;
                insertUserSelectedIsotopes(group,isotopeName);
      }
}

void CSVReports::insertUserSelectedIsotopes(PeakGroup* group, string isotopeName) {

      int counter = 0;
      for (unsigned int k = 0; k < group->children.size() && counter == 0; k++) {
          //output non-zero-intensity peaks
          counter = insertIsotpesFoundInSamples(group, isotopeName, counter, k);
      }
      /*
       * Commented out in Maven776 - Kiran
      if (counter == 0) {
          //output zero-intensity peaks
          insertIsotpesNotFoundInSamples(group, isotopeName);
      }
      */
}

int CSVReports::insertIsotpesFoundInSamples (PeakGroup* group, string isotopeName, int counter, int k) {

      PeakGroup* subgroup = &group->children[k];
      if (subgroup->tagString == isotopeName) {
          subgroup->metaGroupId = group->metaGroupId;
          writeGroupInfo(subgroup);
          counter = 1;
      }
      return counter;
}

/*
 * Merged with Maven776 - Kiran
void CSVReports::insertIsotpesNotFoundInSamples (PeakGroup* group, string isotopeName) {

      groupReport << "No peak" << SEP << setprecision(7)
                  << group->metaGroupId << SEP << "N/A" << SEP
                  << "N/A" << SEP << "N/A" << SEP << "N/A" << SEP
                  << "N/A" << SEP << isotopeName << SEP
                  << group->compound->name << SEP
                  << group->compound->id << SEP << "N/A" << SEP
                  << "N/A";

       if (group->parent != NULL)
       groupReport << SEP << group->parent->meanMz;
       else
           groupReport << SEP << group->meanMz;
       for (unsigned int j = 0; j < samples.size(); j++)
           groupReport << SEP << 0;
       groupReport << endl;

}
*/
/*
 void CSVReports::addGroup(PeakGroup* group) {
 writeGroupInfo(group);
 writePeakInfo(group);
 }
 */

void CSVReports::closeFiles() {
    if (groupReport.is_open())
        groupReport.close();
    if (peakReport.is_open())
        peakReport.close();
}

void CSVReports::writeGroupInfo(PeakGroup* group) {
    if (!groupReport.is_open())
        return;
    groupId++;

    char lab;
    lab = group->label;

    PeakGroup* parentGroup = group->getParent();
    if (parentGroup) {
        if (group->label == '\0') {
            lab = parentGroup->label;
        }
    }

    if (selectionFlag == 2) {
        if(lab !='g') return;
    } else if (selectionFlag == 3) {
        if(lab !='b') return;
    } else {

    }

    vector<float> yvalues = group->getOrderedIntensityVector(samples, qtype);
    //if ( group->metaGroupId == 0 ) { group->metaGroupId=groupId; }

    string tagString = group->srmId + group->tagString;
    // using the new funtionality added - Kiran
    tagString = sanitizeString(tagString.c_str()).toStdString();
    char label[2];
    sprintf(label, "%c", group->label);

    groupReport << label << SEP << setprecision(7) << group->metaGroupId << SEP
            << groupId << SEP << group->goodPeakCount << SEP << group->meanMz
            << SEP << group->meanRt << SEP << group->maxQuality << SEP
            << tagString;

    string compoundName;
    string compoundID;
    string formula;
    string categoryString;
    float expectedRtDiff = 0;
    float ppmDist = 0;
    compoundName = sanitizeString(group->getName().c_str()).toStdString();

    if (group->compound != NULL) {
        compoundID   = sanitizeString(group->compound->id.c_str()).toStdString();
        formula = sanitizeString(group->compound->formula.c_str()).toStdString();
        if (!group->compound->formula.empty()) {
            int charge = getMavenParameters()->getCharge(group->compound);
            if (group->parent != NULL) {
                ppmDist = mzUtils::massCutoffDist((double) group->getExpectedMz(charge),
                (double) group->meanMz,getMavenParameters()->massCutoffMerge);
            }
            else {
                ppmDist = mzUtils::massCutoffDist((double) group->compound->adjustedMass(charge),
                (double) group->meanMz,getMavenParameters()->massCutoffMerge);
            }
        }
        else {
            ppmDist = mzUtils::massCutoffDist((double) group->compound->mass, (double) group->meanMz,getMavenParameters()->massCutoffMerge);
        }
        expectedRtDiff = group->expectedRtDiff;

        // TODO: Added this while merging this file
        //for(int i=0;i<group->compound->category.size(); i++) {
        //    categoryString += group->compound->category[i] + ";";
        //}
        //categoryString=sanitizeString(categoryString.c_str()).toStdString();

    }

    groupReport << SEP << compoundName;
    groupReport << SEP << compoundID;
    groupReport << SEP << formula;
    //groupReport << SEP << categoryString;
    groupReport << SEP << expectedRtDiff;
    groupReport << SEP << ppmDist;

    if (group->parent != NULL) {
        groupReport << SEP << group->parent->meanMz;
    } else {
        groupReport << SEP << group->meanMz;
    }

    for (unsigned int j = 0; j < samples.size(); j++){
        for(int i=0;i<group->samples.size();++i){
            if(samples[j]->sampleName==group->samples[i]->sampleName){
                groupReport << SEP << yvalues[j];
                break;
            }
            else if(i==group->samples.size()-1){
                groupReport << SEP << "NA";
            }
        }   
    }

    groupReport << endl;

}

void CSVReports::writePeakInfo(PeakGroup* group) {
    if (!peakReport.is_open())
        return;
    string compoundName = "";
    string compoundID = "";
    string formula = "";
    compoundName = sanitizeString(group->getName().c_str()).toStdString();

    if (group->compound != NULL) {
        compoundID   = sanitizeString(group->compound->id.c_str()).toStdString();
        formula = sanitizeString(group->compound->formula.c_str()).toStdString();
    }

    if (selectionFlag == 2) {
        if(group->label !='g') return;
    } else if (selectionFlag == 3) {
        if(group->label !='b') return;
    } else {

    }
    for (unsigned int j = 0; j < group->peaks.size(); j++) {
        Peak& peak = group->peaks[j];
        mzSample* sample = peak.getSample();
        string sampleName;
        if (sample != NULL) {

            string sampleId = "";
            sampleId = sample->sampleName;
            if (peak.getScan()->sampleNumber != -1) sampleId = sampleId + " | Sample Number = " + to_string(peak.getScan()->sampleNumber);

            sampleName = sanitizeString(sampleId.c_str()).toStdString();
        }


        peakReport << setprecision(8)
                << groupId << SEP
                << compoundName << SEP
                << compoundID << SEP
                << formula << SEP
                << sampleName << SEP
                << peak.peakMz <<  SEP
                << peak.medianMz <<  SEP
                << peak.baseMz <<  SEP
                << setprecision(3)
                << peak.rt <<  SEP
                << peak.rtmin <<  SEP
                << peak.rtmax <<  SEP
                << peak.quality << SEP
                << peak.peakIntensity << SEP
                << peak.peakArea <<  SEP
                << peak.peakSplineArea <<  SEP                
                << peak.peakAreaTop <<  SEP
                << peak.peakAreaCorrected <<  SEP
                << peak.peakAreaTopCorrected << SEP
                << peak.noNoiseObs <<  SEP
                << peak.signalBaselineRatio <<  SEP
                << peak.fromBlankSample << SEP
                << endl;
    }
}


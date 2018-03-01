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
CSVReports::CSVReports(vector<mzSample*>& insamples,
                        MavenParameters* mp,
                        PeakGroup::QType t,
                        string fileName,
                        ExportType exportType,
                        int selectionFlag,
                        bool includeSetNamesLine
                        ){
    samples = insamples;
    mavenparameters = mp;
    qtype = t;
    _fileName = fileName;
    _exportType = exportType;
    _selectionFlag = selectionFlag;
    _includeSetNamesLine = includeSetNamesLine;
    groups.clear();
    groupId = 0;
    errorReport = "";

    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
}

CSVReports::~CSVReports() {
    /**
    *@details-    close all open output files opened for writing csv or tab file
    */
    closeFiles();
}

bool CSVReports::exportGroup(){

    if(groups.size() == 0){
        errorReport = "No groups found!";
        return 0;
    }
    if(_fileName.length() < 4){
        errorReport="invalid file name";
        return 0;
    }
    string fileFormat = _fileName.substr(_fileName.length()-4 , 4 );
    if( fileFormat == ".csv" )
        SEP = ",";
    if( fileFormat == ".tab" )
        SEP = "\t";
    outFileStream.open(_fileName);
    if(!outFileStream.is_open()){
        errorReport="file can't open to write\ncheck write permission";
        return 0;
    }
    else{
        addColumnNames();
        for(int i=0 ; i<groups.size(); ++i){
            PeakGroup* group=groups[i];

            if(_exportType == PeakExport)
                writePeakInfo(group);

            if(_exportType == GroupExport){
                if( group->children.size() == 0 )
                    writeGroupInfo(group);

                for( int i = 0 ; i < group->children.size() ; ++i ){
                    group->children[i].metaGroupId = group->metaGroupId;
                    writeGroupInfo(&group->children[i]);
                }      
            }
        }

        outFileStream.close();
    }
    return 1;
}

void CSVReports::addColumnNames(){
    vector<string> columnNames;
    if(_exportType == GroupExport){
        string columns[]={"label" , "metaGroupId" , "groupId" , "goodPeakCount" , "medMz" , "medRt" ,
                         "maxQuality" , "isotopeLabel" , "compound" , "compoundId" , "formula"
                         , "expectedRtDiff" , "ppmDiff" , "parent"};
                         
        columnNames.assign(columns,columns + sizeof(columns)/sizeof(string));
        int cohort_offset = columnNames.size() - 1;

        columnNames = mzUtils::join(columnNames , SEP);

        for (unsigned int i = 0; i < samples.size(); i++) {
            columnNames.push_back(SEP);
            columnNames.push_back(sanitizeString(samples[i]->sampleName.c_str()).toStdString());
        }
        columnNames.push_back("\n");
        if (_includeSetNamesLine){
            for(unsigned int i = 0; i < cohort_offset; i++) { columnNames.push_back(SEP); }
            for(unsigned int i = 0; i < samples.size(); i++) {
                columnNames.push_back(SEP);
                columnNames.push_back(sanitizeString(samples[i]->getSetName().c_str()).toStdString());
            }
            columnNames.push_back("\n");
        }

    }

    if(_exportType == PeakExport){
        columnNames.clear();
        string columns[]={ "groupId" , "compound" , "compoundId" , "formula" , "sample" , "peakMz" ,
                         "medianMz" , "baseMz" , "rt" , "rtmin" , "rtmax" , "quality" , "peakIntensity" ,
                         "peakArea" , "peakSplineArea" , "peakAreaTop" , "peakAreaCorrected" ,
                         "peakAreaTopCorrected" , "noNoiseObs" , "signalBaseLineRatio" , "fromBlankSample"
                          };
        columnNames.assign(columns,columns + sizeof(columns)/sizeof(string));
        columnNames = mzUtils::join(columnNames , SEP);
        columnNames.push_back("\n");
    }

    for(int i=0 ; i < columnNames.size();++i)
        outFileStream << columnNames[i];
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

void CSVReports::closeFiles() {
    if (groupReport.is_open())
        groupReport.close();
    if (peakReport.is_open())
        peakReport.close();
}

void CSVReports::writeGroupInfo(PeakGroup* group) {
    if (!outFileStream.is_open())
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

    outFileStream << label << SEP << setprecision(7) << group->metaGroupId << SEP
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

    outFileStream << SEP << compoundName;
    outFileStream << SEP << compoundID;
    outFileStream << SEP << formula;
    //groupReport << SEP << categoryString;
    outFileStream << SEP << expectedRtDiff;
    outFileStream << SEP << ppmDist;

    if (group->parent != NULL) {
        outFileStream << SEP << group->parent->meanMz;
    } else {
        outFileStream << SEP << group->meanMz;
    }

    for (unsigned int j = 0; j < samples.size(); j++){
        for(int i=0;i<group->samples.size();++i){
            if(samples[j]->sampleName==group->samples[i]->sampleName){
                outFileStream << SEP << yvalues[j];
                break;
            }
            else if(i==group->samples.size()-1){
                outFileStream << SEP << "NA";
            }
        }   
    }

    outFileStream << endl;

}

void CSVReports::writePeakInfo(PeakGroup* group) {
    if (!outFileStream.is_open())
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


        outFileStream << setprecision(8)
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


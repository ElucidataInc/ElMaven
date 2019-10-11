#include "constants.h"
#include "Compound.h"
#include "csvreports.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"

using namespace mzUtils;

CSVReports::CSVReports(vector<mzSample*>&insamples, bool pollyExport)
{
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
    _pollyExport = pollyExport;
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

void CSVReports::openGroupReport(string outputfile,
                                 bool prmReport,
                                 bool includeSetNamesLine)
{
    // if number of sample is zero, output file will not open
    initialCheck(outputfile);

    // after checking initial check, open output file
    openGroupReportCSVFile(outputfile);

    // write name of column  if output file is open
    insertGroupReportColumnNamesintoCSVFile(outputfile,
                                            prmReport,
                                            includeSetNamesLine);
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

void CSVReports::insertGroupReportColumnNamesintoCSVFile(string outputfile,
                                                         bool prmReport,
                                                         bool includeSetNamesLine)
{
    if (groupReport.is_open()) {
        QStringList groupReportcolnames;

        groupReportcolnames << "label"
                            << "metaGroupId"
                            << "groupId"
                            << "goodPeakCount"
                            << "medMz"
                            << "medRt"
                            << "maxQuality"
                            << "isotopeLabel"
                            << "compound"
                            << "compoundId"
                            << "formula"
                            << "expectedRtDiff"
                            << "ppmDiff"
                            << "parent";

        // if this is a PRM report, add PRM specific columns
        if (prmReport && !_pollyExport) {
            groupReportcolnames << "ms2EventCount"
                                << "fragNumIonsMatched"
                                << "fragmentFractionMatched"
                                << "TICMatched"
                                << "dotProduct"
                                << "weigtedDotProduct"
                                << "hyperGeomScore"
                                << "spearmanRankCorrelation"
                                << "mzFragmentError"
                                << "ms2Purity";
        }

        int cohort_offset = groupReportcolnames.size() - 1;
        QString header = groupReportcolnames.join(SEP.c_str());
        groupReport << header.toStdString();
        for (unsigned int i = 0; i < samples.size(); i++) {
            string name = samples[i]->getSampleName();
            groupReport << SEP
                        << sanitizeString(name.c_str()).toStdString();
        }
        groupReport << endl;

        if (includeSetNamesLine) {
            for(unsigned int i = 0; i < cohort_offset; i++)
                groupReport << SEP;

            for(unsigned int i = 0; i < samples.size(); i++) {
                string name = samples[i]->getSetName();
                groupReport << SEP
                            << sanitizeString(name.c_str()).toStdString();
            }
            groupReport << endl;
        }
    }
    else {
        errorReport = "Unable to write to file \""
            + QString::fromStdString(outputfile)
            + "\"\n"
            + "Please check if you have permission to write to the specified "
            + "location or the file is not in use";
    }
}

void CSVReports::insertPeakReportColumnNamesintoCSVFile()
{
    if (peakReport.is_open()) {
        QStringList peakReportcolnames;
        peakReportcolnames << "groupId"
                           << "compound"
                           << "compoundId"
                           << "formula"
                           << "sample"
                           << "peakMz"
                           << "mzmin"
                           << "mzmax"
                           << "rt"
                           << "rtmin"
                           << "rtmax"
                           << "quality"
                           << "peakIntensity"
                           << "peakArea"
                           << "peakSplineArea"
                           << "peakAreaTop"
                           << "peakAreaCorrected"
                           << "peakAreaTopCorrected"
                           << "noNoiseObs"
                           << "signalBaseLineRatio"
                           << "fromBlankSample";

        QString header = peakReportcolnames.join(SEP.c_str());
        peakReport << header.toStdString() << endl;
    }
}

//Feng note: CSVReports::addGroup modified to (1) output only C12 data without labeling or wen compound is unknown, (2) output all related isotopic forms with labeling,
//even when peak height is zero

void CSVReports::addGroup (PeakGroup* group) {

      insertPeakInformationIntoCSVFile(group);

      if(!groupReport.is_open()) {
		return;
	  }
      //get ionization mode
      insertGroupInformationIntoCSVFile(group);

}

void CSVReports::insertPeakInformationIntoCSVFile(PeakGroup* group) {

      writePeakInfo(group);

}

void CSVReports::insertGroupInformationIntoCSVFile(PeakGroup* group)
{
    if(group->compound == NULL || group->childCount() == 0) {
        writeGroupInfo(group);
    } else {
        insertIsotopes(group);
    }
}

void CSVReports::insertIsotopes(PeakGroup* group, bool userSelectedIsotopesOnly)
{
    if (userSelectedIsotopesOnly) {
        insertUserSelectedIsotopes(group);
    } else {
        insertAllIsotopes(group);
    }
}

void CSVReports::insertUserSelectedIsotopes(PeakGroup* group)
{
    bool C13Flag = getMavenParameters()->C13Labeled_BPE;
    bool N15Flag = getMavenParameters()->N15Labeled_BPE;
    bool S34Flag = getMavenParameters()->S34Labeled_BPE;
    bool D2Flag = getMavenParameters()->D2Labeled_BPE;

    // iterate over all existing subgroups and for each isotope flag
    // check if the subgroup contains the isotope's name as tagstring
    // before writing it to the report. If any of the unselected
    // labels are found, we discard the child group.
    for (auto subGroup: group->children) {
        if (!C13Flag && subGroup.tagString.find("C13") != std::string::npos)
            continue;
        if (!N15Flag && subGroup.tagString.find("N15") != std::string::npos)
            continue;
        if (!S34Flag && subGroup.tagString.find("S34") != std::string::npos)
            continue;
        if (!D2Flag && subGroup.tagString.find("D2") != std::string::npos)
            continue;

        subGroup.metaGroupId = group->metaGroupId;
        writeGroupInfo(&subGroup);
    }
}

void CSVReports::insertAllIsotopes(PeakGroup* group)
{
    for (auto subGroup: group->children) {
        subGroup.metaGroupId = group->metaGroupId;
        writeGroupInfo(&subGroup);
    }
}

void CSVReports::closeFiles() {
    if (groupReport.is_open())
        groupReport.close();
    if (peakReport.is_open())
        peakReport.close();
}

void CSVReports::writeDataForPolly(const std::string& file, std::list<PeakGroup> groups)
{
    groupReport.open(file.c_str());
    if(groupReport.is_open()) {
        groupReport << "labelML" << "," << "isotopeLabel" << "," << "compound";
        groupReport << endl;
        for(auto grp: groups) {
            for(auto child: grp.children) {

                int mlLabel =  (child.markedGoodByCloudModel) ? 1 : (child.markedBadByCloudModel) ? 0 : -1;
                groupReport << mlLabel;
                groupReport << ",";

                string tagString = child.srmId + child.tagString;
                tagString = sanitizeString(tagString.c_str()).toStdString();
                groupReport << tagString ;
                groupReport << ",";


                string compoundName = "";
                if(child.compound != NULL)
                    compoundName = sanitizeString(child.compound->name.c_str()).toStdString();
                else
                    compoundName = std::to_string(child.meanMz) + "@" + std::to_string(child.meanRt);
                groupReport << compoundName;

                groupReport << endl;
            }
        }
    }
    groupReport.close();
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
    else{
        parentGroup=group;
    }

    if (selectionFlag == 2) {
        if(lab !='g') return;
    } else if (selectionFlag == 3) {
        if(lab !='b') return;
    } else if (selectionFlag == 4) {
        if (lab == 'b') return;
    }

    vector<float> yvalues = group->getOrderedIntensityVector(samples, qtype);
    // if ( group->metaGroupId == 0 ) { group->metaGroupId=groupId; }

    string tagString = group->srmId + group->tagString;
    // using the new funtionality added - Kiran
    tagString = sanitizeString(tagString.c_str()).toStdString();

    char label[2];
    sprintf(label, "%c", group->label);

    groupReport << label
                << SEP << parentGroup->groupId
                << SEP << groupId
                << SEP << group->goodPeakCount
                << fixed
                << SEP << setprecision(6) << group->meanMz
                << SEP << setprecision(3) << group->meanRt
                << SEP << setprecision(6) << group->maxQuality
                << SEP << tagString;

    string compoundName = "";
    string compoundID = "";
    string formula = "";
    string categoryString;
    float expectedRtDiff = 0;
    float ppmDist = 0;

    if (group->compound != NULL) {
        compoundName = sanitizeString(group->compound->name.c_str()).toStdString();
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

    } else {
        // absence of a group compound means this group was created using untargeted detection,
        // we set compound name and ID to {mz}@{rt} strings for untargeted sets.
        compoundName = std::to_string(group->meanMz) + "@" + std::to_string(group->meanRt);
        compoundID = compoundName;
    }

    groupReport << SEP << compoundName
                << SEP << compoundID
                << SEP << formula
                << SEP << setprecision(3) << expectedRtDiff
                << SEP << setprecision(6) << ppmDist;

    if (group->parent != NULL) {
        groupReport << SEP << group->parent->meanMz;
    } else {
        groupReport << SEP << group->meanMz;
    }

    if (group->compound && group->compound->type() == Compound::Type::PRM && !_pollyExport) {
        auto groupToWrite = group;

        // if this is a C12 PARENT, then all PRM attributes should be taken from
        // its parent group.
        if (group->tagString.find("C12 PARENT") != std::string::npos)
            groupToWrite = group->parent;

        groupReport << SEP << groupToWrite->ms2EventCount
                    << SEP << groupToWrite->fragMatchScore.numMatches
                    << SEP << groupToWrite->fragMatchScore.fractionMatched
                    << SEP << groupToWrite->fragMatchScore.ticMatched
                    << SEP << groupToWrite->fragMatchScore.dotProduct
                    << SEP << groupToWrite->fragMatchScore.weightedDotProduct
                    << SEP << groupToWrite->fragMatchScore.hypergeomScore
                    << SEP << groupToWrite->fragMatchScore.spearmanRankCorrelation
                    << SEP << groupToWrite->fragMatchScore.mzFragError
                    << SEP << groupToWrite->fragmentationPattern.purity;
    }

    // for intensity values, we only write two digits of floating point precision
    // since these values are supposed to be large (in the order of > 10^3).
    groupReport << setprecision(2);
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
    if (group->compound != NULL) {
        compoundName = sanitizeString(group->compound->name.c_str()).toStdString();
        compoundID   = sanitizeString(group->compound->id.c_str()).toStdString();
        formula = sanitizeString(group->compound->formula.c_str()).toStdString();
    } else {
        // absence of a group compound means this group was created using untargeted detection,
        // we set compound name and ID to {mz}@{rt} strings for untargeted sets.
        compoundName = std::to_string(group->meanMz) + "@" + std::to_string(group->meanRt);
        compoundID = compoundName;
    }

    if (selectionFlag == 2) {
        if(group->label !='g') return;
    } else if (selectionFlag == 3) {
        if(group->label !='b') return;
    } else if (selectionFlag == 4) {
        if (group->label == 'b') return;
    }

    // sort the peaks in the group according to the sample names using a comparison function
    // this ensures that the order in which the peaks are written is same across different systems.
    std::sort(group->peaks.begin(), group->peaks.end(), Peak::compSampleName);

    for (unsigned int j = 0; j < group->peaks.size(); j++) {
        Peak& peak = group->peaks[j];
        mzSample* sample = peak.getSample();
        string sampleName;
        if (sample != NULL) {

            string sampleId = "";
            sampleId = sample->sampleName;
            if (sample->sampleNumber != -1) sampleId = sampleId + " | Sample Number = " + to_string(sample->sampleNumber);

            sampleName = sanitizeString(sampleId.c_str()).toStdString();
        }


        peakReport << fixed << setprecision(6)
                   << group->groupId
                   << SEP << compoundName
                   << SEP << compoundID
                   << SEP << formula
                   << SEP << sampleName
                   << SEP << peak.peakMz
                   << SEP << peak.mzmin
                   << SEP << peak.mzmax
                   << setprecision(3)
                   << SEP << peak.rt
                   << SEP << peak.rtmin
                   << SEP << peak.rtmax
                   << SEP << peak.quality
                   // for intensity values, we only write two digits of floating point precision
                   // since these values are supposed to be large (in the order of > 10^3).
                   << setprecision(2)
                   << SEP << peak.peakIntensity
                   << SEP << peak.peakArea
                   << SEP << peak.peakSplineArea
                   << SEP << peak.peakAreaTop
                   << SEP << peak.peakAreaCorrected
                   << SEP << peak.peakAreaTopCorrected
                   << SEP << peak.noNoiseObs
                   << SEP << peak.signalBaselineRatio
                   << SEP << peak.fromBlankSample << endl;
    }
}


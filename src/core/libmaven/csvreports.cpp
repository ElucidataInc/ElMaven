#include "doctest.h"

#include "csvreports.h"
#include "constants.h"
#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"
#include "databases.h"
#include "classifierNeuralNet.h"
#include "PeakDetector.h"
#include "masscutofftype.h"
#include <boost/lexical_cast.hpp>


CSVReports::CSVReports(string filename, ReportType reportType, 
                   vector<mzSample*>& insamples,PeakGroup::QType qt, 
                   bool prmReport, bool includeSetNamesLine,
                   MavenParameters * mp, bool pollyExport)
{
    /*
     *@detail -   constructor for opening the type of report needed by the user
     *(group or peak report)and instantiating class by all samples uploaded,
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
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
    errorReport = "";
    mavenparameters = mp;
    qtype = qt;
    _reportType = reportType;
    _prmReport = prmReport;
    _includeSetNamesLine = includeSetNamesLine;
    
    if(reportType == ReportType::PeakReport){
        if (samples.size() == 0)
            return;
        if (QString(filename.c_str()).endsWith(".csv", Qt::CaseInsensitive))
            setCommaDelimited();
        else
            setTabDelimited();
        _reportStream.open(filename.c_str());
        
        _insertPeakReportColumnNamesintoCSVFile();      /**@brief-  write name of column  if output file is open */
    }
        
    else{
        
        // if number of sample is zero, output file will not open
        if (insamples.size() == 0)
            return;
        if (QString(filename.c_str()).endsWith(".csv", Qt::CaseInsensitive))
            setCommaDelimited();
        else
            setTabDelimited();
        // after checking initial check, open output file
        _reportStream.open(filename.c_str());
        
        // write name of column  if output file is open
        _insertGroupReportColumnNamesintoCSVFile(filename,
                                                _prmReport,
                                                _includeSetNamesLine);
    }
        
  
    
}

CSVReports::~CSVReports()
{
    if (_reportStream.is_open())
        _reportStream.close();
}

QString CSVReports::_sanitizeString(const char* s) 
{
    QString out=s;
    out.replace(QString("\""),QString("\"\""));
    if(out.contains(SEP.c_str())){
        out="\""+out+"\"";
    }
    return out;
}



void CSVReports::_insertGroupReportColumnNamesintoCSVFile(string outputfile,
                                                         bool prmReport,
                                                         bool includeSetNamesLine)
{
    if (_reportStream.is_open()) {
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
        _reportStream << header.toStdString();
        for (unsigned int i = 0; i < samples.size(); i++) {
            string name = samples[i]->getSampleName();
            _reportStream << SEP
                          << _sanitizeString(name.c_str()).toStdString();
        }
        _reportStream << endl;

        if (includeSetNamesLine) {
            for(int i = 0; i < cohort_offset; i++)
                _reportStream << SEP;

            for(int i = 0; i < static_cast<int>(samples.size()); i++) {
                string name = samples[i]->getSetName();
                _reportStream << SEP
                              << _sanitizeString(name.c_str()).toStdString();
            }
            _reportStream << endl;
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

void CSVReports::_insertPeakReportColumnNamesintoCSVFile()
{
    if (_reportStream.is_open()) {
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
        _reportStream << header.toStdString() << endl;
    }
}


void CSVReports::addGroup (PeakGroup* group) 
{
    if (_reportType == ReportType::PeakReport)
       _writePeakInfo(group);


    if(_reportType == ReportType::GroupReport){
        if(group->getCompound() == NULL || group->childCount() == 0) {
            _writeGroupInfo(group);
        } 
        else{
            _insertIsotopes(group);
        }      
    }
}

void CSVReports::_insertIsotopes(PeakGroup* group, bool userSelectedIsotopesOnly)
{
    if (userSelectedIsotopesOnly) {
        _insertUserSelectedIsotopes(group);
    } 
    else {
        for (auto subGroup: group->children) {
            subGroup.metaGroupId = group->metaGroupId;
            _writeGroupInfo(&subGroup);
        }
    }
}

void CSVReports::_insertUserSelectedIsotopes(PeakGroup* group)
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
        _writeGroupInfo(&subGroup);
    }
}


void CSVReports::_writeGroupInfo(PeakGroup* group) {
    if (!_reportStream.is_open())
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
    tagString = _sanitizeString(tagString.c_str()).toStdString();

    char label[2];
    sprintf(label, "%c", group->label);

    _reportStream << label
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

    if (group->getCompound() != NULL) {
        compoundName = _sanitizeString(group->getCompound()->name.c_str()).toStdString();
        compoundID   = _sanitizeString(group->getCompound()->id.c_str()).toStdString();
        formula = _sanitizeString(group->getCompound()->formula.c_str()).toStdString();
        if (!group->getCompound()->formula.empty()) {
            int charge = getMavenParameters()->getCharge(group->getCompound());
            if (group->parent != NULL) {
                ppmDist = mzUtils::massCutoffDist((double) group->getExpectedMz(charge),
                (double) group->meanMz,getMavenParameters()->massCutoffMerge);
            }
            else {
                ppmDist = mzUtils::massCutoffDist((double) group->getCompound()->adjustedMass(charge),
                (double) group->meanMz,getMavenParameters()->massCutoffMerge);
            }
        }
        else {
            ppmDist = mzUtils::massCutoffDist((double) group->getCompound()->mass, (double) group->meanMz,getMavenParameters()->massCutoffMerge);
        }
        expectedRtDiff = group->expectedRtDiff;

        // TODO: Added this while merging this file
        //for(int i=0;i<group->getCompound()->category.size(); i++) {
        //    categoryString += group->getCompound()->category[i] + ";";
        //}
        //categoryString=_sanitizeString(categoryString.c_str()).toStdString();

    } else {
        // absence of a group compound means this group was created using untargeted detection,
        // we set compound name and ID to {mz}@{rt} strings for untargeted sets.
        compoundName = std::to_string(group->meanMz) + "@" + std::to_string(group->meanRt);
        compoundID = compoundName;
    }
    
    _reportStream << SEP << compoundName
                  << SEP << compoundID
                  << SEP << formula
                  << SEP << setprecision(3) << expectedRtDiff
                  << SEP << setprecision(6) << ppmDist;

    if (group->parent != NULL) {
        _reportStream << SEP << group->parent->meanMz;
    } else {
        _reportStream << SEP << group->meanMz;
    }

    if (group->getCompound() && group->getCompound()->type() == Compound::Type::PRM && !_pollyExport) {
        auto groupToWrite = group;

        // if this is a C12 PARENT, then all PRM attributes should be taken from
        // its parent group.
        if (group->tagString.find("C12 PARENT") != std::string::npos)
            groupToWrite = group->parent;

        _reportStream << SEP << groupToWrite->ms2EventCount
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
    _reportStream << setprecision(2);
    for (unsigned int j = 0; j < samples.size(); j++){
        for(int i=0; i< static_cast<int>(group->samples.size());++i){
            if(samples[j]->sampleName==group->samples[i]->sampleName){
                _reportStream << SEP << yvalues[j];
                break;
            }
            else if(i == static_cast<int>(group->samples.size())-1){
                _reportStream << SEP << "NA";
            }
        }   
    }
    
    _reportStream << endl;
    
}

void CSVReports::_writePeakInfo(PeakGroup* group) {
    
    string compoundName = "";
    string compoundID = "";
    string formula = "";
    if (group->getCompound() != NULL) {
        compoundName = _sanitizeString(group->getCompound()->name.c_str()).toStdString();
        compoundID   = _sanitizeString(group->getCompound()->id.c_str()).toStdString();
        formula = _sanitizeString(group->getCompound()->formula.c_str()).toStdString();
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

    vector<mzSample*> samplesWithNoPeak = samples;
    for (unsigned int j = 0; j < group->peaks.size(); j++) {
        Peak& peak = group->peaks[j];
        mzSample* sample = peak.getSample();
        string sampleName;
        if (sample != NULL) {
            samplesWithNoPeak.erase(remove_if(begin(samplesWithNoPeak),
                                              end(samplesWithNoPeak),
                                              [sample] (mzSample* s) {
                                                  return s == sample;
                                              }),
                                    end(samplesWithNoPeak));

            sampleName = _sanitizeString(sample->sampleName.c_str()).toStdString();
        }


        _reportStream << fixed << setprecision(6)
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
    for (auto sample : samplesWithNoPeak) {
        string sampleName = "";
        if (sample != nullptr) {
            sampleName = _sanitizeString(sample->sampleName.c_str()).toStdString();
        }
        _reportStream << fixed << setprecision(6)
                   << group->groupId
                   << SEP << compoundName
                   << SEP << compoundID
                   << SEP << formula
                   << SEP << sampleName
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << setprecision(3)
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << setprecision(2)
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0.0f
                   << SEP << 0 << endl;
    }
    
  
}


void CSVReports::writeDataForPolly(const std::string& file, std::list<PeakGroup> groups)
{
    _reportStream.open(file.c_str());
    if(_reportStream.is_open()) {
        _reportStream << "labelML" << "," << "isotopeLabel" << "," << "compound";
        _reportStream << endl;
        for(auto grp: groups) {
            for(auto child: grp.children) {

                int mlLabel =  (child.markedGoodByCloudModel) ? 1 : (child.markedBadByCloudModel) ? 0 : -1;
                _reportStream << mlLabel;
                _reportStream << ",";

                string tagString = child.srmId + child.tagString;
                tagString = _sanitizeString(tagString.c_str()).toStdString();
                _reportStream << tagString ;
                _reportStream << ",";

                string compoundName = "";
                if(child.getCompound() != NULL)
                    compoundName = _sanitizeString(child.getCompound()->name.c_str()).toStdString();
                else
                    compoundName = std::to_string(child.meanMz) + "@" + std::to_string(child.meanRt);
                _reportStream << compoundName;

                _reportStream << endl;
            }
        }
    }
    _reportStream.close();
}
    



class CSVReportFixture
{
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
            CSVReportFixture(){
                mavenparameters= new MavenParameters();
                                
                _makeSampleList();
          
                allgroups=_getGroupsFromProcessCompounds();
                 
            }
                
            ~CSVReportFixture(){
                delete samples[0];
                delete samples[1];
                delete samples[2];
                delete samples[3];
                delete mavenparameters;
            }            
            
};


TEST_CASE_FIXTURE(CSVReportFixture,"Testing csvReports")
{
   
    SUBCASE("Testing Group File"){
        
        string groupReport = "groupReport.csv";
   
        CSVReports* csvReports = new CSVReports(groupReport,CSVReports::ReportType::GroupReport,samples, 
                                                        PeakGroup::AreaTop, false, true, mavenparameters);
        
        for(int i=0; i < static_cast<int>(allgroups.size()); i++ ) {
            PeakGroup* peakGroup = new PeakGroup(allgroups[i]);
            csvReports->addGroup(peakGroup);
        }

        ifstream inputGroupFile("groupReport.csv");
        ifstream savedGroupFile("C:\\msys64\\home\\kashika\\ElMaven\\tests\\doctest\\test_groupReport.csv");
        
        string headerInput;
        string headerSaved;
        
        if( !inputGroupFile.eof())       
            getline(inputGroupFile,headerInput);
        if( !savedGroupFile.eof())
            getline(savedGroupFile,headerSaved);
        
        vector<std::string> headerInputValues;
        mzUtils::splitNew(headerInput, "," , headerInputValues);
        
        vector<std::string> headerSavedValues;
        mzUtils::splitNew(headerSaved, "," , headerSavedValues);
        
        for(int i = 0; i < static_cast<int>(headerInputValues.size()); i++)
            REQUIRE(headerInputValues[i] == headerSavedValues[i]);
        
        while(!inputGroupFile.eof() && !savedGroupFile.eof())
        {
            string input;
            string saved;
            
            getline(inputGroupFile,input);
            getline(savedGroupFile,saved);
                
            vector<std::string> inputValues;
            mzUtils::splitNew(input, "," , inputValues);
        
            vector<std::string> savedValues;
            mzUtils::splitNew(saved, "," , savedValues);


            double inputFloat; 
            double savedFloat;
            
            for(int i = 0; i < static_cast<int>(inputValues.size()); i++){  
                inputFloat = string2float(inputValues[i]);
                savedFloat = string2float(savedValues[i]);
                REQUIRE( inputFloat == doctest::Approx(savedFloat).epsilon(0.05));
            }
            
        }
        inputGroupFile.close();
        savedGroupFile.close();
        remove("groupReport.csv");
    }
    
    
    SUBCASE("Testing Peak File"){
        
        
        string peakReport = "peakReport.csv";
   
        CSVReports* csvReports = new CSVReports(peakReport,CSVReports::ReportType::PeakReport,samples, 
                                                        PeakGroup::AreaTop, false, true, mavenparameters);
        
        for(int i=0; i < static_cast<int>(allgroups.size()); i++ ) {
            PeakGroup* peakGroup = new PeakGroup(allgroups[i]);
            csvReports->addGroup(peakGroup);
        }
        
        ifstream inputPeakFile("peakReport.csv");
        ifstream savedPeakFile("C:\\msys64\\home\\kashika\\ElMaven\\tests\\doctest\\test_peakReport.csv");
        
        string headerInput;
        string headerSaved;
        
        if( !inputPeakFile.eof())       
            getline(inputPeakFile,headerInput);
        if( !savedPeakFile.eof())
            getline(savedPeakFile,headerSaved);
        
        vector<std::string> headerInputValues;
        mzUtils::splitNew(headerInput, "," , headerInputValues);
        
        vector<std::string> headerSavedValues;
        mzUtils::splitNew(headerSaved, "," , headerSavedValues);
        
        for(int i = 0; i < static_cast<int>(headerInputValues.size()); i++)
            REQUIRE(headerInputValues[i] == headerSavedValues[i]);
        
        while(!inputPeakFile.eof() && !savedPeakFile.eof())
        {
            string input;
            string saved;
            
            getline(inputPeakFile,input);
            getline(savedPeakFile,saved);
                
            vector<std::string> inputValues;
            mzUtils::splitNew(input, "," , inputValues);
        
            vector<std::string> savedValues;
            mzUtils::splitNew(saved, "," , savedValues);


            double inputFloat; 
            double savedFloat;
            
            for(int i = 0; i < static_cast<int>(inputValues.size()); i++){  
                inputFloat = string2float(inputValues[i]);
                savedFloat = string2float(savedValues[i]);
                REQUIRE( inputFloat == doctest::Approx(savedFloat).epsilon(0.05));
            }
            
        }
        
        inputPeakFile.close();
        savedPeakFile.close();
        remove("peakReport.csv");
    }
    
}


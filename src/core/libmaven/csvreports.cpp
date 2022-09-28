#include "doctest.h"
#include "testUtils.h"
#include "csvreports.h"
#include <boost/lexical_cast.hpp>
#include "Compound.h"
#include "datastructures/adduct.h"
#include "peakdetector.h"
#include "classifierNeuralNet.h"
#include "constants.h"
#include "database.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "mzUtils.h"

CSVReports::CSVReports(string filename,
                       ReportType reportType,
                       vector<mzSample*>& insamples,
                       PeakGroup::QType quantType,
                       bool prmReport,
                       bool includeSetNamesLine,
                       MavenParameters* mp,
                       bool pollyExport, 
                       bool groupsClassified)
{
    samples = insamples;
    _pollyExport = pollyExport;
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
    errorReport = "";
    mavenparameters = mp;
    _qtype = quantType;
    _reportType = reportType;
    _prmReport = prmReport;
    _includeSetNamesLine = includeSetNamesLine;
    _groupsClassifed = groupsClassified;

    if (reportType == ReportType::PeakReport) {
        if (samples.size() == 0)
            return;
        if (QString(filename.c_str()).endsWith(".csv", Qt::CaseInsensitive))
            setCommaDelimited();
        else
            setTabDelimited();
        _reportStream.open(filename.c_str(), ios::out);
        /**@brief-  write name of column  if output file is open */
        _insertPeakReportColumnNamesintoCSVFile();
    }

    else if (reportType == ReportType::GroupReport) {
        // if number of sample is zero, output file will not open
        if (insamples.size() == 0)
            return;
        if (QString(filename.c_str()).endsWith(".csv", Qt::CaseInsensitive))
            setCommaDelimited();
        else
            setTabDelimited();
        // after checking initial check, open output file
        _reportStream.open(filename.c_str(), ios::out);

        // write name of column  if output file is open
        _insertGroupReportColumnNamesintoCSVFile(
            filename, _prmReport, _includeSetNamesLine);
    }
}

CSVReports::~CSVReports()
{
    if (_reportStream.is_open())
        _reportStream.close();
}

QString CSVReports::_sanitizeString(const char* s)
{
    QString out = s;
    out.replace(QString("\""), QString("\"\""));
    if (out.contains(SEP.c_str())) {
        out = "\"" + out + "\"";
    }
    return out;
}

void CSVReports::_insertGroupReportColumnNamesintoCSVFile(
    string outputfile,
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
                            << "adductName"
                            << "isotopeLabel"
                            << "compound"
                            << "compoundId"
                            << "formula"
                            << "expectedRtDiff"
                            << "ppmDiff"
                            << "parent";

        // if this is a MS2 report, add MS2 specific columns
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

        // If tables are classified.
        if (!_pollyExport && _groupsClassifed) {
            groupReportcolnames << "peakML_label_id"
                                << "peakML_label"
                                << "peakML_probability"
                                << "peakML_inference_key"
                                << "peakML_inference_value"
                                << "peakML_correlated_groups"
                                << "peakML_base_value"
                                << "peakML_output_value";
        }

        int cohort_offset = groupReportcolnames.size() - 1;
        QString header = groupReportcolnames.join(SEP.c_str());
        _reportStream << header.toStdString();
        for (unsigned int i = 0; i < samples.size(); i++) {
            string name = samples[i]->getSampleName();
            _reportStream << SEP << _sanitizeString(name.c_str()).toStdString();
        }
        _reportStream << endl;

        if (includeSetNamesLine) {
            for (int i = 0; i < cohort_offset; i++)
                _reportStream << SEP;

            for (size_t i = 0; i < samples.size(); i++) {
                string name = samples[i]->getSetName();
                _reportStream << SEP
                              << _sanitizeString(name.c_str()).toStdString();
            }
            _reportStream << endl;
        }
    } else {
        errorReport =
            "Unable to write to file \"" + QString::fromStdString(outputfile)
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
                           << "adductName"
                           << "isotopeLabel"
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
        
        // If tables are classified.
        if (!_pollyExport && _groupsClassifed) {
            peakReportcolnames << "peakML_label_id"
                                << "peakML_label"
                                << "peakML_probability"
                                << "peakML_inference_key"
                                << "peakML_inference_value"
                                << "peakML_correlated_groups"
                                << "peakML_base_value"
                                << "peakML_output_value";
        }                   

        QString header = peakReportcolnames.join(SEP.c_str());
        _reportStream << header.toStdString() << endl;
    }
}

void CSVReports::addGroup(PeakGroup* group)
{
    if (_reportType == ReportType::PeakReport) {
        _writePeakInfo(group);
        for (auto subGroup : group->childIsotopes())
            _writePeakInfo(subGroup.get());
        for (auto subGroup : group->childAdducts())
            _writePeakInfo(subGroup.get());
    }

    if (_reportType == ReportType::GroupReport) {
        _writeGroupInfo(group);
        for (auto subGroup : group->childIsotopes())
            _writeGroupInfo(subGroup.get());
        for (auto subGroup : group->childAdducts())
            _writeGroupInfo(subGroup.get());
    }
}

void CSVReports::_writeGroupInfo(PeakGroup* group)
{
    if (!_reportStream.is_open())
        return;

    // for exports to Polly, add empty ghost parents as well
    if (group->isGhost() && group->hasCompoundLink() && _pollyExport) {
        auto compoundName = _sanitizeString(
                           group->getCompound()->name().c_str()).toStdString();
        auto compoundId = _sanitizeString(
                         group->getCompound()->id().c_str()).toStdString();
        auto compoundFormula = _sanitizeString(
                      group->getCompound()->formula().c_str()).toStdString();
        Adduct* adduct = nullptr;
        for (auto parentAdduct : mavenparameters->getDefaultAdductList()) {
            if (SIGN(parentAdduct->getCharge())
                == SIGN(mavenparameters->ionizationMode)) {
                adduct = parentAdduct;
            }
        }
        _reportStream << ""
                      << SEP << group->metaGroupId()
                      << SEP << group->groupId()
                      << SEP << 0
                      << fixed << setprecision(6)
                      << SEP << 0.0
                      << setprecision(3)
                      << SEP << 0.0
                      << setprecision(6)
                      << SEP << 0.0
                      << SEP << (adduct == nullptr ? "" : adduct->getName())
                      << SEP << "C12 PARENT"
                      << SEP << compoundName
                      << SEP << compoundId
                      << SEP << compoundFormula
                      << SEP << "NA"
                      << SEP << "NA"
                      << SEP << 0.0;

        // if this is a MS2 report, add MS2 specific columns
        if (_prmReport && !_pollyExport) {
            for (int i = 0; i < 10; ++i)
                _reportStream << SEP <<  0.0;
        }

        _reportStream << setprecision(2);
        for (unsigned int j = 0; j < samples.size(); j++) {
            for (int i = 0; i < static_cast<int>(group->samples.size()); ++i) {
                if (samples[j]->sampleName == group->samples[i]->sampleName) {
                    _reportStream << SEP << 0.0;
                    break;
                } else if (i == static_cast<int>(group->samples.size()) - 1) {
                    _reportStream << SEP << "NA";
                }
            }
        }
        _reportStream << endl;
        return;
    } else if (group->isGhost()) {
        return;
    }

    char label = group->userLabel();
    if (!_groupsClassifed) {
        if (group->parent != nullptr && !group->parent->isGhost())
            label = group->parent->userLabel();
    } 
    if (selectionFlag == 2) {
        if (label != 'g')
            return;
    } else if (selectionFlag == 3) {
        if (label != 'b')
            return;
    } else if (selectionFlag == 4) {
        if (label == 'b')
            return;
    } else if ( selectionFlag == 10) {
        if (group->isGroupHidden())
            return;
    }

    vector<float> yvalues = group->getOrderedIntensityVector(samples, _qtype);

    string tagString = group->srmId + group->tagString;
    tagString = _sanitizeString(tagString.c_str()).toStdString();

    char labelStr[2];
    sprintf(labelStr, "%c", label);

    string adductName = "";
    if (group->adduct() != nullptr && !group->isIsotope())
        adductName = group->adduct()->getName();

    _reportStream << labelStr
                  << SEP << group->metaGroupId()
                  << SEP << group->groupId()
                  << SEP << group->goodPeakCount
                  << fixed << setprecision(6)
                  << SEP << group->meanMz
                  << setprecision(3)
                  << SEP << group->meanRt
                  << setprecision(6)
                  << SEP << group->maxQuality
                  << SEP << adductName
                  << SEP << tagString;

    string compoundName = "";
    string compoundID = "";
    string formula = "";
    float expectedRtDiff = -1.0f;
    float ppmDist = -1.0f;
    if (group->hasCompoundLink()) {
        compoundName = _sanitizeString(
                           group->getCompound()->name().c_str()).toStdString();
        compoundID = _sanitizeString(
                         group->getCompound()->id().c_str()).toStdString();
        formula = _sanitizeString(
                      group->getCompound()->formula().c_str()).toStdString();

        int charge = getMavenParameters()->getCharge(group->getCompound());
        double expectedMz = group->getExpectedMz(charge);
        double observedMz = static_cast<double>(group->meanMz);
        auto cutoff = getMavenParameters()->massCutoffMerge;
        ppmDist = mzUtils::massCutoffDist(expectedMz, observedMz, cutoff);

        expectedRtDiff = group->expectedRtDiff();
    } else {
        // absence of a group compound means this group was created using
        // untargeted detection, we set compound name and ID to {mz}@{rt}
        // strings for untargeted sets.
        compoundName =
            std::to_string(group->meanMz) + "@" + std::to_string(group->meanRt);
        compoundID = compoundName;
    }

    string expRtString = expectedRtDiff < 0.0f ? "NA"
                                               : to_string(expectedRtDiff);
    string ppmDistString = ppmDist < 0.0f ? "NA" : to_string(ppmDist);
    _reportStream << SEP << compoundName
                  << SEP << compoundID
                  << SEP << formula
                  << setprecision(3)
                  << SEP << expRtString
                  << setprecision(6)
                  << SEP << ppmDistString;

    if (group->parent != NULL) {
        _reportStream << SEP << group->parent->meanMz;
    } else {
        _reportStream << SEP << group->meanMz;
    }

    if (group->getCompound()
        && group->getCompound()->type() == Compound::Type::MS2
        && !_pollyExport) {
        _reportStream << SEP << group->ms2EventCount
                      << SEP << group->fragMatchScore.numMatches
                      << SEP << group->fragMatchScore.fractionMatched
                      << SEP << group->fragMatchScore.ticMatched
                      << SEP << group->fragMatchScore.dotProduct
                      << SEP << group->fragMatchScore.weightedDotProduct
                      << SEP << group->fragMatchScore.hypergeomScore
                      << SEP << group->fragMatchScore.spearmanRankCorrelation
                      << SEP << group->fragMatchScore.mzFragError
                      << SEP << group->fragmentationPattern.purity;
    }

    if (!_pollyExport && _groupsClassifed) {
        _writePeakMLInfo(group);
    }
    // for intensity values, we only write two digits of floating point
    // precision since these values are supposed to be large (in the order of
    // >10^3).
    _reportStream << setprecision(2);
    for (unsigned int j = 0; j < samples.size(); j++) {
        for (int i = 0; i < static_cast<int>(group->samples.size()); ++i) {
            if (samples[j]->sampleName == group->samples[i]->sampleName) {
                _reportStream << SEP << yvalues[j];
                break;
            } else if (i == static_cast<int>(group->samples.size()) - 1) {
                _reportStream << SEP << "NA";
            }
        }
    }
    _reportStream << endl;
}

void CSVReports::_writePeakMLInfo(PeakGroup* group) 
{
    string peakML_label = "";
    if (group->isClassified) {
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 0)
            peakML_label = "Noise";  
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 1)
            peakML_label = "Signal";
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 2)
            peakML_label = "Signal with only correlation";
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 3)
            peakML_label = "Signal with only cohort-variance";        
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 4)
            peakML_label = "Signal with correlation and cohort-variance";
        if (PeakGroup::integralValueForLabel(group->predictedLabel()) == 5)
            peakML_label = "May be good signal";   
    }

    auto inference = group->predictionInference();
    string keyString = "";
    string valueString = "";
    if (!inference.empty()) {
        for (auto& element : inference) {
            keyString += to_string(element.first) + "; ";
            valueString += element.second + "; ";
        }
        // remove extra delimiters at the end
        keyString.pop_back();
        valueString.pop_back();
    }

    string correlatedGroups = "";
    if (group->predictedLabel() == 
        PeakGroup::ClassifiedLabel::Correlation
        || group->predictedLabel() == 
        PeakGroup::ClassifiedLabel::CorrelationAndPattern)
    {
        correlatedGroups += "{";
        auto correlatedGroupMap = group->getCorrelatedGroups();
        int size = correlatedGroupMap.size();
        int count = 0;
        for(auto it = correlatedGroupMap.begin(); 
        it != correlatedGroupMap.end(); 
        it++)
        {   
            count++;
            correlatedGroups += mzUtils::integer2string(it->first);
            correlatedGroups += ": ";
            correlatedGroups += mzUtils::float2string(it->second, 2);
            if(count != size)
                correlatedGroups += "; ";
        }
        correlatedGroups += "}";
    }

    _reportStream << SEP << int(group->predictedLabel())
                  << SEP << peakML_label 
                  << SEP << group->predictionProbability()
                  << SEP << keyString
                  << SEP << valueString
                  << SEP << correlatedGroups
                  << SEP << group->peakMLBaseValue
                  << SEP << group->peakMLOutputValue;
}

void CSVReports::_writePeakInfo(PeakGroup* group)
{
    if (!_reportStream.is_open())
        return;

    // for exports to Polly, add empty ghost parents as well
    if (group->isGhost() && group->hasCompoundLink() && _pollyExport) {
        auto compoundName = _sanitizeString(
                           group->getCompound()->name().c_str()).toStdString();
        auto compoundId = _sanitizeString(
                         group->getCompound()->id().c_str()).toStdString();
        auto compoundFormula = _sanitizeString(
                      group->getCompound()->formula().c_str()).toStdString();
        Adduct* adduct = nullptr;
        for (auto parentAdduct : mavenparameters->getDefaultAdductList()) {
            if (SIGN(parentAdduct->getCharge())
                == SIGN(mavenparameters->ionizationMode)) {
                adduct = parentAdduct;
            }
        }

        for (auto sample : group->samples) {
            string sampleName = "";
            if (sample != nullptr) {
                sampleName =
                    _sanitizeString(sample->sampleName.c_str()).toStdString();
            }
            _reportStream << fixed << setprecision(6)
                          << group->groupId()
                          << SEP << compoundName
                          << SEP << compoundId
                          << SEP << compoundFormula
                          << SEP << sampleName
                          << SEP << (adduct == nullptr ? "" : adduct->getName())
                          << SEP << "C12 PARENT"
                          << SEP << "NA"
                          << SEP << "NA"
                          << SEP << "NA"
                          << setprecision(3)
                          << SEP << "NA"
                          << SEP << "NA"
                          << SEP << "NA"
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
                          << SEP << "NA"
                          << endl;
        }
    } else if (group->isGhost()) {
        return;
    }

    char label = group->userLabel();
    if (group->parent != nullptr && !group->parent->isGhost())
        label = group->parent->userLabel();
    if (selectionFlag == 2) {
        if (label != 'g')
            return;
    } else if (selectionFlag == 3) {
        if (label != 'b')
            return;
    } else if (selectionFlag == 4) {
        if (label == 'b')
            return;
    }

    string compoundName = "";
    string compoundID = "";
    string formula = "";
    if (group->getCompound() != NULL) {
        compoundName = _sanitizeString(group->getCompound()->name().c_str()).toStdString();
        compoundID   = _sanitizeString(group->getCompound()->id().c_str()).toStdString();
        formula = _sanitizeString(group->getCompound()->formula().c_str()).toStdString();
    } else {
        // absence of a group compound means this group was created using
        // untargeted detection,
        // we set compound name and ID to {mz}@{rt} strings for untargeted sets.
        compoundName =
            std::to_string(group->meanMz) + "@" + std::to_string(group->meanRt);
        compoundID = compoundName;
    }

    string adductName = "";
    if (group->adduct() != nullptr && !group->isIsotope())
        adductName = group->adduct()->getName();

    string tagString = group->srmId + group->tagString;
    tagString = _sanitizeString(tagString.c_str()).toStdString();

    // sort the peaks in the group according to the sample names using a
    // comparison function
    // this ensures that the order in which the peaks are written is same across
    // different systems.
    std::sort(group->peaks.begin(), group->peaks.end(), Peak::compSampleName);

    vector<mzSample*> samplesWithNoPeak = samples;
    for (unsigned int j = 0; j < group->peaks.size(); j++) {
        Peak& peak = group->peaks[j];
        mzSample* sample = peak.getSample();
        string sampleName;
        if (sample != NULL) {
            samplesWithNoPeak.erase(
                remove_if(begin(samplesWithNoPeak),
                          end(samplesWithNoPeak),
                          [sample](mzSample* s) { return s == sample; }),
                end(samplesWithNoPeak));

            sampleName =
                _sanitizeString(sample->sampleName.c_str()).toStdString();
        }

        _reportStream << fixed << setprecision(6)
                      << group->groupId()
                      << SEP << compoundName
                      << SEP << compoundID
                      << SEP << formula
                      << SEP << sampleName
                      << SEP << adductName
                      << SEP << tagString
                      << SEP << peak.peakMz
                      << SEP << peak.mzmin
                      << SEP << peak.mzmax
                      << setprecision(3)
                      << SEP << peak.rt
                      << SEP << peak.rtmin
                      << SEP << peak.rtmax
                      << SEP << peak.quality
                      // for intensity values, we only write two digits of
                      // floating point precision
                      // since these values are supposed to be large
                      // (in the order of >10^3).
                      << setprecision(2)
                      << SEP << peak.peakIntensity
                      << SEP << peak.peakArea
                      << SEP << peak.peakSplineArea
                      << SEP << peak.peakAreaTop
                      << SEP << peak.peakAreaCorrected
                      << SEP << peak.peakAreaTopCorrected
                      << SEP << peak.noNoiseObs
                      << SEP << peak.signalBaselineRatio
                      << SEP << peak.fromBlankSample;
            if (!_pollyExport && _groupsClassifed) {
                _writePeakMLInfo(group);
            }
            _reportStream << endl;
    }
    for (auto sample : samplesWithNoPeak) {
        string sampleName = "";
        if (sample != nullptr) {
            sampleName =
                _sanitizeString(sample->sampleName.c_str()).toStdString();
        }
        _reportStream << fixed << setprecision(6)
                      << group->groupId()
                      << SEP << compoundName
                      << SEP << compoundID
                      << SEP << formula
                      << SEP << sampleName
                      << SEP << adductName
                      << SEP << tagString
                      << SEP << "NA"
                      << SEP << "NA"
                      << SEP << "NA"
                      << setprecision(3)
                      << SEP << "NA"
                      << SEP << "NA"
                      << SEP << "NA"
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
                      << SEP << "NA";
            if (!_pollyExport && _groupsClassifed) {
                _writePeakMLInfo(group);
            }
            _reportStream << endl;
    }
}

bool CSVReports::writeDataForPeakMl(const string& filePath,
                                    vector<PeakGroup>& groups)
{
    ofstream file(filePath);

    auto writePeakInfo = [&] (PeakGroup group, int groupId)
                        {
                            for (auto& peak : group.peaks) {
                                file << groupId << ","
                                    << "\"" << peak.getSample()->getSampleName() << "\"" << ","
                                    << "\"" << peak.getSample()->getSetName() << "\"" << ","
                                    << peak.peakArea << ","
                                    << peak.peakAreaTop << ","
                                    << peak.peakAreaFractional << ","
                                    << peak.peakIntensity << ","
                                    << peak.peakMz << ","
                                    << peak.rt << ","
                                    << peak.rtmin << ","
                                    << peak.rtmax << ","
                                    << peak.quality << ","
                                    << peak.width << ","
                                    << peak.gaussFitR2 << ","
                                    << peak.noNoiseFraction << ","
                                    << peak.symmetry << ","
                                    << peak.signalBaselineRatio << ","
                                    << peak.groupOverlap << "\n";
                            }
                        };

    if (file.is_open()) {
        file << "groupId" << ","
             << "sampleName" << ","
             << "cohortName" << ","
             << "peakArea" << ","
             << "peakAreaTop" << ","
             << "peakAreaFractional" << ","
             << "peakIntensity" << ","
             << "peakMz" << ","
             << "peakRt" << ","
             << "peakRtMin" << ","
             << "peakRtMax" << ","
             << "quality" << ","
             << "width" << ","
             << "gaussFitR2" << ","
             << "noNoiseFraction" << ","
             << "symmetry" << ","
             << "signalBaselineRatio" << ","
             << "groupOverlap" << "\n";
        file << fixed << setprecision(6);

        for (auto& group : groups) {
            if (!group.isGhost())
                writePeakInfo(group, group.groupId());
        }
        for (auto& group : groups) {
            for (auto&child : group.childIsotopes()) {
                if (!child->isGhost())
                    writePeakInfo(*child, child.get()->groupId());
            }
            for (auto &child : group.childAdducts()) {
                if (!child->isGhost())
                    writePeakInfo(*child, child.get()->groupId());
            }
        } 
        file.close();
        return true;
    }
    return false;
}

void CSVReports::writeDataForPolly(const std::string& file,
                                   std::list<PeakGroup> groups)
{
    _reportStream.open(file.c_str(), ios::out);
    if (_reportStream.is_open()) {
        _reportStream << "labelML"
                      << ","
                      << "isotopeLabel"
                      << ","
                      << "compound";
        _reportStream << endl;

        for (auto grp : groups) {
            for (auto child : grp.childIsotopes()) {
                int mlLabel = (child->markedGoodByCloudModel)
                                  ? 1
                                  : (child->markedBadByCloudModel) ? 0 : -1;
                _reportStream << mlLabel;
                _reportStream << ",";

                string tagString = child->srmId + child->tagString;
                tagString = _sanitizeString(tagString.c_str()).toStdString();
                _reportStream << tagString;
                _reportStream << ",";

                string compoundName = "";
                if(child->hasCompoundLink()) {
                    compoundName = _sanitizeString(
                                       child->getCompound()->name().c_str())
                                       .toStdString();
                } else {
                    compoundName = std::to_string(child->meanMz) + "@"
                                   + std::to_string(child->meanRt);
                }
                _reportStream << compoundName;
                _reportStream << endl;
            }
        }
    }
    _reportStream.close();
}

///////////////////////////Test Cases//////////////////////////////

TEST_CASE_FIXTURE(SampleLoadingFixture, "Testing CSV reports")
{
    // SUBCASE("Testing group report")
    // {
    //     targetedGroup();
    //     auto sample = samples();
    //     auto mavenparameter = mavenparameters();
    //     string groupReport = "groupReport.csv";
    //     CSVReports* csvReports =
    //         new CSVReports(groupReport,
    //                        CSVReports::ReportType::GroupReport,
    //                        sample,
    //                        PeakGroup::AreaTop,
    //                        false,
    //                        true,
    //                        mavenparameter);
    //     auto allgroup = allgroups();
    //     for (int i = 0; i < static_cast<int>(allgroups().size()); i++) {
    //         PeakGroup* peakGroup = new PeakGroup(allgroup[i]);
    //         csvReports->addGroup(peakGroup);
    //     }

    //     ifstream inputGroupFile("groupReport.csv");
    //     ifstream savedGroupFile(
    //         "tests/test-libmaven/test_TargetedGroupReport.csv");
    //     string headerInput;
    //     getline(inputGroupFile, headerInput);
    //     getline(inputGroupFile, headerInput);
    //     string headerSaved;
    //     getline(savedGroupFile, headerSaved);
    //     getline(savedGroupFile, headerSaved);

    //     int cnt = 0;
    //     while (!inputGroupFile.eof()) {
    //         cnt++;
    //         string input;
    //         getline(inputGroupFile, input);
    //         if (input.empty())
    //             continue;

    //         if (input.size() > 0) {
    //             vector<string> inputValues;
    //             inputValues = mzUtils::split(input, ",");
    //             if (cnt > 1) {
    //                 savedGroupFile.clear();
    //                 savedGroupFile.seekg(0, ios::beg);
    //                 string headerSaved;
    //                 getline(savedGroupFile, headerSaved);
    //                 getline(savedGroupFile, headerSaved);
    //             }

    //             while (!savedGroupFile.eof()) {
    //                 string saved;
    //                 getline(savedGroupFile, saved);
    //                 if (saved.empty())
    //                     continue;

    //                 vector<string> savedValues;
    //                 savedValues = mzUtils::split(saved, ",");

    //                 if (string2float(inputValues[4])
    //                         == doctest::Approx(string2float(savedValues[4]))
    //                     && string2float(inputValues[5])
    //                            == doctest::Approx(string2float(savedValues[5]))
    //                     &&
    //                     /*epsilon value has to be a greater term i.e 15% as
    //                       inputValue[12] is parts per millions. Thus, it may
    //                       show a more deviation that normal */
    //                     string2float(inputValues[13])
    //                         == doctest::Approx(string2float(savedValues[13]))
    //                                .epsilon(0.15)
    //                     && inputValues[9] == savedValues[9]) {
    //                     double inputFloat;
    //                     double savedFloat;
    //                     for (int i = 3;
    //                          i < static_cast<int>(inputValues.size());
    //                          i++) {
    //                         if (i == 9 || i == 10 || i == 11) {
    //                             REQUIRE(inputValues[i] == savedValues[i]);
    //                         } else if (i == 7) {
    //                             // adducts
    //                             REQUIRE (inputValues[i] == savedValues[i]);
    //                         } else {
    //                             inputFloat = string2float(inputValues[i]);
    //                             savedFloat = string2float(savedValues[i]);
    //                             REQUIRE(inputFloat
    //                                     == doctest::Approx(savedFloat)
    //                                            .epsilon(0.15));
    //                         }
    //                     }
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //     inputGroupFile.close();
    //     savedGroupFile.close();
    //     remove("groupReport.csv");
    // }

    SUBCASE("Testing peak report")
    {
        targetedGroup();
        string peakReport = "peakReport.csv";
        auto sample = samples();
        auto mavenparameter = mavenparameters();

        CSVReports* csvReports =
            new CSVReports(peakReport,
                           CSVReports::ReportType::PeakReport,
                           sample,
                           PeakGroup::AreaTop,
                           false,
                           true,
                           mavenparameter);

        auto allgroup = allgroups();
        for (int i = 0; i < static_cast<int>(allgroup.size()); i++) {
            PeakGroup* peakGroup = new PeakGroup(allgroup[i]);
            csvReports->addGroup(peakGroup);
        }

        ifstream inputPeakFile("peakReport.csv");
        ifstream savedPeakFile(
            "tests/test-libmaven/test_TargetedPeakReport.csv");

        string headerInput;
        getline(inputPeakFile, headerInput);

        string headerSaved;
        getline(savedPeakFile, headerSaved);

        int cnt = 0;
        while (!inputPeakFile.eof()) {
            cnt++;
            string input;
            getline(inputPeakFile, input);

            if (input.size() > 0) {
                vector<string> inputValues;
                inputValues = mzUtils::split(input, ",");

                if (cnt > 1) {
                    savedPeakFile.clear();
                    savedPeakFile.seekg(0, ios::beg);
                    string headerSaved;
                    getline(savedPeakFile, headerSaved);
                }

                while (!savedPeakFile.eof()) {
                    string saved;
                    getline(savedPeakFile, saved);
                    if (saved.empty())
                        continue;

                    vector<string> savedValues;
                    savedValues = mzUtils::split(saved, ",");
                    if (string2float(inputValues[10])        // rt
                            == doctest::Approx(
                                   string2float(savedValues[10])).epsilon(0.3)
                        && string2float(inputValues[14])     // intensity?
                               == doctest::Approx(
                                   string2float(savedValues[14]))
                        && inputValues[1] == savedValues[1]  // compound-name
                        && inputValues[2] == savedValues[2]  // compound-ID
                        && inputValues[6] == savedValues[6]) // isotope label
                    {
                        double inputFloat;
                        double savedFloat;
                        for (int i = 1;
                             i < static_cast<int>(inputValues.size());
                             i++) {
                            if (i == 4) { // skip sample name
                                continue;
                            } else if (i <= 5) {
                                REQUIRE(inputValues[i] == savedValues[i]);
                            } else {
                                inputFloat = string2float(inputValues[i]);
                                savedFloat = string2float(savedValues[i]);
                                REQUIRE(inputFloat
                                        == doctest::Approx(savedFloat)
                                               .epsilon(0.15));
                            }
                        }
                        break;
                    }
                }
            }
        }
        inputPeakFile.close();
        savedPeakFile.close();
        remove("peakReport.csv");
    }

    SUBCASE("Testing write for polly")
    {
        targetedGroup();
        string pollyFile = "polly.csv";
        auto sample = samples();
        auto mavenparameter = mavenparameters();
        CSVReports* csvReports =
            new CSVReports(pollyFile,
                           CSVReports::ReportType::PollyReport,
                           sample,
                           PeakGroup::AreaTop,
                           false,
                           true,
                           mavenparameter);
        std::list<PeakGroup> group = isotopeGroup();
        csvReports->writeDataForPolly(pollyFile, group);

        ifstream inputPeakFile("polly.csv");
        ifstream savedPeakFile("tests/test-libmaven/test_polly.csv");

        string headerInput;
        getline(inputPeakFile, headerInput);
        string headerSaved;
        getline(savedPeakFile, headerSaved);

        int cnt = 0;
        while (!inputPeakFile.eof()) {
            cnt++;
            string input;
            getline(inputPeakFile, input);

            if (input.size() > 0) {
                vector<string> inputValues;
                inputValues = mzUtils::split(input, ",");

                if (cnt > 1) {
                    savedPeakFile.clear();
                    savedPeakFile.seekg(0, ios::beg);
                    string headerSaved;
                    getline(savedPeakFile, headerSaved);
                }

                while (!savedPeakFile.eof()) {
                    string saved;
                    getline(savedPeakFile, saved);
                    if (saved.empty())
                        continue;

                    vector<string> savedValues;
                    savedValues = mzUtils::split(saved, ",");

                    if (inputValues[1] == savedValues[1]
                        && inputValues[2] == savedValues[2]) {
                        for (size_t i = 0; i < inputValues.size(); i++)
                            REQUIRE(inputValues[i] == savedValues[i]);
                        break;
                    }
                }
            }
        }
        inputPeakFile.close();
        savedPeakFile.close();
        remove("polly.csv");
    }

    // SUBCASE("Testing feature group report")
    // {
    //     untargetedGroup();
    //     auto sample = samples();
    //     auto mavenparameter = mavenparameters();
    //     string groupReport = "groupReport.csv";
    //     CSVReports* csvReports =
    //         new CSVReports(groupReport,
    //                        CSVReports::ReportType::GroupReport,
    //                        sample,
    //                        PeakGroup::AreaTop,
    //                        false,
    //                        true,
    //                        mavenparameter);
    //     auto allgroup = allgroups();
    //     for (int i = 0; i < static_cast<int>(allgroups().size()); i++) {
    //         PeakGroup* peakGroup = new PeakGroup(allgroup[i]);
    //         csvReports->addGroup(peakGroup);
    //     }
    //     ifstream inputGroupFile("groupReport.csv");
    //     ifstream savedGroupFile(
    //         "tests/test-libmaven/test_untargetedGroupReport.csv");
    //     string headerInput;
    //     getline(inputGroupFile, headerInput);
    //     getline(inputGroupFile, headerInput);
    //     string headerSaved;
    //     getline(savedGroupFile, headerSaved);
    //     getline(savedGroupFile, headerSaved);

    //     int cnt = 0;

    //     while (!inputGroupFile.eof()) {
    //         cnt++;
    //         string input;
    //         getline(inputGroupFile, input);
    //         if (input.empty())
    //             continue;

    //         if (input.size() > 0) {
    //             vector<string> inputValues;
    //             inputValues = mzUtils::split(input, ",");
    //             if (cnt > 1) {
    //                 savedGroupFile.clear();
    //                 savedGroupFile.seekg(0, ios::beg);
    //                 string headerSaved;
    //                 getline(savedGroupFile, headerSaved);
    //                 getline(savedGroupFile, headerSaved);
    //             }

    //             while (!savedGroupFile.eof()) {
    //                 string saved;
    //                 getline(savedGroupFile, saved);
    //                 if (saved.empty())
    //                     continue;

    //                 vector<string> savedValues;
    //                 savedValues = mzUtils::split(saved, ",");
    //                 if (string2float(inputValues[4])
    //                         == doctest::Approx(string2float(savedValues[4]))
    //                     && string2float(inputValues[5])
    //                            == doctest::Approx(string2float(savedValues[5]))
    //                                   .epsilon(0.01)
    //                     && inputValues[3] == savedValues[3]) {
    //                     double inputFloat;
    //                     double savedFloat;
    //                     // TODO: why not use column names instead of indexes
    //                     for (size_t i = 3; i < inputValues.size(); i++) {
    //                         if (i == 9 || i == 10) {
    //                             continue;
    //                         } else if (i == 7) {
    //                             // adducts
    //                             REQUIRE (inputValues[i] == savedValues[i]);
    //                         } else {
    //                             inputFloat = string2float(inputValues[i]);
    //                             savedFloat = string2float(savedValues[i]);
    //                             REQUIRE(inputFloat
    //                                     == doctest::Approx(savedFloat)
    //                                            .epsilon(0.15));
    //                         }
    //                     }
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //     inputGroupFile.close();
    //     savedGroupFile.close();
    //     remove("groupReport.csv");
    // }

    SUBCASE("Testing feature peak report")
    {
        untargetedGroup();
        string peakReport = "peakReport.csv";
        auto sample = samples();
        auto mavenparameter = mavenparameters();

        CSVReports* csvReports =
            new CSVReports(peakReport,
                           CSVReports::ReportType::PeakReport,
                           sample,
                           PeakGroup::AreaTop,
                           false,
                           true,
                           mavenparameter);

        auto allgroup = allgroups();
        for (int i = 0; i < static_cast<int>(allgroup.size()); i++) {
            PeakGroup* peakGroup = new PeakGroup(allgroup[i]);
            csvReports->addGroup(peakGroup);
        }
        ifstream inputPeakFile("peakReport.csv");
        ifstream savedPeakFile(
            "tests/test-libmaven/test_untargetedPeakReport.csv");

        string headerInput;
        getline(inputPeakFile, headerInput);

        string headerSaved;
        getline(savedPeakFile, headerSaved);

        int cnt = 0;
        while (!inputPeakFile.eof()) {
            cnt++;

            string input;
            getline(inputPeakFile, input);

            if (input.size() > 0) {
                vector<string> inputValues;
                inputValues = mzUtils::split(input, ",");

                if (cnt > 1) {
                    savedPeakFile.clear();
                    savedPeakFile.seekg(0, ios::beg);
                    string headerSaved;
                    getline(savedPeakFile, headerSaved);
                }

                while (!savedPeakFile.eof()) {
                    string saved;
                    getline(savedPeakFile, saved);
                    if (saved.empty())
                        continue;

                    vector<string> savedValues;
                    savedValues = mzUtils::split(saved, ",");

                    if (string2float(inputValues[17])
                            == doctest::Approx(string2float(savedValues[17]))
                                   .epsilon(0.0005)
                        && string2float(inputValues[13])
                               == doctest::Approx(string2float(savedValues[13]))
                                      .epsilon(0.0005)
                        && inputValues[4] == savedValues[4]
                        && string2float(inputValues[12])
                               == doctest::Approx(string2float(savedValues[12]))
                                      .epsilon(0.0005)) {
                        double inputFloat;
                        double savedFloat;
                        for (size_t i = 3; i < inputValues.size(); i++) {
                            if (i == 4 || i == 5) {
                                REQUIRE(inputValues[i] == savedValues[i]);
                            } else {
                                inputFloat = string2float(inputValues[i]);
                                savedFloat = string2float(savedValues[i]);
                                REQUIRE(inputFloat
                                        == doctest::Approx(savedFloat)
                                               .epsilon(0.15));
                            }
                        }
                        break;
                    }
                }
            }
        }
        inputPeakFile.close();
        savedPeakFile.close();
        remove("peakReport.csv");
    }
}

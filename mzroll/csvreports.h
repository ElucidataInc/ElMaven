#ifndef CSVREPORT_H
#define CSVREPORT_H

#include "stable.h"
#include "mzUtils.h"
#include "mzSample.h"

using namespace std;
using namespace mzUtils;

class mzSample;
class EIC;
class PeakGroup;


/**
 * \class CSVReports
 *
 * \ingroup mzroll
 *
 * \brief Class for CSVReports to generate CSV Reports.
 *
 * This class is used for CSVReports to generate CSV Reports.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class CSVReports {

public:
    CSVReports() {};
    CSVReports(vector<mzSample*>& insamples);
    ~CSVReports();
    void openGroupReport(string filename);
    void openPeakReport(string filename);
    void addGroup(PeakGroup* group);
    void closeFiles();
    void setSamples(vector<mzSample*>& insamples) {
        samples = insamples;
    }
    void setUserQuantType(PeakGroup::QType t) {
        qtype = t;
    }

private:
    void writeGroupInfo(PeakGroup* group);
    void writePeakInfo(PeakGroup* group);

    int groupId;	//sequential group numbering

    vector<mzSample*>samples;
    ofstream groupReport;
    ofstream peakReport;
    PeakGroup::QType qtype;


};

#endif

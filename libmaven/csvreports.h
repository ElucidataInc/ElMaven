#ifndef CSVREPORT_H
#define CSVREPORT_H

#include "mzUtils.h"
#include "mzSample.h"
#include <QString>
#include <QStringList>

using namespace std;
using namespace mzUtils;

class mzSample;
class EIC;
class PeakGroup;

class CSVReports {

public:
    CSVReports();
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
    void setTabDelimited() {
        SEP = "\t";
    }
    void setCommaDelimited() {
        SEP = ",";
    }
    ofstream groupReport;
    ofstream peakReport;

private:
    void writeGroupInfo(PeakGroup* group);
    void writePeakInfo(PeakGroup* group);
    void initialCheck(string outputfile);
    void openGroupReportCSVFile(string outputfile);
    void openPeakReportCSVFile(string outputfile);
    void insertGroupReportColumnNamesintoCSVFile();
    void insertPeakReportColumnNamesintoCSVFile();


    int groupId;	//sequential group numbering
    string SEP;

    vector<mzSample*> samples;
    PeakGroup::QType qtype;

};

#endif

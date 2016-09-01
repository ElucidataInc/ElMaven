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
    //TODO: updated when merged with Maven776 - Kiran
    void openGroupReport(string filename,bool includeSetNamesLine);
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
    //TODO: added when merged with Maven776 - Kiran
    QString sanitizeString(const char* s);
    ofstream groupReport;
    ofstream peakReport;

private:
    void writeGroupInfo(PeakGroup* group);
    void writePeakInfo(PeakGroup* group);
    void initialCheck(string outputfile);
    void openGroupReportCSVFile(string outputfile);
    void openPeakReportCSVFile(string outputfile);
    void insertGroupReportColumnNamesintoCSVFile(string outputfile, bool includeSetNamesLine);
    void insertPeakReportColumnNamesintoCSVFile();
    void insertPeakInformationIntoCSVFile(PeakGroup* group);
    void insertGroupInformationIntoCSVFile (PeakGroup* group);
    int getIonisationMode();
    vector<Isotope> computeIsotopes (PeakGroup* group, int ionizationMode);
    void insertIsotopes (PeakGroup* group, vector<Isotope> masslist);
    void insertUserSelectedIsotopes(PeakGroup* group, string isotopeName);
    int insertIsotpesFoundInSamples (PeakGroup* group, string isotopeName, int counter, int k);
    // Not Required after Merging with Maven776 - Kiran
    // void insertIsotpesNotFoundInSamples (PeakGroup* group, string isotopeName);


    int groupId;	//sequential group numbering
    string SEP;

    vector<mzSample*> samples;
    PeakGroup::QType qtype;

};

#endif

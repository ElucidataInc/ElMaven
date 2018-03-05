#ifndef CSVREPORT_H
#define CSVREPORT_H

#include <QString>
#include <QStringList>
#include "constants.h"
#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"

using namespace std;
using namespace mzUtils;

class mzSample;
class EIC;
class PeakGroup;
class MavenParameters;
class CSVReports {
    /**@brief -  class to write and export csv file 
    *@details   -   CSVReports will do all stuf to export group info in csv format
    *such as rt,mz, samples used, compound it, formula, etc
    */
public:
    enum ExportType{ GroupExport, PeakExport};
    /**
    *empty constructor
    */


    /**
     * 1. move some calculation of group into PeakGroup.cpp
     * 2. remove Qt code
     * 3. write test cases
     */
    CSVReports();

    CSVReports(vector<mzSample*>& insamples,
                MavenParameters* mp,
                PeakGroup::QType t,
                string fileName,
                ExportType exportType,
                int selectionFlag,
                bool includeSetNamesLine = false
                );
    ~CSVReports();

    inline void addItem(PeakGroup* group){
        groups.push_back(group);
    }

    bool exportGroup();
    string getErrorReport(void) {
        /**
        *@brief-    return error occured during csv writing
        *TODO-  libmaven is written in standard c++ but here QString is returned.
        */
        return errorReport;
    }
    /**brief-   update string with escape sequence for writing special character    */
    QString sanitizeString(const char* s);
private:
    void writeGroupInfo(PeakGroup* group);      /**@brief-  helper function to write group info*/
    void writePeakInfo(PeakGroup* group);           /**@brief-  helper function to write peak info*/
    void addColumnNames();

    int groupId;	/**@param-  incremental group numbering. Increment by 1 when a group is added for csv report  */
    string SEP;     /**@param-  separator in output file*/

    string errorReport;    /**@param-  error message, TODO- QString should not be in libmaven folder, only standard C++ statement should be here*/

    vector<mzSample*> samples;      /**@param-  pointers to all mz samples uploaded*/
    PeakGroup::QType qtype;             /**@param-  user quant type, represents intensity of peaks*/
    MavenParameters * mavenparameters;
    ofstream outFileStream;
    string _fileName;
    ExportType _exportType;
    vector<PeakGroup*> groups;
    bool _includeSetNamesLine;
    int _selectionFlag;
};

#endif

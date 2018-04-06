#ifndef CSVREPORT_H
#define CSVREPORT_H

#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"
#include "constants.h"

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
    CSVReports();

    /**
     * CSV exporting is a four step process-
     * 1. Create one instance of this class by giving required value of
     * all parameters in below constructor.      
     * 2. Add all groups one by one to be exported by calling addItem method of this class
     * 3. call exportGroup() method of this class which will return true 
     * if report is successful otherwise false
     * 4. If returned value from exportGroup() is false call getErrorReport
     *  method to get error message
     */ 
    /**
     * Note: don't use same instance of this class to export more that one csv file.
     * Otherwise, it'll export mixed groups.
     */
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
    string sanitizeString(string str);
private:
    /**@brief-  helper function to write group info
     */
    void writeGroupInfo(PeakGroup* group);
    /**@brief-  helper function to write peak info
     */      
    void writePeakInfo(PeakGroup* group);
    void addColumnNames();
    /**@brief-  incremental group numbering.
     * Increment by 1 when a group is added for csv report
     */
    int groupId;
    /**@brief-  separator in output file*/
    string SEP;

    /**@param-  error message, TODO- QString should not be in libmaven folder,
     * only standard C++ statement should be here
     */
    string errorReport;

    /**@param-  pointers to all mz samples uploaded
     */
    vector<mzSample*> samples;
    /**@param-  user quant type, represents intensity of peaks
     */
    PeakGroup::QType qtype;
    MavenParameters * mavenparameters;
    ofstream outFileStream;
    string _fileName;
    /**@param- specify either you want to export group info or peak info, 
     * check enum ExportType
     */
    ExportType _exportType;
    vector<PeakGroup*> groups;
    /**@param- samples can belong to many sets, specify here you want to 
     * export set names or not
     */
    bool _includeSetNamesLine;
    /**@param- groups can be labelled as good groups or bad groups, 
     * default assign it -1
     */
    int _selectionFlag;
};

#endif

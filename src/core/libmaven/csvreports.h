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
    /**
    *empty constructor
    */
    CSVReports();
    /**
    *@brief-    creating CSVReports by all the samples uploaded
    *@see -     see details in its definition
    */
    CSVReports(vector<mzSample*>& insamples);
    /**
    *@brief-    destructor, just close all open output files opened for writing csv or tab file
    */
    ~CSVReports();
    /**
    * @brief-   open output file in which group info will be written
    */
    void openGroupReport(string filename,bool includeSetNamesLine = false);
    /**
    * @brief-   open output file in which peak info will be written
    */
    void openPeakReport(string filename);
    /**
    *@brief-    add group for writing csv about
    */
    void addGroup(PeakGroup* group);
    /**
    *close output files either of peak report or group report file
    */
    void closeFiles();
    void setSamples(vector<mzSample*>& insamples) {
        /**
        *@brief-    set all samples uploaded
        */
        samples = insamples;
    }
    void setUserQuantType(PeakGroup::QType t) {
        /**
        *@details-  set user quant type.
        *its a parameter which represent intensity
        */
        qtype = t;
    }

    QString getErrorReport(void) {
        /**
        *@brief-    return error occured during csv writing
        *TODO-  libmaven is written in standard c++ but here QString is returned.
        */
        return errorReport;
    }

    void setMavenParameters(MavenParameters * mp) { 
        /**
        *@brief- set MavenParameters. MavenParameters holds all variable value input by users
        *either from CLI(peakdetector) or GUI(mzroll)
        */
        mavenparameters = mp;
    }
    
    MavenParameters* getMavenParameters() {
       /**
        *@brief- return set MavenParameters. MavenParameters holds all variable value input by users
        *either from CLI(peakdetector) or GUI(mzroll)
        */
        return mavenparameters;
    }

    void setTabDelimited() {
        /**
        *@brief- set the separator as tab   ("\t")
        */
        SEP = "\t";
    }
    void setCommaDelimited() {
        /**
        *@brief- set the separator as comma   (",")
        */
        SEP = ",";
    }
    inline void setSelectionFlag(int selFlag) {
        selectionFlag = selFlag;
    }
    /**brief-   update string with escape sequence for writing special character    */
    QString sanitizeString(const char* s);
    ofstream groupReport;       /**@param-  output file for groups report*/
    ofstream peakReport;         /**@param-  output file for peaks report*/
    int groupId;	/**@param-  incremental group numbering. Increment by 1 when a group is added for csv report  */

private:
    void writeGroupInfo(PeakGroup* group);      /**@brief-  helper function to write group info*/
    void writePeakInfo(PeakGroup* group);           /**@brief-  helper function to write peak info*/
    void initialCheck(string outputfile);                   /**@brief-  if number of samples is zero, no output file will be opened*/
    void openGroupReportCSVFile(string outputfile);     /**@brief-  after performing initial check, it will open output file for groups report*/
    void openPeakReportCSVFile(string outputfile);        /**@brief-  after performing initial check, it will open output file for peaks report*/
    void insertGroupReportColumnNamesintoCSVFile(string outputfile, bool includeSetNamesLine);  /**@brief-  write column name in output file for group report*/
    void insertPeakReportColumnNamesintoCSVFile();          /**@brief-  write column name in output file for group report*/
    void insertPeakInformationIntoCSVFile(PeakGroup* group);        /**@brief-  TODO, not a required method, it's just calling another function. Maybe
                                                                                                                *written to look consistent with   insertGroupInformationIntoCSVFile
                                                                                                                */
    void insertGroupInformationIntoCSVFile (PeakGroup* group);  /**@brief-  after performing some checks, it writes group info to output file*/
    int getIonisationMode();            /**@brief- determine ionization mode (+1 or -1) and return it*/

    void insertIsotopes (PeakGroup* group, vector<Isotope> masslist);                   /**@brief-  through sequencials method calls for isotope detection, write group info into output file*/
    void insertUserSelectedIsotopes(PeakGroup* group, string isotopeName);        /**@brief-   iterate through all isotope of a group and calls insertIsotpesFoundInSamples
                                                                                                                                    *to write isotopes info to group
                                                                                                                                    */
    int insertIsotpesFoundInSamples (PeakGroup* group, string isotopeName, int counter, int k);     /**@brief-  tke child of group and write about it to csv file*/


    string SEP;     /**@param-  separator in output file*/

    QString errorReport;    /**@param-  error message, TODO- QString should not be in libmaven folder, only standard C++ statement should be here*/

    vector<mzSample*> samples;      /**@param-  pointers to all mz samples uploaded*/
    PeakGroup::QType qtype;             /**@param-  user quant type, represents intensity of peaks*/
    MavenParameters * mavenparameters;
    int selectionFlag;      /**@param-  TODO*/
};

#endif

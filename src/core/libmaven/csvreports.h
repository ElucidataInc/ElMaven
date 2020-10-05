#ifndef CSVREPORT_H
#define CSVREPORT_H

#include <QString>
#include <QStringList>

#include "PeakGroup.h"

using namespace std;

class mzSample;
class EIC;
class MavenParameters;

class CSVReports
{
    /**@brief -  class to write and export csv file
     *@details   -   CSVReports will do all stuf to export group info in csv
     *format such as rt,mz, samples used, compound it, formula, etc
     */
    public:
        /**
         *@brief enum is the type of the
         *file the user wants to create
         */
        enum class ReportType { GroupReport, PeakReport, PollyReport };

        /**
         *empty constructor
         */
        CSVReports() {}

        /**
         *@brief Parameterised Constructor
         *@param filename is the name of file
         *@param reportType gives the type of
         *  file being created
         *@param qt is the user quant type
         *@param prmReport
         *@param includeSetNamesLine
         *@param mp is for maven parameters
         *@param pollyExport for exporting to polly
         */
        /*
         * @detail -   constructor for opening the type of report needed by the user
         * (group or peak report)and instantiating class by all samples uploaded,
         * different from samples vector of PeakGroup which will hold
         * samples used for that particular group. it will be used to export group
         * info only for samples used by a group and for other group, fields will
         * be marked NA. Note that these samples are represented by pointers which
         * will change their state even after group has been determine and detected.
         * Only way to get those samples used for particular group by comparing
         * these sample and samples from PeakGroup
         */
        CSVReports(string filename,
                   ReportType reportType,
                   vector<mzSample*>& insamples,
                   PeakGroup::QType quantType = PeakGroup::AreaTop,
                   bool prmReport = false,
                   bool includeSetNamesLine = false,
                   MavenParameters* mp = NULL,
                   bool pollyExport = false, 
                   bool throughCLI = false);

        /**
         *@brief-    destructor, just close all open output files opened for writing
         *csv or tab file
         */
        ~CSVReports();

        /**
         * @brief Open output file in which peak info will be written.
         */
        void openPeakReport(string filename);

        /**
         *@brief-    add group for writing csv about
         */
        void addGroup(PeakGroup* group);

        QString getErrorReport(void)
        {
            /**
             *@brief-    return error occured during csv writing
             *TODO-  libmaven is written in standard c++ but here QString is
             *returned.
             */
            return errorReport;
        }

        void writeDataForPolly(const std::string& file,
                               std::list<PeakGroup> groups);

        MavenParameters* getMavenParameters()
        {
            /**
             *@brief- return set MavenParameters. MavenParameters holds all variable
             *value input by users either from CLI(peakdetector) or GUI(mzroll)
             */
            return mavenparameters;
        }

        inline void setSelectionFlag(int selFlag)
        {
            selectionFlag = selFlag;
        }

    private:
        /**
         *@brief-  helper function to write group info
         */
        void _writeGroupInfo(PeakGroup* group);
        /**
         *@brief-  helper function to write peak info
         */
        void _writePeakInfo(PeakGroup* group);

        /**
         *@brief -   update string with escape sequence for
         *  writing special character
         */
        QString _sanitizeString(const char* s);

        /**
         * @brief   Type of the report to be produced
         * @details
         */

        ReportType _reportType;

        /**
         *@param -  output file for  report
         */
        fstream _reportStream;

        /**
         *@param -  separator in output file
         */
        string SEP;

        /**
         *@param -  error message, TODO- QString should
         *not be in libmaven folder, only standard C++
         *statement should be here
         */
        QString errorReport;

        /**
         *@param-  pointers to all mz samples uploaded
         */
        vector<mzSample*> samples;

        /**
         *@param -  user quant type, represents intensity of peaks
         */
        PeakGroup::QType _qtype;
        MavenParameters* mavenparameters;
        int selectionFlag; /**@param-  TODO*/
        bool _pollyExport;
        bool _prmReport;
        bool _includeSetNamesLine;
        bool _throughCLI;

        /**
         * @brief Write column name in output file for group report.
         */
        void _insertGroupReportColumnNamesintoCSVFile(string outputfile,
                                                      bool prmReport,
                                                      bool includeSetNamesLine);

        /**
         * Write column name in output file for group report.
         */
        void _insertPeakReportColumnNamesintoCSVFile();

        void setTabDelimited()
        {
            /**
             *@brief- set the separator as tab   ("\t")
             */
            SEP = "\t";
        }
        void setCommaDelimited()
        {
            /**
             *@brief- set the separator as comma   (",")
             */
            SEP = ",";
        }
};
#endif

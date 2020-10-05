#ifndef PEAKDETECTORCLI_H
#define PEAKDETECTORCLI_H

#include <limits.h>
#include <sys/time.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#ifndef __APPLE__
#include <omp.h>
#endif

#include <QDateTime>
#include <QDomDocument>
#include <QtCore>
#include "QByteArray"
#include "QMap"
#include "QString"
#include "QStringList"

#include "peakdetector.h"
#include "classifierNeuralNet.h"
#include "csvreports.h"
#include "database.h"
#include "jsonReports.h"
#include "massslicer.h"
#include "mzSample.h"
#include "options.h"
#include "parseoptions.h"
#include "pollyintegration.h"
#include "pugixml.hpp"

#ifndef __APPLE__
    #ifdef OMP_PARALLEL
        #define getTime() omp_get_wtime()
    #else
        #define getTime() get_wall_time()
        #define omp_get_thread_num()  1
    #endif
#endif

using namespace std;

class PollyIntegration;
class ParseOptions;
class DownloadManager;
class Logger;
class Analytics;

class PeakDetectorCLI
{
public:
    enum class AlignmentMode {
        None,
        ObiWarp,
        PolyFit
    };

    bool status;
    string textStatus;
    vector<string> filenames;
    MavenParameters* mavenParameters;
    PeakDetector* peakDetector;
    bool saveJsonEIC;
    PeakGroup::QType quantitationType;
    string clsfModelFilename;
    QString pollyArgs;
    AlignmentMode alignMode;

    PeakDetectorCLI(Logger* log,
                    Analytics* analytics,
                    int argc = 0,
                    char* argv[] = nullptr);
    ~PeakDetectorCLI();

    /**
     * [process command line Options]
     * @param argc [argument counter]
     * @param argv [argument variables]
     */
    void processOptions(int argc, char* argv[]);

    void createXMLFile(const char* fileName);

    void exportPeakReport(string setName);

    void exportSampleReport(string setName);

    /**
     * [handle loading of arguments using XML]
     */
    void processXML(const char* fileName);

    /**
     * @brief Parse XML (saved settings) file generated from El-MAVEN's GUI.
     * @param settingsFilepath Path to settings file.
     */
    void processSettingsFromGui(const string &settingsFilepath);

    /**
     * [load Classfication model]
     * @param clsfModelFilename [name of classifier model]
     */
    void loadClassificationModel(string clsfModelFilename);

    /**
     * @brief load user provided compound database
     * @details load the compound database from mavenparameters and given
     * appropriate warnings or exit
     */
    void loadCompoundsFile();

    /**
     * [loadSamples description]
     * @param filenames [description]
     */
    void loadSamples(vector<string>& filenames);

    /**
     * @brief Align samples using the given alignment method.
     * @method alignSamples
     */
    void alignSamples(const int& method);

    /**
     * [reduce number of Groups]
     */
    void reduceGroups();

    /**
     * [write multiple types of Reports]
     * @param setName [name of the set]
     * @param jsPath [path for index.js file]
     * @param nodePath [path for node executable]
     */
    void writeReport(string setName, QString jsPath, QString nodePath);

    /**
     * @brief save output in a json file
     * @param setName file name with full path
     */
    void saveJson(string setName);

    /**
     * @brief save project as CSV
     * @param setName file name with full path
     */
    void saveCSV(string setName, bool pollyExport, CSVReports::ReportType reportType);

    /**
     * [Uploads Maven data to Polly and redirects the user to polly]
     * @param jspath  [path to index.js file]
     * @param nodepath  [path to node executable]
     * @param filenames [List of files to be uploaded on polly]
     */
    QString uploadToPolly(QString jsPath,
                          QString nodePath,
                          QStringList filenames,
                          QMap<QString, QString> creds);

    int prepareCompoundDbForPolly(QString fileName);

    bool saveAnalysisAsProject() { return !_projectName.isEmpty(); }

    /**
     * @brief Save the analysis (mostly just peak-group information) as an
     * emDB project.
     */
    void saveEmdb();

    inline const vector<char*> getOptions()
    {
        const vector<char*> options = {
            "a?alignSamples: Enter 1 for Obi-Warp alignment, 2 for Polyfit.",
            "c?matchRtFlag: Enter non-zero integer to match retention time to "
                "the database values. <int>",
            "C?compoundPPMWindow: Enter ppm window for m/z. <float>",
            "d?db: Enter full path to database file. <string>",
            "e?processAllSlices: Enter non-zero integer to run untargeted peak "
                "detection. <int>",
            "f?pullIsotopes: Enter 1111 to pull all isotopic labels, 0000 for "
                "no isotopes. <int>",
            "g?grouping_maxRtWindow: Enter the maximum Rt difference between "
                "peaks in a group. <float>",
            "h?help: Print this help message. Refer to \"https://github.com/"
                "ElucidataInc/ElMaven/wiki/El-MAVEN-Command-Line-Interface\" "
                "for more details.",
            "i?minGroupIntensity: Enter min group intensity threshold for a "
                "group. <float>",
            "I?quantileIntensity: Specify required percentage of peaks above "
                "the intensity threshold. <float>",
            "j?saveEicJson: Enter non-zero integer to save EIC JSON in the "
                "output folder. <int>",
            "k?charge: Enter the magnitude of charge on each compound. <int>",
            "m?model: Enter full path to the model file. <string>",
            "n?eicMaxGroups: Enter maximum number of groups reported per "
                "compound. <int>",
            "o?outputdir: Enter full path to output folder. <string>",
            "p?ppmMerge: Enter ppm window for untargeted peak detection and "
                "removing duplicate groups. <float>",
            "q?minQuality: Enter min peak quality threshold for a group. "
                "<float>",
            "Q?quantileQuality: Specify required percentage of peaks above "
                "quality threshold. <float>",
            "r?rtStepSize: Enter retention time window for untargeted peak "
                "detection. <float>",
            "s?saveProject: If set to a <name>, the analysis will be saved as "
                "<name>.emDB project. If given <name> contains the string "
                "\".raw\" in it, the emDB will be saved with raw peak data. "
                "<string>",
            "v?ionizationMode: Enter 0, -1 or 1 ionization mode. <int>",
            "w?minPeakWidth: Enter min peak width threshold in a group. <int>",
            "x?xml: Enter full path to the config file or a settings file from "
                "El-MAVEN. <string>",
            "X?defaultXml: Create a template config file.",
            "y?eicSmoothingWindow: Enter number of scans used for smoothing at "
                "a time. <int>",
            "z?minSignalBaseLineRatio: Enter min signal to baseline ratio "
                "threshold for a group. <float>",
            "A?pollyApp: Polly application to upload to after peak detection "
                "finishes. Enter 1 for PollyPhi or 2 for QuantFit. <int>",
            "E?pollyExtra: Any miscellaneous information that needs to be sent "
                "to Polly. <string>",
            "N?pollyProject: Polly project where we want to upload our files. "
                "<string>",
            "P?pollyCred: Polly sign in credentials, username, password "
                "provided in xml file. <string>",
            "S?sampleCohort: Sample cohort file needed for PollyPhi workflow. "
                "<string>",
            nullptr
        };
        return options;
    }

private:
    DownloadManager* _dlManager;
    QString _pollyProject;
    QString _sampleCohortFile;
    QString _redirectTo;
    PollyIntegration* _pollyIntegration;
    vector<mzSample*> _samples;
    ParseOptions* _parseOptions;
    Database _db;
    JSONReports* _jsonReports;
    bool _reduceGroupsFlag;
    PollyApp _currentPollyApp;
    QString _pollyExtraInfo;
    Logger *_log;
    Analytics* _analytics;
    QString _projectName;

    /**
     * [Load Arguments for Options Dialog]
     * @param optionsArgs [pugixml xml node]
     */
    void _processOptionsArgsXML(xml_node& optionsArgs);

    /**
     * [Load Arguments for Peaks Dialog]
     * @param peaksArgs [pugixml xml node]
     */
    void _processPeaksArgsXML(xml_node& peaksArgs);

    /**
     * [Load general Arguments]
     * @param generalArgs [pugixml xml node]
     */
    void _processGeneralArgsXML(xml_node& generalArgs);

    void _groupReduction();

    QStringList _getSampleList();

    void _makeSampleCohortFile(QString sampleCohortFilename,
                               QStringList loadedSamples);

    bool _sendUserEmail(QMap<QString, QString> creds, QString redirectionUrl);

    QMap<QString, QString> _readCredentialsFromXml(QString filename);

    /**
     * @brief checks if data is not compatible with Polly
     * @return QString message/warning regarding incompatibility
     */
    QString _isReadyForPolly();

    /**
     * @brief Creates a redirection URL based on the current state of the
     * dialog.
     * @param datetimestamp A timestamp string that will be used to generate a
     * unique URL for a project.
     * @param uploadProjectIdThread Project ID of the user project to send files
     * to.
     * @return Redirection URL as a QString.
     */
    QString _getRedirectionUrl(QString datetimestamp, QString uploadProjectId);

    bool _incompatibleWithPollyApp();
};

struct Arguments
{
    QStringList generalArgs;
    QStringList peakDialogArgs;
    QStringList optionsDialogArgs;

    void populateArgs() {
        generalArgs << "int" << "alignSamples" << "0";
        generalArgs << "int" << "saveEicJson" << "0";
        generalArgs << "string" << "outputdir" << "0";
        generalArgs << "string" << "pollyExtra" << "";
        generalArgs << "string" << "samples" << "path/to/sample1";
        generalArgs << "string" << "samples" << "path/to/sample2";
        generalArgs << "string" << "samples" << "path/to/sample3";

        peakDialogArgs << "int" << "matchRtFlag" << "0";
        peakDialogArgs << "string" << "Db" << "0";
        peakDialogArgs << "int" << "processAllSlices" << "0";
        peakDialogArgs << "int" << "pullIsotopes" << "0";
        peakDialogArgs << "float" << "grouping_maxRtWindow" << "0.5";
        peakDialogArgs << "float" << "minGroupIntensity" << "5000";
        peakDialogArgs << "float" << "quantileIntensity" << "0.0";
        peakDialogArgs << "string" << "model" << "0";
        peakDialogArgs << "int" << "eicMaxGroups" << "1000000";
        peakDialogArgs << "float" << "ppmMerge" << "30";
        peakDialogArgs << "float" << "minQuality" << "0.5";
        peakDialogArgs << "float" << "quantileQuality" << "0.0";
        peakDialogArgs << "float" << "rtStepSize" << "20";
        peakDialogArgs << "int" << "minPeakWidth" << "1";
        peakDialogArgs << "int" << "eicSmoothingWindow" << "10";
        peakDialogArgs << "float" << "minSignalBaseLineRatio" << "2";
        peakDialogArgs << "int" << "quantitationType" << "0";
        peakDialogArgs << "float" << "minScanMz" << "0";
        peakDialogArgs << "float" << "maxScanMz" << "1000000000";
        peakDialogArgs << "float" << "minScanRt" << "0";
        peakDialogArgs << "float" << "maxScanRt" << "1000000000";
        peakDialogArgs << "float" << "minScanIntensity" << "0";
        peakDialogArgs << "float" << "maxScanIntensity" << "9999999999";

        optionsDialogArgs << "int" << "ionizationMode" << "-1";
        optionsDialogArgs << "int" << "charge" << "1";
        optionsDialogArgs << "float" << "compoundPPMWindow" << "10";
    }
};

double get_wall_time();
double get_cpu_time();

#endif

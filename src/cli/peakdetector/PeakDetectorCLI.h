#ifndef PEAKDETECTORCLI_H
#define PEAKDETECTORCLI_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <limits.h>
#include <algorithm>
#include <sys/time.h>
#ifndef __APPLE__
#include <omp.h>
#endif

#include "QMap"
#include "QString"
#include "QStringList"
#include "QByteArray"
#include <QDateTime>

#include "parseOptions.h"
#include "options.h"
#include "mzSample.h"
#include "mzMassSlicer.h"
#include "parseOptions.h"
#include "databases.h"
#include "csvreports.h"
#include "PeakDetector.h"
#include "classifierNeuralNet.h"
#include "jsonReports.h"
#include "pollyintegration.h"

#include <QtCore>

#include "pugixml.hpp"

using namespace std;

class PollyIntegration;
class ParseOptions;

#ifndef __APPLE__
 #ifdef OMP_PARALLEL
    #define getTime() omp_get_wtime()
 #else
    #define getTime() get_wall_time()
    #define omp_get_thread_num()  1
 #endif
#endif

class PeakDetectorCLI {

	public:
		PeakDetectorCLI();

		QString username;
		QString password;
		QString projectname;
		QString pollyArgs;
		QString filedir;
		QStringList pollyargs_list;
		PollyIntegration* _pollyIntegration;

		vector<mzSample*> samples;
		vector<Compound*> compoundsDB;
		vector<PeakGroup> allgroups;
		MassCalculator mcalc;

		ParseOptions *parseOptions;

		bool status = true;
		string textStatus = "";

		time_t startTime, curTime, stopTime;

		vector<string> filenames;

		Databases DB;

		MavenParameters* mavenParameters = new MavenParameters ();

		JSONReports *jsonReports;

		PeakDetector* peakDetector = new PeakDetector ();

		bool reduceGroupsFlag = true;
		bool saveJsonEIC=false;
		bool uploadToPolly_bool = false;
		bool saveMzrollFile=true;
		string csvFileFieldSeparator=",";
		PeakGroup::QType quantitationType = PeakGroup::AreaTop;

		string clsfModelFilename = "default.model";

		/**
		* [process command line Options]
		* @param argc [argument counter]
		* @param argv [argument variables]
		*/
		void processOptions(int argc, char* argv[]);

		void createXMLFile(const char* fileName);

		/**
		* [handle loading of arguments using XML]
		*/
		void processXML(const char* fileName);

		/**
		* [Load Arguments for Options Dialog]
		* @param optionsArgs [pugixml xml node]
		*/
		void processOptionsArgsXML(xml_node& optionsArgs);

		/**
		* [Load Arguments for Peaks Dialog]
		* @param peaksArgs [pugixml xml node]
		*/
		void processPeaksArgsXML(xml_node& peaksArgs);

		/**
		* [Load general Arguments]
		* @param generalArgs [pugixml xml node]
		*/
		void processGeneralArgsXML(xml_node& generalArgs);

		/**
		* [load Classfication model]
		* @param clsfModelFilename [name of classifier model]
		*/
		void loadClassificationModel(string clsfModelFilename);


		void loadCompoundsFile();

		/**
		* [loadSamples description]
		* @param filenames [description]
		*/
		void loadSamples(vector<string>&filenames);

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
		void writeReport(string setName,QString jsPath,QString nodePath);

		void groupReduction();

		void saveJson(string setName);

		void saveMzRoll(string setName);

		void saveCSV(string setName);

		/**
		 * [Uploads Maven data to Polly and redirects the user to polly]
		 * @param jspath  [path to index.js file]
		 * @param nodepath  [path to node executable]
		 * @param filenames [List of files to be uploaded on polly]
		*/
		QString UploadToPolly(QString jsPath,QString nodePath,QStringList filenames);
		
		/**
		* [write Sample List in XML]
		* @param parent [parent node]
		*/
		void writeSampleListXML(xml_node& parent);

		/**
		* [write Peak Table in XML]
		* @param filename [name of the file]
		*/
		void writePeakTableXML(string filename);

		/**
		* [write Group information in XML]
		* @param parent [parent ion]
		* @param g      [peak group]
		*/
		void writeGroupXML(xml_node& parent, PeakGroup* g);

		/**
		* [write Parameters of ion in XML]
		* @param parent [parent ion]
		*/
		void writeParametersXML(xml_node& parent);
		// bool addPeakGroup(PeakGroup& grp);

		vector<mzSlice*> getSrmSlices();

		/**
		* [clean Sample Name]
		* @param  sampleName [name of the sample]
		* @return            [cleaned name]
		*/
		string cleanSampleName(string sampleName);

		/**
		* [get EICs from a group]
		* @param rtmin [minimum retention time]
		* @param rtmax [maximum retention time]
		* @param grp [the peak group]
		*/
		vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp);

};

struct Arguments {

	QStringList generalArgs;
	QStringList peakDialogArgs;
	QStringList optionsDialogArgs;

	void populateArgs() {
		generalArgs << "int" << "alignSamples" << "0";
		generalArgs << "int" << "saveEicJson" << "0";
		generalArgs << "string" << "outputdir" << "0";
		generalArgs << "int" << "savemzroll" << "0";
		generalArgs << "string" << "samples" << "path/to/sample1";
		generalArgs << "string" << "samples" << "path/to/sample2";
		generalArgs << "string" << "samples" << "path/to/sample3";

		peakDialogArgs << "int" << "minGoodGroupCount" << "1";
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

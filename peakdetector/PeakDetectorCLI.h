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
#include "omp.h"


#include "parseOptions.h"
#include "options.h"
#include "mzSample.h"
#include "mzMassSlicer.h"
#include "parseOptions.h"
#include "../libmaven/databases.h"
#include "../libmaven/csvreports.h"
#include "../libmaven/PeakDetector.h"
#include "../libmaven/classifierNeuralNet.h"

#include <QtCore>

#include "pugixml.hpp"

using namespace std;
#ifdef OMP_PARALLEL
	#define getTime() omp_get_wtime()
#else 
	#define getTime() get_wall_time()
	#define omp_get_thread_num()  1
#endif

class PeakDetectorCLI {

	public: 
		vector<mzSample*> samples;
		vector<Compound*> compoundsDB;
		vector<PeakGroup> allgroups;
		MassCalculator mcalc;

		time_t startTime, curTime, stopTime;

		vector<string> filenames;

		Databases DB;

		MavenParameters* mavenParameters = new MavenParameters ();

		PeakDetector* peakDetector = new PeakDetector ();

		bool reduceGroupsFlag = true;
		bool saveJsonEIC=false;
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
		*/
		void writeReport(string setName);

		void groupReduction();

		void saveJson(string setName);

		void saveMzRoll(string setName);

		void saveCSV(string setName);

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
		* [save EICs Json]
		* @param filename [name of the file]
		*/
		void saveEICsJson(string filename);

		/**
		* [save EIC json]
		* @param out [the output file]
		* @param eic [the eic to be saved]
		*/
		void saveEICJson(ofstream& out, EIC* eic);

		/**
		* [get EICs from a group]
		* @param rtmin [minimum retention time]
		* @param rtmax [maximum retention time]
		* @param grp [the peak group]
		*/
		vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp);

};


struct GeneralArgs {
	int alignSamples = 0;
	int saveEicJson = 0;
	string outputdir;
	int savemzroll = 0;

};

struct PeakDialogArgs {
	int minGoodGroupCount = 1;
	int matchRtFlag = 0;
	string db;
	int processAllSlices = 0;
	int pullIsotopes = 0;
	float grouping_maxRtWindow = 0.5;
	float minGroupIntensity = 5000;
	float quantileIntensity = 0.0;
	string model;
	int eicMaxGroups = INT_MAX;
	float ppmMerge = 30;
	float minQuality = 0.5;
	float quantileQuality = 0.0;
	float rtStepSize = 20;
	int minPeakWidth = 1;
	int eicSmoothingWindow = 10;
	float minSignalBaseLineRatio = 2;

};

struct OptionsDialogArgs {
	int ionizationMode = -1;
	int charge = 1;
};

struct Arguments {

	GeneralArgs general;
	PeakDialogArgs peakDialog;
	OptionsDialogArgs optionsDialog;
	
};

double get_wall_time();
double get_cpu_time(); 

#endif
#include <qglobal.h>
#include <qstring.h>
#include <qstringlist.h>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include "../libmaven/classifier.h"
#include "../libmaven/mzUtils.h"
#include "../libmaven/PeakDetector.h"
#include "options.h"

PeakDetector* peakDetector;
#include "../libmaven/mzSample.h"
#include "mainwindow.h"

using namespace std;

//variables
vector<Compound*> compoundsDB;
vector<string> filenames;
vector<mzSample*> samples;

void processOptions(int argc, char* argv[]) {

	//command line options
	const char * optv[] = { "a?alignSamples", "b?minGoodPeakCount <int>",
			"c?matchRtFlag", "d?db <sting>", "e?processMassSlicesFlag",
			"g:grouping_maxRtWindow <float>", "h?help",
			"i:minGroupIntensity <float>", "m?model <string>",
			"l?list  <string>", "o?outputdir <string>", "p?ppmMerge <float>",
			"r?rtStepSize <float>", "q:minQuality <float>", "v?ver",
			"w?minPeakWidth <int>", "y:?eicSmoothingWindow <int>",
			"z:minSignalBaseLineRatio <float>",
			NULL };

	//parse input options
	Options opts(*argv, optv);
	OptArgvIter iter(--argc, ++argv);
	const char * optarg;

	while (const char optchar = opts(iter, optarg)) {
		switch (optchar) {
		case 'a':
			peakDetector->alignSamplesFlag = true;
			break;

		case 'b':
			peakDetector->minGoodPeakCount = atoi(optarg);

			break;
		case 'c':
			peakDetector->matchRtFlag = true;
			break;
		case 'e':
			peakDetector->processMassSlicesFlag = true;
			break;
		case 'h':
			opts.usage(cerr, "files ...");
			exit(0);
			break;
		case 'i':
			peakDetector->minGroupIntensity = atof(optarg);
			break;
		case 'y':
			peakDetector->eic_smoothingWindow = atoi(optarg);
			break;
		case 'g':
			peakDetector->grouping_maxRtWindow = atof(optarg);
			break;
		case 'w':
			peakDetector->minNoNoiseObs = atoi(optarg);
			break;
		case 'r':
			peakDetector->rtStepSize = atoi(optarg);
			break;
		case 'p':
			peakDetector->ppmMerge = atof(optarg);
			break;
		case 'q':
			peakDetector->minQuality = atof(optarg);
			break;
		case 'z':
			peakDetector->minSignalBaseLineRatio = atof(optarg);
			break;
		case 'o':
			peakDetector->setOutputDir(optarg + string(DIR_SEPARATOR_STR));
			break;
		case 'd':
			peakDetector->setDBLigandName(optarg);
			break;
		case 'm':
			peakDetector->setClassifierModelFile(optarg);
			break;
		default:
			break;
		}
	}

	cerr << "#Command:\t";
	for (int i = 0; i < argc; i++)
		cerr << argv[i] << " ";
	cerr << endl;

	if (iter.index() < argc) {
		for (int i = iter.index(); i < argc; i++)
			filenames.push_back(argv[i]);
	}
}

string cleanSampleName(string sampleName) {
	unsigned int pos = sampleName.find_last_of("/");
	if (pos != std::string::npos) {
		sampleName = sampleName.substr(pos + 1, sampleName.length());

	}

	pos = sampleName.find_last_of("\\");
	if (pos != std::string::npos) {
		sampleName = sampleName.substr(pos + 1, sampleName.length());
	}
	return sampleName;

}

void loadSamples(vector<string>&filenames) {
	cerr << "Loading samples" << endl;
	for (unsigned int i = 0; i < filenames.size(); i++) {
		cerr << "Loading " << filenames[i] << endl;
		mzSample* sample = new mzSample();
		sample->loadSample(filenames[i].c_str());
		sample->sampleName = cleanSampleName(filenames[i]);

		if (sample->scans.size() >= 1) {
			samples.push_back(sample);
			sample->summary();
		} else {
			if (sample != NULL) {
				delete sample;
				sample = NULL;
			}
		}
	}
	cerr << "loadSamples done: loaded " << samples.size() << " samples\n";
}

QStringList VectorString_to_QStringList(std::vector<std::string>& svector) {
	QStringList result;
	for (uint i = 0; i < svector.size(); i++) {
		result
				<< QString::fromUtf8(svector.at(i).c_str(),
						svector.at(i).size());
	}
	return result;
}

int main(int argc, char *argv[]) {

	// MainWindow* mainWindow = NULL;

	peakDetector = new PeakDetector();
	// samples = peakDetector->getSamples();

	//read command line options
	// processOptions(argc, argv);

	//load classification model
	cerr << "Loading classifiation model" << endl;

	peakDetector->getClassifier()->loadModel(
			peakDetector->getClassifierModelFile());

	// Database* dbase = new Database();
	// string ligandDB = peakDetector->getDBLigandName();
	//
	// //load compound list
	// if (!ligandDB.empty()) {
	// 	peakDetector->processMassSlicesFlag = false;
	// 	cerr << "Loading ligand database" << endl;
	//
	// 	dbase->loadCompoundCSVFile(ligandDB);
	// }

	//	QStringList files = VectorString_to_QStringList(filenames);

	loadSamples(filenames); //TODO convert to QStringList

	if (samples.size() == 0) {
		cerr << "Exiting .. nothing to process " << endl;
		exit(1);
	}

	//get retenation time resoluution
	peakDetector->avgScanTime = samples[0]->getAverageFullScanTime();

	//ionization
	if (samples[0]->getPolarity() == '+')
		peakDetector->ionizationMode = +1;

	//align samples
	if (samples.size() > 1 && peakDetector->alignSamplesFlag) {
		peakDetector->alignSamples();
	}

	//process compound list
	if (compoundsDB.size()) {
		peakDetector->processCompounds(compoundsDB, "compounds");
	}

	//procall all mass slices
	if (peakDetector->processMassSlicesFlag == true) {
		peakDetector->matchRtFlag = false;
		peakDetector->checkConvergance = true;
		peakDetector->processMassSlices();
	}

	//cleanup
	delete_all(samples);
	samples.clear();
	peakDetector->getAllGroups().clear();

	return 0;
}

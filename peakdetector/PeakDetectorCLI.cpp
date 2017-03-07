#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <limits.h>
#include <algorithm>

#include "mzSample.h"
#include "mzMassSlicer.h"
#include "options.h"
#include "omp.h"
#include "../libmaven/classifierNeuralNet.h"
#include <sys/time.h>

#include <QtCore>

#include "pugixml.hpp"
#include "../libmaven/PeakDetector.h"

using namespace std;
#ifdef OMP_PARALLEL
	#define getTime() omp_get_wtime()
#else 
	#define getTime() get_wall_time()
	#define omp_get_thread_num()  1
#endif

//variables
Classifier* clsf;
vector<mzSample*> samples;
vector<Compound*> compoundsDB;
vector<PeakGroup> allgroups;
MassCalculator mcalc;

time_t startTime, curTime, stopTime;

vector<string> filenames;

bool alignSamplesFlag = false;
bool processAllSlices = true;
bool reduceGroupsFlag = true;
bool matchRtFlag = true;
bool writeReportFlag = true;
bool checkConvergance = true;
bool saveJsonEIC=false;
bool saveMzrollFile=true;

bool pullIsotopesFlag = false;
bool C13Labeled_BPE =false;
bool N15Labeled_BPE =false;
bool S34Labeled_BPE =false;
bool D2Labeled_BPE =false;
int noOfIsotopes = 4;
string csvFileFieldSeparator=",";
PeakGroup::QType quantitationType = PeakGroup::AreaTop;

string ligandDbFilename;
string clsfModelFilename = "default.model";
string outputdir = "reports" + string(DIR_SEPARATOR_STR);

int ionizationMode = -1;

//mass slice detection
int rtStepSize = 10;
float ppmMerge = 30;
float avgScanTime = 0.2;

//peak detection
int eic_smoothingWindow = 5;

//maxGroupsOption
int eicMaxGroups=10;

//peak grouping across samples
float grouping_maxRtWindow = 0.5;

//peak filtering criteria
int minGoodGroupCount = 1;
float minSignalBlankRatio = 2;
int minNoNoiseObs = 3;
float minSignalBaseLineRatio = 2;
float minGroupIntensity = 100;
float minQuality = 0.5;


// Temporary variable for pullIsotopesFlag
int label =0;

/**
 * [process command line Options]
 * @param argc [argument counter]
 * @param argv [argument variables]
 */
void processOptions(int argc, char* argv[]);

/**
 * [loadSamples description]
 * @param filenames [description]
 */
void loadSamples(vector<string>&filenames);

/**
 * [loadCompoundCSVFile description]
 * @param filename [description]
 */
void loadCompoundCSVFile(string filename);


// void printSettings();

/**
 * [reduce number of Groups]
 */
void reduceGroups();

/**
 * [write multiple types of Reports]
 * @param setName [name of the set]
 */
void writeReport(string setName);

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
 * [write CSV Report]
 * @param filename [name of the file]
 */
void writeCSVReport(string filename);

/**
 * [write Group information in XML]
 * @param parent [parent ion]
 * @param g      [peak group]
 */
void writeGroupXML(xml_node& parent, PeakGroup* g);

void writeGroupInfoCSV(PeakGroup* group,  ofstream& groupReport);

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

double get_wall_time();
double get_cpu_time(); 

MavenParameters* mavenParameters = new MavenParameters ();

PeakDetector* peakDetector = new PeakDetector ();

int main(int argc, char *argv[]) {

    double programStartTime = getTime();
	//read command line options
	processOptions(argc, argv);

	//load classification model
	cerr << "Loading classifiation model" << endl;
	cerr << "clsfModelFilename " << clsfModelFilename << endl;
	clsf = new ClassifierNeuralNet();
	clsf->loadModel(clsfModelFilename);
	mavenParameters->clsf = clsf;

	peakDetector->setMavenParameters(mavenParameters);


	//load compound list
	if (!mavenParameters->ligandDbFilename.empty()) {
		mavenParameters->processAllSlices = false;
		cerr << "Loading ligand database" << endl;
		loadCompoundCSVFile(mavenParameters->ligandDbFilename);
	}

	//load files
    double startLoadingTime = getTime();
	loadSamples(filenames);

	cerr << "Execution time (Sample loading) : %f seconds \n", getTime() - startLoadingTime;

	if (mavenParameters->samples.size() == 0) {
		cerr << "Exiting .. nothing to process " << endl;
		exit(1);
	}

	//get retention time resolution
	mavenParameters->setAverageScanTime();

	//ionization
	mavenParameters->setIonizationMode();

	//align samples
	if (mavenParameters->samples.size() > 1 && mavenParameters->alignSamplesFlag){
		peakDetector->alignSamples();
	}
	

	//process compound list
	if (mavenParameters->compounds.size()) {
		vector<mzSlice*> slices = peakDetector->processCompounds(
				mavenParameters->compounds, "compounds");
		peakDetector->processSlices(slices, "compounds");

		writeReport("compounds");
		delete_all(slices);
	}

	//process all mass slices
	if (mavenParameters->processAllSlices == true) {
		mavenParameters->matchRtFlag = false;
		mavenParameters->checkConvergance = true;
		peakDetector->processMassSlices();
	}

	//cleanup
	delete_all(mavenParameters->samples);
	mavenParameters->samples.clear();
	mavenParameters->allgroups.clear();

    cerr << "Total program execution time : %f seconds \n",getTime() - programStartTime;
	return(0);
}

// vector<mzSlice*> getSrmSlices() {
// 	map<string, int> uniqSrms;
// 	vector<mzSlice*> slices;
// 	for (int i = 0; i < samples.size(); i++) {
// 		for (int j = 0; j < samples[i]->scans.size(); j++) {
// 			Scan* scan = samples[i]->scans[j];
// 			string filterLine = scan->filterLine;
// 			if (uniqSrms.count(filterLine) == 0) {
// 				uniqSrms[filterLine] = 1;
// 				slices.push_back(new mzSlice(filterLine));
// 			}
// 		}
// 	}
// 	cerr << "getSrmSlices() " << slices.size() << endl;
// 	return slices;
// }

void saveEICsJson(string filename) {
	ofstream myfile(filename.c_str());
	if (!myfile.is_open()) return;
    myfile << "[\n";
	
	for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
		PeakGroup& grp = mavenParameters->allgroups[i];
		float rtmin = grp.minRt - 3;
		float rtmax = grp.maxRt + 3;

		myfile << "{\n";
		myfile << "\"groupId\": " << i << "," << endl;
		myfile << "\"rtmin\": " << rtmin << "," << endl;
		myfile << "\"rtmax\": " << rtmax << "," << endl;
		myfile << "\"eics\": [ " << endl;
		vector<EIC*> eics = getEICs(rtmin, rtmax, grp); //get EICs

		for(int j=0; j < (eics.size() / grp.peaks.size()); j++ ) {
				myfile << "{\n";
				saveEICJson(myfile, eics[j] ); //save EICs
				myfile << "}\n";
				if ( j < ((eics.size() / grp.peaks.size())-1)) myfile << ",\n";
		}
		myfile << "]" << endl;
		myfile << "}" << endl;
        
        if (i != (mavenParameters->allgroups.size()-1)){
            myfile << ",";        	
        }

		delete_all(eics); //cleanup
	}
	myfile << "]";

	myfile.close();
}

void saveEICJson(ofstream& out, EIC* eic) {
	int N = eic->rt.size();
	int count = 0;

		out << "\"label\":" << "\"" << eic->getSample()->sampleName << "\"," << endl;
		out << "\"data\": [";
		out << setprecision(4);
		for(int i=0; i<N; i++) { 
				if (eic->intensity[i]>0) {
					  if(count && i<N) out << ",";
					  out << "[" << eic->rt[i] << "," <<  eic->intensity[i] << "]"; 
					  count++;
				};
		}
		out << "]\n"; 
}

vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp) {
	vector<EIC*> eics;
	for (int i = 0; i < grp.peaks.size(); i++) {
		float mzmin = grp.meanMz - 0.2;
		float mzmax = grp.meanMz + 0.2;
		//cerr <<setprecision(5) << "getEICs: mz:" << mzmin << "-" << mzmax << " rt:" << rtmin << "-" << rtmax << endl;

		for (unsigned int j = 0; j < samples.size(); j++) {
			if (!grp.srmId.empty()) {
				EIC* eic = mavenParameters->samples[j]->getEIC(grp.srmId);
				eics.push_back(eic);
			} else {
				EIC* eic = samples[j]->getEIC(mzmin, mzmax, rtmin, rtmax, 1);
				eics.push_back(eic);
			}
		}
	}
	return (eics);
}

void processOptions(int argc, char* argv[]) {

	//command line options
	const char * optv[] = {
							"a?alignSamples <int>",
							"b?minGoodGroupCount <int>",
							"c?matchRtFlag <int>",
							"d?db <string>",
							"e?processAllSlices",
							"f?pullIsotopes <int>",				//C13(1st bit), S34i(2nd bit), N15i(3rd bit), D2(4th bit)
							"g:grouping_maxRtWindow <float>",
							"h?help",
							"i?minGroupIntensity <float>",
							"m?model <string>",
							"n?eicMaxGroups <int>",
							"l?list  <string>",
							"o?outputdir <string>",
							"p?ppmMerge <float>",
							"r?rtStepSize <float>",
                            "s?savemzroll <int>",
							"q:minQuality <float>",
                            "v?ionizationMode <int>",
							"w?minPeakWidth <int>",
							"y?eicSmoothingWindow <int>",
							"z:minSignalBaseLineRatio <float>",
							NULL 
	};

	//parse input options
	Options opts(*argv, optv);
	OptArgvIter iter(--argc, ++argv);
	const char * optarg;

	while (const char optchar = opts(iter, optarg)) {
		switch (optchar) {

		case 'a':
			mavenParameters->alignSamplesFlag = true;
			if (atoi(optarg) == 0) mavenParameters->alignSamplesFlag = false;
			break;
        case 'v' : 
                    mavenParameters->ionizationMode = atoi(optarg);
        case 'f' :
                    mavenParameters->pullIsotopesFlag = 0;
                    label = atoi(optarg);
                    if (label > 0) {
                        mavenParameters->pullIsotopesFlag = 1;
                        if (label & 1) mavenParameters->C13Labeled_BPE = true; 
                        if (label & 2) mavenParameters->S34Labeled_BPE = true; 
                        if (label & 4) mavenParameters->N15Labeled_BPE = true; 
                        if (label & 8) mavenParameters->D2Labeled_BPE  = true; 
                    }
                    
                    break;
		case 'b':
			mavenParameters->minGoodGroupCount = atoi(optarg);
			break;

		case 'c':
        	mavenParameters->compoundRTWindow=atof(optarg);
            mavenParameters->matchRtFlag=true;
            if(mavenParameters->compoundRTWindow==0) mavenParameters->matchRtFlag=false;
            break;

		case 'e':
			mavenParameters->processAllSlices = atoi(optarg);
			break;

		case 'h':
			opts.usage(cerr, "files ...");
			exit(0);
			break;

		case 'i':
			mavenParameters->minGroupIntensity = atof(optarg);
			break;

        case 'j' :
        	saveJsonEIC=atoi(optarg);
            break;

        case 's':
        	saveMzrollFile=atoi(optarg);

		case 'y':
			mavenParameters->eic_smoothingWindow = atoi(optarg);
			break;

        case 'n' :
        	mavenParameters->eicMaxGroups = atoi(optarg);

		case 'g':
			mavenParameters->grouping_maxRtWindow = atof(optarg);
			break;

		case 'w':
			mavenParameters->minNoNoiseObs = atoi(optarg);
			break;

		case 'r':
			mavenParameters->rtStepSize = atoi(optarg);
			break;

		case 'p':
			mavenParameters->ppmMerge = atof(optarg);
			break;

		case 'q':
			mavenParameters->minQuality = atof(optarg);
			break;

		case 'z':
			mavenParameters->minSignalBaseLineRatio = atof(optarg);
			break;

		case 'o':
			mavenParameters->outputdir = optarg + string(DIR_SEPARATOR_STR);
			break;

		case 'd':
			mavenParameters->ligandDbFilename = optarg;
			break;

		case 'm':
			clsfModelFilename = optarg;
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

void loadSamples(vector<string>&filenames) {
	cerr << "Loading samples" << endl;
	for (unsigned int i = 0; i < filenames.size(); i++) {
		cerr << "Loading " << filenames[i] << endl;
		mzSample* sample = new mzSample();
		sample->loadSample(filenames[i].c_str());
		sample->sampleName = cleanSampleName(filenames[i]);

		if (sample->scans.size() >= 1) {
			mavenParameters->samples.push_back(sample);
			sample->summary();
		} else {
			if (sample != NULL) {
				delete sample;
				sample = NULL;
			}
		}
	}
	cerr << "loadSamples done: loaded " << mavenParameters->samples.size() << " samples\n";
}

string cleanSampleName(string sampleName) {
        QString out(sampleName.c_str());
        out.replace(QRegExp(".*/"),"");
        out.replace(QRegExp(".*\\"),"");

        QStringList fileExtensions = QStringList()
                <<"\\.mzCSV$"<<"\\.mzdata$"<<"\\.mzXML$"<<"\\.mzML$"<<"\\.mz5$"<<"\\.pepXML$"<<"\\.xml$"<<"\\.cdf$"<<"\\.raw$";
        //copied from maven_stable/mzfileio.cpp

        Q_FOREACH (const QString& s, fileExtensions) {
            out.replace(QRegExp(s,Qt::CaseInsensitive),"");
        }
        return out.toStdString();
}

void loadCompoundCSVFile(string filename){

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0;
    int lineCount=0;
    map<string, int>header;
    vector<string> headers;
    MassCalculator mcalc;

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    string sep= "\t";
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=",";
    while ( getline(myfile,line) ) {
        if (!line.empty() && line[0] == '#') continue;
        if (lineCount == 0 and line.find("\t") != std::string::npos) { sep ='\t'; }

        //trim spaces on the left
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        lineCount++;

        vector<string>fields;
        mzUtils::splitNew(line, sep, fields);

        for(unsigned int i=0; i < fields.size(); i++ ) {
            int n = fields[i].length();
            if (n>2 && fields[i][0] == '"' && fields[i][n-1] == '"') {
                fields[i]= fields[i].substr(1,n-2);
            }
            if (n>2 && fields[i][0] == '\'' && fields[i][n-1] == '\'') {
                fields[i]= fields[i].substr(1,n-2);
            }
        }

        if (lineCount==1) {
            headers = fields;
            for(unsigned int i=0; i < fields.size(); i++ ) {
                fields[i]=makeLowerCase(fields[i]);
                header[ fields[i] ] = i;
            }
            continue;
        }

        string id, name, formula;
        float rt=0;
        float mz=0;
        float charge=0;
        float collisionenergy=0;
        float precursormz=0;
        float productmz=0;
        int N=fields.size();
        vector<string>categorylist;


        if ( header.count("mz") && header["mz"]<N)  mz = string2float(fields[ header["mz"]]);
        if ( header.count("rt") && header["rt"]<N)  rt = string2float(fields[ header["rt"]]);
        if ( header.count("expectedrt") && header["expectedrt"]<N) rt = string2float(fields[ header["expectedrt"]]);
        if ( header.count("charge")&& header["charge"]<N) charge = string2float(fields[ header["charge"]]);
        //Spelling fixed : formala > formula
        if ( header.count("formula")&& header["formula"]<N) formula = fields[ header["formula"] ];
        if ( header.count("id")&& header["id"]<N) 	 id = fields[ header["id"] ];
        if ( header.count("name")&& header["name"]<N) 	 name = fields[ header["name"] ];
        if ( header.count("compound")&& header["compound"]<N) 	 name = fields[ header["compound"] ];

        if ( header.count("precursormz") && header["precursormz"]<N) precursormz=string2float(fields[ header["precursormz"]]);
        if ( header.count("productmz") && header["productmz"]<N)  productmz = string2float(fields[header["productmz"]]);
        if ( header.count("collisionenergy") && header["collisionenergy"]<N) collisionenergy=string2float(fields[ header["collisionenergy"]]);

        if ( header.count("Q1") && header["Q1"]<N) precursormz=string2float(fields[ header["Q1"]]);
        if ( header.count("Q3") && header["Q3"]<N)  productmz = string2float(fields[header["Q3"]]);
        if ( header.count("CE") && header["CE"]<N) collisionenergy=string2float(fields[ header["CE"]]);

        //cout << lineCount << " " << endl;
        //for(int i=0; i<headers.size(); i++) cout << headers[i] << ", ";
        //cout << "   -> category=" << header.count("category") << endl;
        if ( header.count("category") && header["category"]<N) {
            string catstring=fields[header["category"]];
            if (!catstring.empty()) {
                mzUtils::split(catstring,';', categorylist);
                if(categorylist.size() == 0) categorylist.push_back(catstring);
                //cout << catstring << " ListSize=" << categorylist.size() << endl;
            }
         }

        if ( header.count("polarity") && header["polarity"] <N)  {
            string x = fields[ header["polarity"]];
            if ( x == "+" ) {
                charge = 1;
            } else if ( x == "-" ) {
                charge = -1;
            } else  {
                charge = string2float(x);
            }

		}



        //cout << "Loading: " << id << " " << formula << "mz=" << mz << " rt=" << rt << " charge=" << charge << endl;

        if (mz == 0) mz=precursormz;
        if (id.empty()&& !name.empty()) id=name;
        if (id.empty() && name.empty()) id="cmpd:" + integer2string(loadCount);

        if ( mz > 0 || ! formula.empty() ) {
            Compound* compound = new Compound(id,name,formula,charge);

            compound->expectedRt = rt;

            if (mz == 0) mz = mcalc.computeMass(formula,charge);
            compound->mass = mz;

            //cout << "Computed Mass" << compound->mass << compound->name << compound->formula << endl;
            //cout << "Test";
            compound->db = dbname;
            compound->expectedRt=rt;
            compound->precursorMz=precursormz;
            compound->productMz=productmz;
            compound->collisionEnergy=collisionenergy;
            for(int i=0; i < categorylist.size(); i++) compound->category.push_back(categorylist[i]);
			mavenParameters->compounds.push_back(compound);
			loadCount++;
		}
	}
	sort(mavenParameters->compounds.begin(), mavenParameters->compounds.end(), Compound::compMass);
    cout << "Loading compound list from file:" << dbname << " Found: " << loadCount << " compounds" << endl;
	myfile.close();
}

//
// bool addPeakGroup(PeakGroup& grup1) {
//
// 	for (unsigned int i = 0; i < allgroups.size(); i++) {
// 		PeakGroup& grup2 = allgroups[i];
// 		float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt,
// 				grup2.minRt, grup2.maxRt);
// 		if (rtoverlap > 0.9 && ppmDist(grup1.meanMz, grup2.meanMz) < ppmMerge
// 				&& grup1.maxIntensity < grup2.maxIntensity) {
// 			return false;
// 		}
// 	}
//
// 	//cerr << "\t\t accepting " << grup1.meanMz << "@" << grup1.meanRt;
// 	allgroups.push_back(grup1);
// 	return true;
// }

void reduceGroups() {
	sort(mavenParameters->allgroups.begin(), mavenParameters->allgroups.end(), PeakGroup::compMz);
	cerr << "reduceGroups(): " << mavenParameters->allgroups.size();
	//init deleteFlag 
	for(unsigned int i=0; i<mavenParameters->allgroups.size(); i++) {
			mavenParameters->allgroups[i].deletedFlag=false;
	}

	for(unsigned int i=0; i<mavenParameters->allgroups.size(); i++) {
		PeakGroup& grup1 = mavenParameters->allgroups[i];
        if(grup1.deletedFlag) continue;
		for(unsigned int j=i+1; j<mavenParameters->allgroups.size(); j++) {
			PeakGroup& grup2 = mavenParameters->allgroups[j];
            if( grup2.deletedFlag) continue;

			float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt, grup2.minRt, grup2.maxRt );
			float ppmdist = ppmDist(grup2.meanMz, grup1.meanMz);
		    if ( ppmdist > ppmMerge ) break;

			if (rtoverlap > 0.8 && ppmdist < ppmMerge) {
				if (grup1.maxIntensity <= grup2.maxIntensity) {
                     grup1.deletedFlag = true;
					 //allgroups.erase(allgroups.begin()+i);
					 //i--;
					 break;
				} else if ( grup1.maxIntensity > grup2.maxIntensity) {
                     grup2.deletedFlag = true;
					 //allgroups.erase(allgroups.begin()+j);
					 //i--;
					// break;
				}
			}
		}
	}
    int reducedGroupCount = 0;
    vector<PeakGroup> allgroups_;
    for(int i=0; i <mavenParameters->allgroups.size(); i++)
    {
        PeakGroup& grup1 = mavenParameters->allgroups[i];
        if(grup1.deletedFlag == false)
        {
            allgroups_.push_back(grup1);
            reducedGroupCount++;
        }
    }
    printf("Reduced count of groups : %d \n",  reducedGroupCount);
    mavenParameters->allgroups = allgroups_;
	cerr << " done final group count(): " << mavenParameters->allgroups.size() << endl;
}

void writeReport(string setName) {


	//create an output folder
	mzUtils::createDir(mavenParameters->outputdir.c_str());

	cerr << "writeReport() " << mavenParameters->allgroups.size() << " groups ";
    double startGroupReduction = getTime();
	if (reduceGroupsFlag) reduceGroups();
    cerr << "\tExecution time (Group reduction) : %f seconds \n", getTime() - startGroupReduction;
	if (saveJsonEIC)      saveEICsJson(outputdir + setName + ".eics.json");
	if (saveMzrollFile == true)
	{
		double startSavingMzroll = getTime();
		writePeakTableXML(mavenParameters->outputdir + setName + ".mzroll");
		printf("\tExecution time (Saving mzroll)   : %f seconds \n", getTime() - startSavingMzroll);
	
    }
    double startSavingCSV = getTime();
	writeCSVReport(mavenParameters->outputdir + setName + ".csv");
    printf("\tExecution time (Saving CSV)      : %f seconds \n", getTime() - startSavingCSV);
}

void writeCSVReport( string filename) {
    ofstream groupReport;
    groupReport.open(filename.c_str());
    if(! groupReport.is_open()) return;

    int groupId=0;
    int metaGroupId=0;
    string SEP = csvFileFieldSeparator;


    groupReport << "label,metaGroupId,groupId,goodPeakCount,medMz,medRt,maxQuality,note,compound,compoundId,expectedRtDiff,ppmDiff,parent";
    for(unsigned int i=0; i< mavenParameters->samples.size(); i++) { groupReport << "," << mavenParameters->samples[i]->sampleName; }
    groupReport << endl;

    for (int i=0; i < mavenParameters->allgroups.size(); i++ ) {
        PeakGroup* group = &mavenParameters->allgroups[i];

        //if compound is unknown, output only the unlabeled form information
        if( group->compound == NULL || group->childCount() == 0 ) {
            group->groupId= ++groupId;
            group->metaGroupId= ++metaGroupId;
            writeGroupInfoCSV(group,groupReport);
        } else { //output all relevant isotope info otherwise
            group->groupId = group->children[0].groupId = ++groupId;
            group->metaGroupId = group->children[0].metaGroupId= ++metaGroupId;
            writeGroupInfoCSV( &group->children[0],groupReport); //C12 info

            string formula = group->compound->formula;
            vector<Isotope> masslist = MassCalculator::computeIsotopes(formula, mavenParameters->ionizationMode, 
												            mavenParameters->isotopeAtom, mavenParameters->noOfIsotopes);
            for( int i=0; i<masslist.size(); i++ ) {
            Isotope& x = masslist[i];
            string isotopeName = x.name;

            if( 
                    (isotopeName.find("C13-label")!=string::npos  && mavenParameters->C13Labeled_BPE) || 
                    (isotopeName.find("N15-label")!=string::npos  && mavenParameters->N15Labeled_BPE) || 
                    (isotopeName.find("S34-label")!=string::npos  && mavenParameters->S34Labeled_BPE) || 
                    (isotopeName.find("D2-label")!=string::npos  && mavenParameters->D2Labeled_BPE)  
                    
            ) {
                int counter=0;
                    for (unsigned int k=0; k<group->children.size() && counter==0; k++) { //output non-zero-intensity peaks
                        PeakGroup* subgroup = &group->children[k];
                        if( subgroup->tagString == isotopeName ) {
                            subgroup->metaGroupId = group->metaGroupId;
                            subgroup->groupId= ++groupId;
                            counter=1;
                            writeGroupInfoCSV(subgroup,groupReport);
                        }
                    }
                }
            }
        }
    }

    if(groupReport.is_open()) groupReport.close();
}

QString sanitizeString(const char* s,const char* SEP) {
    QString out=s;
    out.replace("\"","\"\"");
    if(out.contains(SEP)){
        out="\""+out+"\"";
    }
    return out;
}

void writeGroupInfoCSV(PeakGroup* group,  ofstream& groupReport) {
    if(! groupReport.is_open()) return;
    string SEP = csvFileFieldSeparator;
    PeakGroup::QType qtype = quantitationType;
    vector<float> yvalues = group->getOrderedIntensityVector(mavenParameters->samples,qtype);
    //if ( group->metaGroupId == 0 ) { group->metaGroupId=groupId; }

    string tagString = group->srmId + group->tagString;
    tagString=sanitizeString(tagString.c_str(),SEP.c_str()).toStdString();
    char label[2];
    sprintf(label,"%c",group->label);

    groupReport << label << SEP
                << setprecision(7)
                << group->metaGroupId << SEP
                << group->groupId << SEP
                << group->goodPeakCount << SEP
                << group->meanMz << SEP
                << group->meanRt << SEP
                << group->maxQuality << SEP
                << tagString;

    string compoundName;
    string compoundID;
    float  expectedRtDiff=1000;
    float  ppmDist=1000;

    if ( group->hasCompoundLink() ) {
        Compound* c = group->compound;
        compoundID = c->id;
        if (group->tagString.length()) { 
            cout << "TRUE" << group->tagString.c_str() << group->tagString.length() << endl;
            compoundID = compoundID + " [" + group->tagString.c_str() + "]";
        }

        compoundID = sanitizeString(compoundID.c_str(),SEP.c_str()).toStdString();
    }

    if ( group->compound != NULL or group->parent != NULL ) {
        Compound* c = group->compound;
        compoundName = sanitizeString(c->name.c_str(),SEP.c_str()).toStdString();
        //compoundID =  c->id;
        double mass =c->mass;
        if (!c->formula.empty()) {
            float formula_mass =  mcalc.computeMass(c->formula,mavenParameters->ionizationMode);
            if(formula_mass) mass=formula_mass;
        }
        ppmDist = mzUtils::ppmDist(mass,(double) group->meanMz);
        expectedRtDiff = c->expectedRt-group->meanRt;

    }

    groupReport << SEP << compoundName;
    groupReport << SEP << compoundID;
    
    Compound* c = group->compound;
    if (c->expectedRt == 0 or c->expectedRt == NULL){ // If there is 0 or unspecified rt value
        groupReport << SEP;                           // then the expectedRtDiff field would
    }                                                 // be empty (blank)
    else{
        groupReport << SEP << expectedRtDiff;
    }

    groupReport << SEP << ppmDist;

    if ( group->parent != NULL ) {
        groupReport << SEP << group->parent->meanMz;
    } else {
        groupReport << SEP << group->meanMz;
    }

    for( unsigned int j=0; j < mavenParameters->samples.size(); j++) groupReport << SEP <<  yvalues[j];
    groupReport << endl;

    /*for (unsigned int k=0; k < group->children.size(); k++) {
        group->children[k].metaGroupId = group->metaGroupId;
        writeGroupInfoCSV(&group->children[k]);
        //writePeakInfo(&group->children[k]);
    }*/
}


void writeSampleListXML(xml_node& parent) {
	xml_node samplesset = parent.append_child();
	samplesset.set_name("samples");

	for (int i = 0; i < samples.size(); i++) {
		xml_node _sample = samplesset.append_child();
		_sample.set_name("sample");
		_sample.append_attribute("name") = samples[i]->sampleName.c_str();
		_sample.append_attribute("filename") = samples[i]->fileName.c_str();
		_sample.append_attribute("sampleOrder") = i;
		_sample.append_attribute("setName") = "A";
		_sample.append_attribute("sampleName") = samples[i]->sampleName.c_str();
	}
}

void writePeakTableXML(std::string filename) {

	xml_document doc;
	doc.append_child().set_name("project");
	xml_node peak = doc.child("project");
	peak.append_attribute("Program") = "peakdetector";
	peak.append_attribute("Version") = "Mar112012";

	writeSampleListXML(peak);
	writeParametersXML(peak);

	if (mavenParameters->allgroups.size()) {
		peak.append_child().set_name("PeakGroups");
		xml_node peakgroups = peak.child("PeakGroups");
		for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
			writeGroupXML(peakgroups, &mavenParameters->allgroups[i]);
		}
	}

	doc.save_file(filename.c_str());
}

void writeParametersXML(xml_node& parent) {

	xml_node p = parent.append_child();
	p.set_name("PeakDetectionParameters");
	p.append_attribute("alignSamples") = mavenParameters->alignSamplesFlag;
	p.append_attribute("matchRt") = mavenParameters->matchRtFlag;
	p.append_attribute("ligandDbFilename") =
			mavenParameters->ligandDbFilename.c_str();
	p.append_attribute("clsfModelFilename") = clsfModelFilename.c_str();
	p.append_attribute("rtStepSize") = mavenParameters->rtStepSize;
	p.append_attribute("ppmMerge") = mavenParameters->ppmMerge;
	p.append_attribute("eic_smoothingWindow") =
			mavenParameters->eic_smoothingWindow;
	p.append_attribute("grouping_maxRtWindow") =
			mavenParameters->grouping_maxRtWindow;
	p.append_attribute("minGoodGroupCount") = mavenParameters->minGoodGroupCount;
	p.append_attribute("minSignalBlankRatio") =
			mavenParameters->minSignalBlankRatio;
	p.append_attribute("minPeakWidth") = mavenParameters->minNoNoiseObs;
	p.append_attribute("minSignalBaseLineRatio") =
			mavenParameters->minSignalBaseLineRatio;
	p.append_attribute("minGroupIntensity") = mavenParameters->minGroupIntensity;
	p.append_attribute("minQuality") = mavenParameters->minQuality;
}

void writeGroupXML(xml_node& parent, PeakGroup* g) {
	if (!g)
		return;

	xml_node group = parent.append_child();
	group.set_name("PeakGroup");

	group.append_attribute("groupId") = g->groupId;
	group.append_attribute("tagString") = g->tagString.c_str();
	group.append_attribute("metaGroupId") = g->metaGroupId;
	group.append_attribute("expectedRtDiff") = g->expectedRtDiff;
	group.append_attribute("groupRank") = (float) g->groupRank;
	group.append_attribute("label") = g->label;
	group.append_attribute("type") = (int) g->type();
	group.append_attribute("changeFoldRatio") = (int) g->changeFoldRatio;
	group.append_attribute("changePValue") = (int) g->changePValue;
	if (g->srmId.length())
		group.append_attribute("srmId") = g->srmId.c_str();

	if (g->hasCompoundLink()) {
		Compound* c = g->compound;
		group.append_attribute("compoundId") = c->id.c_str();
		group.append_attribute("compoundDB") = c->db.c_str();
	}

	for (int j = 0; j < g->peaks.size(); j++) {
		Peak& p = g->peaks[j];
		xml_node peak = group.append_child();
		peak.set_name("Peak");
		peak.append_attribute("pos") = ((int) p.pos);
		peak.append_attribute("minpos") = (int) p.minpos;
		peak.append_attribute("maxpos") = (int) p.maxpos;
		peak.append_attribute("rt") = p.rt;
		peak.append_attribute("rtmin") = p.rtmin;
		peak.append_attribute("rtmax") = p.rtmax;
		peak.append_attribute("mzmin") = p.mzmin;
		peak.append_attribute("mzmax") = p.mzmax;
		peak.append_attribute("scan") = (int) p.scan;
		peak.append_attribute("minscan") = (int) p.minscan;
		peak.append_attribute("maxscan") = (int) p.maxscan;
		peak.append_attribute("peakArea") = p.peakArea;
		peak.append_attribute("peakAreaCorrected") = p.peakAreaCorrected;
		peak.append_attribute("peakAreaTop") = p.peakAreaTop;
		peak.append_attribute("peakAreaFractional") = p.peakAreaFractional;
		peak.append_attribute("peakRank") = p.peakRank;
		peak.append_attribute("peakIntensity") = p.peakIntensity;
		;
		peak.append_attribute("peakBaseLineLevel") = p.peakBaseLineLevel;
		peak.append_attribute("peakMz") = p.peakMz;
		peak.append_attribute("medianMz") = p.medianMz;
		peak.append_attribute("baseMz") = p.baseMz;
		peak.append_attribute("quality") = p.quality;
		peak.append_attribute("width") = (int) p.width;
		peak.append_attribute("gaussFitSigma") = p.gaussFitSigma;
		peak.append_attribute("gaussFitR2") = p.gaussFitR2;
		peak.append_attribute("groupNum") = p.groupNum;
		peak.append_attribute("noNoiseObs") = (int) p.noNoiseObs;
		peak.append_attribute("noNoiseFraction") = p.noNoiseFraction;
		peak.append_attribute("symmetry") = p.symmetry;
		peak.append_attribute("signalBaselineRatio") = p.signalBaselineRatio;
		peak.append_attribute("groupOverlap") = p.groupOverlap;
		peak.append_attribute("groupOverlapFrac") = p.groupOverlapFrac;
		peak.append_attribute("localMaxFlag") = p.localMaxFlag;
		peak.append_attribute("fromBlankSample") = p.fromBlankSample;
		peak.append_attribute("label") = p.label;
		peak.append_attribute("sample") = p.getSample()->sampleName.c_str();
	}

	if (g->childCount()) {
		for (int i = 0; i < g->children.size(); i++) {
			PeakGroup* child = &(g->children[i]);
			writeGroupXML(group, child);
		}
	}
}
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
    return (double)getTime() / CLOCKS_PER_SEC;
}

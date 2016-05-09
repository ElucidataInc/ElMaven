#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <limits.h>

#include "mzSample.h"
#include "mzMassSlicer.h"
#include "options.h"
#include "../mzroll/classifierNeuralNet.h"
#include "pugixml.hpp"

//#include "yasper.h"
//using yasper::ptr; 

using namespace std;

//variables
ClassifierNeuralNet clsf;
vector <mzSample*> samples;
vector <Compound*> compoundsDB;
vector<PeakGroup> allgroups;
MassCalculator mcalc;

time_t startTime, curTime, stopTime;

vector<string>filenames;

bool alignSamplesFlag=false;
bool processAllSlices=true;
bool reduceGroupsFlag=true;
bool matchRtFlag = true;
bool writeReportFlag=true;
bool checkConvergance=true;

string ligandDbFilename;
string clsfModelFilename = "default.model";
string outputdir = "reports" + string(DIR_SEPARATOR_STR);

int  ionizationMode = -1;

//mass slice detection
int   rtStepSize=10;
float ppmMerge=30;
float avgScanTime=0.2;

//peak detection
int eic_smoothingWindow = 5;

//peak grouping across samples
float grouping_maxRtWindow=0.5;

//peak filtering criteria
int minGoodGroupCount=1;
float minSignalBlankRatio=2;
int minNoNoiseObs=3;
float minSignalBaseLineRatio=2;
float minGroupIntensity=100;
float minQuality = 0.5;


void processOptions(int argc, char* argv[]);
void loadSamples(vector<string>&filenames);
void loadCompoundCSVFile(string filename);
void printSettings();
void alignSamples();
void processSlices(vector<mzSlice*>&slices,string setName);
void processCompounds(vector<Compound*> set, string setName);
void processMassSlices(int limit=0);
void reduceGroups();
void writeReport(string setName);
void writeSampleListXML(xml_node& parent);
void writePeakTableXML(string filename);
void writeCSVReport(string filename);
void writeGroupXML(xml_node& parent, PeakGroup* g);
void writeParametersXML(xml_node& parent);
bool addPeakGroup(PeakGroup& grp);
vector<mzSlice*> getSrmSlices();
string cleanSampleName( string sampleName);

void saveEICs(string filename);
void saveEIC(ofstream& out, EIC* eic );
vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp);

int main(int argc, char *argv[]) {

	//read command line options
	processOptions(argc,argv);

	//load classification model
	cerr << "Loading classifiation model" << endl;
	clsf.loadModel(clsfModelFilename);

	//load compound list
	if (!ligandDbFilename.empty()) {
		processAllSlices=false;
		cerr << "Loading ligand database" << endl;
		loadCompoundCSVFile(ligandDbFilename);
	}

	//load files
	loadSamples(filenames);
	if (samples.size() == 0 ) { cerr << "Exiting .. nothing to process " << endl;  exit(1); }

	//get retenation time resoluution
	avgScanTime = samples[0]->getAverageFullScanTime();

	//ionization
	if ( samples[0]->getPolarity() == '+' ) ionizationMode = +1;

	//align samples
    	if (samples.size() > 1 && alignSamplesFlag) { alignSamples(); }

	//process compound list
	if (compoundsDB.size()) {
		processCompounds(compoundsDB, "compounds");
	}

	//procall all mass slices
	if (processAllSlices == true) {
		matchRtFlag = false;
		checkConvergance=true;
		processMassSlices();
	}

	//cleanup
	delete_all(samples);
	samples.clear();
	allgroups.clear();

	return(0);
}

void alignSamples() { 
	cerr << "Aligning samples" << endl;

	writeReportFlag = false;
	processMassSlices(1000); //top 1000 groups

	cerr << "Aligner=" << allgroups.size() << endl;
	vector<PeakGroup*>agroups(allgroups.size());
	for(unsigned int i=0; i < allgroups.size();i++) agroups[i]= &allgroups[i];
	Aligner aligner;
	aligner.doAlignment(agroups);
	writeReportFlag=true;
}

void processCompounds(vector<Compound*> set, string setName) { 
		cerr << "processCompounds: " << setName << endl;
		if (set.size() == 0 ) return;
		vector<mzSlice*>slices;
		for (unsigned int i=0; i < set.size();  i++ ) {
				Compound* c = set[i];
				mzSlice* slice = new mzSlice();
				slice->compound = c;
				double mass =  mcalc.computeMass(c->formula,ionizationMode);
				slice->mzmin = mass - ppmMerge * c->mass/1e6;
				slice->mzmax = mass + ppmMerge * c->mass/1e6;
				slice->rtmin  = 0;
				slice->rtmax =  1e9;
				slice->ionCount = FLT_MAX;
				if ( matchRtFlag==true && c->expectedRt > 0 ) { 
						slice->rtmin = c->expectedRt-3;
						slice->rtmax = c->expectedRt+3;
				}
				slices.push_back(slice);
		}

		checkConvergance=false;
		reduceGroupsFlag=false;
		processSlices(slices,setName);
		delete_all(slices);
}

vector<mzSlice*> getSrmSlices() {
	map<string,int>uniqSrms;
	vector<mzSlice*>slices;
	for(int i=0; i < samples.size(); i++) {
			for(int j=0; j < samples[i]->scans.size(); j++ ) {
				Scan* scan = samples[i]->scans[j];
				string filterLine = scan->filterLine;
				if ( uniqSrms.count(filterLine) == 0 ) {
						uniqSrms[filterLine]=1;
						slices.push_back( new mzSlice(filterLine) );
				}
			}
	}
	cerr << "getSrmSlices() " << slices.size() << endl;
	return slices;
}


void processMassSlices(int limit) { 

		cerr << "Process Mass Slices" << endl;
		if ( samples.size() > 0 ) avgScanTime = samples[0]->getAverageFullScanTime();

		MassSlices massSlices;
                massSlices.setSamples(samples);
		if(limit>0) massSlices.setMaxSlices(limit);
		massSlices.algorithmB(ppmMerge,minGroupIntensity,rtStepSize);
		sort(massSlices.slices.begin(), massSlices.slices.end(), mzSlice::compIntensity);

		vector<mzSlice*>goodslices;

		if (limit > 0) {
			goodslices.resize(limit);
			for(int i=0; i< massSlices.slices.size() && i<limit; i++ ) goodslices[i] = massSlices.slices[i];
		} else {
			goodslices.resize(massSlices.slices.size());
			for(int i=0; i< massSlices.slices.size(); i++ ) goodslices[i] = massSlices.slices[i];
		}

		if (massSlices.slices.size() == 0 ) { 
			goodslices = getSrmSlices(); 
		}

		if (goodslices.size() == 0 ) {
			cerr << "Quting! No mass slices found" << endl;
			return;
		}

		reduceGroupsFlag=true;
		string setName = "allslices";
		processSlices(goodslices,setName);
		delete_all(massSlices.slices); 
		massSlices.slices.clear();
		goodslices.clear();
}



void processSlices(vector<mzSlice*>&slices, string setName) { 
		if (slices.size() == 0 ) return;
		allgroups.clear();

		printSettings();
		//process KNOWNS
			
		startTime = time(NULL);
		int converged=0;
		int eicCounter=0;
		int foundGroups=0;

		for (unsigned int i=0; i < slices.size();  i++ ) {
				if (i % 1000==0) cerr <<setprecision(2) << "Processing slices:" <<  i /(float) slices.size()*100 << "% groups=" << allgroups.size() << endl;

				mzSlice* slice = slices[i];
				double mzmin = slices[i]->mzmin;
				double mzmax = slices[i]->mzmax;
				double rtmin = slices[i]->rtmin;
				double rtmax = slices[i]->rtmax;
				//if ( mzmin < 125.0  || mzmax > 125.2 ) continue;
				Compound* compound = slices[i]->compound;

				//cerr << "\tEIC Slice\t" << mzmin <<  "-" << mzmax << " @ " << rtmin << "-" << rtmax << " I=" << slices[i]->ionCount;

				if (checkConvergance ) { //check for converagance
					allgroups.size()-foundGroups > 0 ? converged=0 : converged++;
					if ( converged > 1000 ) { cerr << "Converged.. " << endl; break; }	
					foundGroups = allgroups.size();
				}

				//PULL EICS
				vector<EIC*>eics; float eicMaxIntensity=0;
		  		for ( unsigned int j=0; j < samples.size(); j++ ) {
					EIC* eic;
					if ( !slice->srmId.empty() ) {
						eic = samples[j]->getEIC(slice->srmId);
					} else {
						eic = samples[j]->getEIC(mzmin,mzmax,rtmin,rtmax,1);
					}
					eics.push_back(eic);
					eicCounter++;
					if ( eic->maxIntensity > eicMaxIntensity) eicMaxIntensity=eic->maxIntensity;
				}


				//optimization.. skip over eics with low intensities
				if ( eicMaxIntensity < minGroupIntensity)  {  delete_all(eics); continue; } 

				//compute peak positions

				for (unsigned int i=0; i<eics.size(); i++ ) {
					eics[i]->getPeakPositions(eic_smoothingWindow);
				}

                vector <PeakGroup> peakgroups =
				EIC::groupPeaks(eics,eic_smoothingWindow,grouping_maxRtWindow);

				//sort peaks by intensity
				sort(peakgroups.begin(), peakgroups.end(), PeakGroup::compIntensity);

				for(unsigned int j=0; j < peakgroups.size(); j++ ) {
					PeakGroup& group = peakgroups[j];
					if ( compound ) group.compound = compound;

					if( clsf.hasModel()) { clsf.classify(&group); group.groupStatistics(); }
					if (clsf.hasModel() && group.goodPeakCount < minGoodGroupCount) continue;
					if (clsf.hasModel() && group.maxQuality < minQuality) continue;
					if (group.blankMax*minSignalBlankRatio > group.maxIntensity) continue;
					if (group.maxNoNoiseObs < minNoNoiseObs) continue;
					if (group.maxSignalBaselineRatio < minSignalBaseLineRatio) continue;
					if (group.maxIntensity < minGroupIntensity ) continue;

					if (compound) group.compound = compound;
                                        if (!slice->srmId.empty()) group.srmId=slice->srmId;

					addPeakGroup(group);

					if (matchRtFlag && compound != NULL && compound->expectedRt>0) {
						float rtDiff =  abs(compound->expectedRt - (group.meanRt));
						group.expectedRtDiff = rtDiff;
						group.groupRank = rtDiff*rtDiff*(1.1-group.maxQuality)*(1/log(group.maxIntensity+1));
						if (group.expectedRtDiff > 2 ) continue;
					} else { 
						group.groupRank = (1.1-group.maxQuality)*(1/log(group.maxIntensity+1));
					}

				}


				delete_all(eics); 
				eics.clear();
		}

		writeReport(setName);
}

void saveEICs(string filename) { 
	ofstream myfile(filename.c_str());
    if (! myfile.is_open()) return;
	for(int i=0; i < allgroups.size(); i++ ) {
		PeakGroup& grp = allgroups[i];
		float rtmin  = grp.minRt-3;
		float rtmax  = grp.maxRt+3;

		myfile << "{\n";
		myfile << "\"groupId\": " << i << "," << endl;
		myfile << "\"rtmin\": " << rtmin << "," << endl;
		myfile << "\"rtmax\": " << rtmax << "," << endl;
		myfile << "\"eics\": [ " << endl;
		vector<EIC*> eics = getEICs(rtmin, rtmax, grp); //get EICs
		for(int j=0; j < eics.size(); j++ ) {
				myfile << "{\n";
					saveEIC(myfile, eics[j] ); //save EICs
				myfile << "}\n";
				if ( j < eics.size()-1) myfile << ",\n";
		}
		myfile << "]" << endl;
		myfile << "}" << endl;
		delete_all(eics); //cleanup
	}
	myfile.close();
}

void saveEIC(ofstream& out, EIC* eic ) {
		int N = eic->rt.size();
		int count=0;

		out << "\"label\":" << "\"" << eic->getSample()->sampleName << "\"," << endl;
		out << "\"data\": [";
		out << setprecision(4);
		for(int i=0; i<N; i++) { 
				if (eic->intensity[i]>0) {
					  if(count && i<N-1) out << ",";
					  out << "[" << eic->rt[i] << "," <<  eic->intensity[i] << "]"; 
					  count++;
				};
		}
		out << "]\n"; 
}


vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp) { 
		vector<EIC*>eics;
		for(int i=0; i < grp.peaks.size(); i++ ) {
				float mzmin  = grp.meanMz-0.2;
				float mzmax  = grp.meanMz+0.2;
				//cerr <<setprecision(5) << "getEICs: mz:" << mzmin << "-" << mzmax << " rt:" << rtmin << "-" << rtmax << endl;

		  		for ( unsigned int j=0; j < samples.size(); j++ ) {
					if ( !grp.srmId.empty() ) {
						EIC* eic = samples[j]->getEIC(grp.srmId);
						eics.push_back(eic);
					} else {
						EIC* eic = samples[j]->getEIC(mzmin,mzmax,rtmin,rtmax,1);
						eics.push_back(eic);
					}
				}
		}
		return(eics);
}

void processOptions(int argc, char* argv[]) {

		//command line options
		const char * optv[] = {
                               "a?alignSamples",
                                "b?minGoodGroupCount <int>",
				"c?matchRtFlag",
				"d?db <sting>",
				"e?processAllSlices",
				"g:grouping_maxRtWindow <float>",
				"h?help",
				"i:minGroupIntensity <float>",
				"m?model <string>",
				"l?list  <string>",
				"o?outputdir <string>",
				"p?ppmMerge <float>",
				"r?rtStepSize <float>",
				"q:minQuality <float>",
				"v?ver", 
				"w?minPeakWidth <int>",
				"y:?eicSmoothingWindow <int>",
                                "z:minSignalBaseLineRatio <float>",
				NULL
		};

		//parse input options
		Options  opts(*argv, optv);
		OptArgvIter  iter(--argc, ++argv);
		const char * optarg;

		while( const char optchar = opts(iter, optarg) ) {
				switch (optchar) {
                                                case 'a' :
                                                alignSamplesFlag=true;
                                                break;

                                                case 'b' :
                                                minGoodGroupCount=atoi(optarg);

                                                break;
						case 'c' :
                                                    matchRtFlag = true;
								break;
						case 'e' :
                                                processAllSlices=true;
								break;
						case 'h' :
                                                opts.usage(cerr, "files ..."); exit(0);
								break;
						case 'i' :
                                                minGroupIntensity=atof(optarg);
								break;
						case 'y' : 
                                                eic_smoothingWindow=atoi(optarg);
								break;
						case 'g' : 
                                                grouping_maxRtWindow=atof(optarg);
								break;
						case 'w' : 
                                                minNoNoiseObs=atoi(optarg);
								break;
                                                case 'r' :
                                                rtStepSize=atoi(optarg);
								break;
						case 'p' :
                                                ppmMerge=atof(optarg);
								break;
						case 'q' :
                                                minQuality=atof(optarg);
								break;
						case 'z':
                                                minSignalBaseLineRatio=atof(optarg);
								break;
						case 'o' :
                                                outputdir = optarg + string(DIR_SEPARATOR_STR);
								break;
						case 'd' :
                                                ligandDbFilename = optarg;
								break;
						case 'm' :
                                                clsfModelFilename = optarg;
								break;
						default : 
								break;
				}
		}


        cerr << "#Command:\t"; 
		for (int i=0 ; i<argc ;i++)  cerr << argv[i] << " "; 
		cerr << endl;

       	if (iter.index() < argc) {
          for (int i = iter.index() ; i < argc ; i++) filenames.push_back(argv[i]); 
        }

}

void printSettings() {
        cerr << "#Ligand Database file\t" << ligandDbFilename << endl;
        cerr << "#Classification Model\t" << clsfModelFilename << endl;

        cerr << "#Output folder=" <<  outputdir << endl;
        cerr << "#ionizationMode=" << ionizationMode << endl;
        
        cerr << "#rtStepSize=" << rtStepSize << endl;
        cerr << "#ppmMerge="  << ppmMerge << endl;
        cerr << "#avgScanTime=" << avgScanTime << endl;

        //peak detection
        cerr << "#eic_smoothingWindow=" << eic_smoothingWindow << endl;

        //peak grouping across samples
        cerr << "#grouping_maxRtWindow=" << grouping_maxRtWindow << endl;

        //peak filtering criteria
        cerr << "#minGoodGroupCount=" << minGoodGroupCount << endl;
        cerr << "#minSignalBlankRatio=" << minSignalBlankRatio << endl;
        cerr << "#minPeakWidth=" << minNoNoiseObs << endl;
        cerr << "#minSignalBaseLineRatio=" << minSignalBaseLineRatio << endl;
        cerr << "#minGroupIntensity=" << minGroupIntensity << endl;


    for (unsigned int i=0;i <filenames.size(); i++) cerr << "#Sample" << i << " :" << filenames[i] << endl;
	cerr << endl;
}



void loadSamples(vector<string>&filenames) {
		cerr << "Loading samples" << endl;
		for (unsigned int i=0; i < filenames.size(); i++ ) {
			cerr << "Loading " << filenames[i] << endl;
			mzSample* sample = new mzSample();
			sample->loadSample(filenames[i].c_str());
			sample->sampleName = cleanSampleName(filenames[i]);

			if ( sample->scans.size() >= 1 ) {
					samples.push_back(sample);
					sample->summary();
				} else {
				if(sample != NULL) { 
					delete sample; 
					sample = NULL; 
				}
			}
		}
		cerr << "loadSamples done: loaded " << samples.size() << " samples\n";
}


string cleanSampleName( string sampleName) { 
		unsigned int pos =sampleName.find_last_of("/");
		if (pos != std::string::npos) { 
				sampleName=sampleName.substr(pos+1, sampleName.length());
				
		}

		pos=sampleName.find_last_of("\\");
		if (pos != std::string::npos) { 
				sampleName=sampleName.substr(pos+1, sampleName.length());
		}
		return sampleName;
		
}

void loadCompoundCSVFile(string filename){

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0; 
	int lineCount=0;
	map<string, int>header;
	MassCalculator mcalc;

    while ( getline(myfile,line) ) {
		if (!line.empty() && line[0] == '#') continue;
        lineCount++;
        vector<string>fields;
        mzUtils::split(line,',', fields);
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
			for(unsigned int i=0; i < fields.size(); i++ ) { 
					fields[i]=makeLowerCase(fields[i]); 
					header[ fields[i] ] = i; 
					//cerr << fields[i] << endl;
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
		if ( header.count("mz") && header["mz"]<N)  mz = string2float(fields[ header["mz"]]); 
		if ( header.count("rt") && header["rt"]<N)  rt = string2float(fields[ header["rt"]]);
		if ( header.count("expectedrt") && header["expectedrt"]<N) rt = string2float(fields[ header["expectedrt"]]);
		if ( header.count("charge")&& header["charge"]<N) charge = string2float(fields[ header["charge"]]);
		if ( header.count("formula")&& header["formala"]<N) formula = fields[ header["formula"] ];
		if ( header.count("id")&& header["id"]<N) 	 id = fields[ header["id"] ];
		if ( header.count("name")&& header["name"]<N) 	 name = fields[ header["name"] ];
		if ( header.count("compound")&& header["compound"]<N) 	 name = fields[ header["compound"] ];
		if ( header.count("precursormz") && header["precursormz"]<N) precursormz=string2float(fields[ header["precursormz"]]);
		if ( header.count("productmz") && header["productmz"]<N)  productmz = string2float(fields[header["productmz"]]);
		if ( header.count("collisionenergy") && header["collisionenergy"]<N) collisionenergy=string2float(fields[ header["collisionenergy"]]);

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

		//cerr << "Loading: " << id << " " << formula << "mz=" << mz << " rt=" << rt << " charge=" << charge << endl;
		if (mz == 0 ) mz=precursormz;

        if ( mz > 0 || ! formula.empty() ) {
            Compound* compound = new Compound(id,name,formula,charge);
            compound->expectedRt = rt;
			if (mz == 0) mz = mcalc.computeMass(formula,charge);
            compound->mass = mz;
            compound->db = dbname;
			compound->expectedRt=rt;
			compound->precursorMz=precursormz;
			compound->productMz=productmz;
			compound->collisionEnergy=collisionenergy;
			compoundsDB.push_back(compound);
            loadCount++;
        }
    }
    sort(compoundsDB.begin(),compoundsDB.end(), Compound::compMass);
	cerr << "Loading " << dbname << " " << loadCount << endl;
	myfile.close();
}

bool addPeakGroup(PeakGroup& grp1) { 


	for(unsigned int i=0; i<allgroups.size(); i++) {
		PeakGroup& grp2 = allgroups[i];
		float rtoverlap = mzUtils::checkOverlap(grp1.minRt, grp1.maxRt, grp2.minRt, grp2.maxRt );
		if ( rtoverlap > 0.9 && ppmDist(grp1.meanMz, grp2.meanMz) < ppmMerge && 
						grp1.maxIntensity < grp2.maxIntensity) {
			return false;
		}
	}

	//cerr << "\t\t accepting " << grp1.meanMz << "@" << grp1.meanRt;
	allgroups.push_back(grp1); 
	return true;
}

void reduceGroups() {
	sort(allgroups.begin(),allgroups.end(), PeakGroup::compMz);
	cerr << "reduceGroups(): " << allgroups.size();
	for(unsigned int i=0; i<allgroups.size(); i++) {
		PeakGroup& grp1 = allgroups[i];
		for(unsigned int j=i+1; j<allgroups.size(); j++) {
			PeakGroup& grp2 = allgroups[j];

			float rtoverlap = mzUtils::checkOverlap(grp1.minRt, grp1.maxRt, grp2.minRt, grp2.maxRt );
			float ppmdist = ppmDist(grp2.meanMz, grp1.meanMz);
			if ( ppmdist > ppmMerge ) break;

			if ( rtoverlap > 0.8 && ppmdist < ppmMerge) {
				if ( grp1.maxIntensity <= grp2.maxIntensity) {
					 allgroups.erase(allgroups.begin()+i);
					 i--;
					 break;
				} else if ( grp1.maxIntensity > grp2.maxIntensity) {
					 allgroups.erase(allgroups.begin()+j);
					 i--;
					 break;
				}
			}
		}
	}
	cerr << " done final group count(): " << allgroups.size() << endl;
}




void writeReport(string setName) { 

	if (writeReportFlag == false ) return;

	//create an output folder
	mzUtils::createDir(outputdir.c_str());

	cerr << "writeReport() " << allgroups.size() << " groups ";
	if (reduceGroupsFlag) reduceGroups();
	saveEICs(outputdir + setName + ".eics.json");
    writePeakTableXML(outputdir + setName + ".mzroll");
	writeCSVReport   (outputdir + setName + ".csv");
}

void writeCSVReport( string filename) {
    ofstream groupReport;
    groupReport.open(filename.c_str());
    if(! groupReport.is_open()) return;

    groupReport << "label,metaGroupId,groupId,goodPeakCount,medMz,medRt,maxQuality,note,compound,compoundId,expectedRtDiff,ppmDiff,parent";
    for(unsigned int i=0; i< samples.size(); i++) { groupReport << "," << samples[i]->sampleName; }
    groupReport << endl;

    int groupId=0;

    for (int i=0; i < allgroups.size(); i++ ) {
        PeakGroup* group = &allgroups[i];
        groupId=i;

        vector<float> yvalues = group->getOrderedIntensityVector(samples, PeakGroup::AreaTop);    //areatop
        //if ( group->metaGroupId == 0 ) { group->metaGroupId=groupId; }

        string tagString = group->srmId + group->tagString;
        tagString = mzUtils::substituteInQuotedString(tagString,"\",'","---");
        char label[2];
        sprintf(label,"%c",group->label);

        groupReport << label << "," << setprecision(7) << group->metaGroupId << "," << groupId << "," << group->goodPeakCount << ", "
            << group->meanMz << "," << group->meanRt << "," << group->maxQuality << "," << tagString;

        string compoundName;
        string compoundID;
        float  expectedRtDiff=0;
        float  ppmDist=0;

        if ( group->compound != NULL ) {
            compoundName = mzUtils::substituteInQuotedString(group->compound->name,"\",'","---");
            compoundID =  group->compound->id;
            ppmDist = mzUtils::ppmDist((double) group->compound->mass,(double) group->meanMz);
            expectedRtDiff = group->expectedRtDiff;
        }

        groupReport << "," << compoundName;
        groupReport << "," << compoundID;
        groupReport << "," << expectedRtDiff;
        groupReport << "," << ppmDist;

        if ( group->parent != NULL ) {
            groupReport << "," << group->parent->meanMz;
        } else {
            groupReport << "," << group->meanMz;
        }

        for( unsigned int j=0; j < samples.size(); j++) groupReport << "," <<  yvalues[j];
        groupReport << endl;
    }
}


void writeSampleListXML(xml_node& parent ) {
     xml_node samplesset = parent.append_child();
     samplesset.set_name("samples");

     for(int i=0; i < samples.size(); i++ ) {
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
        xml_node peakDetector= doc.child("project");
        peakDetector.append_attribute("Program") = "peakdetector";
        peakDetector.append_attribute("Version") = "Mar112012";



	writeSampleListXML(peakDetector);
        writeParametersXML(peakDetector);

	if (allgroups.size()) {
		peakDetector.append_child().set_name("PeakGroups");
		xml_node peakgroups = peakDetector.child("PeakGroups");
		for(int i=0; i < allgroups.size(); i++ ) {
			writeGroupXML(peakgroups, &allgroups[i]);
		}
	}

	doc.save_file(filename.c_str());
}


void writeParametersXML(xml_node& parent) {

        xml_node p = parent.append_child();
        p.set_name("PeakDetectionParameters");
        p.append_attribute("alignSamples") = alignSamplesFlag;
        p.append_attribute("matchRt") = matchRtFlag;
        p.append_attribute("ligandDbFilename") = ligandDbFilename.c_str();
        p.append_attribute("clsfModelFilename") = clsfModelFilename.c_str();
        p.append_attribute("rtStepSize") = rtStepSize;
        p.append_attribute("ppmMerge") = ppmMerge;
        p.append_attribute("eic_smoothingWindow") = eic_smoothingWindow;
        p.append_attribute("grouping_maxRtWindow") = grouping_maxRtWindow;
        p.append_attribute("minGoodGroupCount") = minGoodGroupCount;
        p.append_attribute("minSignalBlankRatio") = minSignalBlankRatio;
        p.append_attribute("minPeakWidth") = minNoNoiseObs;
        p.append_attribute("minSignalBaseLineRatio") = minSignalBaseLineRatio;
        p.append_attribute("minGroupIntensity") = minGroupIntensity;
        p.append_attribute("minQuality") = minQuality;
}


void writeGroupXML(xml_node& parent, PeakGroup* g) { 
		if (!g)return;

        xml_node group = parent.append_child();
        group.set_name("PeakGroup");

        group.append_attribute("groupId") = g->groupId;
        group.append_attribute("tagString") = g->tagString.c_str();
        group.append_attribute("metaGroupId") = g->metaGroupId;
	group.append_attribute("expectedRtDiff")= g->expectedRtDiff;
	group.append_attribute("groupRank")= (float) g->groupRank;
	group.append_attribute("label")=g->label;
	group.append_attribute("type")= (int) g->type();
	group.append_attribute("changeFoldRatio")= (int) g->changeFoldRatio;
	group.append_attribute("changePValue")= (int) g->changePValue;
	if(g->srmId.length())	group.append_attribute("srmId")= g->srmId.c_str();

        if ( g->hasCompoundLink() ) {
			Compound* c = g->compound;
		    group.append_attribute("compoundId")= c->id.c_str();
		    group.append_attribute("compoundDB")= c->db.c_str();
		}

		for(int j=0; j < g->peaks.size(); j++ ) {
					Peak& p = g->peaks[j];
                    xml_node peak = group.append_child();
                    peak.set_name("Peak");
					peak.append_attribute("pos")=p.pos;
					peak.append_attribute("minpos")=p.minpos;
					peak.append_attribute("maxpos")=p.maxpos;
					peak.append_attribute("rt")=p.rt;
					peak.append_attribute("rtmin")=p.rtmin;
					peak.append_attribute("rtmax")=p.rtmax;
					peak.append_attribute("mzmin")=p.mzmin;
					peak.append_attribute("mzmax")=p.mzmax;
					peak.append_attribute("scan")=p.scan;
					peak.append_attribute("minscan")=p.minscan;
					peak.append_attribute("maxscan")=p.maxscan;
					peak.append_attribute("peakArea")=p.peakArea;
					peak.append_attribute("peakAreaCorrected")=p.peakAreaCorrected;
					peak.append_attribute("peakAreaTop")=p.peakAreaTop;
					peak.append_attribute("peakAreaFractional")=p.peakAreaFractional;
					peak.append_attribute("peakRank")=p.peakRank;
					peak.append_attribute("peakIntensity")=p.peakIntensity;;
					peak.append_attribute("peakBaseLineLevel")=p.peakBaseLineLevel;
					peak.append_attribute("peakMz")=p.peakMz;
					peak.append_attribute("medianMz")=p.medianMz;
					peak.append_attribute("baseMz")=p.baseMz;
					peak.append_attribute("quality")=p.quality;
					peak.append_attribute("width")=p.width;
					peak.append_attribute("gaussFitSigma")=p.gaussFitSigma;
					peak.append_attribute("gaussFitR2")=p.gaussFitR2;
					peak.append_attribute("groupNum")=p.groupNum;
					peak.append_attribute("noNoiseObs")=p.noNoiseObs;
					peak.append_attribute("noNoiseFraction")=p.noNoiseFraction;
					peak.append_attribute("symmetry")=p.symmetry;
					peak.append_attribute("signalBaselineRatio")=p.signalBaselineRatio;
					peak.append_attribute("groupOverlap")=p.groupOverlap;
					peak.append_attribute("groupOverlapFrac")=p.groupOverlapFrac;
					peak.append_attribute("localMaxFlag")=p.localMaxFlag;
					peak.append_attribute("fromBlankSample")=p.fromBlankSample;
					peak.append_attribute("label")=p.label;
					peak.append_attribute("sample")=p.getSample()->sampleName.c_str();
		}

        if ( g->childCount() ) {
            for(int i=0; i < g->children.size(); i++ ) {
                PeakGroup* child = &(g->children[i]);
                writeGroupXML(group,child);
            }	
        }
}


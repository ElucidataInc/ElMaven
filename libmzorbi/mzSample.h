#ifndef MZSAMPLE_H
#define MZSAMPLE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <sstream>
#include <cstring>
#include  <limits.h>
#include  <float.h>
#include <iomanip>
#include "assert.h"
#include "pugixml.hpp"
#include "base64.h"
#include "statistics.h"
#include "mzUtils.h"
#include "mzPatterns.h"
#include "mzFit.h"
#include "mzMassCalculator.h"
#include "Matrix.h"

#ifdef CDFPARSER
#include "../libcdfread/ms10.h"
#endif

class mzSample;
class Scan;
class Peak;
class PeakGroup;
class mzSlice;
class EIC;
class Compound;
class Adduct;
class mzLink;
class Reaction;
class MassCalculator;

using namespace pugi;
using namespace mzUtils;
using namespace std;



/**
 * \class mzPoint
 *
 * \ingroup mzroll
 *
 * \brief MZ wrapper for Scan
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */
class mzPoint {
public:
	mzPoint() {x = y = z = 0; }
	mzPoint(double ix, double iy, double iz) { x = ix; y = iy; z = iz; }
	mzPoint& operator=(const mzPoint& b) { x = b.x; y = b.y; z = b.z; return *this;}


	static bool compX(const mzPoint& a, const mzPoint& b ) { return a.x < b.x; }
	static bool compY(const mzPoint& a, const mzPoint& b ) { return a.y < b.y; }
	static bool compZ(const mzPoint& a, const mzPoint& b ) { return a.z < b.z; }

	double x, y, z;
};


/**
 * \class mzScan
 *
 * \ingroup mzroll
 *
 * \brief MZ wrapper for Scan
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class Scan {
public:

	Scan(mzSample* sample, int scannum, int mslevel, float rt, float precursorMz, int polarity);

	inline int nobs() { return mz.size(); }
	inline mzSample* getSample() { return sample; }
	vector<int> findMatchingMzs(float mzmin, float mzmax);
	int findHighestIntensityPos(float mz, float ppm);
	bool hasMz(float mz, float ppm);
	bool isCentroided() { return centroided; }
	bool isProfile()    { return !centroided; }

	int totalIntensity() { int sum = 0; for (unsigned int i = 0; i < intensity.size(); i++) sum += intensity[i]; return sum; }

	static bool compRt(Scan* a, Scan* b ) { return a->rt < b->rt; }
	bool operator< (const Scan& b) { return rt < b.rt; }  //default comparision operation

	inline int getPolarity() { return polarity; }
	void  setPolarity(int x) { polarity = x; }
	void  simpleCentroid();
	void  intensityFilter( int minIntensity);
	void  quantileFilter(int minQuantile);
	void  summary();

	float rt;
	int   scannum;
	float precursorMz;
	float productMz;
	float collisionEnergy;
	int mslevel;
	bool centroided;

	vector <float> intensity;
	vector <float> mz;
	string scanType;
	string filterLine;
	mzSample* sample;

	vector<float> chargeSeries(float Mx, unsigned int Zx);

private:
	int polarity;


};


/**
 * \class mzSlice
 *
 * \ingroup mzroll
 *
 * \brief MZ wrapper for a metabolite slice.
 *
 * This class is used as a wrapper for a metabolite slice read from mz file.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */
class mzSlice {
public:
	mzSlice(float a, float b, float c, float d) { mzmin = a; mzmax = b; rtmin = c; rtmax = d; mz = a + (b - a) / 2; rt = c + (d - c) / 2; compound = NULL; ionCount = 0; }
	mzSlice(float a, float b, float c ) {  mz = mzmin = mzmax = a; rt = rtmin = rtmax = b; ionCount = c;  compound = NULL; ionCount = 0;}
	mzSlice(string filterLine) { mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0; compound = NULL; srmId = filterLine; }
	mzSlice() { mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0; compound = NULL; }
	mzSlice(const mzSlice& b) { mzmin = b.mzmin; mzmax = b.mzmax; rtmin = b.rtmin; rtmax = b.rtmax; ionCount = b.ionCount; mz = b.mz; rt = b.rt; compound = b.compound; srmId = b.srmId; }
	mzSlice& operator= (const mzSlice& b) {
		mzmin = b.mzmin; mzmax = b.mzmax; rtmin = b.rtmin; rtmax = b.rtmax; ionCount = b.ionCount;
		compound = b.compound; srmId = b.srmId; mz = b.mz; rt = b.rt; return *this;
	}

	float mzmin;
	float mzmax;
	float rtmin;
	float rtmax;
	float mz;
	float rt;
	float ionCount;
	Compound* compound;
	string srmId;

	static bool compIntensity(const mzSlice* a, const mzSlice* b ) { return b->ionCount < a->ionCount; }
	static bool compMz(const mzSlice* a, const mzSlice* b ) { return a->mz < b->mz; }
	static bool compRt(const mzSlice* a, const mzSlice* b ) { return a->rt < b->rt; }
	bool operator< (const mzSlice* b) { return mz < b->mz; }
};


/**
 * \class mzLink
 *
 * \ingroup mzroll
 *
 * \brief MZ wrapper for a metabolite reaction.
 *
 * This class is used as a wrapper for a metabolite reaction read from mz file.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */
class mzLink {
public:
	mzLink() { mz1 = mz2 = 0; value1 = value2 = 0.0; data1 = data2 = NULL; correlation = 0;}
	mzLink( int a, int b, string n ) { value1 = a; value2 = b; note = n; correlation = 0; }
	mzLink( float a, float b, string n ) { mz1 = a; mz2 = b; note = n; correlation = 0; }
	~mzLink() {}
	//link between two mzs
	float mz1;          //from
	float mz2;          //to
	string note;        //note about this link

	//generic placeholders to attach values to links
	float value1;
	float value2;

	//generic  placeholders to attach objects to the link
	void* data1;
	void* data2;
	//
	float correlation;
	static bool compMz(const mzLink& a, const mzLink& b ) { return a.mz1 < b.mz1; }
	static bool compCorrelation(const mzLink& a, const mzLink& b) { return a.correlation > b.correlation; }
};

/**
 * \class mzSample
 *
 * \ingroup mzroll
 *
 * \brief MZ wrapper for a metabolite samples.
 *
 * This class is used as a wrapper for a metabolite sample read from mz file.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */
class mzSample {
public:

	/** Constructor for mzSample.
	*/
	mzSample();
	~mzSample();

	/** Load metabolite sample from
	* an input file.
	* @param const char pointer to inpute filename
	*/
	void loadSample(const char* filename);

	/** Load data from
	* a mzData file.
	* @param const char pointer to inpute filename
	*/
	void parseMzData(const char*);

	/** Load data from
	* a MzCSV file.
	* @param const char pointer to inpute filename
	*/
	void parseMzCSV(const char*);

	/** Load data from
	* a MzXML file.
	* @param const char pointer to inpute filename
	*/
	void parseMzXML(const char*);

	/** Load data from
	* an netcdf file.
	* @param const char pointer to inpute filename, int is_verbose
	*/
	int  parseCDF (const char *filename, int is_verbose);


	/** parse individual scan
	* @param const xml_node pointer to scan, int scan number
	*/
	void parseMzXMLScan(const xml_node& scan, int scannum);

	/** print info about sample
	*/
	void summary();

	/** compute min and max values for mz and rt
	*/
	void calculateMzRtRange();

	/** average time difference between scans
	*/
	float getAverageFullScanTime();

	/** srm->scan mapping for QQQ
	*/
	void enumerateSRMScans();


	/** correlation in EIC space
	*/
	float correlation(float mz1,  float mz2, float ppm, float rt1, float rt2 );
	float getNormalizationConstant() { return _normalizationConstant; }
	void  setNormalizationConstant(float x) { _normalizationConstant = x; }


	/** get indexes for a given scan
	*/
	Scan* getScan(int scanNum);
	Scan* getAverageScan(float rtmin, float rtmax, int mslevel, float resolution);


	/** get eic based on minMz, maxMz, minRt, maxRt,mslevel
	*/
	EIC* getEIC(float, float, float, float, int);

	/** get eic based on srmId
	*/
	EIC* getEIC(string srmId);
	EIC* getEIC(float precursorMz, float collisionEnergy, float productMz, float amuQ1, float amuQ2 );

	/** get Total Ion Chromatogram
	*/
	EIC* getTIC(float, float, int);


	deque <Scan*> scans;
	string sampleName;
	string fileName;
	bool isSelected;
	bool isBlank;

	/** sample display color, [r,g,b, alpha]
		*/
	float color[4];
	float minMz;
	float maxMz;
	float minRt;
	float maxRt;
	float maxIntensity;
	float minIntensity;
	float totalIntensity;


	/** sample display order
		*/
	int   _sampleOrder;
	bool  _C13Labeled;
	bool  _N15Labeled;

	/** Feng note: added to track S34 labeling state
		*/
	bool  _S34Labeled;

	/** Feng note: added to track D2 labeling state
		*/
	bool  _D2Labeled;
	float _normalizationConstant;
	string  _setName;

	/** srm to scan mapping
		*/
	map<string, vector<int> >srmScans;

	/** tags associated with this sample
		*/
	map<string, string> instrumentInfo;


	/** saved retention times prior to alignment. saving and restoring retention times
		*/
	vector<float>originalRetentionTimes;
	void saveOriginalRetentionTimes();
	void restoreOriginalRetentionTimes();

	//class functions
	void addScan(Scan*s);
	inline int getPolarity() { if ( scans.size() > 0) return scans[0]->getPolarity(); return 0; }
	inline int   scanCount() { return (scans.size()); }
	inline string getSampleName() { return sampleName; }
	void setSampleOrder(int x) { _sampleOrder = x; }
	inline int	  getSampleOrder() { return _sampleOrder; }
	inline string  getSetName()  { return _setName; }
	void   setSetName(string x) { _setName = x; }
	void   setSampleName(string x) { sampleName = x; }

	static float getMaxRt(const vector<mzSample*>&samples);
	bool C13Labeled() { return _C13Labeled; }
	bool N15Labeled() { return _N15Labeled; }

	static bool compSampleOrder(const mzSample* a, const mzSample* b ) { return a->_sampleOrder < b->_sampleOrder; }
	static mzSlice getMinMaxDimentions(const vector<mzSample*>& samples);


	static void setFilter_minIntensity(int x ) { filter_minIntensity = x; }
	static void setFilter_centroidScans( bool x) { filter_centroidScans = x; }
	static void setFilter_intensityQuantile(int x ) { filter_intensityQuantile = x; }

	static int getFilter_minIntensity() { return filter_minIntensity; }
	static int getFilter_intensityQuantile() { return filter_intensityQuantile; }
	static int getFilter_centroidScans() { return filter_centroidScans; }

private:
	static int filter_minIntensity;
	static bool filter_centroidScans;
	static int filter_intensityQuantile;

};

class EIC {

public:
	EIC()  {
		sample = NULL; spline = NULL; baseline = NULL;
		mzmin = mzmax = rtmin = rtmax = maxIntensity = totalIntensity = eic_noNoiseObs = 0;
		smootherType = GAUSSIAN;
	} // constructor
	~EIC();

	enum SmootherType { GAUSSIAN = 0, AVG = 1, SAVGOL = 2 };
	vector <int> scannum;
	vector <float> rt;
	vector <float> mz;
	vector <float> intensity;
	vector <Peak>  peaks;
	string sampleName;

	mzSample* sample;	//pointer to originating sample
	float color[4];		//color of the eic line, [r,g,b, alpha]
	float *spline;          //pointer to smoothed intentsity array
	float *baseline;        //pointer to baseline array

	float maxIntensity;
	float totalIntensity;
	int   eic_noNoiseObs;

	float mzmin;
	float mzmax;
	float rtmin;
	float rtmax;

	Peak* addPeak(int peakPos);
	void deletePeak(int i);
	void getPeakPositions(int smoothWindow);
	void getPeakDetails(Peak& peak);
	void getPeakWidth(Peak& peak);
	void computeBaseLine(int smoothingWindow, int cutQuantile);
	void computeSpline(int smoothWindow);
	void findPeakBounds(Peak& peak);
	void getPeakStatistics();
	void checkGaussianFit(Peak& peak);
	vector<mzPoint> getIntensityVector(Peak& peak);
	void summary();
	void setSmootherType(EIC::SmootherType x) { smootherType = x; }

	inline int size() { return intensity.size();}
	inline mzSample* getSample() { return sample; }
	static vector<PeakGroup> groupPeaks(vector<EIC*>&eics, int smoothingWindow, float maxRtDiff);

	static EIC* eicMerge(const vector<EIC*>& eics);
	static void removeLowRankGroups(vector<PeakGroup>&groups, int rankLimit );
	static bool compMaxIntensity(EIC* a, EIC* b ) { return a->maxIntensity > b->maxIntensity; }

private:
	SmootherType smootherType;

};

class Peak {
public:
	Peak();
	Peak(EIC* e, int p);
	Peak(const Peak& p);
	Peak& operator=(const Peak& o);
	void copyObj(const Peak& o);

	//~Peak() { cerr << "~Peak() " << this << endl; }

	int pos;
	int minpos;
	int maxpos;

	float rt;
	float rtmin;
	float rtmax;
	float mzmin;
	float mzmax;

	int scan;
	int minscan;
	int maxscan;

	float peakArea;                    //non corrected sum of all intensities
	float peakAreaCorrected;           //baseline substracted area
	float peakAreaTop;                  //top 3points of the peak
	float peakAreaFractional;          //area of the peak dived by total area in the EIC
	float peakRank;                     //peak rank (sorted by peakAreaCorrected)

	float peakIntensity;               //not corrected intensity at top of the pek
	float peakBaseLineLevel;            //baseline level below the highest point

	float peakMz;	//mz value at the top of the peak
	float medianMz;	//averaged mz value across all points in the peak
	float baseMz;	//mz value across base of the peak

	float quality;	//from 0 to 1. indicator of peak goodness
	int width;		//width of the peak at the baseline
	float gaussFitSigma;	//fit to gaussian curve
	float gaussFitR2;		//fit to gaussian curve
	int groupNum;

	int noNoiseObs;
	float noNoiseFraction;
	float symmetry;
	float signalBaselineRatio;
	float groupOverlap;			// 0 no overlap, 1 perfect overlap
	float groupOverlapFrac;

	bool localMaxFlag;
	bool fromBlankSample;		//true if peak is from blank sample

	char label;		//classification label

private:
	EIC* eic; 		//pointer to eic
	mzSample *sample;  //pointer to sample

public:
	void setEIC(EIC* e) { eic = e; }
	inline EIC*	 getEIC() { return eic;    }
	inline bool hasEIC() { return eic != NULL; }

	Scan* getScan() { if (sample) return sample->getScan(scan); else return NULL; }

	void   setSample(mzSample* s ) { sample = s; }
	inline mzSample* getSample() { return sample; }
	inline bool hasSample() 	{  return sample != NULL; }

	void setLabel(char label) { this->label = label;}
	inline char getLabel() { return label;}

	static bool compRt(const Peak& a, const Peak& b ) { return a.rt < b.rt; }
	static bool compIntensity(const Peak& a, const Peak& b ) { return b.peakIntensity < a.peakIntensity; }
	static bool compArea(const Peak& a, const Peak& b ) { return b.peakAreaFractional < a.peakAreaFractional; }
	static bool compMz(const Peak& a, const Peak& b ) { return a.peakMz < b.peakMz; }
	static bool compSampleName(const Peak& a, const Peak& b ) { return a.sample->getSampleName() < b.sample->getSampleName(); }
	static bool compSampleOrder(const Peak& a, const Peak& b ) { return a.sample->getSampleOrder() < b.sample->getSampleOrder(); }
	inline static float overlap(const Peak& a, const Peak& b) {	return ( checkOverlap(a.rtmin, a.rtmax, b.rtmin, b.rtmax)); }
	vector<mzLink> findCovariants();
};

class PeakGroup {

public:
	enum GroupType {None = 0, C13 = 1, Adduct = 2, Fragment = 3, Covariant = 4, Isotope = 5 }; //group types
	enum QType	   {AreaTop = 0, Area = 1, Height = 2, RetentionTime = 3, Quality = 4, SNRatio = 5 };
	PeakGroup();
	PeakGroup(const PeakGroup& o);
	PeakGroup& operator=(const PeakGroup& o);
	bool operator==(const PeakGroup* o);
	void copyObj(const PeakGroup& o);
	void copy(const PeakGroup* o);

	~PeakGroup();

	PeakGroup* parent;
	Compound* compound;

	vector<Peak> peaks;
	deque<PeakGroup> children;

	string srmId;
	string tagString;
	char label;			//classification label
	string getName();               //compound name + tagString + srmid

	bool isFocused;

	int groupId;
	int metaGroupId;

	float maxIntensity;
	float meanRt;
	float meanMz;
	int totalSampleCount;

	//isotopic information
	float expectedAbundance;
	int   isotopeC13count;

	float minRt;
	float maxRt;
	float minMz;
	float maxMz;

	float blankMax;
	float blankMean;
	int   blankSampleCount;

	int sampleCount;
	float sampleMean;
	float sampleMax;

	int maxNoNoiseObs;
	int  maxPeakOverlap;
	float maxQuality;
	float maxPeakFracionalArea;
	float maxSignalBaseRatio;
	float maxSignalBaselineRatio;
	int goodPeakCount;
	float expectedRtDiff;
	unsigned int groupRank;

	//for sample contrasts  ratio and pvalue
	float changeFoldRatio;
	float changePValue;

	bool  	hasSrmId()  { return srmId.empty(); }
	void  	setSrmId(string id)	  { srmId = id; }
	inline  string getSrmId() { return srmId; }

	bool isPrimaryGroup();
	inline bool hasCompoundLink()  { if (compound != NULL) return true ; return false; }
	inline bool isEmpty()   { if (peaks.size() == 0) return true; return false; }
	inline int peakCount()  { return peaks.size(); 	  }
	inline int childCount() { return children.size(); }
	inline Compound* getCompound() { return compound; }

	inline PeakGroup* getParent() { return parent; }

	inline vector<Peak>& getPeaks() { return peaks; }
	inline deque<PeakGroup>& getChildren()  { return children; }

	inline void setParent(PeakGroup* p) {parent = p;}
	inline void setLabel(char label) { this->label = label;}
	inline float ppmDist(float cmass) { return mzUtils::ppmDist(cmass, meanMz); }

	inline void addPeak(const Peak& peak) { peaks.push_back(peak); peaks.back().groupNum = groupId; }
	inline void addChild(const PeakGroup& child) { children.push_back(child); children.back().parent = this;   }
	Peak* getPeak(mzSample* sample);

	GroupType _type;
	inline GroupType type() { return _type; }
	inline void setType(GroupType t)  { _type = t; }
	inline bool isIsotope() { return _type == Isotope; }
	inline bool isFragment() { return _type == Fragment; }
	inline bool isAdduct() {  return _type == Adduct; }


	void summary();
	void groupStatistics();
	void updateQuality();
	float medianRt();
	float meanRtW();

	void reduce();
	void fillInPeaks(const vector<EIC*>& eics);
	void computeAvgBlankArea(const vector<EIC*>& eics);
	void groupOverlapMatrix();

	Peak* getSamplePeak(mzSample* sample);

	void deletePeaks();
	bool deletePeak(unsigned int index);

	void clear();
	void deleteChildren();
	bool deleteChild(unsigned int index);
	bool deleteChild(PeakGroup* child);

	vector<float> getOrderedIntensityVector(vector<mzSample*>& samples, QType type);
	void reorderSamples();

	static bool compRt(const PeakGroup& a, const PeakGroup& b ) { return (a.meanRt < b.meanRt); }
	static bool compMz(const PeakGroup& a, const PeakGroup& b ) { return (a.meanMz > b.meanMz); }
	static bool compIntensity(const PeakGroup& a, const PeakGroup& b ) { return (a.maxIntensity > b.maxIntensity); }
	static bool compArea(const PeakGroup& a, const PeakGroup& b ) { return (a.maxPeakFracionalArea > b.maxPeakFracionalArea); }
	static bool compQuality(const PeakGroup& a, const PeakGroup& b ) { return (a.maxQuality > b.maxQuality); }
	//static bool compInfoScore(const PeakGroup& a, const PeakGroup& b ) { return(a.informationScore > b.informationScore); }
	static bool compRank(const PeakGroup& a, const PeakGroup& b ) { return (a.groupRank > b.groupRank); }
	static bool compRankPtr(const PeakGroup* a, const PeakGroup* b ) { return (a->groupRank > b->groupRank); }
	static bool compRatio(const PeakGroup& a, const PeakGroup& b ) { return (a.changeFoldRatio < b.changeFoldRatio); }
	static bool compPvalue(const PeakGroup* a, const PeakGroup* b ) { return (a->changePValue < b->changePValue); }
	static bool compC13(const PeakGroup* a, const PeakGroup* b) { return (a->isotopeC13count < b->isotopeC13count); }
	static bool compMetaGroup(const PeakGroup& a, const PeakGroup& b) { return (a.metaGroupId < b.metaGroupId); }
	bool operator< (const PeakGroup* b) { return this->maxIntensity < b->maxIntensity; }
};

class Compound {
	static MassCalculator* mcalc;

private:
	PeakGroup _group;			//link to peak group
	bool      _groupUnlinked;

public:
	Compound(string id, string name, string formula, int charge );
	~Compound() {}; //empty destructor

	PeakGroup* getPeakGroup() { return &_group; }
	void setPeakGroup(const PeakGroup& group ) { _group = group; _group.compound = this; }
	bool hasGroup()    { if (_group.meanMz != 0 ) return true; return false; }
	void clearGroup()  { _group.clear(); }
	void unlinkGroup() { _group.clear(); _groupUnlinked = true; }
	bool groupUnlinked() { return _groupUnlinked; }

	vector<Reaction*>reactions;
	string id;

	string name;
	string formula;
	string kegg_id;
	string pubchem_id;
	string hmdb_id;
	string alias;

	string srmId;
	float expectedRt;

	int charge;
	float mass;
	float massDelta;

	//QQQ mapping
	string method_id;
	float precursorMz;	//QQQ parent ion
	float productMz;	//QQQ child ion
	float collisionEnergy; //QQQ collision energy

	string db;			//name of database for example KEGG, ECOCYC.. etc..

	int transition_id;
	vector<float>fragment_mzs;
	vector<float>fragment_intensity;
	vector<string> category;

	float ajustedMass(int charge);
	void addReaction(Reaction* r) { reactions.push_back(r); }
	static bool compMass(const Compound* a, const Compound* b )      { return (a->mass < b->mass);       }
	static bool compName(const Compound* a, const Compound* b )    { return (a->name < b->name);       }
	static bool compFormula(const Compound* a, const Compound* b ) { return (a->formula < b->formula); }
	static bool compReactionCount(const Compound* a, const Compound* b ) { return (a->reactions.size() < b->reactions.size()); }


};

class Pathway {
public:
	Pathway( string id, string name) {  this->id = id; this->name = name; }
	string id;
	string name;
	vector<Reaction*>reactions;
};

class Isotope {
public:
	string name;
	double mass;
	double abundance;
	int N15;
	int C13;
	int S34;
	int H2;

	Isotope(string name, float mass, int c = 0, int n = 0, int s = 0, int h = 0) {
		this->mass = mass; this->name = name;
		C13 = c; N15 = n; S34 = s; H2 = h;
	}

	Isotope() {
		mass = 0; abundance = 0; N15 = 0; C13 = 0; S34 = 0; H2 = 0;
	}

	Isotope(const Isotope& b) {
		name = b.name;
		mass = b.mass;
		abundance = b.abundance;
		N15 = b.N15; S34 = b.S34; C13 = b.C13; H2 = b.H2;
	}

	Isotope& operator=(const Isotope& b) {
		name = b.name; mass = b.mass; abundance = b.abundance;
		N15 = b.N15; S34 = b.S34; C13 = b.C13; H2 = b.H2;
		return *this;
	}

};

class  Reaction {
public:
	Reaction(string db, string id, string name) { this->db  = db; this->id =  id; this->name = name; reversable = false; }
	void addReactant(Compound* r, int s) {  reactants.push_back(r); stoichiometry[r] = s; }
	void addProduct(Compound* p, int s)  {  products.push_back(p); stoichiometry[p] = s; }
	void setReversable(bool r) { reversable = r; }

	string db;
	string id;
	string name;
	deque<Compound*> reactants;
	deque<Compound*> products;
	map<Compound*, int> stoichiometry;
	bool reversable;

};

class Adduct {

public:
	Adduct() { isParent = false; mass = 0; charge = 1; nmol = 1; }
	string name;
	int	  nmol;
	float mass;
	float charge;
	bool isParent;

	//given adduct mass compute parent ion mass
	inline float computeParentMass(float mz)  { return  (mz * abs(charge) - mass) / nmol; }
	//given perent compute adduct mass
	inline float computeAdductMass(float pmz) { return (pmz * nmol + mass) / abs(charge); }
};

class Aligner {

public:
	Aligner();
	void doAlignment(vector<PeakGroup*>& peakgroups);
	vector<double> groupMeanRt();
	double checkFit();
	void Fit();
	void saveFit();
	void restoreFit();
	void setMaxItterations(int x) { maxItterations = x; }
	void setPolymialDegree(int x) { polynomialDegree = x; }
private:
	vector< vector<float> > fit;
	vector<mzSample*> samples;
	vector<PeakGroup*> allgroups;
	int maxItterations;
	int polynomialDegree;

};



#endif

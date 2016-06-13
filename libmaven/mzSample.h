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

#ifdef ZLIB
#include <zlib.h>
#endif


#ifdef CDFPARSER
#include "../libcdfread/ms10.h"
#endif

#if defined(WIN32) || defined(WIN64)
#define strncasecmp strnicmp
#define isnan(x) ((x) = (x))
#endif /* Def WIN32 or Def WIN64 */

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
class ChargedSpecies;

using namespace pugi;
using namespace mzUtils;
using namespace std;


/**
 * @class mzPoint
 * @ingroup libmaven
 * @brief Wrapper class for Point
 * @author Elucidata
 */
class mzPoint {
	public:

	/**
	 * [Constructor for mzPoint]
	 */
		mzPoint() {x=y=z=0; }

		/**
			 [Constructor for mzPoint]
			 @param ix    ix.
			 @param iy    iy.
			 @param iz 		iz.
		 */
		mzPoint(double ix,double iy,double iz) { x=ix; y=iy; z=iz; }

		mzPoint& operator=(const mzPoint& b) { x=b.x; y=b.y; z=b.z; return *this;}

		/**
		 * [compare with x]
		 * @method compX
		 * @param  a     [Point a]
		 * @param  b     [Point b]
		 * @return [bool less or more]
		 */
		static bool compX(const mzPoint& a, const mzPoint& b ) { return a.x < b.x; }

		/**
		 * [compare with y]
		 * @method compY
		 * @param  a     [Point a]
		 * @param  b     [Point b]
		 * @return [bool less or more]
		 */
		static bool compY(const mzPoint& a, const mzPoint& b ) { return a.y < b.y; }

		/**
		 * [compare with z]
		 * @method compZ
		 * @param  a     [Point a]
		 * @param  b     [Point b]
		 * @return [bool less or more]
		 */
		static bool compZ(const mzPoint& a, const mzPoint& b ) { return a.z < b.z; }

        double x,y,z;
};

/**
 * @class Scan
 * @ingroup libmaven
 * @brief Wrapper class for a MS Scan.
 * @author Elucidata
 */
class Scan {
    public:

		/**
			 [Constructor for mzPoint]
			 @param sample    [metabolite sample].
			 @param scannum   number of scans.
			 @param mslevel 	MS level
			 @param rt 				retention time
			 @param precursorMz precursor mz ration
			 @param polarity ionization mode.
		 */
    Scan(mzSample* sample, int scannum, int mslevel, float rt, float precursorMz, int polarity);

		/**
		 * [copy constructor]
		 * @method deepcopy
		 * @param  b        [Scan pointer]
		 */
		void deepcopy(Scan* b);

		/**
		 * [number of observations]
		 * @method nobs
		 * @return [number of observations]
		 */
    inline unsigned int nobs() { return mz.size(); }

		/**
		 * [get samples]
		 * @method getSample
		 * @return [sample]
		 */
    inline mzSample* getSample() { return sample; }

		/**
		 * [finds matching m/zs in a given range]
		 * @method findMatchingMzs
		 * @return [returns an integer vector between the min and max m/z value]
		 */
		vector<int> findMatchingMzs(float mzmin, float mzmax); //TODO why int vector?

		/**
		 * [finds highest intensity position]
		 * @method findHighestIntensityPos
		 * @param  mz                      [m/z value]
		 * @param  ppm                     [ppm window]
		 * @return [highest intensity value for an m/z with a ppm window]
		 */
    int findHighestIntensityPos(float mz, float ppm);

		/**
		 * [checks if an input m/z value is in the m/z total list]
		 * @method hasMz
		 * @param  mz    [input m/z value]
		 * @param  ppm   [ppm window]
		 * @return [returns true if input m/z exists, otherwise false]
		 */
    bool hasMz(float mz, float ppm);

		/**
		 * [Checks if the data is centroided]
		 * @method isCentroided
		 * @return [return true if data is centroided else false]
		 */
    bool isCentroided() { return centroided; }

    /**
     * [Checks if the data is not centroided]
     * @method isProfile
     * @return [returns true if data is not centroided else false]
     */
    bool isProfile()    { return !centroided; }

    /**
 * [Gets the polarity of the scan]
 * @method getPolarity
 * @return [Returns the polarity of the scan (+1 for positive, -1 for
 * negative and 0 for neutral)]
     */
    inline int getPolarity() { return polarity; }

    /**
 * [Sets the polarity of a scan]
 * @method setPolarity
 * @param  x 	[polarity to be set (+1 for positive, -1 for negative and 0
 * for neutral)]
     */
    void  setPolarity(int x) { polarity = x; }

    /**
     * [Calculate the sum of all the intensities for a scan]
     * @method totalIntensity
     * @return [returns the sum of all the intensities for a scan]
     */
    int totalIntensity(){ int sum=0; for(unsigned int i=0;i<intensity.size();i++) sum += intensity[i]; return sum; }

		/**
 		 * [return pairing of m/z, intensity values for top intensities.
		 * intensities are normalized to a maximum intensity in a scan * 100]
		 * @param minFracCutoff [specfies mininum relative intensity; for example
		 * 0.05, filters out all intensites below 5% of maxium scan intensity]
		 * @return
		 */
    vector<pair<float,float> > getTopPeaks(float minFracCutoff);

		/**
		 * [generate multi charges series..endingin in change Zx,Mx]
		 * @param Mx []
		 * @param
		 */
    vector<float> chargeSeries(float Mx, unsigned int Zx); //TODO what does this do chargeSeries?

		/**
		 * [Deconvolution]
		 * @method deconvolute
		 * @param  mzfocus                [description]
		 * @param  noiseLevel             [description]
		 * @param  ppmMerge               [description]
		 * @param  minSigNoiseRatio       [description]
		 * @param  minDeconvolutionCharge [description]
		 * @param  maxDeconvolutionCharge [description]
		 * @param  minDeconvolutionMass   [description]
		 * @param  maxDeconvolutionMass   [description]
		 * @param  minChargedStates       [description]
		 * @return [description]
		 */
		ChargedSpecies* deconvolute(float mzfocus, float noiseLevel, float ppmMerge, float minSigNoiseRatio, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates );
		//TODO deconvolute

		/**
		 * [centroid the data]
		 * @method simpleCentroid
		 */
    void  simpleCentroid();

		/**
		 * [removes intensities from scan that lower than X]
		 * @method intensityFilter
		 * @param  minIntensity    [X: minimum intensity]
		 */
    void  intensityFilter( int minIntensity);

		/**
		 * [removes intensities from scan that lower than X]
		 * @method quantileFilter
		 * @param  minQuantile    [X: minimum Quantile]
		 */
    void  quantileFilter(int minQuantile);

		/**
		 * [prints the summary of fields in Scan class]
		 * @method summary
		 */
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

		/**
		 * [compare retention times of two scans]
		 * @method compRt
		 * @param  a      [Scan 1]
		 * @param  b      [Scan 2]
		 * @return [True if Scan 1 has lower retention time than Scan b, else false]
		 */
    static bool compRt(Scan* a, Scan* b ) { return a->rt < b->rt; }

		/**
		 * [compare precursor m/z of two samples]
		 * @method compPrecursor
		 * @param  a             [Scan 1]
		 * @param  b             [Scan 2]
		 * @return [True if Scan 1 has lower precursor m/z than Scan b, else false]
		 */
		static bool compPrecursor(Scan* a, Scan* b ) { return a->precursorMz < b->precursorMz; }

		/**
		 * [default comparision operation]
		 */
    bool operator< (const Scan& b) { return rt < b.rt; }

    private:
        int polarity;
};


/**
 * @class mzSlice
 * @ingroup libmaven
 * @brief Wrapper class for a MS Scan Slice.
 * @author Elucidata
 */
class mzSlice {
    public:

        mzSlice(float a, float b, float c, float d) { mzmin= a; mzmax=b; rtmin=c; rtmax=d; mz=a+(b-a)/2; rt=c+(d-c)/2; compound=NULL; ionCount=0; }
				mzSlice(float a, float b, float c ) {  mz=mzmin=mzmax=a; rt=rtmin=rtmax=b; ionCount=c;  compound=NULL; ionCount=0;}
	mzSlice(string filterLine) { mzmin=mzmax=rtmin=rtmax=mz=rt=ionCount=0; compound=NULL; srmId=filterLine; }
        mzSlice() { mzmin=mzmax=rtmin=rtmax=mz=rt=ionCount=0; compound=NULL; }
	mzSlice(const mzSlice& b) { mzmin=b.mzmin; mzmax=b.mzmax; rtmin=b.rtmin; rtmax=b.rtmax; ionCount=b.ionCount; mz=b.mz; rt=b.rt; compound=b.compound; srmId=b.srmId; }
	mzSlice& operator= (const mzSlice& b) { mzmin=b.mzmin; mzmax=b.mzmax; rtmin=b.rtmin; rtmax=b.rtmax; ionCount=b.ionCount;
				compound=b.compound; srmId=b.srmId; mz=b.mz; rt=b.rt; return *this; }

        float mzmin;
        float mzmax;
        float rtmin;
        float rtmax;
        float mz;
        float rt;
        float ionCount;
        Compound* compound;
	string srmId;

	/**
	 * [compare intensity of two samples]
	 * @method compIntensity
	 * @param  a             [Scan 1]
	 * @param  b             [Scan 2]
	 * @return [True if Scan 1 has lower intensity than Scan b, else false]
	 */
	static bool compIntensity(const mzSlice* a, const mzSlice* b ) { return b->ionCount < a->ionCount; }

	/**
	 * [compare m/z of two samples]
	 * @method compMz
	 * @param  a             [Scan 1]
	 * @param  b             [Scan 2]
	 * @return [True if Scan 1 has lower m/z than Scan b, else false]
	 */
	static bool compMz(const mzSlice* a, const mzSlice* b ) { return a->mz < b->mz; }

	/**
	 * [compare retention time of two samples]
	 * @method compRt
	 * @param  a             [Scan 1]
	 * @param  b             [Scan 2]
	 * @return [True if Scan 1 has lower retention time than Scan b, else false]
	 */
	static bool compRt(const mzSlice* a, const mzSlice* b ) { return a->rt < b->rt; }
	bool operator< (const mzSlice* b) { return mz < b->mz; }
};


/**
 * @class mzLink
 * @ingroup libmaven
 * @brief Wrapper class for a link between two mzs.
 * @author Elucidata
 */
class mzLink {
		public:
			mzLink(){ mz1=mz2=0; value1=value2=0.0; data1=data2=NULL; correlation=0;}
			mzLink( int a, int b, string n ) { value1=a; value2=b; note=n; correlation=0; }
			mzLink( float a,float b, string n ) { mz1=a; mz2=b; note=n; correlation=0; }
			~mzLink() {}

			/** from */
			float mz1;
			/** to */
			float mz2;

			/** note about this link */
			string note;

            //generic placeholders to attach values to links
			float value1;
			float value2;

            //generic  placeholders to attach objects to the link
            void* data1;
            void* data2;

           float correlation;

					 /**
						* [compare m/z of two links]
						* @method compMz
						* @param  a             [Link 1]
						* @param  b             [Link 2]
						* @return [True if Link 1 has lower m/z than Link b, else false]
						*/
           static bool compMz(const mzLink& a, const mzLink& b ) { return a.mz1 < b.mz1; }

					 /**
						* [compare correlation of two links]
						* @method compMz
						* @param  a             [Link 1]
						* @param  b             [Link 2]
						* @return [True if Link 1 has lower correlation than Link b, else false]
						*/
           static bool compCorrelation(const mzLink& a, const mzLink& b) { return a.correlation > b.correlation; }
       };


 /**
  * @class mzSample
  * @ingroup libmaven
  * @brief Wrapper class for a sample.
  * @author Elucidata
  */
class mzSample {
public:

    mzSample();
    ~mzSample();

		/**
		 * [load from file]
		 * @method loadSample
		 * @param  filename   [input string filename]
		 */
    void loadSample(const char* filename);

		/**
		 * [load data from mzData file]
		 * @method parseMzData
		 * @param  char        [mzData file]
		 */
    void parseMzData(const char);

		/**
		 * [load data from mzXML file]
		 * @method parseMzXML
		 * @param  char*        [mzXML file]
		 */
		void parseMzXML(const char*);

		/**
		 * [load data from mzML file]
		 * @method parseMzML
		 * @param  char*        [mzML file]
		 */
		void parseMzML(const char*);

		/**
		 * [load netcdf file]
		 * @method parseCDF
		 * @param  filename   [netcdf file]
		 * @param  is_verbose [verbose or not]
		 * @return            [int]
		 */
    int  parseCDF (const char *filename, int is_verbose);

		/**
		 * [parse individual scan]
		 * @method parseMzXMLScan
		 * @param  scan           [scan]
		 * @param  scannum        [scan number]
		 */
    void parseMzXMLScan(const xml_node& scan, int scannum);

		/**
		 * [write mzCSV file]
		 * @method writeMzCSV
		 * @param  char*           [character pointer]
		 */
    void writeMzCSV(const char*);


    map<string,string> mzML_cvParams(xml_node node);

		/**
		 * [parseMzMLChromatogromList description]
		 * @method parseMzMLChromatogromList
		 * @param  xml_node                  [description]
		 */
	  void parseMzMLChromatogromList(xml_node);

		/**
		 * [parseMzMLSpectrumList description]
		 * @method parseMzMLSpectrumList
		 * @param  xml_node              [description]
		 */
    void parseMzMLSpectrumList(xml_node);

		/**
		 * [print info about sample]
		 * @method summary
		 */
    void summary();

		/**
		 * [compute min and max values for mz and rt]
		 * @method calculateMzRtRange
		 */
    void calculateMzRtRange();

		/**
		 * [average time difference between scans]
		 * @method getAverageFullScanTime
		 * @return average scan time
		 */
    float getAverageFullScanTime();

		/**
		 * [srm->scan mapping for QQQ]
		 * @method enumerateSRMScans
		 */
    void enumerateSRMScans();

		/**
		 * [correlation in EIC space]]
		 * @method correlation
		 * @param  mz1         [description]
		 * @param  mz2         [description]
		 * @param  ppm         [description]
		 * @param  rt1         [description]
		 * @param  rt2         [description]
		 * @return [description]
		 */
    float correlation(float mz1,  float mz2, float ppm, float rt1, float rt2 );

		/**
		 * [get normalization constant]
		 * @method getNormalizationConstant
		 * @return [normalization constant]
		 */
		float getNormalizationConstant() { return _normalizationConstant; }

		/**
		 * [set Normalization Constant]
		 * @method setNormalizationConstant
		 * @param  x                        [normalization constant]
		 */
    void  setNormalizationConstant(float x) { _normalizationConstant = x; }

		/**
		 * [get indexes for a given scan]
		 * @method getScan
		 * @param  scanNum [number of scans]
		 * @return [pointer to Scan]
		 */
    Scan* getScan(unsigned int scanNum);

		/**
		 * [getAverageScan description]
		 * @method getAverageScan
		 * @param  rtmin          [description]
		 * @param  rtmax          [description]
		 * @param  mslevel        [description]
		 * @param  polarity       [description]
		 * @param  resolution     [description]
		 * @return [pointer to Scan]
		 */
    Scan* getAverageScan(float rtmin, float rtmax, int mslevel, int polarity, float resolution);

		/**
		 * [get eic based on minMz, maxMz, minRt, maxRt,mslevel]
		 * @method getEIC
		 * @param  float  [description]
		 * @param  float  [description]
		 * @param  float  [description]
		 * @param  float  [description]
		 * @param  int    [description]
		 * @return [description]
		 */
    EIC* getEIC(float,float,float,float,int);

		/**
		 * [get eic based on srmId]
		 * @method getEIC
		 * @param  srmId  [description]
		 * @return [description]
		 */
    EIC* getEIC(string srmId);

		/**
		 * [getEIC description]
		 * @method getEIC
		 * @param  precursorMz     [description]
		 * @param  collisionEnergy [description]
		 * @param  productMz       [description]
		 * @param  amuQ1           [description]
		 * @param  amuQ2           [description]
		 * @return [description]
		 */
    EIC* getEIC(float precursorMz, float collisionEnergy, float productMz, float amuQ1, float amuQ2 );

		/**
		 * [get Total Ion Chromatogram]
		 * @method getTIC
		 * @param  float  [description]
		 * @param  float  [description]
		 * @param  int    [description]
		 * @return [description]
		 */
		EIC* getTIC(float,float,int);

    deque <Scan*> scans;
    string sampleName;
    string fileName;
    bool isSelected;
    bool isBlank;

		/**  sample display color, [r,g,b, alpha] */
		float color[4];
    float minMz;
    float maxMz;
    float minRt;
    float maxRt;
    float maxIntensity;
    float minIntensity;
    float totalIntensity;

		/** sample display order */
    int   _sampleOrder;
    bool  _C13Labeled;
    bool  _N15Labeled;

		/**  Feng note: added to track S34 labeling state */
    bool  _S34Labeled;

		/** Feng note: added to track D2 labeling state */
    bool  _D2Labeled;
    float _normalizationConstant;
    string  _setName;

		/**  srm to scan mapping */
    map<string,vector<int> >srmScans;

		/** tags associated with this sample */
    map<string,string> instrumentInfo;

    //saving and restoring retention times

		/** saved retention times prior to alignment */
    vector<float>originalRetentionTimes;

		/**
		 * [save Original Retention Times]
		 * @method saveOriginalRetentionTimes
		 */
    void saveOriginalRetentionTimes();

		/**
		 * [restore Original Retention Times]
		 * @method restoreOriginalRetentionTimes
		 */
		void restoreOriginalRetentionTimes();

    //class functions

		/**
		 * [addScan description]
		 * @method addScan
		 * @param  s       [description]
		 */
    void addScan(Scan*s);

		/**
		 * [getPolarity description]
		 * @method getPolarity
		 * @return [description]
		 */
    inline int getPolarity() { if( scans.size()>0) return scans[0]->getPolarity(); return 0; }

		/**
		 * [scanCount description]
		 * @method scanCount
		 * @return [description]
		 */
    inline unsigned int   scanCount() { return(scans.size()); }

		/**
		 * [getSampleName description]
		 * @method getSampleName
		 * @return [description]
		 */
    inline string getSampleName() { return sampleName; }

		/**
		 * [setSampleOrder description]
		 * @method setSampleOrder
		 * @param  x              [description]
		 */
    void setSampleOrder(int x) { _sampleOrder=x; }

		/**
		 * [getSampleOrder description]
		 * @method getSampleOrder
		 * @return [description]
		 */
    inline int	  getSampleOrder() { return _sampleOrder; }

		/**
		 * [getSetName description]
		 * @method getSetName
		 * @return [description]
		 */
    inline string  getSetName()  { return _setName; }

		/**
		 * [setSetName description]
		 * @method setSetName
		 * @param  x          [description]
		 * @return [description]
		 */
    void   setSetName(string x) { _setName=x; }

		/**
		 * [setSampleName description]
		 * @method setSampleName
		 * @param  x             [description]
		 * @return [description]
		 */
    void   setSampleName(string x) { sampleName=x; }

		/**
		 * [getMaxRt description]
		 * @method getMaxRt
		 * @param  samples  [description]
		 * @return [description]
		 */
    static float getMaxRt(const vector<mzSample*>&samples);

		/**
		 * [C13Labeled description]
		 * @method C13Labeled
		 * @return [description]
		 */
    bool C13Labeled(){ return _C13Labeled; }

		/**
		 * [N15Labeled description]
		 * @method N15Labeled
		 * @return [description]
		 */
    bool N15Labeled(){ return _N15Labeled; }

		/**
		 * [compSampleOrder description]
		 * @method compSampleOrder
		 * @param  a               [description]
		 * @param  b               [description]
		 * @return [description]
		 */
    static bool compSampleOrder(const mzSample* a, const mzSample* b ) { return a->_sampleOrder < b->_sampleOrder; }

		/**
		 * [getMinMaxDimentions description]
		 * @method getMinMaxDimentions
		 * @param  samples             [description]
		 * @return [description]
		 */
		static mzSlice getMinMaxDimentions(const vector<mzSample*>& samples);

		/**
		 * [setFilter_minIntensity description]
		 * @method setFilter_minIntensity
		 * @param  x                      [description]
		 */
    static void setFilter_minIntensity(int x ) { filter_minIntensity=x; }

		/**
		 * [setFilter_centroidScans description]
		 * @method setFilter_centroidScans
		 * @param  x                       [description]
		 */
    static void setFilter_centroidScans( bool x) { filter_centroidScans=x; }

		/**
		 * [setFilter_intensityQuantile description]
		 * @method setFilter_intensityQuantile
		 * @param  x                           [description]
		 */
    static void setFilter_intensityQuantile(int x ) { filter_intensityQuantile=x; }

		/**
		 * [setFilter_mslevel description]
		 * @method setFilter_mslevel
		 * @param  x                 [description]
		 */
		static void setFilter_mslevel(int x ) { filter_mslevel=x; }

		/**
		 * [setFilter_polarity description]
		 * @method setFilter_polarity
		 * @param  x                  [description]
		 */
    static void setFilter_polarity(int x ) { filter_polarity=x; }

		/**
		 * [getFilter_minIntensity description]
		 * @method getFilter_minIntensity
		 * @return [description]
		 */
    static int getFilter_minIntensity() { return filter_minIntensity; }

		/**
		 * [getFilter_intensityQuantile description]
		 * @method getFilter_intensityQuantile
		 * @return [description]
		 */
    static int getFilter_intensityQuantile() { return filter_intensityQuantile; }

		/**
		 * [getFilter_centroidScans description]
		 * @method getFilter_centroidScans
		 * @return [description]
		 */
    static int getFilter_centroidScans() { return filter_centroidScans; }

		/**
		 * [getFilter_mslevel description]
		 * @method getFilter_mslevel
		 * @return [description]
		 */
    static int getFilter_mslevel()  { return filter_mslevel; }

		/**
		 * [getFilter_polarity description]
		 * @method getFilter_polarity
		 * @return [description]
		 */
    static int getFilter_polarity() { return filter_polarity; }


    vector<float> getIntensityDistribution(int mslevel);

    private:
        static int filter_minIntensity;
        static bool filter_centroidScans;
        static int filter_intensityQuantile;
	static int filter_mslevel;
	static int filter_polarity;

};

/**
 * @class EIC
 * @ingroup libmaven
 * @brief Wrapper class for a eic.
 * @author Elucidata
 */
class EIC {

	public:

    EIC() {
        sample=NULL;
        spline=NULL;
        baseline=NULL;
        mzmin=mzmax=rtmin=rtmax=0;
        maxIntensity=totalIntensity=0;
        eic_noNoiseObs=0;
        smootherType=GAUSSIAN;
        baselineSmoothingWindow=5;
        baselineDropTopX=60;
        for(unsigned int i=0; i<4;i++) color[i]=0;
    };

		~EIC();

        enum SmootherType { GAUSSIAN=0, AVG=1, SAVGOL=2 };
        vector <int> scannum;
		vector <float> rt;
		vector <float> mz;
		vector <float> intensity;
		vector <Peak>  peaks;
		string sampleName;

				/** pointer to originating sample */
        mzSample* sample;

				/**  color of the eic line, [r,g,b, alpha] */
				float color[4];

				/**  pointer to smoothed intentsity array */
        float *spline;

									/** pointer to baseline array */
        float *baseline;

				/** maxItensity in eics */
        float maxIntensity;

				/**      sum of all intensities in EIC */
        float totalIntensity;

				/** number of observatiosn above baseline. */
        int   eic_noNoiseObs;

        float mzmin;
        float mzmax;
        float rtmin;
        float rtmax;

			/**
			 * [addPeak description]
			 * @method addPeak
			 * @param  peakPos [description]
			 * @return [description]
			 */
		Peak* addPeak(int peakPos);

		/**
		 * [deletePeak description]
		 * @method deletePeak
		 * @param  i          [description]
		 */
		void deletePeak(unsigned int i);

		/**
		 * [getPeakPositions description]
		 * @method getPeakPositions
		 * @param  smoothWindow     [description]
		 */
        void getPeakPositions(int smoothWindow);

				/**
				 * [getPeakDetails description]
				 * @method getPeakDetails
				 * @param  peak           [description]
				 */
		void getPeakDetails(Peak& peak);

		/**
		 * [getPeakWidth description]
		 * @method getPeakWidth
		 * @param  peak         [description]
		 */
		void getPeakWidth(Peak& peak);

		/**
		 * [computeBaseLine description]
		 * @method computeBaseLine
		 * @param  smoothingWindow [description]
		 * @param  dropTopX        [description]
		 */
        void computeBaseLine(int smoothingWindow, int dropTopX);

				/**
				 * [computeSpline description]
				 * @method computeSpline
				 * @param  smoothWindow  [description]
				 */
        void computeSpline(int smoothWindow);

				/**
				 * [findPeakBounds description]
				 * @method findPeakBounds
				 * @param  peak           [description]
				 */
		void findPeakBounds(Peak& peak);

		/**
		 * [getPeakStatistics description]
		 * @method getPeakStatistics
		 */
		void getPeakStatistics();

		/**
		 * [checkGaussianFit description]
		 * @method checkGaussianFit
		 * @param  peak             [description]
		 */
		void checkGaussianFit(Peak& peak);


		vector<mzPoint> getIntensityVector(Peak& peak);

		/**
		 * [summary description]
		 * @method summary
		 */
		void summary();

		/**
		 * [setSmootherType description]
		 * @method setSmootherType
		 * @param  x               [description]
		 */
        void setSmootherType(EIC::SmootherType x) { smootherType=x; }

				/**
				 * [setBaselineSmoothingWindow description]
				 * @method setBaselineSmoothingWindow
				 * @param  x                          [description]
				 */
        void setBaselineSmoothingWindow(int x) { baselineSmoothingWindow=x;}

				/**
				 * [setBaselineDropTopX description]
				 * @method setBaselineDropTopX
				 * @param  x                   [description]
				 */
        void setBaselineDropTopX(int x) { baselineDropTopX=x; }

				/**
				 * [size description]
				 * @method size
				 * @return [description]
				 */
		inline unsigned int size() { return intensity.size();}

		/**
		 * [getSample description]
		 * @method getSample
		 * @return [description]
		 */
		inline mzSample* getSample() { return sample; }


		static vector<PeakGroup> groupPeaks(vector<EIC*>&eics, int smoothingWindow, float maxRtDiff);
/**
 * [eicMerge description]
 * @method eicMerge
 * @param  eics     [description]
 * @return [description]
 */
		static EIC* eicMerge(const vector<EIC*>& eics);

		/**
		 * [removeLowRankGroups description]
		 * @method removeLowRankGroups
		 * @param  groups              [description]
		 * @param  rankLimit           [description]
		 */
		static void removeLowRankGroups(vector<PeakGroup>&groups, unsigned int rankLimit );

		/**
		 * [compMaxIntensity description]
		 * @method compMaxIntensity
		 * @param  a                [description]
		 * @param  b                [description]
		 * @return [description]
		 */
		static bool compMaxIntensity(EIC* a, EIC* b ) { return a->maxIntensity > b->maxIntensity; }

        private:
                SmootherType smootherType;

								/** baseline smoothin window */
                int baselineSmoothingWindow;

								/** fraction of point to remove when computing baseline */
                int baselineDropTopX;

};

/**
 * @class Peak
 * @ingroup libmaven
 * @brief Wrapper class for a peak.
 * @author Elucidata
 */
class Peak {
	public:
		Peak();

		Peak(EIC* e, int p);
                Peak(const Peak& p);
		Peak& operator=(const Peak& o);
                void copyObj(const Peak& o);

		unsigned int pos;
		unsigned int minpos;
		unsigned int maxpos;

		float rt;
		float rtmin;
		float rtmax;
		float mzmin;
		float mzmax;

		unsigned int scan;
		unsigned int minscan;
		unsigned int maxscan;

		/** non corrected sum of all intensities */
		float peakArea;
		/** baseline substracted area */
		float peakAreaCorrected;
		       	/** top 3 points of the peak */
        	float peakAreaTop;
					        	/** area of the peak divided by total area in the EIC */
                float peakAreaFractional;
								/**  peak rank (sorted by peakAreaCorrected) */
                float peakRank;
								/** not corrected intensity at top of the peak */
		float peakIntensity;
								/** baseline level below the highest point */
                float peakBaseLineLevel;
								/** mz value at the top of the peak */
		float peakMz;
		/** averaged mz value across all points in the peak */
		float medianMz;
		/** mz value across base of the peak */
		float baseMz;
		/** from 0 to 1. indicator of peak goodness */
		float quality;
		/** width of the peak at the baseline */
                unsigned int width;
								/** fit to gaussian curve */
		float gaussFitSigma;
		/** fit to gaussian curve */
		float gaussFitR2;
                int groupNum;

		unsigned int noNoiseObs;
                float noNoiseFraction;
                float symmetry;
                float signalBaselineRatio;
								/** 0 no overlap, 1 perfect overlap */
		float groupOverlap;
		float groupOverlapFrac;

		bool localMaxFlag;

		/** true if peak is from blank sample */
		bool fromBlankSample;

		/** classification label */
		char label;

         private:
					 /** pointer to eic */
		EIC* eic;
				/** pointer to sample */
        mzSample *sample;

	public:
		/**
		 * [setEIC description]
		 * @method setEIC
		 * @param  e      [description]
		 */
		void setEIC(EIC* e) { eic=e; }

		/**
		 * [getEIC description]
		 * @method getEIC
		 * @return [description]
		 */
		inline EIC*	 getEIC() { return eic;    }

		/**
		 * [hasEIC description]
		 * @method hasEIC
		 * @return [description]
		 */
		inline bool hasEIC() { return eic != NULL; }

		/**
		 * [getScan description]
		 * @method getScan
		 * @return [description]
		 */
		Scan* getScan() { if(sample) return sample->getScan(scan); else return NULL; }

		/**
		 * [setSample description]
		 * @method setSample
		 * @param  s         [description]
		 * @return [description]
		 */
		void   setSample(mzSample* s ) { sample=s; }

		/**
		 * [getSample description]
		 * @method getSample
		 * @return [description]
		 */
		inline mzSample* getSample() { return sample; }

		/**
		 * [hasSample description]
		 * @method hasSample
		 * @return [description]
		 */
		inline bool hasSample() 	{  return sample != NULL; }

		/**
		 * [setLabel description]
		 * @method setLabel
		 * @param  label    [description]
		 */
		void setLabel(char label) { this->label=label;}

		/**
		 * [getLabel description]
		 * @method getLabel
		 * @return [description]
		 */
		inline char getLabel() { return label;}


/**
 * [compRt description]
 * @method compRt
 * @param  a      [description]
 * @param  b      [description]
 * @return [description]
 */
		static bool compRt(const Peak& a, const Peak& b ) { return a.rt < b.rt; }

		/**
		 * [compIntensity description]
		 * @method compIntensity
		 * @param  a             [description]
		 * @param  b             [description]
		 * @return [description]
		 */
		static bool compIntensity(const Peak& a, const Peak& b ) { return b.peakIntensity < a.peakIntensity; }

		/**
		 * [compArea description]
		 * @method compArea
		 * @param  a        [description]
		 * @param  b        [description]
		 * @return [description]
		 */
		static bool compArea(const Peak& a, const Peak& b ) { return b.peakAreaFractional < a.peakAreaFractional; }

		/**
		 * [compMz description]
		 * @method compMz
		 * @param  a      [description]
		 * @param  b      [description]
		 * @return [description]
		 */
		static bool compMz(const Peak& a, const Peak& b ) { return a.peakMz < b.peakMz; }

		/**
		 * [compSampleName description]
		 * @method compSampleName
		 * @param  a              [description]
		 * @param  b              [description]
		 * @return [description]
		 */
		static bool compSampleName(const Peak& a, const Peak& b ) { return a.sample->getSampleName() < b.sample->getSampleName(); }

		/**
		 * [compSampleOrder description]
		 * @method compSampleOrder
		 * @param  a               [description]
		 * @param  b               [description]
		 * @return [description]
		 */
		static bool compSampleOrder(const Peak& a, const Peak& b ) { return a.sample->getSampleOrder() < b.sample->getSampleOrder(); }

		/**
		 * [overlap description]
		 * @method overlap
		 * @param  a       [description]
		 * @param  b       [description]
		 * @return [description]
		 */
		inline static float overlap(const Peak& a, const Peak& b) {	return( checkOverlap(a.rtmin, a.rtmax, b.rtmin, b.rtmax)); }
                vector<mzLink> findCovariants();
};

/**
 * @class PeakGroup
 * @ingroup libmaven
 * @brief Wrapper class for a peak group.
 * @author Elucidata
 */
class PeakGroup {

	public:
		enum GroupType {None=0, C13=1, Adduct=2, Fragment=3, Covariant=4, Isotope=5 };     //group types
		enum QType	   {AreaTop=0, Area=1, Height=2, RetentionTime=3, Quality=4, SNRatio=5 };
		PeakGroup();
		PeakGroup(const PeakGroup& o);
		PeakGroup& operator=(const PeakGroup& o);

		bool operator==(const PeakGroup* o);
		/**
		 * [copyObj description]
		 * @method copyObj
		 * @param  o       [description]
		 */
		void copyObj(const PeakGroup& o);

		/**
		 * [copy description]
		 * @method copy
		 * @param  o    [description]
		 */
		void copy(const PeakGroup* o);

		~PeakGroup();

		PeakGroup* parent;
		Compound* compound;

		vector<Peak> peaks;
        deque<PeakGroup> children;

		string srmId;
		string tagString;
		/** classification label */
		char label;
		/**
		 * [compound name + tagString + srmid]
		 * @method getName
		 * @return [description]
		 */
                string getName();

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
		unsigned int blankSampleCount;

		int sampleCount;
		float sampleMean;
		float sampleMax;

		unsigned int maxNoNoiseObs;
                unsigned int  maxPeakOverlap;
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

		/**
		 * [hasSrmId description]
		 * @method hasSrmId
		 * @return [description]
		 */
		bool  	hasSrmId()  { return srmId.empty(); }

		/**
		 * [setSrmId description]
		 * @method setSrmId
		 * @param  id       [description]
		 * @return [description]
		 */
		void  	setSrmId(string id)	  { srmId=id; }

		/**
		 * [getSrmId description]
		 * @method getSrmId
		 * @return [description]
		 */
		inline  string getSrmId() { return srmId; }


		/**
		 * [isPrimaryGroup description]
		 * @method isPrimaryGroup
		 * @return [description]
		 */
		 bool isPrimaryGroup();

		 /**
		  * [hasCompoundLink description]
		  * @method hasCompoundLink
		  * @return [description]
		  */
		inline bool hasCompoundLink()  { if(compound != NULL) return true ; return false; }

/**
 * [isEmpty description]
 * @method isEmpty
 * @return [description]
 */
	inline bool isEmpty()   { if(peaks.size() == 0) return true; return false; }

		/**
		 * [peakCount description]
		 * @method peakCount
		 * @return [description]
		 */
	inline unsigned int peakCount()  { return peaks.size(); 	  }

		/**
		 * [childCount description]
		 * @method childCount
		 * @return [description]
		 */
	inline unsigned int childCount() { return children.size(); }

	/**
	 * [getCompound description]
	 * @method getCompound
	 * @return [description]
	 */
	inline Compound* getCompound() { return compound; }

	/**
	 * [getParent description]
	 * @method getParent
	 * @return [description]
	 */
		inline PeakGroup* getParent() { return parent; }


		inline vector<Peak>& getPeaks() { return peaks; }


		inline deque<PeakGroup>& getChildren()  { return children; }

/**
 * [setParent description]
 * @method setParent
 * @param  p         [description]
 */
		inline void setParent(PeakGroup* p) {parent=p;}

/**
 * [setLabel description]
 * @method setLabel
 * @param  label    [description]
 */
		inline void setLabel(char label) { this->label=label;}

		/**
		 * [ppmDist description]
		 * @method ppmDist
		 * @param  cmass   [description]
		 * @return [description]
		 */
	inline float ppmDist(float cmass) { return mzUtils::ppmDist(cmass,meanMz); }

	/**
	 * [addPeak description]
	 * @method addPeak
	 * @param  peak    [description]
	 */
		inline void addPeak(const Peak& peak) { peaks.push_back(peak); peaks.back().groupNum=groupId; }

/**
 * [addChild description]
 * @method addChild
 * @param  child    [description]
 */
		inline void addChild(const PeakGroup& child) { children.push_back(child); children.back().parent = this;   }

/**
 * [getPeak description]
 * @method getPeak
 * @param  sample  [description]
 * @return [description]
 */

		Peak* getPeak(mzSample* sample);

		GroupType _type;

/**
 * [type description]
 * @method type
 * @return [description]
 */
		inline GroupType type() { return _type; }
/**
 * [setType description]
 * @method setType
 * @param  t       [description]
 */
		inline void setType(GroupType t)  { _type = t; }

/**
 * [isIsotope description]
 * @method isIsotope
 * @return [description]
 */
	inline bool isIsotope() { return _type == Isotope; }

/**
 * [isFragment description]
 * @method isFragment
 * @return [description]
 */
		inline bool isFragment() { return _type == Fragment; }

		/**
		 * [isAdduct description]
		 * @method isAdduct
		 * @return [description]
		 */
	inline bool isAdduct() {  return _type == Adduct; }

	/**
	 * [summary description]
	 * @method summary
	 */
		void summary();

		/**
		 * [groupStatistics description]
		 * @method groupStatistics
		 */
		void groupStatistics();

		/**
		 * [updateQuality description]
		 * @method updateQuality
		 */
		void updateQuality();

		/**
		 * [medianRt description]
		 * @method medianRt
		 * @return [description]
		 */
		float medianRt();

		/**
		 * [meanRtW description]
		 * @method meanRtW
		 * @return [description]
		 */
		float meanRtW();

/**
 * [reduce description]
 * @method reduce
 */
		void reduce();

		/**
		 * [fillInPeaks description]
		 * @method fillInPeaks
		 * @param  eics        [description]
		 */
		void fillInPeaks(const vector<EIC*>& eics);

		/**
		 * [computeAvgBlankArea description]
		 * @method computeAvgBlankArea
		 * @param  eics                [description]
		 */
		void computeAvgBlankArea(const vector<EIC*>& eics);

		/**
		 * [groupOverlapMatrix description]
		 * @method groupOverlapMatrix
		 */
		void groupOverlapMatrix();

/**
 * [getSamplePeak description]
 * @method getSamplePeak
 * @param  sample        [description]
 * @return [description]
 */
		Peak* getSamplePeak(mzSample* sample);

/**
 * [deletePeaks description]
 * @method deletePeaks
 */
		void deletePeaks();

			/**
			 * [deletePeak description]
			 * @method deletePeak
			 * @param  index      [description]
			 * @return [description]
			 */
		bool deletePeak(unsigned int index);

/**
 * [clear description]
 * @method clear
 */
		void clear();

		/**
		 * [deleteChildren description]
		 * @method deleteChildren
		 */
		void deleteChildren();

		/**
		 * [deleteChild description]
		 * @method deleteChild
		 * @param  index       [description]
		 * @return [description]
		 */
		bool deleteChild(unsigned int index);

		/**
		 * [deleteChild description]
		 * @method deleteChild
		 * @param  child       [description]
		 * @return [description]
		 */
		bool deleteChild(PeakGroup* child);

		/**
		 * [copyChildren description]
		 * @method copyChildren
		 * @param  other        [description]
		 */
        void copyChildren(const PeakGroup& other);

		vector<float> getOrderedIntensityVector(vector<mzSample*>& samples, QType type);

		/**
		 * [reorderSamples description]
		 * @method reorderSamples
		 */
		void reorderSamples();

		/**
		 * [compRt description]
		 * @method compRt
		 * @param  a      [description]
		 * @param  b      [description]
		 * @return [description]
		 */
		static bool compRt(const PeakGroup& a, const PeakGroup& b ) { return(a.meanRt < b.meanRt); }

/**
 * [compMz description]
 * @method compMz
 * @param  a      [description]
 * @param  b      [description]
 * @return [description]
 */
								static bool compMz(const PeakGroup& a, const PeakGroup& b ) { return(a.meanMz > b.meanMz); }

								/**
								 * [compIntensity description]
								 * @method compIntensity
								 * @param  a             [description]
								 * @param  b             [description]
								 * @return [description]
								 */
								static bool compIntensity(const PeakGroup& a, const PeakGroup& b ) { return(a.maxIntensity > b.maxIntensity); }

								/**
								 * [compArea description]
								 * @method compArea
								 * @param  a        [description]
								 * @param  b        [description]
								 * @return [description]
								 */
		static bool compArea(const PeakGroup& a, const PeakGroup& b ) { return(a.maxPeakFracionalArea > b.maxPeakFracionalArea); }

		/**
		 * [compQuality description]
		 * @method compQuality
		 * @param  a           [description]
		 * @param  b           [description]
		 * @return [description]
		 */
		static bool compQuality(const PeakGroup& a, const PeakGroup& b ) { return(a.maxQuality > b.maxQuality); }
		//static bool compInfoScore(const PeakGroup& a, const PeakGroup& b ) { return(a.informationScore > b.informationScore); }

		/**
		 * [compRank description]
		 * @method compRank
		 * @param  a        [description]
		 * @param  b        [description]
		 * @return [description]
		 */
			          static bool compRank(const PeakGroup& a, const PeakGroup& b ) { return(a.groupRank > b.groupRank); }

/**
 * [compRankPtr description]
 * @method compRankPtr
 * @param  a           [description]
 * @param  b           [description]
 * @return [description]
 */
								static bool compRankPtr(const PeakGroup* a, const PeakGroup* b ) { return(a->groupRank > b->groupRank); }

/**
 * [compRatio description]
 * @method compRatio
 * @param  a         [description]
 * @param  b         [description]
 * @return [description]
 */
		static bool compRatio(const PeakGroup& a, const PeakGroup& b ) { return(a.changeFoldRatio < b.changeFoldRatio); }

/**
 * [compPvalue description]
 * @method compPvalue
 * @param  a          [description]
 * @param  b          [description]
 * @return [description]
 */
		static bool compPvalue(const PeakGroup* a, const PeakGroup* b ) { return(a->changePValue< b->changePValue); }

/**
 * [compC13 description]
 * @method compC13
 * @param  a       [description]
 * @param  b       [description]
 * @return [description]
 */
								static bool compC13(const PeakGroup* a, const PeakGroup* b) { return(a->isotopeC13count < b->isotopeC13count); }

/**
 * [compMetaGroup description]
 * @method compMetaGroup
 * @param  a             [description]
 * @param  b             [description]
 * @return [description]
 */
								static bool compMetaGroup(const PeakGroup& a, const PeakGroup& b) { return(a.metaGroupId < b.metaGroupId); }
                bool operator< (const PeakGroup* b) { return this->maxIntensity < b->maxIntensity; }
};

class Compound {
        static MassCalculator* mcalc;

		private:
			PeakGroup _group;			//link to peak group
                        bool      _groupUnlinked;

		public:
			Compound(string id, string name, string formula, int charge );
            ~Compound(){}; //empty destructor

            PeakGroup* getPeakGroup() { return &_group; }
			void setPeakGroup(const PeakGroup& group ) { _group = group; _group.compound = this; }
			bool hasGroup()    { if(_group.meanMz != 0 ) return true; return false; }
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
            static bool compMass(const Compound* a, const Compound* b )      { return(a->mass < b->mass);       }
            static bool compName(const Compound* a, const Compound* b )    { return(a->name < b->name);       }
            static bool compFormula(const Compound* a, const Compound* b ) { return(a->formula < b->formula); }
            static bool compReactionCount(const Compound* a, const Compound* b ) { return(a->reactions.size() < b->reactions.size()); }


};

class Pathway {
		public:
		Pathway( string id, string name) {  this->id=id; this->name=name; }
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

    Isotope(string name, float mass, int c=0, int n=0, int s=0, int h=0) {
        this->mass=mass; this->name=name;
        C13=c; N15=n; S34=s; H2=h;
    }

    Isotope() {
        mass=0; abundance=0; N15=0; C13=0; S34=0; H2=0;
    }

    Isotope(const Isotope& b) {
        name=b.name;
        mass=b.mass;
        abundance=b.abundance;
        N15=b.N15; S34=b.S34; C13=b.C13; H2=b.H2;
    }

    Isotope& operator=(const Isotope& b) {
        name=b.name; mass=b.mass; abundance=b.abundance;
        N15=b.N15; S34=b.S34; C13=b.C13; H2=b.H2;
        return *this;
    }

};

class  Reaction{
		public:
		Reaction(string db, string id, string name) { this->db  = db; this->id =  id; this->name = name; reversable=false; }
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
		Adduct(){ isParent=false; mass=0; charge=1; nmol=1; }
		string name;
		int	  nmol;
		float mass;
		float charge;
		bool isParent;

        //given adduct mass compute parent ion mass
		inline float computeParentMass(float mz)  { return  (mz*abs(charge)-mass)/nmol; }
        //given perent compute adduct mass
		inline float computeAdductMass(float pmz) { return (pmz*nmol+mass)/abs(charge); }
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
		void setMaxItterations(int x) { maxItterations=x; }
		void setPolymialDegree(int x) { polynomialDegree=x; }
	private:
		vector< vector<float> > fit;
		vector<mzSample*> samples;
		vector<PeakGroup*> allgroups;
		int maxItterations;
		int polynomialDegree;

};

/**
 * @class ChargedSpecies
 * @ingroup libmaven
 * @brief Wrapper class for a charged species.
 * @author Elucidata
 */
class ChargedSpecies{
public:
    ChargedSpecies(){
        deconvolutedMass=0; minZ=0; maxZ=0; countMatches=0; error=0; upCount=0; downCount=0; scan=NULL; totalIntensity=0; isotopicClusterId=0;
        minRt=maxRt=meanRt=0; istotopicParentFlag=false; minIsotopeMass=0; maxIsotopeMass=0; massDiffectCluster=-100; filterOutFlag=false;
        qscore=0;
    }

		/**
		 * parent mass guess
		 */
    float deconvolutedMass;
    float error;
    int minZ;
    int maxZ;
    int countMatches;
    float totalIntensity;
    float qscore;
    int upCount;
    int downCount;
    Scan* scan;


    vector<float> observedMzs;
    vector<float> observedIntensities;
    vector<float> observedCharges;

    int isotopicClusterId;
    int massDiffectCluster;
    bool istotopicParentFlag;
    bool filterOutFlag;

    float meanRt;
    float minRt;
    float maxRt;

    vector<ChargedSpecies*> isotopes;
    int minIsotopeMass;
    int maxIsotopeMass;

		/**
		 * [compIntensity description]
		 * @method compIntensity
		 * @param  a             [description]
		 * @param  b             [description]
		 * @return [description]
		 */
    static bool compIntensity(ChargedSpecies* a, ChargedSpecies* b ) { return a->totalIntensity > b->totalIntensity; }

		/**
		 * [compMass description]
		 * @method compMass
		 * @param  a        [description]
		 * @param  b        [description]
		 * @return [description]
		 */
		static bool compMass(ChargedSpecies* a, ChargedSpecies* b ) { return a->deconvolutedMass < b->deconvolutedMass; }

		/**
		 * [compMatches description]
		 * @method compMatches
		 * @param  a           [description]
		 * @param  b           [description]
		 * @return [description]
		 */
		static bool compMatches(ChargedSpecies* a, ChargedSpecies* b ) { return a->countMatches > b->countMatches; }

		/**
		 * [compRt description]
		 * @method compRt
		 * @param  a      [description]
		 * @param  b      [description]
		 * @return [description]
		 */
		static bool compRt(ChargedSpecies* a, ChargedSpecies* b ) { return a->meanRt < b->meanRt; }

		/**
		 * [compMetaGroup description]
		 * @method compMetaGroup
		 * @param  a             [description]
		 * @param  b             [description]
		 * @return [description]
		 */
    static bool compMetaGroup(ChargedSpecies* a, ChargedSpecies* b ) {
        return (a->isotopicClusterId * 100000 + a->deconvolutedMass  < b->isotopicClusterId* 100000 + b->deconvolutedMass);
    }

		/**
		 * [updateIsotopeStatistics description]
		 * @method updateIsotopeStatistics
		 */
    void updateIsotopeStatistics() {
        minIsotopeMass = maxIsotopeMass =deconvolutedMass;
        for(unsigned int i=0; i < isotopes.size(); i++) {
            if (isotopes[i]->deconvolutedMass < minIsotopeMass) minIsotopeMass=isotopes[i]->deconvolutedMass;
            if (isotopes[i]->deconvolutedMass > maxIsotopeMass) maxIsotopeMass=isotopes[i]->deconvolutedMass;
        }
    }

};


#endif

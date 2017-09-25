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
#include <limits.h>
#include <float.h>
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
#include "EIC.h"
#include "Scan.h"
#include <QRegExp>
#include <QString>
#include <QStringList>

#ifdef ZLIB
#include <zlib.h>
#endif

#ifdef CDFPARSER
#include "ms10.h"
#endif

#if defined(WIN32) || defined(WIN64)
#ifndef strncasecmp
#define strncasecmp strnicmp
#endif
#define isnanwin(x) ((x) = (x))
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
* @brief Store point in rt, intensity and mz space (3 Dimensional)
* 
* @details The data we get from Mass Spectrometry has 3 datatypes, namely
* rt (retention time), intensity and mz (Mass by charge ratio). Class mzPoint
* stores a point in 3-D space where x, y and z are rt, intensity and mz axis
*
*/
class mzPoint
{
  public:
    /**
    * @brief Constructor with default values
    */
    mzPoint() { x = y = z = 0; }

    /**
    * @brief Constructor with input values
    * @param ix Value in x axis
    * @param iy Value in y axis
    * @param iz Value in z axis
    */
    mzPoint(double ix, double iy, double iz)
    {
        x = ix;
        y = iy;
        z = iz;
    }

    mzPoint &operator=(const mzPoint &b)
    {
        x = b.x;
        y = b.y;
        z = b.z;
        return *this;
    }

    /**
    * @brief Compare point in x plane
    * @param a object of class mzPoint
    * @param b object of class mzPoint
    * @return bool True if b is greater than a in x plane
    */
    static bool compX(const mzPoint &a, const mzPoint &b)
    {
        return a.x < b.x;
    }

    /**
    * @brief Compare point in y plane
    * @param a object of class mzPoint
    * @param b object of class mzPoint
    * @return bool True if b is greater than a in y plane
    */
    static bool compY(const mzPoint &a, const mzPoint &b)
    {
        return a.y < b.y;
    }

    /**
    * @brief Compare point in z plane
    * @param a object of class mzPoint
    * @param b object of class mzPoint
    * @return bool True if b is greater than a in z plane
    */
    static bool compZ(const mzPoint &a, const mzPoint &b)
    {
        return a.z < b.z;
    }

    double x, y, z;
};

/**
* @brief Stores a slice in mz and rt plane
*
* @details An mzSlice is a slice in mz and rt plane. This class
* stores an mzSlice and also provides couple of utility functions
* for a mzSlice
*
*/
class mzSlice
{
  public:
    /**
    * @brief Constructor for class mzSlice having rt and mz range
    * @param a mz minimum of a mzSlice
    * @param b mz maximum of a mzSlice
    * @param c rt minimum of a mzSlice
    * @param d rt maximum of a mzSlice
    */
    mzSlice(float a, float b, float c, float d)
    {
        mzmin = a;
        mzmax = b;
        rtmin = c;
        rtmax = d;
        mz = a + (b - a) / 2;
        rt = c + (d - c) / 2;
        compound = NULL;
        ionCount = 0;
    }

    /**
    * @brief Constructor for class mzSlice having a filterline
    * @param filterLine srm Id of a scan in a sample
    * @see Scan:filterLine
    */
    mzSlice(string filterLine)
    {
        mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
        compound = NULL;
        srmId = filterLine;
    }

    /**
    * @brief Constructor for class mzSlice having no inputs parameters
    */
    mzSlice()
    {
        mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
        compound = NULL;
    }

    mzSlice &operator=(const mzSlice &b)
    {
        mzmin = b.mzmin;
        mzmax = b.mzmax;
        rtmin = b.rtmin;
        rtmax = b.rtmax;
        ionCount = b.ionCount;
        compound = b.compound;
        srmId = b.srmId;
        mz = b.mz;
        rt = b.rt;
        return *this;
    }

    /**
    * Average rt of a mzSlice. Zero in case mzSlice is based on filterline
    * or constructor is empty
    */
    float rt;

    /**
    * Average mz of a mzSlice. Zero in case mzSlice is based on filterline
    * or constructor is empty
    */
    float mz;
    float mzmin;
    float mzmax;
    float rtmin;
    float rtmax;
    float ionCount;
    Compound *compound;
    string srmId;

    /**
    * @brief Compare total intensity (ion count) of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower intensity than mzSlice b
    */
    static bool compIntensity(const mzSlice *a, const mzSlice *b)
    {
        return b->ionCount < a->ionCount;
    }

    /**
    * @brief Compare average m/z of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower average m/z than mzSlice b
    */
    static bool compMz(const mzSlice *a, const mzSlice *b)
    {
        return a->mz < b->mz;
    }

    /**
    * @brief Compare average rt of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower average rt than mzSlice b
    */
    static bool compRt(const mzSlice *a, const mzSlice *b)
    {
        return a->rt < b->rt;
    }

    /**
    * @brief operator overloading for less than operator in class mzSlice
    * @param b object of class mzSlice
    * @return True if average m/z of input mzSlice object is greater than
    * current mzSlice object
    */
    bool operator<(const mzSlice *b) const
    {
        return mz < b->mz;
    }

    /**
    * @brief Calculate mzmin and mzmax of mzSlice using mass accuracy (in ppm)
    * @details mzmin and mzmax of an mzSlice are calculated using compound m/z
    * and mass accuracy (in ppm). Compound m/z is being calculated using formula
    * or taken from Compound class if formula is not present
    * @param CompoundppmWindow Mass accuracy in ppm (parts per million)
    * @param charge Charge of the compound
    * @return bool True if compound mass exists
    */
    bool calculateMzMinMax(float CompoundppmWindow, int charge);

    /**
    * @brief Calculate rtmin and rtmax of mzSlice using rt window
    * @param matchRtFlag This flag is true if retention time in compound database
    * is being used
    * @param compoundRTWindow Retention time window for matching Rt from compound
    * database
    */
    void calculateRTMinMax(bool matchRtFlag, float compoundRTWindow);

    /**
    * @brief Set Srm Id of mzslice using the srm Id of compound
    */
    void setSRMId();
};

/**
 * @class mzLink
 * @ingroup libmaven
 * @brief Wrapper class for a link between two mzs.  @author Elucidata
 */
class mzLink
{
  public:
    float mz1;
    float mz2;
    void *data1;
    void *data2;
    float value1;
    float value2;
    string note;
    mzLink();
    mzLink(int a, int b, string n);
    mzLink(float a, float b, string n);
    ~mzLink() {}
    float correlation;                                                             /** [compare m/z of two links] @method compMz @param  a             [Link 1] @param  b             [Link 2] @return [True if Link 1 has lower m/z than Link b, else false] */
    static bool compMz(const mzLink &a, const mzLink &b) { return a.mz1 < b.mz1; } /** [compare correlation of two links] @method compMz @param  a             [Link 1] @param  b             [Link 2] @return [True if Link 1 has lower correlation than Link b, else false] */
    static bool compCorrelation(const mzLink &a, const mzLink &b) { return a.correlation > b.correlation; }
}; /** @class mzSample @ingroup libmaven @brief Wrapper class for a sample.  @author Elucidata */
class mzSample
{
  private:
    void sampleNaming(const char *filename);
    void checkSampleBlank(const char *filename);

    void setInstrumentSettigs(xml_document &doc, xml_node spectrumstore);

    void parseMzXMLData(xml_document &staticdoc, xml_node spectrumstore);

    xml_node getmzXMLSpectrumData(xml_document &doc, const char *filename);

    float parseRTFromMzXML(xml_attribute &attr);

    static int parsePolarityFromMzXML(xml_attribute &attr);

    static int getPolarityFromfilterLine(string filterLine);

    vector<float> parsePeaksFromMzXML(const xml_node &scan);

    void populateMzAndIntensity(vector<float> mzint, Scan *_scan);

    void populateFilterline(string filterLine, Scan *_scan);

    void loadAnySample(const char *filename);

    //TODO: This should be moved
    static string getFileName(const string &filename);

  public:
    mzSample();
    ~mzSample();

    /**
             * [load from file]
             * @method loadSample
             * @param  filename   [input string filename]
             */
    void loadSample(const char *filename);

    /**
             * [load data from mzData file]
             * @method parseMzData
             * @param  char        [mzData file]
             */
    void parseMzData(const char *);

    /**
             * [load data from mzCSV file]
             * @method parseMzData
             * @param  char        [mzCSV file]
             */
    void parseMzCSV(const char *);

    /**
             * [load data from mzXML file]
             * @method parseMzXML
             * @param  char*        [mzXML file]
             */
    void parseMzXML(const char *);

    /**
             * [load data from mzML file]
             * @method parseMzML
             * @param  char*        [mzML file]
             */
    void parseMzML(const char *);

    /**
             * [load netcdf file]
             * @method parseCDF
             * @param  filename   [netcdf file]
             * @param  is_verbose [verbose or not]
             * @return            [int]
             */
    int parseCDF(const char *filename, int is_verbose);

    /**
             * [parse individual scan]
             * @method parseMzXMLScan
             * @param  scan           [scan]
             * @param  scannum        [scan number]
             */
    void parseMzXMLScan(const xml_node &scan, int scannum);

    /**
             * [write mzCSV file]
             * @method writeMzCSV
             * @param  char*           [character pointer]
             */
    void writeMzCSV(const char *);

    static map<string, string> mzML_cvParams(xml_node node);

    /**
             * [parse MzML Chromatogrom List]
             * @method parseMzMLChromatogromList
             * @param  xml_node                  [xml node]
             */
    void parseMzMLChromatogromList(xml_node);

    /**
             * [parse MzML Spectrum List]
             * @method parseMzMLSpectrumList
             * @param  xml_node              [xml node]
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
             * [find correlation in EIC space]
             * @method correlation
             * @param  mz1         [m/z for first sample]
             * @param  mz2         [m/z for second sample]
             * @param  ppm         [ppm window]
             * @param  rt1         [retention time for first sample]
             * @param  rt2         [retention time for second sample]
             * @return [correlation]
             */
    float correlation(float mz1, float mz2, float ppm, float rt1, float rt2, int eicType, string filterline);

    /**
             * [get normalization constant]
             * @method getNormalizationConstant
             * @return [normalization constant]
             */
    float getNormalizationConstant() const { return _normalizationConstant; }

    /**
             * [set Normalization Constant]
             * @method setNormalizationConstant
             * @param  x                        [normalization constant]
             */
    void setNormalizationConstant(float x) { _normalizationConstant = x; }

    /**
             * [get indexes for a given scan]
             * @method getScan
             * @param  scanNum [number of scans]
             * @return [pointer to Scan]
             */
    Scan *getScan(unsigned int scanNum);

    /**
             * [get Average Scan]
             * @method getAverageScan
             * @param  rtmin          [minimum retention time]
             * @param  rtmax          [maximum retention time]
             * @param  mslevel        [MS Level]
             * @param  polarity       [ionization mode/polarity]
             * @param  resolution     [mass resolution of the MS machine]
             * @return [pointer to Scan]
             */
    Scan *getAverageScan(float rtmin, float rtmax, int mslevel, int polarity, float resolution);

    /**
             * [get eic based on minMz, maxMz, minRt, maxRt,mslevel]
             * @param  mzmin   [minimum m/z]
             * @param  mzmax   [maximum m/z]
             * @param  rtmin   [minimum retention time]
             * @param  rtmax   [maximum retention time]
             * @param  mslevel [MS Level]
             * @return         [pointer to EIC]
             */
    EIC *getEIC(float mzmin, float mzmax, float rtmin, float rtmax, int mslevel, int eicType, string filterline);

    /**
             * [get eic based on srmId]
             * @method getEIC
             * @param  srmId  [single reaction monitoring ID]
             * @return [pointer to EIC]
             */
    EIC *getEIC(string srmId, int eicType);

    /**
             * [get EIC]
             * @method getEIC
             * @param  precursorMz     [m/z of precur Ion]
             * @param  collisionEnergy [collision Energy]
             * @param  productMz       [m/z of product Ion]
             * @param  amuQ1           [amu in Q1 step]
             * @param  amuQ2           [amu in Q2 step]
             * @return [pointer to EIC]
             */
    EIC *getEIC(float precursorMz, float collisionEnergy, float productMz, int eicType, string filterline, float amuQ1, float amuQ2);

    /**
             * [get Total Ion Chromatogram]
             * @param  rtmin   [minimum retention time]
             * @param  rtmax   [maximum retention time]
             * @param  mslevel [MS level of the MS machine]
             * @return         [pointer to the EIC]
             */
    EIC *getTIC(float rtmin, float rtmax, int mslevel);

    // TODO: Sahil this change is made from merging EIC wisget
    EIC *getBIC(float, float, int); // Base peak chromatogram

    deque<Scan *> scans;
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
    int _sampleOrder;
    bool _C13Labeled;
    bool _N15Labeled;

    /**  Feng note: added to track S34 labeling state */
    bool _S34Labeled;

    /** Feng note: added to track D2 labeling state */
    bool _D2Labeled;
    float _normalizationConstant;
    string _setName;
    int injectionOrder;

    /**  srm to scan mapping */
    map<string, vector<int>> srmScans;

    /** tags associated with this sample */
    map<string, string> instrumentInfo;

    //saving and restoring retention times

    /** saved retention times prior to alignment */
    vector<float> originalRetentionTimes;

    vector<double> polynomialAlignmentTransformation; //parameters for polynomial transform

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

    void applyPolynomialTransform(); //TODO: Sahil, Added while merging projectdockwidget

    //class functions

    /**
             * [add Scan]
             * @method addScan
             * @param  s       [scan]
             */
    void addScan(Scan *s);

    /**
             * [get Polarity]
             * @method getPolarity
             * @return [polarity of the scan]
             */
    int getPolarity();

    /**
             * [scan Count]
             * @method scanCount
             * @return [scan count]
             */
    inline unsigned int scanCount() const { return (scans.size()); }

    /**
             * [get Sample Name]
             * @method getSampleName
             * @return [name of the sample]
             */
    inline string getSampleName() const { return sampleName; }

    /**
             * [set Sample Order]
             * @method setSampleOrder
             * @param  x              [sample order]
             */
    void setSampleOrder(int x) { _sampleOrder = x; }

    /**
             * [get Sample Order]
             * @method getSampleOrder
             * @return [sample order]
             */
    inline int getSampleOrder() const { return _sampleOrder; }

    /**
              * @brief the order in  which the samples are inserted into the LC column.
              * @method setInjectionOrder
              * @param time
              */
    void setInjectionOrder(int order)
    {
        injectionOrder = order;
    }

    /**
              * @brief return the injection order
              * @method getInjectionOrder
              * @return injectionOrder
              */

    int getInjectionOrder()
    {
        return injectionOrder;
    }
    /**
             * [get Set Name]
             * @method getSetName
             * @return [set name]
             */
    inline string getSetName() const { return _setName; }

    /**
             * [set set Name]
             * @method setSetName
             * @param  x          [set name]
             */
    void setSetName(string x) { _setName = x; }

    /**
             * [set Sample Name]
             * @method setSampleName
             * @param  x             [sample name]
             */
    void setSampleName(string x) { sampleName = x; }

    /**
             * [get Max retention time]
             * @method getMaxRt
             * @param  samples  [vector of pointer to mzSample]
             * @return [maximum retention time]
             */
    static float getMaxRt(const vector<mzSample *> &samples);

    /**
             * [C13Labeled?]
             * @method C13Labeled
             * @return [true or false]
             */
    bool C13Labeled() const { return _C13Labeled; }

    /**
             * [N15Labeled]
             * @method N15Labeled
             * @return [true or false]
             */
    bool N15Labeled() const { return _N15Labeled; }

    /**
             * [compare Sample Order]
             * @method compSampleOrder
             * @param  a               [sample a]
             * @param  b               [sample b]
             * @return [true if sample order for sample a is less than b else false]
             */
    static bool compSampleOrder(const mzSample *a, const mzSample *b) { return a->_sampleOrder < b->_sampleOrder; }

    static bool compSampleSort(const mzSample *a, const mzSample *b) { return mzUtils::strcasecmp_withNumbers(a->sampleName, b->sampleName); }

    /**
             * [getMinMaxDimentions ]
             * @method getMinMaxDimentions
             * @param  samples             []
             * @return []
             */
    static mzSlice getMinMaxDimentions(const vector<mzSample *> &samples);

    /**
             * [setFilter_minIntensity ]
             * @method setFilter_minIntensity
             * @param  x                      []
             */
    static void setFilter_minIntensity(int x) { filter_minIntensity = x; }

    /**
             * [setFilter_centroidScans ]
             * @method setFilter_centroidScans
             * @param  x                       []
             */
    static void setFilter_centroidScans(bool x) { filter_centroidScans = x; }

    /**
             * [setFilter_intensityQuantile ]
             * @method setFilter_intensityQuantile
             * @param  x                           []
             */
    static void setFilter_intensityQuantile(int x) { filter_intensityQuantile = x; }

    /**
             * [setFilter_mslevel ]
             * @method setFilter_mslevel
             * @param  x                 []
             */
    static void setFilter_mslevel(int x) { filter_mslevel = x; }

    /**
             * [setFilter_polarity ]
             * @method setFilter_polarity
             * @param  x                  []
             */
    static void setFilter_polarity(int x) { filter_polarity = x; }

    /**
             * [getFilter_minIntensity ]
             * @method getFilter_minIntensity
             * @return []
             */
    static int getFilter_minIntensity() { return filter_minIntensity; }

    /**
             * [getFilter_intensityQuantile ]
             * @method getFilter_intensityQuantile
             * @return []
             */
    static int getFilter_intensityQuantile() { return filter_intensityQuantile; }

    /**
             * [getFilter_centroidScans ]
             * @method getFilter_centroidScans
             * @return []
             */
    static int getFilter_centroidScans() { return filter_centroidScans; }

    /**
             * [getFilter_mslevel ]
             * @method getFilter_mslevel
             * @return []
             */
    static int getFilter_mslevel() { return filter_mslevel; }

    /**
             * [getFilter_polarity ]
             * @method getFilter_polarity
             * @return []
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

class Pathway
{
  public:
    Pathway(string id, string name)
    {
        this->id = id;
        this->name = name;
    }
    string id;
    string name;
    vector<Reaction *> reactions;
};

class Isotope
{
  public:
    string name;
    double mass;
    double abundance;
    int N15;
    int C13;
    int S34;
    int H2;

    Isotope(string name, float mass, int c = 0, int n = 0, int s = 0, int h = 0)
    {
        this->mass = mass;
        this->name = name;
        C13 = c;
        N15 = n;
        S34 = s;
        H2 = h;
        abundance = 0; //naman: was unintialized
    }

    Isotope()
    {
        mass = 0;
        abundance = 0;
        N15 = 0;
        C13 = 0;
        S34 = 0;
        H2 = 0;
    }

    Isotope(const Isotope &b)
    {
        name = b.name; //naman: Consider performing initialization in initialization list.
        mass = b.mass;
        abundance = b.abundance;
        N15 = b.N15;
        S34 = b.S34;
        C13 = b.C13;
        H2 = b.H2;
    }

    Isotope &operator=(const Isotope &b)
    {
        name = b.name;
        mass = b.mass;
        abundance = b.abundance;
        N15 = b.N15;
        S34 = b.S34;
        C13 = b.C13;
        H2 = b.H2;
        return *this;
    }
};

class Reaction
{
  public:
    Reaction(string db, string id, string name)
    {
        this->db = db;
        this->id = id;
        this->name = name;
        reversable = false;
    }
    void addReactant(Compound *r, int s)
    {
        reactants.push_back(r);
        stoichiometry[r] = s;
    }
    void addProduct(Compound *p, int s)
    {
        products.push_back(p);
        stoichiometry[p] = s;
    }
    void setReversable(bool r) { reversable = r; }

    string db;
    string id;
    string name;
    deque<Compound *> reactants;
    deque<Compound *> products;
    map<Compound *, int> stoichiometry;
    bool reversable;
};

class Adduct
{

  public:
    Adduct()
    {
        isParent = false;
        mass = 0;
        charge = 1;
        nmol = 1;
    }
    string name;
    int nmol;
    float mass;
    float charge;
    bool isParent;

    //given adduct mass compute parent ion mass
    inline float computeParentMass(float mz) { return (mz * abs(charge) - mass) / nmol; }
    //given perent compute adduct mass
    inline float computeAdductMass(float pmz) { return (pmz * nmol + mass) / abs(charge); }
};

/**
 * @class ChargedSpecies
 * @ingroup libmaven
 * @brief Wrapper class for a charged species.
 * @author Elucidata
 */
class ChargedSpecies
{
  public:
    ChargedSpecies()
    {
        deconvolutedMass = 0;
        minZ = 0;
        maxZ = 0;
        countMatches = 0;
        error = 0;
        upCount = 0;
        downCount = 0;
        scan = NULL;
        totalIntensity = 0;
        isotopicClusterId = 0;
        minRt = maxRt = meanRt = 0;
        istotopicParentFlag = false;
        minIsotopeMass = 0;
        maxIsotopeMass = 0;
        massDiffectCluster = -100;
        filterOutFlag = false;
        qscore = 0;
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
    Scan *scan;

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

    vector<ChargedSpecies *> isotopes;
    int minIsotopeMass;
    int maxIsotopeMass;

    /**
         * [compIntensity ]
         * @method compIntensity
         * @param  a             []
         * @param  b             []
         * @return []
         */
    static bool compIntensity(ChargedSpecies *a, ChargedSpecies *b) { return a->totalIntensity > b->totalIntensity; }

    /**
         * [compMass ]
         * @method compMass
         * @param  a        []
         * @param  b        []
         * @return []
         */
    static bool compMass(ChargedSpecies *a, ChargedSpecies *b) { return a->deconvolutedMass < b->deconvolutedMass; }

    /**
         * [compMatches ]
         * @method compMatches
         * @param  a           []
         * @param  b           []
         * @return []
         */
    static bool compMatches(ChargedSpecies *a, ChargedSpecies *b) { return a->countMatches > b->countMatches; }

    /**
         * [compRt ]
         * @method compRt
         * @param  a      []
         * @param  b      []
         * @return []
         */
    static bool compRt(ChargedSpecies *a, ChargedSpecies *b) { return a->meanRt < b->meanRt; }

    /**
         * [compMetaGroup ]
         * @method compMetaGroup
         * @param  a             []
         * @param  b             []
         * @return []
         */
    static bool compMetaGroup(ChargedSpecies *a, ChargedSpecies *b)
    {
        return (a->isotopicClusterId * 100000 + a->deconvolutedMass < b->isotopicClusterId * 100000 + b->deconvolutedMass);
    }

    /**
         * [updateIsotopeStatistics ]
         * @method updateIsotopeStatistics
         */
    void updateIsotopeStatistics()
    {
        minIsotopeMass = maxIsotopeMass = deconvolutedMass;
        for (unsigned int i = 0; i < isotopes.size(); i++)
        {
            if (isotopes[i]->deconvolutedMass < minIsotopeMass)
                minIsotopeMass = isotopes[i]->deconvolutedMass;
            if (isotopes[i]->deconvolutedMass > maxIsotopeMass)
                maxIsotopeMass = isotopes[i]->deconvolutedMass;
        }
    }
};

#endif

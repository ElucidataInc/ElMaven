#ifndef MZSAMPLE_H
#define MZSAMPLE_H

#include <chrono_io.h>
#include <date.h>

#include "assert.h"
#include "mzUtils.h"
#include "pugixml.hpp"
#include "standardincludes.h"

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

class Scan;
class Peak;
class PeakGroup;
class EIC;
class Compound;
class Adduct;
class mzLink;
class mzSlice;
class MassCalculator;
class MassCutoff;
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
* @brief Link two mass by charge ratios
*/
class mzLink
{
  public:
    /**
    * @brief Constructor for mzLink with no input arguments
    */
    mzLink();

    /**
    * @brief Constructor for mzLink with integer mzs
    */
    mzLink(int a, int b, string n);

    /**
    * @brief Constructor for mzLink with float mzs
    */
    mzLink(float a, float b, string n);

    /**
    * @brief Destructor for mzLink
    */
    ~mzLink() {}

    float mz1;
    float mz2;
    void *data1;
    void *data2;
    float value1;
    float value2;
    string note;
    float correlation;

    /**
    * @brief Compare m/z of two mzLinks
    * @param a mzLink 1
    * @param b mzLink 2
    * @return True if mzLink 1 has lower m/z than mzLink b, else false
    */
    static bool compMz(const mzLink &a, const mzLink &b)
    {
        return a.mz1 < b.mz1;
    }

    /**
    * @brief Compare correlation of two links
    * @param a mzLink 1
    * @param b mzLink 2
    * @return True if Link 1 has lower correlation than Link b, else false
    */
    static bool compCorrelation(const mzLink &a, const mzLink &b)
    {
        return a.correlation > b.correlation;
    }
};

/** 
* @brief Parses input sample files and stores related metadata
*
* @details Loads and parses sample files from mass spectrometry. Supported file
* formats: .mzxml, .mzml, .mzdata, .mzcsv and .cdf
*
*/
class mzSample
{

  public:
    /**
    * @brief Constructor for class mzSample
    */
    mzSample();

    /**
    * @brief Destructor for class mzSample
    */
    ~mzSample();

    /**
    * @brief Load sample (supported formats: .mzxml, .mzml, .mzdata, .mzcsv and .cdf)
    * @param filename Sample file name
    */

    void loadSample(string filename);

    /**
    * @brief Parse mzData file format
    * @param char* mzData file name
    */
    void parseMzData(const char *);

    /**
    * @brief Parse mzCSV file format
    * @param char* mzCSV file name
    */
    void parseMzCSV(const char *);

    /**
    * @brief Parse mzXML file format
    * @param char* mzXML file name
    */
    void parseMzXML(const char *);

    /**
    * @brief Parse mzML file format
    * @param char* mzML file name
    */
    void parseMzML(const char *);

    /**
    * @brief Parse netcdf file format
    * @param filename netcdf file name
    * @param is_verbose verbose or not
    * @return Returns 0 if error in loading else 1
    */
    int parseCDF(const char *filename, int is_verbose);

    /**
    * @brief Parse scan in mzXml file format
    * @param scan xml_node object of pugixml library
    * @param scannum scan number
    */
    void parseMzXMLScan(const xml_node &scan, const int& scannum);

    /**
    * @brief Write mzCSV file
    * @param char* mzCSV file name
    */
    void writeMzCSV(const char *);

    /**
    * @brief Get cv parameters of an xml_node from mzML format
    * @param node xml_node object of pugixml lirary
    * @return Returns a map of name and corresponding value of an xml_node object
    */
    static map<string, string> mzML_cvParams(xml_node node);

    /**
     * @brief Update injection time stamp
     * @param xml_node xml_node object of pugixml library
     */
    void parseMzMLInjectionTimeStamp(const xml_attribute&);

    /**
    * @brief Parse mzML chromatogrom list
    * @param xml_node xml_node object of pugixml library
    */
    void parseMzMLChromatogramList(const xml_node&);


    int getSampleNoChromatogram(const string &chromatogramId);

    void cleanFilterLine(string &filterline);

    /**
    * @brief Parse mzML spectrum list
    * @param xml_node xml_node object of pugixml library
    */
    void parseMzMLSpectrumList(const xml_node&);

    /**
    * @brief Print info about sample 
    * @details Print data of sample: 1. Number of observations 2. rt range
    * 3. mz range
    */
    void summary();

    /**
    * @brief Compute min and max values for mz and rt
    * @details Compute min and max values for mz and rt by iterating over
    * the scans. Also while iterating over scans, calculate min and max
    * Intensity
    */
    void calculateMzRtRange();

    /**
    * @brief Average time difference between scans
    * @return Average scan time
    */
    float getAverageFullScanTime();

    /**
    * @brief Map scan numbers to filterline
    * @details Update map srmScans where key is the filterline and value is int vector.
    * int vector contains scan numbers
    * @see mzSample:srmScans
    */
    void enumerateSRMScans();

    /**
    * @brief Find correlation between two EICs
    * @param mz1 m/z for first EIC
    * @param mz2 m/z for second EIC
    * @param ppm ppm window
    * @param rt1 Retention time for first EIC
    * @param rt2 Retention time for second EIC
    * @param eicType Type of EIC (max or sum)
    * @param filterline selected filterline
    * @return correlation
    */
    float correlation(float mz1, float mz2, MassCutoff *massCutoff, float rt1, float rt2, int eicType, string filterline);

    /**
     * @brief Find correlation between two EICs from different mass ranges but
     * the same time range. Useful for finding correlation between co-eluents.
     * @param mzMin1 The lower limit for first m/z range.
     * @param mzMax1 The upper limit for first m/z range.
     * @param mzMin2 The lower limit for first m/z range.
     * @param mzMax2 The upper limit for first m/z range.
     * @param rtMin The lower limit for the common RT range.
     * @param rtMax The upper limit for the common RT range.
     * @param msLevel MS level for which the chromatograms will be pulled.
     * @param eicType Type of EIC (max or sum).
     * @param filterline Selected filterline.
     * @return A floating point number denoting the correlation between the two
     * extracted chromatograms.
     */
    float correlation(float mzMin1,
                      float mzMax1,
                      float mzMin2,
                      float mzMax2,
                      float rtMin,
                      float rtMax,
                      int msLevel,
                      int eicType,
                      string filterline);

    /**
    * @brief Get normalization constant
    * @return Normalization constant
    */
    float getNormalizationConstant() const
    {
        return _normalizationConstant;
    }

    /**
    * @brief Set normalization constant
    * @param x Normalization constant
    */
    void setNormalizationConstant(float x)
    {
        _normalizationConstant = x;
    }

    /**
    * @brief Get scan for a given scan number
    * @param scanNum Scan number
    * @return Scan class object
    * @see Scan
    */
    Scan *getScan(unsigned int scanNum);

    /**
    * @brief Get Average Scan
    * @param rtmin Minimum retention time
    * @param rtmax Maximum retention time
    * @param mslevel MS Level
    * @param polarity Ionization mode/polarity
    * @param resolution Mass resolution of the MS machine
    * @return Scan class object
    */
    Scan *getAverageScan(float rtmin, float rtmax, int mslevel, int polarity, float resolution);

    /**
    * @brief Get EIC based on minMz, maxMz, minRt, maxRt, mslevel
    * @param mzmin Minimum m/z
    * @param mzmax Maximum m/z
    * @param rtmin Minimum retention time
    * @param rtmax Maximum retention time
    * @param mslevel MS Level. MS Level is 1 for MS data and 2 for MS/MS data
    * @param eicType Type of EIC (max or sum)
    * @param filterline selected filterline
    * @return EIC class object
    * @see EIC
    */
    EIC *getEIC(float mzmin, float mzmax, float rtmin, float rtmax, int mslevel, int eicType, string filterline);

    /**
    * @brief Get EIC based on srmId
    * @param srmId Filterline
    * @param eicType Type of EIC (max or sum)
    * @return EIC class object
    * @see EIC
    */
    EIC *getEIC(string srmId, int eicType);

    /**
    * @brief Get EIC for MS-MS dataset
    * @param precursorMz m/z of precursor Ion
    * @param collisionEnergy collision Energy
    * @param productMz m/z of product Ion]
    * @param eicType Type of EIC (max or sum)
    * @param filterline selected filterline
    * @param amuQ1 delta difference in Q1
    * @param amuQ3 delta difference in Q3
    * @return EIC class object
    */
    EIC *getEIC(float precursorMz, float collisionEnergy, float productMz, int eicType, string filterline, float amuQ1, float amuQ3);

    /**
    * @brief Get Total Ion Chromatogram
    * @param rtmin Minimum retention time
    * @param rtmax Maximum retention time
    * @param mslevel MS level of the MS machine
    * @return EIC class object
    */
    EIC *getTIC(float rtmin, float rtmax, int mslevel);

    /**
    * @brief Get Base Peak Chromatogram
    * @param rtmin Minimum retention time
    * @param rtmax Maximum retention time
    * @param mslevel MS level of the MS machine
    * @return EIC class object
    */
    EIC *getBIC(float rtmin, float rtmax, int mslevel);

    /**
     * @brief save alignment state before next alignment is performed
     **/
    void saveCurrentRetentionTimes();

    /**
     * @brief restore last state of alignment
     * @details last saved state is restored on cancelling alignment
     **/
    void restorePreviousRetentionTimes();

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
     * @brief Get the number of MS1 level scans detected in this sample.
     * @return MS1 scan count as unsigned int.
     */
    inline const unsigned int ms1ScanCount() { return _numMS1Scans; }

    /**
     * @brief Get the number of MS2 level scans detected in this sample.
     * @return MS2 scan count as unsigned int.
     */
    inline const unsigned int ms2ScanCount() { return _numMS2Scans; }

    /**
     * @brief Obtain the unique sample ID for the sample.
     * @return Sample ID as an integer.
     */
    inline int getSampleId() { return _id; }

    /**
     * @brief Set the sample ID for this sample.
     * @param id An integer which is not an ID for another sample.
     */
    inline void setSampleId(const int id) { _id = id; }

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
     * @brief find all MS2 scans within the slice
     * @return vector of all matching MS2 scans
     */
    vector<Scan*> getFragmentationEvents(mzSlice* slice);

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

    /**
     * @brief Compare sample order of two samples
     * @param a object of class mzSample
     * @param b object of class mzSample
     * @return True if sample order of 'a' is higher than 'b' else false
     **/
    static bool compRevSampleOrder(const mzSample *a, const mzSample *b) { return a->_sampleOrder > b->_sampleOrder; }

    static bool compSampleSort(const mzSample *a, const mzSample *b) { return mzUtils::compareStringsWithNumbers(a->sampleName, b->sampleName); }

    /**
    * @brief Compare injection time (in epoch seconds) of two samples
    * @param a object of class mzSample
    * @param b object of class mzSample
    * @return True if mzSample a has lower injection time than mzSample b
    */
    static bool compInjectionTime(const mzSample *a, const mzSample *b) { return a->injectionTime < b->injectionTime; }

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

    void setPeakCapacity(vector<PeakGroup> groups);

    vector<float> getIntensityDistribution(int mslevel);

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
    float peakCapacity;

    int _sampleOrder; //Sample display order

    int sampleNumber;

    bool _C13Labeled;
    bool _N15Labeled;
    bool _S34Labeled; //Feng note: added to track S34 labeling state
    bool _D2Labeled; //Feng note: added to track D2 labeling state

    float _normalizationConstant;
    string _setName;

    unsigned long int injectionTime; //Injection Time Stamp
    int injectionOrder; //Injection order

    map<string, vector<int> > srmScans; //SRM to scan mapping

    /** tags associated with this sample */
    map<string, string> instrumentInfo;

    //saving and restoring retention times

    //saved retention times prior to every alignment
    vector<float> lastSavedRTs;

    vector<double> polynomialAlignmentTransformation; //parameters for polynomial transform

  private:
    int _id;
    unsigned int _numMS1Scans;
    unsigned int _numMS2Scans;

    void sampleNaming(const char *filename);
    void checkSampleBlank(const char *filename);

    void setInstrumentSettigs(xml_document &doc, xml_node spectrumstore);

    void parseMzXMLData(const xml_node& spectrumstore);

    xml_node getmzXMLSpectrumData(xml_document &doc, const char *filename);

    float parseRTFromMzXML(xml_attribute &attr);

    static int parsePolarityFromMzXML(xml_attribute &attr);

    static int getPolarityFromfilterLine(string filterLine);

    vector<float> parsePeaksFromMzXML(const xml_node &scan);

    void populateMzAndIntensity(const vector<float>& mzint, Scan *_scan);

    void populateFilterline(const string& filterLine, Scan *_scan);

    void loadAnySample(string filename);

    //TODO: This should be moved
    static string getFileName(const string &filename);
    static int filter_minIntensity;
    static bool filter_centroidScans;
    static int filter_intensityQuantile;
    static int filter_mslevel;
    static int filter_polarity;

    vector<string> filterChromatogram {
        "sample", 
        "start",
        "end",
        "index",
        "scan"
    };
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

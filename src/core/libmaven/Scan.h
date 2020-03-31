#ifndef SCAN_H
#define SCAN_H

#include <QRegExp>
#include <QString>
#include <QStringList>

#include "standardincludes.h"

class mzSample;
class mzPoint;
class ChargedSpecies;
class MassCutoff;

using namespace std;

/**
* @class Scan
* @ingroup libmaven
* @brief Scan contains information about retention time, m/z, intensity
* @details Each scan stores  m/z and it's corresponding intensity for a particular Retention time.
There are multiple m/z and intenstities in one scan
*/

class Scan
{
  public:
    /**
     * @brief The MsType enum defines types, which instances of this class can
     * exist in, depending on what type of information is stored within a scan.
     */
    enum class MsType {
        MS1,
        DDA,
        DIA
    };

    Scan(mzSample *sample,
         int scannum,
         int mslevel,
         float rt,
         float precursorMz,
         int polarity,
         MsType msType = MsType::MS1);

    void deepcopy(Scan *b);

    /**
    * @brief return number of m/z's(number of observatiosn) recorded in a scan.
    */
    inline unsigned int nobs() const { return mz.size(); }

    /**
     * @brief Obtain the smallest m/z value stored.
     * @return Fractional m/z value.
     */
    inline float minMz() {
        if(nobs() > 0)
            return *(std::min_element(begin(mz),
                                      end(mz)));
        return 0.0f;
    }

    /**
     * @brief Obtain the largest m/z value stored.
     * @return Fractional m/z value.
     */
    inline float maxMz() {
        if(nobs() > 0)
            return *(std::max_element(begin(mz),
                                      end(mz)));
        return 0.0f; }

    /**
    *@brief return the corresponding sample
    */
    inline mzSample *getSample() { return sample; }


    vector<int> findMatchingMzs(float mzmin, float mzmax); //TODO why int vector?

    /**
    *@brief In a given m/z range (mz + ppm) return the position of Highest intensity present in the scan
    *@param m/z and ppm(parts per million). Together they define the m/z range
    */
    int findHighestIntensityPos(float mz, MassCutoff *massCutoff);


    int findClosestHighestIntensityPos(float mz, MassCutoff *massCutoff); //TODO: Sahil, Added while merging point

    /**
    * @brief checks if an input m/z value is present in the scan
    * @param  mz(input m/z value) and ppm(prats per million)
    * @return returns true if input m/z exists, otherwise false
    */
    bool hasMz(float mz, MassCutoff *massCutoff);

    /**
    * @brief check if the data is centroided
    * @return return true if data is centroided else false
    */
    bool isCentroided() const { return centroided; }

    //TODO: This function is not needed, isCentroided can be used
    /**
    * @brief check if the data is centroided
    * @return returns true if data is not centroided else false
    */
    bool isProfile() const { return !centroided; }

    /**
    * @brief polarity of the scan defines whether the metabolites were positively or negatively charged
    * @return returns the polarity of the scan (+1 for positive, -1 for
    * negative and 0 for neutral)
    */
    inline int getPolarity() const { return polarity; }

    /**
    * @brief set the polarity of scan
    * @param  x is the polarity to be set (+1 for positive, -1 for negative and 0
    * for neutral)
    */
    void setPolarity(int x) { polarity = x; }

    /**
    * @brief Calculate the sum of all the intensities for a scan
    * @return return total intensity
    */
    int totalIntensity() const
    {
        int sum = 0;
        for (unsigned int i = 0; i < intensity.size(); i++)
            sum += intensity[i];
        return sum;
    }


    //TODO: basePeakIntensity value in every scan of mzXml file represents maxIntensity. Use it rather than looping over all the  intensities
    /**
    * @brief return the maxIntensity in scan
    */

    float maxIntensity()
    {
        float max = 0;
        for (unsigned int i = 0; i < intensity.size(); i++)
            if (intensity[i] > max)
                max = intensity[i];
        return max;
    }

    /**
    * @brief return pairs of m/z, intensity values for top intensities.
    * @details intensities are normalized to a maximum intensity in a scan * 100]
    * @param minFracCutoff specfies mininum relative intensity; for example
    * 0.05, filters out all intensites below 5% of maxium scan intensity]
    */
    vector<pair<float, float> > getTopPeaks(float minFracCutoff, float minSigNoiseRatio, int dropTopX);

    vector<int> assignCharges(MassCutoff *massCutoffTolr);

    vector<float> chargeSeries(float Mx, unsigned int Zx); //TODO what does this do chargeSeries?

    ChargedSpecies *deconvolute(float mzfocus, float noiseLevel, MassCutoff *massCutoffMerge, float minSigNoiseRatio, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates);

    string toMGF();

    /**
    *@brief  return position of intensities in descending order(highest to lowest)
    */
    vector<int> intensityOrderDesc();

    /**
    *@brief centroid the data
    *@see also refer to findLocalMaximaInIntensitySpace
    */
    void simpleCentroid();

    /**
    * @brief removes intensities from scan that are  lower than minIntensity
    */
    void intensityFilter(int minIntensity);

    /**
    * @brief removes intensities from scan that are  lower than minQuantile
    */
    void quantileFilter(int minQuantile);

    /**
    * @brief adjusts precursor m/z for MS2 scans
    * @details precursor m/z available in the MS2 scans have
    * lower precision than m/z values recorded in the MS1 fullscans
    * To ensure correct mapping of MS2 scans to parent scans, precursor
    * m/z needs to be adjusted
    * @param ppm lowest ppm range for finding the correct
    * precursor m/z
    */
    void recalculatePrecursorMz(float ppm);

    /**
     * @brief calculates purity of the spectra
     * @details if the parent full scan has multiple readings within a precursor m/z window
     * the fragmentation scan would be a mixture of fragments from all those species
     */ 
    double getPrecursorPurity(float ppm = 10.0);

    /**
    *@brief print the info present in a scan
    */
    void summary();

    int mslevel;
    bool centroided;
    float rt; /**< retention time at which the scan was recorded */
    /** originalRt will hold original retention time when rt is modified in case of alignment
     */
    float originalRt;
    int scannum;

    float precursorMz;
    float precursorIntensity;
    int precursorCharge;
    int precursorScanNum;

    float productMz;
    float collisionEnergy;

    vector<float> intensity; /**< intensities found in one scan */
    vector<float> mz; /**< m/z's found in one scan */
    string scanType;
    string filterLine;
    mzSample *sample; /**< sample corresponding to the scan */
    int polarity; /**< +1 for positively charged, -1 for negatively charged, 0 for neutral*/

    /**
     * @brief Set the limits of the m/z range covered by this scan.
     * @param lower The lower m/z limit for this scan.
     * @param upper The upper m/z limit for this scan.
     */
    void setScanWindow(const float lower, const float upper);

    /**
     * @brief Obtain the lower limit for this scan's m/z range.
     * @return A floating point value.
     */
    float scanWindowLowerBound() const { return _scanWindowLowerBound; }

    /**
     * @brief Obtain the upper limit for this scan's m/z range.
     * @return A floating point value.
     */
    float scanWindowUpperBound() const { return _scanWindowUpperBound; }

    /**
     * @brief Set the size of the precursor window from which this MS/MS scan
     * orignates.
     * @details Depending on the isolation window, this scan is assumed to be of
     * DDA type, if `window` is less than or equal to 1.0 and of type SWATH if
     * greater than that. Its internal type identifier is changed accordingly.
     * @param window Size of the isolation window for this MS/MS scan.
     */
    void setIsolationWindow(const float window);

    /**
     * @brief Obtain the size of the precursor window from which this MS/MS
     * scan originates. If this is a plain MS scan, then the resulting value is
     * meaningless.
     * @return A `float` representing the size of this scan's isolation window.
     */
    float isolationWindow() const { return _isolationWindow; }

    /**
     * @brief The lower limit of this scan's SWATH window.
     * @return A `float` storing this scan's lower SWATH window in m/z
     * dimension, if this is an MS/MS scan. Otherwise, 0.0 is returned.
     */
    float swathWindowMin() { return _swathWindowMin; }

    /**
     * @brief The upper limit of this scan's SWATH window.
     * @return A `float` storing this scan's upper SWATH window in m/z
     * dimension, if this is an MS/MS scan. Otherwise, 0.0 is returned.
     */
    float swathWindowMax() { return _swathWindowMax; }

    /**
     * @brief Obtain the type of information stored within this scan.
     * @return A `Scan::MsType` value.
     */
    MsType msType();

    /**
     * @brief compare total intensity of two scans
     * @return true if Scan a has a higher totalIntensity than b, else false
     */
    static bool compIntensity(Scan* a, Scan* b) { return a->totalIntensity() > b->totalIntensity(); }

    /**
    * @brief compare retention times of two scans
    * @return return True if Scan a has lower retention time than Scan b, else false
    */
    static bool compRt(Scan *a, Scan *b) { return a->rt < b->rt; }

    /**
    * @brief compare precursor m/z of two samples
    * @return return True if Scan a has lower precursor m/z than Scan b, else false
    */
    static bool compPrecursor(Scan *a, Scan *b) { return a->precursorMz < b->precursorMz; }

    bool operator<(const Scan &b) const { return rt < b.rt; }

  private:
    float parentPeakIntensity;

    /**
     * @brief precursor mass resolution for fragmentation event
     */
    float _isolationWindow;

    float _scanWindowLowerBound;
    float _scanWindowUpperBound;
    float _swathWindowMin;
    float _swathWindowMax;

    /**
     * @brief Type of MS data, stored by this scan.
     */
    MsType _msType;

    struct BrotherData
    {
        float expectedMass;
        int countMatches;
        float totalIntensity;
        int upCount;
        int downCount;
        int minZ;
        int maxZ;
    };

    ofstream file;
    BrotherData *brotherdata, b;
    void initialiseBrotherData(int z, float mzfocus);
    void updateBrotherDataIfPeakFound(int loopdirection, int ii, bool *flag, bool *lastMatched, float *lastIntensity, float noiseLevel, MassCutoff *massCutoffMerge);
    void updateChargedSpeciesDataAndFindQScore(ChargedSpecies *x, int z, float mzfocus, float noiseLevel, MassCutoff *massCutoffMerge, int minChargedStates);
    void findBrotherPeaks(ChargedSpecies *x, float mzfocus, float noiseLevel, MassCutoff *massCutoffMerge, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates);
    bool setParentPeakData(float mzfocus, float noiseLevel, MassCutoff *massCutoffMerge, float minSigNoiseRatio);
    void findError(ChargedSpecies *x);
    
    /**
     * @brief gets the previous MS1 scan till historySize
     */ 
    Scan* getLastFullScan(int historySize = 50);
    
    /**
     * @brief gets the full-scan m/z-int readings that fall within the isolation window of the precursor
     */
    vector<mzPoint> getIsolatedRegion(float isolationWindowAmu = 1.0);

    vector<float> smoothenIntensitites();
    /**
    *@brief compare the current intensity with the left and the right intensity and store the largest amongest them
    */
    void findLocalMaximaInIntensitySpace(int vsize, vector<float> *cMz, vector<float> *cIntensity, vector<float> *spline);
    void updateIntensityWithTheLocalMaximas(vector<float> *cMz, vector<float> *cIntensity);
};
#endif

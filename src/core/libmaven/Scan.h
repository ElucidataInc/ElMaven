#ifndef SCAN_H
#define SCAN_H
#include "mzSample.h"
#include <QRegExp>
#include <QString>
#include <QStringList>

class mzSample;
class ChargedSpecies;
class MassCutoff;

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
    Scan(mzSample *sample, int scannum, int mslevel, float rt, float precursorMz, int polarity);

    void deepcopy(Scan *b);

    /**
    * @brief return number of m/z's(number of observatiosn) recorded in a scan.
    */
    inline unsigned int nobs() const { return mz.size(); }

    /**
     * @brief Obtain the smallest m/z value stored.
     * @return Fractional m/z value.
     */
    inline float minMz()  { if(nobs() > 0) return mz[0]; return 0; }

    /**
     * @brief Obtain the largest m/z value stored.
     * @return Fractional m/z value.
     */
    inline float maxMz()  { if(nobs() > 0) return mz[nobs()-1]; return 0; }

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

    // TODO: from MAVEN (upstream). What for? Always returns 0.0  for now.
    double getPrecursorPurity(float ppm);

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
    string activationMethod;
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

    vector<float> smoothenIntensitites();
    /**
    *@brief compare the current intensity with the left and the right intensity and store the largest amongest them
    */
    void findLocalMaximaInIntensitySpace(int vsize, vector<float> *cMz, vector<float> *cIntensity, vector<float> *spline);
    void updateIntensityWithTheLocalMaximas(vector<float> *cMz, vector<float> *cIntensity);
};
#endif

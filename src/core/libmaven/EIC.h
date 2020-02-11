/**
 * @class EIC
 * @ingroup libmaven
 * @brief Wrapper class for a eic.
 * @author Sabu George
 */
#ifndef MZEIC_H
#define MZEIC_H

#include <Eigen>

#include "standardincludes.h"

class Peak;
class PeakGroup;
class mzSample;
class mzPoint;
class Scan;
class Compound;
class mzSlice;

using namespace std;

class EIC
{

  public:
    /**
    *  Default constructor. 
    */
    EIC();

    /**
    *  Destructor
    */
    ~EIC();

    enum SmootherType /**<Enumeration to select the smoothing algorithm */
    {
        SAVGOL = 0,
        GAUSSIAN = 1,
        AVG = 2
    };
    enum EicType /**<Enumeration to select how intensity and/or mass is calculated at a particular retention time */
    {
        MAX = 0,
        SUM = 1
    };

    enum class BaselineMode {
        Threshold,
        AsLSSmoothing
    };

    vector<int> scannum;     /**< Store all scan numbers in an EIC */
    vector<float> rt;        /**< Store all retention times in an EIC */
    vector<float> mz;        /**< Store all mass/charge ratios in an EIC */
    vector<float> intensity; /**< Store all intensities in an EIC */
    vector<Peak> peaks;      /**< Store all peak objects in an EIC */
    string sampleName;       /**< Store name of the sample associated with the EIC */

    mzSample *sample; /**< pointer to originating sample */

    float color[4]; /**< color of the eic line, [r,g,b, alpha] */

    float *spline; /**< pointer to smoothed intensity array */

    float *baseline; /**< pointer to baseline array */

    /* maximum intensity of all scans*/
    float maxIntensity;

    /* rt value of maximum intensity*/
    float rtAtMaxIntensity;

    /* mz value of maximum intensity*/
    float mzAtMaxIntensity;

    float maxAreaTopIntensity; /**< maximum areaTop intensity (after baseline correction) out of all peaks */

    float maxAreaIntensity; /**< maximum area intensity (after baseline correction) out of all peaks */

    float maxAreaNotCorrectedIntensity; /**< maximum area intensity (without baseline correction) out of all peaks */

    float maxAreaTopNotCorrectedIntensity; /**< maximum areaTop intensity (without baseline correction) out of all peaks */

    double filterSignalBaselineDiff; /**< minimum threshold for peak signal-baseline difference */

    float totalIntensity; /**< sum of all intensities in an EIC */

    int eic_noNoiseObs; /**< number of observations above baseline */

    float mzmin; /**< minimum mass/charge for pulling an EIC */
    float mzmax; /**< maximum mass/charge for pulling an EIC */
    float rtmin; /**< minimum retention time for pulling an EIC */
    float rtmax; /**< maximum retention time for pulling an EIC */

    /**
    * @brief add peak object to vector
    * @details create a peak object for given peak position and append to vector
    * @param  peakPos position of peak in spline
    * @return pointer to newly added peak object in the vector
    */
    Peak *addPeak(int peakPos);

    /**
    * @brief delete peak at given index from the vector
    * @param  i index of peak to be deleted
    */
    void deletePeak(unsigned int i);

    /**
    * @brief find peak positions after smoothing, baseline calculation and peak filtering
    * @param smoothWindow number of scans used for smoothing in each iteration
    */
    void getPeakPositions(int smoothWindow);

    /**
    * @brief set values for all members of a peak object
    * @param  peak peak object
    */
    void getPeakDetails(Peak &peak);

    /**
    * @brief width of given peak in terms of number of scans
    * @details find the number of scans where peak intensity is above baseline
    * @param  peak peak object
    */
    void getPeakWidth(Peak &peak);

    void setBaselineMode(BaselineMode b) { _baselineMode = b; }

    /**
     * @brief Calculate baseline for the current baseline mode.
     */
    void computeBaseline();

    /**
    * @brief calculate spline of the EIC
    * @details smoothen intensity data according to selected algorithm. stores it as spline
    * @param  smoothWindow  number of scans used for smoothing in each iteration
    */
    void computeSpline(int smoothWindow);

    /**
    * @brief find the first and last position of a peak
    * @param  peak peak object
    */
    void findPeakBounds(Peak &peak);

    /**
    * @brief find all parameter values for every peak in an EIC
    * @method getPeakStatistics
    */
    void getPeakStatistics();

    /**
    * @brief find all peaks in an EIC
    * @details find all local maxima in an EIC and save them as objects
    */
    void findPeaks();

    /**
    * @brief remove peaks with parameter values below user-set thresholds
    */
    void filterPeaks();

    /**
    * brief 
    * @param  peak             [peak]
    */
    void checkGaussianFit(Peak &peak);

    /**
    * @brief get vector of all intensity points in a peak
    * @param peak peak object
    * @return mzPoint vector of intensity points in the peak
    */
    vector<mzPoint> getIntensityVector(Peak &peak);

    /**
    * @brief print parameter values of an EIC in log window
    */
    void summary();

    /**
    * @brief set smoothing algorithm
    * @param  x SmootherType 
    */
    void setSmootherType(EIC::SmootherType x) { smootherType = x; }

    /**
    * @brief set smoothing window for baseline
    * @param  x number of scans used for smoothing in one iteration
    */
    void setBaselineSmoothingWindow(int x) { baselineSmoothingWindow = x; }

    /**
    * @brief set percentage of top intensity points to remove for setting baseline
    * @param  x percentage of top intensity points to remove
    */
    void setBaselineDropTopX(int x) { baselineDropTopX = x; }

    /**
     * @brief Set smoothness (λ) to be used for default AsLS baseline estimation.
     * @param s smoothness (will be mutated to 10^s when actually used)
     */
    void setAsLSSmoothness(int s) { _aslsSmoothness = s; }

    /**
     * @brief Set asymmetry (p) to be used for default AsLS baseline estimation.
     * @param a asymmetry value (will be divided by 100 when actually used).
     */
    void setAsLSAsymmetry(int a) { _aslsAsymmetry = a; }

    /**
    * @brief set minimum signal baseline difference for every peak
    * @param x signal baseline difference threshold for every peak
    */
    void setFilterSignalBaselineDiff(double x) { filterSignalBaselineDiff = x; }

    /**
    * @brief get EIC of a sample using given mass/charge and retention time range
    * @details 
    * @param
    * @return bool true if EIC is pulled. false otherwise
    */
    bool makeEICSlice(mzSample *sample, float mzmin, float mzmax, float rtmin, float rtmax, int mslevel, int eicType, string filterline);

    void getRTMinMaxPerScan();

    void normalizeIntensityPerScan(float scale);

    void subtractBaseLine();
    void clearEICContents();
    void interpolate();
    /**
         * [size ]
         * @method size
         * @return []
         */
    inline unsigned int size() { return intensity.size(); }

    /**
    * @return sample associated with the EIC
    */
    inline mzSample *getSample() { return sample; }

    /**
     * @brief return list of groups given a set of EICs
     * @details assigns every peak to a group based on the best matching merged EIC
     * @param smoothingWindow number of scans read at a time for smoothing
     * @param maxRtDiff maximum retention time difference between peaks in a group
     * @param minQuality minimum peak quality for every group. used for calculation of good peaks
     * @param distXWeight weight of rt difference between a peak and merged EIC
     * @param distYWeight weight of intensity difference between a peak and merged EIC
     * @param overlapWeight weight of peak overlap between a peak and merged EIC
     * @param userOverlap flag to determine which group score formula is used
     * @param minSignalBaselineDifference minimum difference between peak and baseline for peak to be marked
     * @return vector of peak groups found
    **/
    static vector<PeakGroup> groupPeaks(vector<EIC *> &eics,
                                        mzSlice* slice,
                                        int smoothingWindow,
                                        float maxRtDiff,
                                        double minQuality,
                                        double distXWeight,
                                        double distYWeight,
                                        double overlapWeight,
                                        bool useOverlap,
                                        double minSignalBaselineDifference,
                                        float fragmentPpmTolerance,
                                        string scoringAlgo);
    /**
         * [eicMerge ]
         * @method eicMerge
         * @param  eics     []
         * @return []
         */
    static EIC *eicMerge(const vector<EIC *> &eics);

    /**
         * [remove Low Rank Groups ]
         * @method removeLowRankGroups
         * @param  groups              [vector of peak groups]
         * @param  rankLimit           [group rank limit ]
         */
    static void removeLowRankGroups(vector<PeakGroup> &groups, unsigned int rankLimit);

    /**
         * [compare Max Intensity]
         * @method compMaxIntensity
         * @param  a                [EIC a]
         * @param  b                [EIC b]
         * @return [true or false]
         */
    static bool compMaxIntensity(EIC *a, EIC *b) { return a->maxIntensity > b->maxIntensity; }

  private:
    /**
     * Name of selected smoothing algorithm
     */
    SmootherType smootherType;

    /**
     * @brief _baselineMode decides which algorithm to use for computing baseline.
     */
    BaselineMode _baselineMode;

    /**
     * Sets the number of scans used for smoothing in one iteration.
     */
    int baselineSmoothingWindow;

    /*
     * Percentage of top intensity points to remove before computing baseline.
     */
    int baselineDropTopX;

    /**
     * @brief Smoothness parameter for AsLS Smoothing algorithm
     */
    int _aslsSmoothness;

    /**
     * @brief Asymmetry parameter for AsLS Smoothing algorithm
     */
    int _aslsAsymmetry;

    /**
     * @brief Clear the baseline if exists and reallocate memory for a new one.
     * @return Whether the baseline should be processed further or not.
     */
    bool _clearBaseline();

    /**
     * @brief Computes a baseline using naive thresholding method.
     * @param smoothingWindow is the size of window used for 1D guassian smoothing.
     * @param dropTopX percent of the highest intensities will be truncated.
     */
    void _computeThresholdBaseline(const int smoothingWindow,
                                   const int dropTopX);

    /**
     * @brief Computes a baseline using Asymmetric Least Squares Smoothing techinique.
     * @details A (Whittaker) smoother is used to get a slowly varying estimate
     * of the baseline. In contrast to ordinary least squares smoothing,
     * however, positive deviations with respect to baseline estimate are
     * weighted (much) less than negative ones.
     *
     * Ref: Baseline Correction with Asymmetric Least Squares Smoothing,
     * P. Eilers, H. Boelens, 2005
     *
     * @param lambda for smoothness. Typical values of lambda for MS data range
     * from 10^2 to 10^9, depending on dataset. But since we resample the
     * intensity signal, we can limit this range to [10^0, 10^3]. The exponent value
     * should be passed here as integer, i.e. lambda should be in range [0, 3].
     * @param p for asymmetry. Values between 0.01 to 0.10 work reasonable well
     * for MS data.
     * @param numIterations for the number of iterations that should be
     * performed (since this is an iterative optimization algorithm).
     */
    void _computeAsLSBaseline(const float lambda,
                              const float p,
                              const int numIterations=10);
};
#endif //MZEIC_H

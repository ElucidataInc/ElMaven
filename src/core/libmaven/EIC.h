/**
 * @class EIC
 * @ingroup libmaven
 * @brief Wrapper class for a eic.
 * @author Sabu George
 */
#ifndef MZEIC_H
#define MZEIC_H
#include "Peak.h"
#include "PeakGroup.h"
#include "mzSample.h"
class Peak;
class PeakGroup;
class mzSample;
class mzPoint;
class Scan;

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

    float maxIntensity; /**< maximum intensity out of all eics */

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

    /**
    * @brief calculate baseline for all peaks in an EIC
    * @param  smoothingWindow number of scans used for smoothing in each iteration 
    * @param  dropTopX percentage of top intensities to be removed before setting baseline
    */
    void computeBaseLine(int smoothingWindow, int dropTopX);

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
    vector<Scan *> getFragmenationEvents();
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

    static vector<PeakGroup> groupPeaks(vector<EIC *> &eics,
                                        int smoothingWindow,
                                        float maxRtDiff,
                                        double minQuality,
                                        double distXWeight,
                                        double distYWeight,
                                        double minPeakRtDiff,
                                        bool useOverlap,
                                        double minSignalBaselineDifference);
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
    SmootherType smootherType; /**< name of selected smoothing algorithm */

    int baselineSmoothingWindow; /**< sets the number of scans used for smoothing in one iteration*/

    int baselineDropTopX; /**< percentage of top intensity points to remove before computing baseline */
};
#endif //MZEIC_H

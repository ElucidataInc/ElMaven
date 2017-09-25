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
        GAUSSIAN = 0,
        AVG = 1,
        SAVGOL = 2
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
         * [addPeak ]
         * @method addPeak
         * @param  peakPos []
         * @return []
         */
    Peak *addPeak(int peakPos);

    /**
         * [delete Peak]
         * @method deletePeak
         * @param  i          [peak position]
         */
    void deletePeak(unsigned int i);

    /**
         * [get Peak Positions]
         * @method getPeakPositions
         * @param  smoothWindow     [smoothing Window]
         */
    void getPeakPositions(int smoothWindow);

    /**
         * [get Peak Details]
         * @method getPeakDetails
         * @param  peak           [peak]
         */
    void getPeakDetails(Peak &peak);

    /**
         * [get Peak Width]
         * @method getPeakWidth
         * @param  peak         [peak]
         */
    void getPeakWidth(Peak &peak);

    /**
         * [compute BaseLine]
         * @method computeBaseLine
         * @param  smoothingWindow [smoothing Window]
         * @param  dropTopX        [drop top X percent]
         */
    void computeBaseLine(int smoothingWindow, int dropTopX);

    /**
         * [compute Spline]
         * @method computeSpline
         * @param  smoothWindow  [smoothing Window]
         */
    void computeSpline(int smoothWindow);

    /**
         * [find Peak Bounds]
         * @method findPeakBounds
         * @param  peak           [peak]
         */
    void findPeakBounds(Peak &peak);

    /**
         * [get Peak Statistics]
         * @method getPeakStatistics
         */
    void getPeakStatistics();

    void findPeaks();

    void filterPeaks();

    /**
         * [check Gaussian Fit]
         * @method checkGaussianFit
         * @param  peak             [peak]
         */
    void checkGaussianFit(Peak &peak);

    vector<mzPoint> getIntensityVector(Peak &peak);

    /**
         * [summary]
         * @method summary
         */
    void summary();

    /**
         * [set Smoother Type]
         * @method setSmootherType
         * @param  x               [Smoother type]
         */
    void setSmootherType(EIC::SmootherType x) { smootherType = x; }

    /**
         * [set Baseline Smoothing Window]
         * @method setBaselineSmoothingWindow
         * @param  x                          [baseline Smoothing Window]
         */
    void setBaselineSmoothingWindow(int x) { baselineSmoothingWindow = x; }

    /**
         * [setBaselineDropTopX ]
         * @method setBaselineDropTopX
         * @param  x                   []
         */
    void setBaselineDropTopX(int x) { baselineDropTopX = x; }

    void setFilterSignalBaselineDiff(double x) { filterSignalBaselineDiff = x; }

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
         * [getSample ]
         * @method getSample
         * @return []
         */
    inline mzSample *getSample() { return sample; }

    static vector<PeakGroup> groupPeaks(vector<EIC *> &eics,
                                        int smoothingWindow,
                                        float maxRtDiff,
                                        double minQuality,
                                        double distXWeight,
                                        double distYWeight,
                                        double overlapWeight,
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
    SmootherType smootherType;  /**< name of selected smoothing algorithm */

    int baselineSmoothingWindow;  /**< sets the number of scans used for smoothing in one iteration*/

    int baselineDropTopX;  /**< percentage of top intensity points to remove before computing baseline */
};
#endif //MZEIC_H

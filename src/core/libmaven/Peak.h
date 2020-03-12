#ifndef PEAK_H
#define PEAK_H

#include "standardincludes.h"

class mzSample;
class EIC;
class Scan;
class mzLink;

using namespace std;

class Peak {
    public:
        Peak();

        Peak(EIC* e, int p);
        Peak(const Peak& p);
        Peak& operator=(const Peak& o);
        void copyObj(const Peak& o);

        unsigned int pos;
        unsigned int minpos; // left bound of peak
        unsigned int maxpos; // right bound of peak
        unsigned int splineminpos; // left bound of spline peak
        unsigned int splinemaxpos; // right bound of spline peak

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
        /** are of spline */
        float peakSplineArea;
        /** baseline substracted area */
        float peakAreaCorrected;
        /** top 3 points of the peak */
        float peakAreaTop;
        /** top 3 points of the peak baseline subtracted */
        float peakAreaTopCorrected;
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
        float signalBaselineDifference;
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
        vector<mzLink> findCovariants();
        /**
         * [setEIC ]
         * @method setEIC
         * @param  e      []
         */
        void setEIC(EIC* e) { eic=e; }

        /**
         * [getEIC ]
         * @method getEIC
         * @return []
         */
        inline EIC*	 getEIC() { return eic;    }

        /**
         * [hasEIC ]
         * @method hasEIC
         * @return []
         */
        inline bool hasEIC() const { return eic != NULL; }

        /**
         * [getScan ]
         * @method getScan
         * @return []
         */
        Scan* getScan();

        /**
         * [setSample ]
         * @method setSample
         * @param  s         []
         * @return []
         */
        void   setSample(mzSample* s ) { sample=s; }

        /**
         * [getSample ]
         * @method getSample
         * @return []
         */
        inline mzSample* getSample() const { return sample; }

        /**
         * [hasSample ]
         * @method hasSample
         * @return []
         */
        inline bool hasSample() const 	{  return sample != NULL; }

        /**
         * [setLabel ]
         * @method setLabel
         * @param  label    []
         */
        void setLabel(char label) { this->label=label;}

        /**
         * [getLabel ]
         * @method getLabel
         * @return []
         */
        inline char getLabel() const { return label;}

        static bool compRtMin(const Peak& a, const Peak& b ) { return a.rtmin < b.rtmin; }
        /**
         * [compRt ]
         * @method compRt
         * @param  a      []
         * @param  b      []
         * @return []
         */
        static bool compRt(const Peak& a, const Peak& b ) { return a.rt < b.rt; }

        /**
         * [compIntensity ]
         * @method compIntensity
         * @param  a             []
         * @param  b             []
         * @return []
         */
        static bool compIntensity(const Peak& a, const Peak& b ) { return b.peakIntensity < a.peakIntensity; }

        /**
         * [compArea ]
         * @method compArea
         * @param  a        []
         * @param  b        []
         * @return []
         */
        static bool compArea(const Peak& a, const Peak& b ); 

        /**
         * [compMz ]
         * @method compMz
         * @param  a      []
         * @param  b      []
         * @return []
         */
        static bool compMz(const Peak& a, const Peak& b ); 

        /**
         * [compSampleName ]
         * @method compSampleName
         * @param  a              []
         * @param  b              []
         * @return []
         */
        static bool compSampleName(const Peak& a, const Peak& b ); 

        /**
         * [compSampleOrder ]
         * @method compSampleOrder
         * @param  a               []
         * @param  b               []
         * @return []
         */
        static bool compSampleOrder(const Peak& a, const Peak& b );

        /**
         * @brief Checks whether the second peak has higher quality than the
         * first one.
         * @param a First peak.
         * @param b Second peak.
         * @return `true` if the second peak has higher quality, `false`
         * otherwise.
         */
        static bool compQuality(const Peak& a, const Peak& b );

        /**
         * [overlap ]
         * @method overlap
         * @param  a       []
         * @param  b       []
         * @return []
         */
        static float overlap(const Peak& a, const Peak& b); 

};
#endif

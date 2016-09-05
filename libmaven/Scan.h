#ifndef SCAN_H
#define SCAN_H
#include "mzSample.h"
class mzSample;
class ChargedSpecies;
class Scan{
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
        inline unsigned int nobs() const { return mz.size(); }

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
         * [highest intensity pos nearest to the cente mz]
         * @method findClosestHighestIntensityPos
         * @param  mz                      [m/z value]
         * @param  ppm                     [ppm window]
         * @return [highest intensity pos nearest to an m/z with a ppm window]
         */
        int findClosestHighestIntensityPos(float mz, float ppm); //TODO: Sahil, Added while merging point

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
        bool isCentroided() const { return centroided; }

        /**
         * [Checks if the data is not centroided]
         * @method isProfile
         * @return [returns true if data is not centroided else false]
         */
        bool isProfile()   const  { return !centroided; }

        /**
         * [Gets the polarity of the scan]
         * @method getPolarity
         * @return [Returns the polarity of the scan (+1 for positive, -1 for
         * negative and 0 for neutral)]
         */
        inline int getPolarity() const { return polarity; }

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
        int totalIntensity() const { int sum=0; for(unsigned int i=0;i<intensity.size();i++) sum += intensity[i]; return sum; }

        //TODO: Sahil, Added while merging point
        float maxIntensity()  { float max=0; for(unsigned int i=0;i<intensity.size();i++) if(intensity[i] > max) max=intensity[i]; return max; }

        /**
         * [return pairing of m/z, intensity values for top intensities.
         * intensities are normalized to a maximum intensity in a scan * 100]
         * @param minFracCutoff [specfies mininum relative intensity; for example
         * 0.05, filters out all intensites below 5% of maxium scan intensity]
         * @return
         */
        vector<pair<float,float> > getTopPeaks(float minFracCutoff,float minSigNoiseRatio,int dropTopX);

        vector<int>assignCharges(float ppmTolr); //TODO: Sahil, Added while merging spectrawidget

        /**
         * [generate multi charges series..endingin in change Zx,Mx]
         * @param Mx []
         * @param
         */
        vector<float> chargeSeries(float Mx, unsigned int Zx); //TODO what does this do chargeSeries?

        /**
         * [Deconvolution]
         * @method deconvolute
         * @param  mzfocus                []
         * @param  noiseLevel             []
         * @param  ppmMerge               []
         * @param  minSigNoiseRatio       []
         * @param  minDeconvolutionCharge []
         * @param  maxDeconvolutionCharge []
         * @param  minDeconvolutionMass   []
         * @param  maxDeconvolutionMass   []
         * @param  minChargedStates       []
         * @return []
         */
        ChargedSpecies* deconvolute(float mzfocus, float noiseLevel, float ppmMerge, float minSigNoiseRatio, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates );
        //TODO deconvolute
        vector<int> intensityOrderDesc();

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

        int mslevel;
        bool centroided;
        float rt;
        int   scannum;

        float precursorMz;
        float precursorIntensity;
        int precursorCharge;
        string activationMethod;
        int precursorScanNum;
        float productMz;
        float collisionEnergy;


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
        bool operator< (const Scan& b) const { return rt < b.rt; }

        int polarity;

    private:


        struct ParentData {
            float parentPeakIntensity;
            bool flag;
        };

        ParentData *parentdata,p;

        struct BrotherData {
            float expectedMass;    
            int countMatches;
            float totalIntensity;
            int upCount;
            int downCount;
            int minZ;
            int maxZ;
        };

        ofstream file;
        BrotherData *brotherdata,b;
        void initialiseBrotherData(int z, float mzfocus);
        void updateBrotherDataIfPeakFound(int loopdirection, int ii, bool *flag, bool *lastMatched, float *lastIntensity, float noiseLevel, float ppmMerge);
        void updateChargedSpeciesDataAndFindQScore(ChargedSpecies* x, int z,float mzfocus, float noiseLevel, float ppmMerge, int minChargedStates);
        void findBrotherPeaks (ChargedSpecies* x, float mzfocus, float noiseLevel, float ppmMerge,int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates);
        bool setParentPeakData(float mzfocus, float noiseLevel, float ppmMerge, float minSigNoiseRatio);
        void findError(ChargedSpecies* x);

        vector<float> smoothenIntensitites();
        void findLocalMaximaInIntensitySpace(int vsize, vector<float> *cMz, vector<float> *cIntensity, vector<float> *spline);
        void updateIntensityWithTheLocalMaximas(vector<float> *cMz, vector<float> *cIntensity);
};
#endif

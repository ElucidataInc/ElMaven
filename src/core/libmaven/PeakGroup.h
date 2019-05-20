#ifndef PEAKGROUP_H
#define PEAKGROUP_H

#include "Fragment.h"
#include "Peak.h"
#include "standardincludes.h"

class mzSample;
class Isotope;
class MassCalculator;
class Compound;
class Peak;
class Scan;
class EIC;
class MassCutoff;

using namespace std;

class PeakGroup{

    public:
        enum GroupType {None=0, C13=1, Adduct=2, Covariant=4, Isotope=5 };     //group types
        enum QType	   {AreaTop=0,
                        Area=1,
                        Height=2,
                        AreaNotCorrected=3,
                        RetentionTime=4,
                        Quality=5,
                        SNRatio=6,
                        AreaTopNotCorrected=7};
        PeakGroup();
        PeakGroup(const PeakGroup& o);
        PeakGroup& operator=(const PeakGroup& o);

        bool operator==(const PeakGroup* o);
        /**
         * [copyObj ]
         * @method copyObj
         * @param  o       []
         */
        void copyObj(const PeakGroup& o);

        /**
         * [copy ]
         * @method copy
         * @param  o    []
         */
        void copy(const PeakGroup* o);

        ~PeakGroup();

        PeakGroup* parent;
        Compound* compound;

        // have to do this since `GroupType` enum also has an Adduct.
        // In future use "enum class" instead. Also from MAVEN (upstream).
        class Adduct* adduct;

        vector<Peak> peaks;
        vector<PeakGroup> children;
        vector<PeakGroup> childrenBarPlot;
        vector<PeakGroup> childrenIsoWidget;
        vector<mzSample*> samples;  //this varibale will hold only those sample which has been
                                    //used for peak detection
        string srmId;
        string tagString;

        // Stores the name of Peak Table this group belongs to.
        string searchTableName;

        /** classification label */
        char label;
        /**
         * @brief returns group name
         * @method getName
         * @detail returns compound name, tagString, srmID, meanMz@meanRt or groupId in this order of preference
         * @return string
         */
        string getName();

        bool isFocused;

        QType quantitationType;

        int groupId;
        int metaGroupId;
        int clusterId;

        bool deletedFlag;

        float maxIntensity;
        float maxAreaTopIntensity;
        float maxAreaIntensity;
        float maxHeightIntensity;
        float maxAreaNotCorrectedIntensity;
        float maxAreaTopNotCorrectedIntensity;
        float currentIntensity;
        float meanRt;
        float meanMz;
        float expectedMz;
        int totalSampleCount;

        int  ms2EventCount;

        FragmentationMatchScore fragMatchScore;
        Fragment fragmentationPattern;

        //isotopic information
        float expectedAbundance;
        int   isotopeC13count;

        double minIntensity;

        //int quantileIntensityPeaks;
        //int quantileQualityPeaks;

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
        float avgPeakQuality;
        //@Kailash: group quality computed using neural network
        int markedGoodByCloudModel = 0;
        int markedBadByCloudModel = 0;
        float groupQuality;
        float weightedAvgPeakQuality;
        int predictedLabel;
        double minQuality;
        float maxPeakFracionalArea;
        float maxSignalBaseRatio;
        float maxSignalBaselineRatio;
        int goodPeakCount;
        float expectedRtDiff;
        float groupRank;

        //for sample contrasts  ratio and pvalue
        float changeFoldRatio;
        float changePValue;

        bool isMS1();

        /**
         * [hasSrmId ]
         * @method hasSrmId
         * @return []
         */
        bool  	hasSrmId() const   { return srmId.empty(); }

        /**
         * [setSrmId ]
         * @method setSrmId
         * @param  id       []
         * @return []
         */
        void  	setSrmId(string id)	  { srmId=id; }

        /**
         * [getSrmId ]
         * @method getSrmId
         * @return []
         */
        inline  string getSrmId() const { return srmId; }


        /**
         * [isPrimaryGroup ]
         * @method isPrimaryGroup
         * @return []
         */
        bool isPrimaryGroup();

        /**
         * [hasCompoundLink ]
         * @method hasCompoundLink
         * @return []
         */
        inline bool hasCompoundLink() const  { if(compound != NULL) return true ; return false; }

        /**
         * [isEmpty ]
         * @method isEmpty
         * @return []
         */
        inline bool isEmpty() const   { if(peaks.size() == 0) return true; return false; }

        /**
         * [peakCount ]
         * @method peakCount
         * @return []
         */
        inline unsigned int peakCount()  const { return peaks.size(); 	  }

        /**
         * [childCount ]
         * @method childCount
         * @return []
         */
        inline unsigned int childCount() const { return children.size(); }

        inline unsigned int childCountBarPlot() const { return childrenBarPlot.size(); }

        inline unsigned int childCountIsoWidget() const { return childrenIsoWidget.size(); }

        /**
         * [getCompound ]
         * @method getCompound
         * @return []
         */
        inline Compound* getCompound() { return compound; }

        /**
         * [getParent ]
         * @method getParent
         * @return []
         */
        inline PeakGroup* getParent() { return parent; }


        inline vector<Peak>& getPeaks() { return peaks; }


        inline vector<PeakGroup>& getChildren()  { return children; }

        vector<Scan*> getRepresentativeFullScans(); //TODO: Sahil - Kiran, Added while merging mainwindow

        /**
         * @brief Creates a mapping of m/z to intensity values, representing the
         * significant spectra for this group averaged over samples.
         * @param cutoff This cutoff will be used to bin m/z values. This is
         * basically the window over which m/z sorted values are averaged.
         * @return A mapping of m/z value to its corresponging intensity.
         */
        map<float, float> createAvgSpectra(MassCutoff* cutoff);

        /**
         * @brief find all MS2 scans for this group
         * @return vector of all MS2 scans for this group
         */
        vector<Scan*> getFragmentationEvents();

        /**
         * @brief build a consensus fragment spectra for this group
         * @param productPpmTolr ppm tolerance for fragment m/z
         */
        void computeFragPattern(float productPpmTolr);

        Scan* getAverageFragmentationScan(float productPpmTolr);

        void matchFragmentation(float ppmTolerance, string scoringAlgo);
        
        double getExpectedMz(int charge);

        /**
         * [setParent ]
         * @method setParent
         * @param  p         []
         */
        inline void setParent(PeakGroup* p) {parent=p;}

        /**
         * [setLabel ]
         * @method setLabel
         * @param  label    []
         */
        inline void setLabel(char label) { this->label=label;}

        /**
         * [ppmDist ]
         * @method ppmDist
         * @param  cmass   []
         * @return []
         */
        float massCutoffDist(float cmass,MassCutoff *massCutoff);

        /**
         * [addPeak ]
         * @method addPeak
         * @param  peak    []
         */
        void addPeak(const Peak& peak); 

        /**
         * [addChild ]
         * @method addChild
         * @param  child    []
         */
        inline void addChild(const PeakGroup& child) { children.push_back(child); children.back().parent = this;   }

        inline void addChildBarPlot(const PeakGroup& child) { childrenBarPlot.push_back(child); childrenBarPlot.back().parent = this;   }

        inline void addChildIsoWidget(const PeakGroup& child) { childrenIsoWidget.push_back(child); childrenIsoWidget.back().parent = this;   }

        /**
         * [getPeak ]
         * @method getPeak
         * @param  sample  []
         * @return []
         */

        Peak* getPeak(mzSample* sample);

        GroupType _type;

        /**
         * [type ]
         * @method type
         * @return []
         */
        inline GroupType type() const { return _type; }
        /**
         * [setType ]
         * @method setType
         * @param  t       []
         */
        inline void setType(GroupType t)  { _type = t; }

        void setQuantitationType(QType type) {quantitationType = type;}

        /**
         * [isIsotope ]
         * @method isIsotope
         * @return []
         */
        inline bool isIsotope() const { return _type == Isotope; }

        /**
         * [isAdduct ]
         * @method isAdduct
         * @return []
         */
        inline bool isAdduct() const {  return _type == Adduct; }

        /**
         * [summary ]
         * @method summary
         */
        void summary();

        /**
         * [groupStatistics ]
         * @method groupStatistics
         */
        void groupStatistics();

        /**
         * [updateQuality ]
         * @method updateQuality
         */
        void updateQuality();

        /**
         * [medianRt ]
         * @method medianRt
         * @return []
         */
        float medianRt();

        /**
         * [meanRtW ]
         * @method meanRtW
         * @return []
         */
        float meanRtW();

        /**
         * [reduce ]
         * @method reduce
         */
        void reduce();

        /**
         * [fillInPeaks ]
         * @method fillInPeaks
         * @param  eics        []
         */
        void fillInPeaks(const vector<EIC*>& eics);

        /**
         * [computeAvgBlankArea ]
         * @method computeAvgBlankArea
         * @param  eics                []
         */
        void computeAvgBlankArea(const vector<EIC*>& eics);

        /**
         * [groupOverlapMatrix ]
         * @method groupOverlapMatrix
         */
        void groupOverlapMatrix();

        /**
         * [getSamplePeak ]
         * @method getSamplePeak
         * @param  sample        []
         * @return []
         */
        Peak* getSamplePeak(mzSample* sample);

        /**
         * [deletePeaks ]
         * @method deletePeaks
         */
        void deletePeaks();

        /**
         * [deletePeak ]
         * @method deletePeak
         * @param  index      []
         * @return []
         */
        bool deletePeak(unsigned int index);

        /**
         * [clear ]
         * @method clear
         */
        void clear();

        /**
         * [deleteChildren ]
         * @method deleteChildren
         */
        void deleteChildren();

        /**
         * [deleteChild ]
         * @method deleteChild
         * @param  index       []
         * @return []
         */
        bool deleteChild(unsigned int index);

        /**
         * [deleteChild ]
         * @method deleteChild
         * @param  child       []
         * @return []
         */
        bool deleteChild(PeakGroup* child);

        /**
         * [copyChildren ]
         * @method copyChildren
         * @param  other        []
         */
        void copyChildren(const PeakGroup& other);

        vector<float> getOrderedIntensityVector(vector<mzSample*>& samples, QType type);

        /**
         * [reorderSamples ]
         * @method reorderSamples
         */
        void reorderSamples();

        /**
         * [compRt ]
         * @method compRt
         * @param  a      []
         * @param  b      []
         * @return []
         */
        static bool compRt(const PeakGroup& a, const PeakGroup& b ) { return(a.meanRt < b.meanRt); }

        /**
         * [compMz ]
         * @method compMz
         * @param  a      []
         * @param  b      []
         * @return []
         */
        static bool compMz(const PeakGroup& a, const PeakGroup& b ) { return(a.meanMz > b.meanMz); }

        /**
         * [compIntensity ]
         * @method compIntensity
         * @param  a             []
         * @param  b             []
         * @return []
         */
        static bool compIntensity(const PeakGroup& a, const PeakGroup& b ) { return(a.maxIntensity > b.maxIntensity); }

        /**
         * [compArea ]
         * @method compArea
         * @param  a        []
         * @param  b        []
         * @return []
         */
        static bool compArea(const PeakGroup& a, const PeakGroup& b ) { return(a.maxPeakFracionalArea > b.maxPeakFracionalArea); }

        /**
         * [compQuality ]
         * @method compQuality
         * @param  a           []
         * @param  b           []
         * @return []
         */
        static bool compQuality(const PeakGroup& a, const PeakGroup& b ) { return(a.maxQuality > b.maxQuality); }
        //static bool compInfoScore(const PeakGroup& a, const PeakGroup& b ) { return(a.informationScore > b.informationScore); }

        /**
         * [compRank ]
         * @method compRank
         * @param  a        []
         * @param  b        []
         * @return []
         */
        static bool compRank(const PeakGroup& a, const PeakGroup& b ) { return(a.groupRank < b.groupRank); }

        /**
         * [compRatio ]
         * @method compRatio
         * @param  a         []
         * @param  b         []
         * @return []
         */
        static bool compRatio(const PeakGroup& a, const PeakGroup& b ) { return(a.changeFoldRatio < b.changeFoldRatio); }

        /**
         * [compPvalue ]
         * @method compPvalue
         * @param  a          []
         * @param  b          []
         * @return []
         */
        static bool compPvalue(const PeakGroup* a, const PeakGroup* b ) { return(a->changePValue< b->changePValue); }

        /**
         * [compC13 ]
         * @method compC13
         * @param  a       []
         * @param  b       []
         * @return []
         */
        static bool compC13(const PeakGroup* a, const PeakGroup* b) { return(a->isotopeC13count < b->isotopeC13count); }

        /**
         * [compMetaGroup ]
         * @method compMetaGroup
         * @param  a             []
         * @param  b             []
         * @return []
         */
        static bool compMetaGroup(const PeakGroup& a, const PeakGroup& b) { return(a.metaGroupId < b.metaGroupId); }
        bool operator< (const PeakGroup* b) const { return this->maxIntensity < b->maxIntensity; }

        void calGroupRank(bool deltaRtCheckFlag,
                            int qualityWeight,
                            int intensityWeight,
                            int deltaRTWeight);
        /**
         * @brief take list of sample and filter those which are marked as selected
         * @details this method used for assigning samples to this group based on whether that samples
         * are marked as selected.
        */
        void setSelectedSamples(vector<mzSample*> vsamples);
};
#endif

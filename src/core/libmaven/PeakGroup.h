#ifndef PEAKGROUP_H
#define PEAKGROUP_H

#include "datastructures/mzSlice.h"
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
class Adduct;
class MavenParameters;

using namespace std;

class PeakGroup{
    public:
        enum class IntegrationType {
            Manual,
            Automated,
            Programmatic,
            Inherit,
            Ghost
        };

        enum class GroupType {
            None,
            Adduct,
            Isotope,
            Covariant
        };

        enum QType {
            AreaTop=0,
            Area=1,
            Height=2,
            AreaNotCorrected=3,
            RetentionTime=4,
            Quality=5,
            SNRatio=6,
            AreaTopNotCorrected=7
        };

        PeakGroup(shared_ptr<MavenParameters> parameters,
                  IntegrationType integrationType);
        PeakGroup(const PeakGroup& o,
                  IntegrationType integrationType = IntegrationType::Inherit);

        enum class ClassifiedLabel {
            None,                // the group has not been classified using ML yet
            Noise,               // the group is too noisy to be a metbolite
            Signal,              // the group shows a clear signal but may not be an interesting metabolite
            Correlation,         // the group is a signal and is correlated to one or more signals
            Pattern,             // the group is a signal and shows interesting inter-cohort intensity pattern
            CorrelationAndPattern // the group is a signal which shows correlation as well as intensity pattern
        };

        static string labelToString (ClassifiedLabel label)
        {
            if (label == ClassifiedLabel::Noise)
                return "ClassifiedLabel::Noise";
            if (label == ClassifiedLabel::Signal)
                return "ClassifiedLabel::Signal";
            if (label == ClassifiedLabel::Correlation)
                return "ClassifiedLabel::Correlation";
            if (label == ClassifiedLabel::Pattern)
                return "ClassifiedLabel::Pattern";
            if (label == ClassifiedLabel::CorrelationAndPattern)
                return "ClassifiedLabel::CorrelationAndPattern";
            return "ClassifiedLabel::None";
        }

        static ClassifiedLabel labelForString(const string& labelString)
        {
            if (labelString == "ClassifiedLabel::Signal") {
                return PeakGroup::ClassifiedLabel::Signal;
            } else if (labelString == "ClassifiedLabel::Noise") {
                return PeakGroup::ClassifiedLabel::Noise;
            } else if (labelString == "ClassifiedLabel::Correlation") {
                return PeakGroup::ClassifiedLabel::Correlation;
            } else if (labelString == "ClassifiedLabel::Pattern") {
                return PeakGroup::ClassifiedLabel::Pattern;
            } else if (labelString == "ClassifiedLabel::CorrelationAndPattern") {
                return PeakGroup::ClassifiedLabel::CorrelationAndPattern;
            }
            return PeakGroup::ClassifiedLabel::None;
        }

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

        vector<Peak> peaks;
        vector<mzSample*> samples;  //this varibale will hold only those sample which has been
                                    //used for peak detection
        string srmId;
        string tagString;

        /**
         * @brief returns group name
         * @method getName
         * @detail returns compound name, tagString, srmID, meanMz@meanRt or groupId in this order of preference
         * @return string
         */
        string getName() const;

        bool isFocused;

        QType quantitationType;

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
        int totalSampleCount;

        int  ms2EventCount;

        FragmentationMatchScore fragMatchScore;
        Fragment fragmentationPattern;

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

        double minQuality;
        float maxPeakFracionalArea;
        float maxSignalBaseRatio;
        float maxSignalBaselineRatio;
        int goodPeakCount;
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
         * [hasCompoundLink ]
         * @method hasCompoundLink
         * @return []
         */
        bool hasCompoundLink() const;

        inline bool isEmpty() const { return peaks.empty(); }

        inline bool isGhost() const { return _integrationType == IntegrationType::Ghost; }

        /**
         * [peakCount ]
         * @method peakCount
         * @return []
         */
        inline unsigned int peakCount()  const { return peaks.size(); 	  }

        inline unsigned int childIsotopeCount() const { return _childIsotopes.size(); }
        inline unsigned int childAdductsCount() const { return _childAdducts.size(); }
        inline unsigned int childIsotopeCountBarPlot() const { return _childIsotopesBarPlot.size(); }

        Compound* getCompound() const;

        void setCompound(Compound* compound);

        void setSlice(const mzSlice& slice);

        const mzSlice& getSlice() const;

        /**
         * @brief Check whether a slice has previosuly been set for this group.
         * @return true if a slice has been set, false otherwise.
         */
        bool hasSlice() const;

        /**
         * @brief Check whether both bounds of the group's slice are close to
         * zero in either m/z or rt dimensions.
         * @return `true` if either slice's `mzmin` and `mzmax` are both close
         * to zero or slice's `rtmin` and `rtmax` are both close to zero, false
         * otherwise.
         */
        bool sliceIsZero() const;

        /**
         * [getParent ]
         * @method getParent
         * @return []
         */
        inline PeakGroup* getParent() { return parent; }


        inline vector<Peak>& getPeaks() { return peaks; }


        inline const vector<shared_ptr<PeakGroup>> childIsotopes() const
        {
            return _childIsotopes;
        }
        inline const vector<shared_ptr<PeakGroup>> childAdducts() const
        {
            return _childAdducts;
        }
        inline const vector<shared_ptr<PeakGroup>> childIsotopesBarPlot() const
        {
            return _childIsotopesBarPlot;
        }

        vector<Scan*> getRepresentativeFullScans(); //TODO: Sahil - Kiran, Added while merging mainwindow

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

        float getExpectedAbundance() const;

        void tagIsotope(string isotopeName,
                        float isotopeMass,
                        float isotopeAbundance);

        /**
         * [setParent ]
         * @method setParent
         * @param  p         []
         */
        inline void setParent(PeakGroup* p)
        {
            parent = p;
            if (parent != nullptr)
                _type = GroupType::Isotope;
        }

        /**
         * @brief Assign a label to this peak group.
         * @details If this method is called with one of the valid user labels,
         * then user decision overrides any PeakML classified predictions.
         * @param label A character that is either 'g' (good), 'b' (bad) or
         * '\0' (unset).
         */
        void setUserLabel(const char label);

        /**
         * @brief Get the current good/bad label assigned to this peak group.
         * @return A character, denoting a good peak if 'g', a bad peak if 'b'
         * or '\0' if unassigned.
         */
        char userLabel() const { return _userLabel; }

        /**
         * @brief Get the adduct form for this `PeakGroup`.
         * @return Pointer to the `Adduct` object set for this group.
         */
        Adduct* adduct() const;

        /**
         * @brief Set the adduct form for this `PeakGroup`.
         * @details If the adduct is a type of parent ion, then this group's
         * `_type` attribute is set to `GroupType::Adduct`.
         * @param adduct Pointer to an `Adduct` object to be assigned.
         */
        void setAdduct(Adduct* adduct);

        /**
         * @brief Get the isotope (if any) associated to this `PeakGroup`.
         * @return Copy of the `Isotope` object associated to this group.
         */
        Isotope isotope() const;

        /**
         * @brief Set an `Isotope` tag for this `PeakGroup`.
         * @details If the isotope has a name other than C12 parent name, then
         * this group's `_type` attribute is set to `GroupType::Isotope`.
         * @param isotope An `Isotope` object to be assigned.
         */
        void setIsotope(Isotope isotope);

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

        shared_ptr<PeakGroup> addIsotopeChild(const PeakGroup& child);

        shared_ptr<PeakGroup> addAdductChild(const PeakGroup& child);

        shared_ptr<PeakGroup> addIsotopeChildBarPlot(const PeakGroup& child);

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
        inline bool isIsotope() const { return _type == GroupType::Isotope; }

        /**
         * [isAdduct ]
         * @method isAdduct
         * @return []
         */
        inline bool isAdduct() const {  return _type == GroupType::Adduct; }

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
         * @brief Obtain the deviation of this peak-group from its expected
         * retention time.
         * @details The RT difference between expected and observed is returned
         * as an absolute value. In case there was no expected value for this
         * group, then the value returned is -1.0.
         * @return A floating point value denoting absolute RT deviation in
         * minutes.
         */
        float expectedRtDiff();

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
         * @brief Clears all peaks in this peak-group, but does not reset the
         * group's stored statistics.
         */
        void deletePeaks();

        /**
         * @brief Delete peak at given index.
         * @param index The index of the peak in this group's peak vector.
         * @return `true` if deletion was successful, `false` otherwise.
         */
        bool deletePeak(unsigned int index);

        /**
         * @brief Delete this group's peak for the given sample, if found.
         * @param sample The `mzSample` whose peak needs to be deleted.
         * @return `true` if deletion was successful, `false` otherwise.
         */
        bool deletePeak(mzSample* sample);

        /**
         * [clear ]
         * @method clear
         */
        void clear();

        void deleteChildIsotopes();
        void deleteChildAdducts();
        void deleteChildIsotopesBarPlot();

        /**
         * @brief Removes a child peak-group (based on pointer equality) from
         * all of the child containers (isotopes and adducts). The child itself
         * is not garbage collected.
         * @param child Pointer to a peak-group, that could be present in this
         * parent group.
         * @return Returns `true` if a removal was performed (if found), `false`
         * otherwise.
         */
        bool removeChild(PeakGroup* child);

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

        static bool compIsotopeLabel(const PeakGroup* a,
                                     const PeakGroup* b)
        {
            return a->tagString < b->tagString;
        }

        /**
         * [compMetaGroup ]
         * @method compMetaGroup
         * @param  a             []
         * @param  b             []
         * @return []
         */
        static bool compMetaGroup(const PeakGroup& a,
                                  const PeakGroup& b)
        {
            return(a.metaGroupId() < b.metaGroupId());
        }

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

        /**
         * @brief Obtain the name of peak-table this group belongs to.
         * @return A string containing peak-table name.
         */
        string tableName() const;

        /**
         * @brief Set the peak-table (name) in which this group is listed.
         * @param tableName A string containing peak-table name.
         */
        void setTableName(string tableName);

        /**
         * @brief Obtain a reference to the parameters object used while
         * integrating this peak-group.
         * @return A constant shared pointer to a `MavenParameters` object.
         */
        const shared_ptr<MavenParameters> parameters() const
        {
            return _parameters;
        }

        IntegrationType integrationType() const { return _integrationType; }

        int groupId() const { return _groupId; }
        int metaGroupId() const { return _metaGroupId; }
        void setGroupId(int groupId);

        /*
         * @brief Set the classification label for this PeakGroup.
         * @details Anything other than `PeakGroup::ClassifiedLabel::Noise` or
         * `PeakGroup::ClassifiedLabel::None` will be marked as a good ('g')
         * group.
         * @param label The PeakML class to assign.
         * @param probability The probability for this classification.
         */
        void setPredictedLabel(const ClassifiedLabel label,
                               const float probability);

        /**
         * @brief Set the inference values for assigned classification.
         * @param inference A multi-map of computed values mapping to each peak
         * attribute. A `multimap` is used so that "views" can later on use it
         * to display top N features responsible for classification, which would
         * be a tedious process if we used a map of attributes:values instead.
         */
        void setPredictionInference(const multimap<float, string>& inference);

        /**
         * @brief Get the predicted label for this peak group.
         * @return A `PeakGroup::ClassifiedLabel` denoting the predicted class.
         */
        ClassifiedLabel predictedLabel() const;

        /**
         * @brief The proability with which this group has been assigned its
         * current PeakML class.
         * @return A floating point value representing confidence of
         * prediction.
         */
        float predictionProbability() const;

        /**
         * @brief Obtain inferences that can help illustrate why the peak-group
         * was assigned its current probability.
         * @return A `std::multimap` storing values mapping to every known
         * attribute.
         */
        multimap<float, string> predictionInference() const;

        /**
         * @brief Add a peak-group ID to the internal correlation records.
         * @param correlatedGroup An ID for a peak-group that is believed to be
         * correlated with this peak-group.
         * @param correlationFactor A floating point value (between 0 and 1)
         * that signifies how closely the two peak-groups resemble in terms of
         * their intensity pattern.
         */
        void addCorrelatedGroup(int groupId, const float correlationFactor);

        /**
         * @brief Obtain a map of peak-groups and their correlation values, that
         * exhibit a similar intensity pattern w.r.t. this peak-group.
         * @return A map of peak-group IDs mapping to their correlation factor
         * with this group.
         */
        map<int, float> getCorrelatedGroups() const;

        /**
         * @brief Converts an integer prediction-class identifier to its
         * corresponding `PeakGroup::ClassifiedLabel`.
         * @details This function should be regarded as complementary to
         * `PeakGroup::integralValueForClass`, assisting in conversion from a
         * primitive representation (integer).
         * @param An integer which has to be converted to a `ClassifiedLabel`.
         * @return A `PeakGroup::ClassifiedLabel`.
         */
        static ClassifiedLabel classificationLabelForValue(int value)
        {
            if (value == 0)
                return PeakGroup::ClassifiedLabel::Noise;
            if (value == 1)
                return PeakGroup::ClassifiedLabel::Signal;
            if (value == 2)
                return PeakGroup::ClassifiedLabel::Correlation;
            if (value == 3)
                return PeakGroup::ClassifiedLabel::Pattern;
            if (value == 4)
                return PeakGroup::ClassifiedLabel::CorrelationAndPattern;
            return PeakGroup::ClassifiedLabel::None;
        };

        /**
         * @brief Converts a `PeakGroup::ClassifiedLabel` identifier to an
         * integer value, guaranteed to be unique for all possible classes.
         * @details This function should be regarded as complementary to
         * `PeakGroup::classificationLabelForValue`, assisting in conversion to
         * a primitive representation (integer).
         * @param label A `PeakGroup::ClassifiedLabel` that needs to be
         * converted to an integer.
         * @return An integer, unique for each enum value.
         */
        static int integralValueForLabel(ClassifiedLabel label)
        {
            if (label == PeakGroup::ClassifiedLabel::Noise)
                return 0;
            if (label == PeakGroup::ClassifiedLabel::Signal)
                return 1;
            if (label == PeakGroup::ClassifiedLabel::Correlation)
                return 2;
            if (label == PeakGroup::ClassifiedLabel::Pattern)
                return 3;
            if (label == PeakGroup::ClassifiedLabel::CorrelationAndPattern)
                return 4;
            return -1;
        }

        void setUniqueId (int id)
        {
            _uniqueId = id;
        }

        int uniqueId()
        {
            return _uniqueId;
        }

    private:
        int _groupId;
        int _metaGroupId;

        mzSlice _slice;
        bool _sliceSet;
        // used in peakML in exporting groups for 
        // classification including child.
        int _uniqueId;

        float _expectedMz;
        float _expectedAbundance;

        vector<shared_ptr<PeakGroup>> _childIsotopes;
        vector<shared_ptr<PeakGroup>> _childAdducts;
        vector<shared_ptr<PeakGroup>> _childIsotopesBarPlot;

        string _tableName;
        shared_ptr<MavenParameters> _parameters;
        IntegrationType _integrationType;

        void _updateType();
        
        // user classification label
        char _userLabel;

        // group IDs (with score) with which this group is correlated
        map<int, float> _correlatedGroups;

        // properties for PeakML classification
        ClassifiedLabel _predictedLabel;
        float _predictionProbability;
        multimap<float, string> _predictionInference;
};
#endif

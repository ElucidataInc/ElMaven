#ifndef MAVENPARAMETERS_H
#define MAVENPARAMETERS_H

#include <qstring.h>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <map>

#include "mzMassCalculator.h"
#include "standardincludes.h"

using namespace std;

class ClassifierNeuralNet;
class Compound;
class MassCutoff;
class mzSample;
class mzSlice;
class PeakGroup;

class MavenParameters 
{
    public:
        MavenParameters(string settingsPath="");
        ~MavenParameters();
        enum Polarity{AutoDetect,Neutral, Positive, Negative};
        boost::signals2::signal< void (const string&,unsigned int , int ) > sig;

        /**
        * [set Output Directory]
        * @method setOutputDir
        * @param  outdir       [name of the output directory]
        */
        void setOutputDir(QString outdir);

        /**
        * [set Maximum Group Count]
        * @method setMaxGroupCount
        * @param  x                [Maximum Group Count]
        */
        void setMaxGroupCount(int x) {
            limitGroupCount = x;
        }

        int getCharge(Compound* compound = NULL);

        /**
        * [set Compounds]
        * @method setCompounds
        * @param  set          [vector of pointer to Compound]
        */
        void setCompounds(vector<Compound*> set) {
            compounds = set;
        }

        /**
        * [set mass Slices]
        * @method setSlices
        * @param  set       [vector of pointer to mzSlice]
        */
        void setSlices(vector<mzSlice*> set) {
            _slices = set;
        }

        /**
        * [set Peak Group]
        * @method setPeakGroup
        * @param  p            [pointer to Peak Group]
        */
        void setPeakGroup(PeakGroup* p) {
            _group = p;
        }

        /**
        * [set Average Scan Time]
        * @method setAverageScanTime
        */
        void setAverageScanTime();

        /**
        * [set Ionization Mode]
        * @method setIonizationMode
        */
        void setIonizationMode(Polarity polarity);

        /**
        * [set Samples]
        * @method setSamples
        * @param  set        [pointer to vector of pointer to mzSample]
        */
        void setSamples(vector<mzSample*>&set);

        /**
        * [cleanup all groups]
        * @method cleanup
        */
        void cleanup();
        vector<mzSample*> getVisibleSamples();

        bool writeCSVFlag;
        bool alignSamplesFlag;
        bool keepFoundGroups;
        bool processAllSlices;
        bool pullIsotopesFlag;
        bool showProgressFlag;
        bool matchRtFlag;

        /**
        * default ionization mode used by mass spec
        */
        int ionizationMode;
        int charge;
        bool formulaFlag = false;

        // For quantile intensity and qualityWeight
        double quantileQuality;
        double quantileIntensity;
        double quantileSignalBaselineRatio;
        double quantileSignalBlankRatio;

        //mass slicing parameters
        float mzBinStep;
        float rtStepSize;
        float avgScanTime;
        MassCutoff *massCutoffMerge;

        //peak detection

        /**
        * smoothing window
        */
        int eic_smoothingWindow;
        int eic_smoothingAlgorithm;

        // baseline estimation
        bool aslsBaselineMode;
        int baseline_smoothingWindow;
        int baseline_dropTopX;
        int aslsSmoothness;
        int aslsAsymmetry;

        bool isIsotopeEqualPeakFilter;
        double minSignalBaselineDifference;
        double isotopicMinSignalBaselineDifference;
        double minPeakQuality;
        double minIsotopicPeakQuality;

        int eicType;

        //peak filtering
        int minGoodGroupCount;
        float minSignalBlankRatio;
        float minNoNoiseObs;
        float minSignalBaseLineRatio;
        float minGroupIntensity;
        int peakQuantitation;

        //match fragmentation settings
        float minFragMatchScore;
        bool matchFragmentationFlag;
        float fragmentTolerance;
        float minFragMatch;
        string scoringAlgo;

        // to allow adduct matching
        bool searchAdducts;
        float adductSearchWindow;
        float adductPercentCorrelation;

        // Peak Group Rank
        int qualityWeight;
        int intensityWeight;
        int deltaRTWeight;
        bool deltaRtCheckFlag;

        /**
        * eic window around compound, compound detection setting
        */
        MassCutoff* compoundMassCutoffWindow;
        float compoundRTWindow;
        int eicMaxGroups;

        /**
         * @brief A flag which denotes whether the user has asked for limiting
         * identified peak-groups using RT window.
         */
        bool identificationMatchRt;

        /**
         * @brief The RT window, within which all identified peak-groups (post
         * untargeted detection) should lie.
         */
        float identificationRtWindow;

        /**
        * grouping of peaks across samples
        * do no group peaks that are greater than differ more than X in retention time
        */
        float grouping_maxRtWindow;

        /**
        * stop looking for groups if group count is greater than X
        */
        int limitGroupCount;

        /**
        * triple quad compound matching Q1
        */
        float amuQ1;

        /**
        * triple quad compound matching Q3
        */
        float amuQ3;

        string filterline;

        float minQuality;
        string ligandDbFilename;

        double maxIsotopeScanDiff;
        double maxNaturalAbundanceErr;
        double minIsotopicCorrelation;
        bool isotopeC13Correction;
        bool C13Labeled_BPE;
        bool N15Labeled_BPE;
        bool S34Labeled_BPE;
        bool D2Labeled_BPE;

        bool C13Labeled_Barplot;
        bool N15Labeled_Barplot;
        bool S34Labeled_Barplot;
        bool D2Labeled_Barplot;

        float minRt;
        float maxRt;
        float minMz;
        float maxMz;
        float minIntensity;
        float maxIntensity;
        float minCharge;
        float maxCharge;

        string outputdir;

        vector<PeakGroup> allgroups;
        vector<PeakGroup> undoAlignmentGroups;
        int alignButton;
        MassCalculator mcalc;
        ClassifierNeuralNet* clsf;
        PeakGroup* _group;
        vector<mzSample*> samples;
        vector<Compound*> compounds;
        vector<mzSlice*> _slices;
        bool stop;

        int alignMaxIterations; //TODO: Sahil - Kiran, Added while merging mainwindow
        int alignPolynomialDegree; //TODO: Sahil - Kiran, Added while merging mainwindow

        /**
        * [print parameter Settings]
        * @method printSettings
        */
        void printSettings();

        //options dialog::peak grouping tab-widget
        double distXWeight;
        double distYWeight;
        double overlapWeight;
        bool useOverlap;


        /*
         * @breif load user defined or default settings(Default_Settings.xml)
         * @param data constains the settings to be loaded
         */
        bool loadSettings(const char* data);

        bool saveSettings(const char* path);

        /*
         * @brief update mavenSettings(map) and settings related to isotopes
         */
        void setIsotopeDialogSettings(const char* key, const char* value);
        
        /*
         * @brief update maveSettings(map) and settings related to peak detection
         */
        void setPeakDetectionSettings(const char* key, const char* value);

        /*
         * @brief update maveSettings(map) and settings related to options Dialog
         */
        void setOptionsDialogSettings(const char* key, const char* value);

        void reset(const std::list<std::string>& keys);

        std::map<string, string>& getSettings();

        std::vector<Adduct*> getDefaultAdductList();
        inline std::vector<Adduct*> getChosenAdductList()
            { return _chosenAdducts; }
        inline void setChosenAdductList(std::vector<Adduct*> chosenAdducts)
            { _chosenAdducts = chosenAdducts; }

    private:
        vector<Adduct*> _chosenAdducts;
        char* defaultSettingsData;
        string lastUsedSettingsPath;
        std::map<string, string> mavenSettings;


};

#endif // MAVENPARAMETERS_H

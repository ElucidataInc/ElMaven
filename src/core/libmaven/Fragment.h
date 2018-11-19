#ifndef FRAGMENT_H
#define FRAGMENT_H
#include <vector>
#include "Peptide.hpp"

class PeptideRecord;
class Scan;

    class Fragment { 
    public: 

        double precursorMz;				//parent
        int polarity;					//scan polarity 	+1 or -1
        vector<float> mzs;				//mz values
        vector<float> intensity_array;		//intensity_array
        vector<Fragment*> brothers;		//pointers to similar fragments 
        string sampleName;				//name of Sample
        int scanNum;					//scan Number
        float rt;						//retention time of parent scan
        float collisionEnergy;
        int precursorCharge;	

        PeptideRecord* assignedPeptide;		//linked peptide information record

        //consensus pattern buuld from brothers generated on buildConsensus call.
        Fragment* consensus; //consensus pattern build on brothers
        vector<int>obscount; // vector size =  mzs vector size, with counts of number of times mz was observed
        map<int,string> annotations; //mz value annotations.. assume that values are sorted by mz


        //empty constructor
        Fragment();

        //build fragment based on MS2 scan
        Fragment(Scan* scan, float minFractionalIntenisty, float minSigNoiseRatio,int maxFragmentSize);


        void linkPeptidRecord(PeptideRecord* record);

        string peptideSequence(); 

        //make a copy of Fragment.
        Fragment( Fragment* other);

        void appendBrothers(Fragment* other);

        void printMzList();

        int findClosestHighestIntensityPos(float _mz, float tolr);

        void printFragment(float productAmuToll);

        void printConsensusMS2(ostream& outstream, int minConsensusFraction);


        void printConsensusMGF(ostream& outstream, int minConsensusFraction); 

        void printConsensusNIST(ostream& outstream, int minConsensusFraction, float productAmuToll, string COMPOUNDNAME);

        void printInclusionList(bool printHeader, ostream& outstream, string COMPOUNDNAME);


        double compareToFragment(Fragment* other, float productAmuToll);

        static vector<int> compareRanks( Fragment* a, Fragment* b, float productAmuToll);


        static vector<int> locatePositions( Fragment* a, Fragment* b, float productAmuToll); 

        void addFragment(Fragment* b);

        void buildConsensus(float productAmuToll);


        void annotatePeptideFragementsSpectraST(float productAmuToll,string fragType);

        vector<int> intensityOrderDesc();

        vector<int> mzOrderInc();


        void sortByIntensity();

        void sortByMz();

        void buildConsensusAvg();


        double spearmanRankCorrelation(const vector<int>& X);

        double fractionMatched(const vector<int>& X);


        static bool compPrecursorMz(const Fragment* a, const Fragment* b);
        bool operator<(const Fragment* b) const;
        bool operator==(const Fragment* b) const;
    };

    // TODO: from MAVEN (upstream). find our what this is.
    struct FragmentationMatchScore {
        double fractionMatched;
        double spearmanRankCorrelation;
        double ticMatched;
        double numMatches;
        double ppmError;
        double mzFragError;
        double mergedScore;
        double dotProduct;
        double weightedDotProduct;
        double dotProductShuffle;
        double hypergeomScore;
        double mvhScore;
        double ms2purity;
        vector<double> matchedQuantiles;

        static vector<string> getScoringAlgorithmNames()
        {
            vector<string> names;
            names.push_back("HyperGeomScore");
            names.push_back("MVH");
            names.push_back("DotProduct");
            names.push_back("WeightedDotProduct");
            names.push_back("SpearmanRank");
            names.push_back("TICMatched");
            names.push_back("NumMatches");
            return names;
        }

        double getScoreByName(string scoringAlgorithm)
        {
            if (scoringAlgorithm == "HyperGeomScore")
                return hypergeomScore;
            else if (scoringAlgorithm == "MVH")
                return mvhScore;
            else if (scoringAlgorithm == "DotProduct")
                return dotProduct;
            else if (scoringAlgorithm == "SpearmanRank")
                return spearmanRankCorrelation;
            else if (scoringAlgorithm == "TICMatched")
                return ticMatched;
            else if (scoringAlgorithm == "WeightedDotProduct")
                return weightedDotProduct;
            else if (scoringAlgorithm == "NumMatches")
                return numMatches;
            else
                return hypergeomScore;
        }

        FragmentationMatchScore()
        {
            fractionMatched = 0;
            spearmanRankCorrelation = 0;
            ticMatched = 0;
            numMatches = 0;
            ppmError = 1000;
            mzFragError = 1000;
            mergedScore = 0;
            dotProduct = 0;
            weightedDotProduct = 0;
            hypergeomScore = 0;
            mvhScore = 0;
            ms2purity = 0;
            dotProductShuffle = 0;
        }

        FragmentationMatchScore& operator=(const FragmentationMatchScore& b)
        {
            fractionMatched = b.fractionMatched;
            spearmanRankCorrelation = b.spearmanRankCorrelation;
            ticMatched = b.ticMatched;
            numMatches = b.numMatches;
            ppmError = b.ppmError;
            mzFragError = b.mzFragError;
            mergedScore = b.mergedScore;
            dotProduct = b.dotProduct;
            weightedDotProduct = b.weightedDotProduct;
            hypergeomScore = b.hypergeomScore;
            mvhScore = b.mvhScore;
            ms2purity = b.ms2purity;
            matchedQuantiles = b.matchedQuantiles;
            dotProductShuffle = b.dotProductShuffle;
            return *this;
        }
    };

#endif

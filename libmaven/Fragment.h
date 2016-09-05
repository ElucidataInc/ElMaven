#ifndef FRAGMENT_H
#define FRAGMENT_H
#include <vector>
#include "Peptide.hpp"
#include "Scan.h"
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
#endif

#ifndef MZALIGNER_H
#define MZALIGNER_H

#include <cmath>
#include <cstddef>
#include <float.h>
#include <limits>
#include <climits>
#include <vector>
#include "mzSample.h"
#include "Compound.h"
#include "obiwarp.h"
#include "PeakGroup.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include "vec.h"
#include "mat.h"
#include "dynprog.h"


using namespace std;

// class Aligner {
//    public:
//     // Aligner();
//     // void preProcessing(vector<PeakGroup*>& peakgroup,bool alignWrtExpectedRt);
//     // void doAlignment(vector<PeakGroup*>& peakgroup);
//     // vector<double> groupMeanRt();
//     // double checkFit();
//     // void Fit(int ideg);
//     // void saveFit();
//     // void PolyFit(int poly_align_degree);
//     // void restoreFit();
//     // void setMaxItterations(int x) { maxItterations = x; }
//     // void setPolymialDegree(int x) { polynomialDegree = x; }
//     // void alignWithObiWarp(vector<mzSample*> samples , ObiParams* obiParams, int referenceSampleIndex = -1);
//     // map<pair<string,string>, double> getDeltaRt() {return deltaRt; }
// 	// map<pair<string, string>, double> deltaRt;
//     // vector<vector<float> > fit;
//     // vector<mzSample*> samples;

//     int medianRt;
//     int compoundDataRt;
//     float tolerance;

//     // void updateRts(QJsonObject& parentObj);
//     // void updateSampleRts(QJsonObject& sampleRts);
//     // void updateGroupsRts(QJsonObject& groupsRts);

//     // QJsonObject groupsJson;
//     // QJsonObject rtsJson;


// //    private:
// //     vector<PeakGroup*> allgroups;
// //     int maxItterations;
// //     int polynomialDegree;

// };


// new aligner starts here


class Aligner {
    public:
        Aligner();
        void setGroups(vector<PeakGroup*> x) { groups = x; }
        // vector<PeakGroup*> getGroups() { return groups; }
        map<pair<string, string>, double> getDeltaRt() { return deltaRt; }
        vector<PeakGroup*> groups;
        map<pair<string, string>, double> deltaRt;
        void alignmentError(QString);

        vector<double> groupMeanRt();
};

class PolyFit : public Aligner {

    struct AlignmentStats { 
        public:
            int poly_align_degree=0;
            int N = 0;
            double R_before = 0;
            double R_after  = 0;
            int transformedFailed = 0;
            vector<double>poly_transform_result;

            int sample1 = 0;
            int sample2 = 0;
            
            bool transformOk() { return transformedFailed > 0; }
            bool transformImproved() { return R_after < R_before; }
            vector<double> getCoeffients() { return poly_transform_result; }

            double predict(double x)  {
                if ( poly_transform_result.size() > 0 ) {
                    return leasev(&poly_transform_result.front(),poly_align_degree,x);
                }   else {
                    cerr  << "Empty transform.. "; 
                    return 0;
                }
            }


            void summary() { 
                if (transformedFailed) cerr << "TRANSFORMED FAILED! "; 

                cerr <<" Degree= " 		<< poly_align_degree 
                    << " Points:" 		<< N 
                    << " R2_before=" 	<< R_before
                    << " R2_after=" 	<< R_after 
                    << endl;

                for(int ii=0; ii <= poly_align_degree; ii++ ) {
                    cerr << poly_transform_result[ii] << " " ;
                }

                cerr << endl;
            }

            static bool compR(AlignmentStats* a, AlignmentStats* b ) { return a->R_after < b->R_after; }
            bool operator< (const AlignmentStats* b) { return this->R_after < b->R_after; }

    };

    public:
        PolyFit(vector<PeakGroup*> groups, vector <mzSample*> sample) { setGroups(groups); maxIterations=10; polynomialDegree=3; samples = sample; }
        void polyFitAlgo();
        class PolyAligner {

            public:
                PolyAligner(StatisticsVector<float>& subj, StatisticsVector<float>& ref);

            
                AlignmentStats* align(int ideg);
                AlignmentStats* optimalPolynomial(int fromDegree, int toDegree, int sampleSize);
                double calculateR2(AlignmentStats* model) ;
                void calculateOutliers(int initDegree);
                double  countInliners(AlignmentStats* model, float zValueCutoff);
                void randomOutliers(double keepFrac);

                StatisticsVector<float> subjVector;
                StatisticsVector<float> refVector;
                vector<bool> outlierVector;
                MTRand* mtRand;

                void test();
        };

        void setPolymialDegree (int x) { polynomialDegree = x; }
        void setMaxIterations (int x) { maxIterations = x; }
        
	   
    private:
        int maxIterations;
        int polynomialDegree;

        vector<mzSample*> samples;
        map<mzSample*, int> sampleDegree;
        vector<vector<float> > fit;
        map<mzSample*, vector<double> > sampleCoefficient;

        
        void polyFit(int poly_align_degree);

        void saveFit();
        double checkFit();
        void restoreFit();

        // polyAligner functions
        // AlignmentStats* align(int ideg);
		// AlignmentStats* optimalPolynomial(int fromDegree, int toDegree, int sampleSize);
		// double calculateR2(AlignmentStats* model) ;
		// void calculateOutliers(int initDegree);
		// double  countInliners(AlignmentStats* model, float zValueCutoff);
		// void randomOutliers(double keepFrac);

        // StatisticsVector<float> subjVector;
		// StatisticsVector<float> refVector;
		// vector<bool> outlierVector;
		// MTRand* mtRand;

		// void test();

        


};



class LoessFit : public Aligner {
    public:
        LoessFit(vector<PeakGroup*> x, bool y, vector <mzSample*> sample) { setGroups(x); alignWrtExpectedRt = y; pythonProg = new QProcess(); samples = sample; }
        void loessFit();
    private:
        QProcess* pythonProg;
        QJsonObject groupsJson;
        QJsonObject rtsJson;

        vector<mzSample*> samples;

        bool alignWrtExpectedRt;
        QByteArray processedDataFromPython;

        void preProcessing();
        void updateSampleRts(QJsonObject &sampleRts);
        void updateGroupsRts(QJsonObject &groupsRts);
        void updateRts(QJsonObject &parentObj);
        void readDataFromPython();
        void writeToPythonProcess(QByteArray data);
        void sendDataToPython();
        void runPythonProg();
        // other helper functions
};

class ObiWarp : public Aligner {
    

    public:
        struct ObiParams{

            string score;
            bool local;
            float factor_diag;
            float factor_gap;
            float gap_init;
            float gap_extend;
            float init_penalty;
            float response;
            bool nostdnrm;
            float binSize;

            ObiParams() {
                string score = "cor";
                bool local = false;
                float factor_diag = 2;
                float factor_gap = 1;
                float gap_init = 0.2;
                float gap_extend = 3.4;
                float init_penalty = 0;
                float response = 20;
                bool nostdnrm = false;
                float binSize = 0.6;
            }
            ObiParams(string score,bool local, float factor_diag, float factor_gap, float gap_init,float gap_extend, 
            float init_penalty, float response, bool nostdnrm, float binSize) {
                this->score = score;
                this->local = local;
                this->factor_diag = factor_diag;
                this->factor_gap = factor_gap;
                this->gap_init = gap_init;
                this->gap_extend = gap_extend;
                this->init_penalty = init_penalty;
                this->response = response;
                this->nostdnrm = nostdnrm;
                this->binSize = binSize;
            }
        };
        ObiWarp(ObiParams *obiParams, vector<mzSample*> samples, int referenceSampleIndex = -1) {
            this->params = new ObiParams();
            *params = *obiParams;

            this->samples = samples;
            this->referenceSampleIndex = referenceSampleIndex;
        }

        void obiWarp ();


        
    
    private:
        ObiParams *params;
        vector<mzSample*> samples;
        int referenceSampleIndex;
        int _tm_vals;
        int _mz_vals;
        float* tmPoint;
        float* mzPoint;
        DynProg dyn;

        VecF _tm;
        VecF _mz;
        MatF _mat;

        void alignSampleRts (mzSample* sample, vector<float> &mzPoints, bool setAsReference);
        void setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
        vector<float> align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
        void tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals);
        void warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm);
};


#endif
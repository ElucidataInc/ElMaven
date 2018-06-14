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
#include <QJsonObject>

using namespace std;

class Aligner {
   public:
    // Aligner();
    // void preProcessing(vector<PeakGroup*>& peakgroups,bool alignWrtExpectedRt);
    // void doAlignment(vector<PeakGroup*>& peakgroups);
    // vector<double> groupMeanRt();
    // double checkFit();
    // void Fit(int ideg);
    // void saveFit();
    // void PolyFit(int poly_align_degree);
    // void restoreFit();
    // void setMaxItterations(int x) { maxItterations = x; }
    // void setPolymialDegree(int x) { polynomialDegree = x; }
    // void alignWithObiWarp(vector<mzSample*> samples , ObiParams* obiParams, int referenceSampleIndex = -1);
    // map<pair<string,string>, double> getDeltaRt() {return deltaRt; }
	// map<pair<string, string>, double> deltaRt;
    // vector<vector<float> > fit;
    // vector<mzSample*> samples;

    int medianRt;
    int compoundDataRt;
    float tolerance;

    // void updateRts(QJsonObject& parentObj);
    // void updateSampleRts(QJsonObject& sampleRts);
    // void updateGroupsRts(QJsonObject& groupsRts);

    // QJsonObject groupsJson;
    // QJsonObject rtsJson;


//    private:
//     vector<PeakGroup*> allgroups;
//     int maxItterations;
//     int polynomialDegree;

};


// new aligner starts here


class Aligner {
    public:
        Aligner();
        void setGroups(vector<PeakGroups*> x) { groups = new(x); }
        // vector<PeakGroups*> getGroups() { return groups; }

    private:
        vector<PeakGroup*> groups;
        map<pair<string, string>, double> deltaRt;

        vector<double> groupMeanRt();
};

class PolyFit : public Aligner {

    struct AlignmentStats { 

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
        PolyFit(vector<peakGroup*> group);
        void polyFitAlgo();
	   
    private:
        int maxIterations;
        int polynomialDegree;

        vector<mzSample*> samples;
        map<mzSample*, int> sampleDegree;
        vector<vector<float> > fit;
        map<mzSample*, vector<double> > sampleCoefficient;

        void setPolymialDegree (int x) { polynomialDegree = x; }
        void setMaxItterations (int x) { maxIterations = x; }

        void saveFit();
        double checkFit();
        void restoreFit();

        // map<pair<string,string>, double> getDeltaRt() { return deltaRt; }


};



class LoessFit : public Aligner {
    public:
        LoessFit(vector<peakGroup*> x, bool y) { groups = new (x); alignWrtExpectedRt = y; pythonProg = new QProcess(); };
        void loessFit();
    private:
        QProcess* pythonProg;
        QJsonObject groupsJson;
        QJsonObject rtsJson;

        bool alignWrtExpectedRt;

        void preProcessing();
        void runPythonProg();
        // other helper functions
}

class ObiWarp : public Aligner {
    struct ObiParams{
        ObiParams();
        ObiParams(string score,bool local, float factor_diag, float factor_gap, float gap_init,float gap_extend, float init_penalty, float response, bool nostdnrm, float binSize);

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
    };

    public:
        obiWarp(ObiParams *obiParams, vector<mzSample*> samples, int referenceSampleIndex = -1) {
            this.params = new ObiParams();
            *params = *obiParams;

            this.samples = new (samples);
            this.referenceSampleIndex = referenceSampleIndex;
        }

        
    
    private:
        ObiParams *params;
        vector<mzSample*> samples;
        int referenceSampleIndex;

        void alignSampleRts (mzSample* sample, vector<float> &mzPoints, bool setAsReference);
        void setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
        vector<float> align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
    
};


#endif
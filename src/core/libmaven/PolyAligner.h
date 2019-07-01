#ifndef POLYALIGNER
#define POLYALIGNER

#include "MersenneTwister.h"
#include "mzFit.h"
#include "statistics.h"

using namespace std;

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

class PolyAligner { 

	public:
		PolyAligner(StatisticsVector<float>& subj, StatisticsVector<float>& ref);


		AlignmentStats* align(int ideg);
		AlignmentStats* optimalPolynomial(int fromDegree, int toDegree, int sampleSize);
		double calculateR2(AlignmentStats* model) ;
		void calculateOutliers(int initDegree);
		double  countInliners(AlignmentStats* model, float zValueCutoff);
		void randomOutliers(double keepFrac);

		void test();

	private:
		StatisticsVector<float> subjVector;
		StatisticsVector<float> refVector;
		vector<bool> outlierVector;
		MTRand* mtRand;

};

#endif

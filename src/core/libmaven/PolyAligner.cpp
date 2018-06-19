// #include "PolyAligner.h"


// PolyAligner::PolyAligner(StatisticsVector<float>& subj, StatisticsVector<float>& ref) {

// 	if( subj.size() != ref.size()) { 
// 		cerr << "alignVector failed.. vectors are different size\n";
// 		exit(-1);
// 	}

// 	//copy values that are presents in both vectors
// 	for(unsigned int indx=0; indx < subj.size(); indx++ ) {
// 		if (subj[indx]> 0 and ref[indx] > 0)  { 
// 			subjVector.push_back(subj[indx]);
// 			refVector.push_back(ref[indx]);
// 		}
// 	}
// 	mtRand = new MTRand(time(NULL));
// 	calculateOutliers(3);
// } 

// double PolyAligner::calculateR2(AlignmentStats* model) { 
// 	double R2=0;
// 	int N = subjVector.size();
// 	if (N == 0) return DBL_MAX;

// 	for(int i=0; i < N; i++ ) {
//          double newrt = model->predict(subjVector[i]);
// 		 R2 += POW2(refVector[i] - newrt);
// 	}
// 	return sqrt(R2/N);
// }

// double PolyAligner::countInliners(AlignmentStats* model, float zValueCutoff) { 

// 	int N = subjVector.size();
// 	if (N == 0) return 0;

// 	StatisticsVector<float>residuals;
// 	for(int i=0; i < N; i++ ) {
// 		 if(outlierVector[i] == true ) {
//          	double newrt = model->predict(subjVector[i]);
// 		 	residuals.push_back(POW2(refVector[i] - newrt));
// 		 }
// 	}

// 	float meanResidual=residuals.mean();
// 	float stdResidual=sqrt(residuals.variance(meanResidual));

// 	int inlinerCount=0; 
// 	double R2=0;
// 	for(unsigned int i=0; i < residuals.size(); i++ ) {
// 		if (abs((residuals[i] - meanResidual)/stdResidual) < zValueCutoff ) {
// 			R2 += residuals[i];
// 			inlinerCount++;
// 		}
// 	}
// 	if (inlinerCount > 0 ) {
// 		return sqrt(R2/inlinerCount);
// 	} else {
// 		return DBL_MAX;
// 	}
// }

// void PolyAligner::randomOutliers(double keepFrac) {

// 	int N = subjVector.size();
// 	if(!outlierVector.size()) outlierVector = vector<bool>(N,false);

// 	for(int i=0; i < N; i++ ) {
// 		 double r = mtRand->rand(); //random number from 0 to 1
// 		 if (r < keepFrac ) {
// 			 outlierVector[i]=false;
// 		 } else {
// 			 outlierVector[i]=true;
// 		 }
// 	}
// }

// AlignmentStats* PolyAligner::align(int degree) {

// 	AlignmentStats* stats = new AlignmentStats();
// 	stats->poly_align_degree = degree; 
// 	if (stats->poly_align_degree >= 100) stats->poly_align_degree=99;
	
// 	int knownInBoth=subjVector.size();
// 	double* x  =   new double[subjVector.size()];
// 	double* ref =  new double[subjVector.size()];

// 	//copy values that are presents in both vectors to c arrays
// 	for(unsigned int indx=0; indx < subjVector.size(); indx++ ) {
// 		x[indx]  =subjVector[indx];
// 		ref[indx]=refVector[indx];
// 	}

// 	stats->N = knownInBoth;

// 	//remove outliers?
// 	if(outlierVector.size() > 0 ) {
// 		int j=0;
// 		stats->N=0;
// 		for(unsigned int i=0; i< outlierVector.size();i++) { 
// 			if(outlierVector[i] == false) { 
// 				x[j]= x[i];
// 				ref[j]=ref[i];
// 				stats->N++;
// 				j++;
// 			}
// 		}
// 		//cerr << "Removed " << knownInBoth - stats->N << endl;
// 	}

// 	if(stats->poly_align_degree > stats->N/3 ) { stats->poly_align_degree = stats->N/3; }

// 	double* result = new double[(stats->poly_align_degree+1)];
// 	double* w = new double[(stats->poly_align_degree+1)*(stats->poly_align_degree+1)];

	
// 	if(stats->N > 0) {
// 		stats->R_before=0; 
// 		stats->R_after=0;

// 		sort_xy(x, ref, stats->N, 1, 0);
//         leasqu(stats->N, x, ref, stats->poly_align_degree, w, (stats->poly_align_degree+1), result);	//polynomial fit

// 		stats->transformedFailed=0;
// 		for(int ii=0; ii < stats->N; ii++)  { 
//             double newrt = leasev(result, stats->poly_align_degree, x[ii]);
//             if (newrt != newrt || std::isinf(newrt)) {
// 				cerr << "Transform failed: " << ii << "\t" << x[ii] << "-> " << newrt << endl;
// 				stats->transformedFailed++;
// 			} else { 
// 				stats->R_before += POW2(ref[ii] - x[ii]);
// 				stats->R_after  += POW2(ref[ii] - newrt);
// 			}
// 		}

// 		if(stats->transformedFailed == 0) {
// 			stats->poly_transform_result = vector<double>(stats->poly_align_degree+1,0);
// 			for(int ii=0; ii <= stats->poly_align_degree; ii++ ) stats->poly_transform_result[ii]=result[ii]; 
// 		}
// 	}

//     delete[] result;
// 	delete[] w;
// 	delete[] x;
// 	delete[] ref;
// 	return stats;
// }

// void PolyAligner::calculateOutliers(int initDegree) { 
// 	if (subjVector.size() <= 2) return;

// 	AlignmentStats* stats = this->align(initDegree); 
// 	int N = subjVector.size();

// 	StatisticsVector<float> distVector(N,0);
// 	for(int i=0; i < N; i++ ) {
// 		if(refVector[i]>0) {
// 			distVector[i] = POW2(refVector[i] - stats->predict(subjVector[i]));
// 		}
// 	}

// 	double meanDelta = distVector.mean();
// 	double stddev = sqrt(distVector.variance(meanDelta));

// 	outlierVector = vector<bool>(N,false);
// 	for(int i=0; i < N; i++ ) {
// 		distVector[i] = abs((distVector[i] - meanDelta)/stddev);
// 		if (distVector[i] > 1.5) {
// 			outlierVector[i] = true;
// 		} else {
// 			outlierVector[i] = false;
// 		}
//     }

// 	/*(
// 	cerr << "OUTLIERS: " << endl;
// 	for(int i=0; i < N; i++ ) {
// 		cerr << outlierVector[i] << " r=" << refVector[i] << " pred=" << stats->predict(subjVector[i]) << " dist=" << distVector[i] << endl;
// 	}
// 	*/

// 	delete stats;
// }


// AlignmentStats* PolyAligner::optimalPolynomial(int fromDegree, int toDegree, int sampleSize=0) { 

// 	float R_best = FLT_MAX;
//     AlignmentStats* bestModel = NULL;

// 	for(int deg=fromDegree; deg<toDegree; deg++) { 
// 		for(int attempt=0; attempt <= sampleSize; attempt++ ) { 

// 			AlignmentStats* thisModel = this->align(deg); 
// 			double R_thismodel = this->calculateR2(thisModel);
// 			//double R_thismodel = this->countInliners(thisModel,2.0);

// 			randomOutliers(0.35); //new random subset of data

// 			if (R_thismodel < R_best ) { 
//                 //cerr << "optimalPolynomial() \t attempt=" << attempt << " R2=" << R_thismodel << endl;
// 				if (bestModel) delete(bestModel);
// 				bestModel = thisModel;
// 				R_best = R_thismodel; 
// 				continue;
// 			}  else {
// 				delete(thisModel);
// 			}

// 		}
// 	}

// 	if ( bestModel ) return bestModel;
// 	else return this->align(1);
// }

// void PolyAligner::test() { 

// 	vector<float> x(100);
// 	vector<float> y(100);

// 	for(float i=0; i < 100; i++ ) { 
// 		float xi = i/100;
// 		x[i] = xi;
// 		y[i] = 3+(2*xi)-0.5*(xi*xi)+0.25*(xi*xi*xi)-0.1*(xi*xi*xi*xi)+0.01*(xi*xi*xi*xi*xi)-0.001*(xi*xi*xi*xi*xi);
// 	}
	
// 	this->subjVector = x;
// 	this->refVector  = y;
// 	this->calculateOutliers(2);
// 	this->optimalPolynomial(0,10);

// 	//for(float i=0; i < 100; i++ ) { 
// 	//	cerr << x[i] << " " << y[i] << " " << stats->predict(x[i]) << endl;
// 	//}

// 	//stats->summary(); 

// 	exit(-1);
// }

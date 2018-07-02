#include "polyFit.h"



void PolyFit::saveFit() {
	cerr << "saveFit()" << endl;
	fit.clear();
	fit.resize(samples.size());
	for(unsigned int i=0; i < samples.size(); i++ ) {
		fit[i].resize(samples[i]->scans.size());
		for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
			fit[i][ii]=samples[i]->scans[ii]->rt;
		}
	}
}

double PolyFit::checkFit() { 
	vector<double> groupRt  = groupMeanRt();

	double sumR2=0;
	for(unsigned int i=0; i < groups.size(); i++ ) {
		for(unsigned int j=0; j < groups[i]->peakCount(); j++ ) {
			sumR2 += POW2(groupRt[i]-groups[i]->peaks[j].rt);
		}
	}
	cerr << "groups=" << groups.size() << " checkFit() " << sumR2 << endl;
	return sumR2;
}

void PolyFit::polyFitAlgo() {
    if (groups.size() == 0) return;


	for (unsigned int ii=0; ii<groups.size();ii++) {
		PeakGroup* grp = groups.at(ii);
		for (unsigned int jj=0; jj<grp->getPeaks().size(); jj++) {
			Peak peak = grp->getPeaks().at(jj);
			deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] = peak.rt;
		}
	}

    samples.clear();

    set<mzSample*> samplesSet;
	
    for (unsigned int i=0; i < groups.size();  i++ ) {
        for ( unsigned int j=0; j < groups[i]->peakCount(); j++ ) {
                Peak& p = groups[i]->peaks[j];
                mzSample* sample = p.getSample();
                if (sample) samplesSet.insert(sample);
        }
	}

    samples.resize(samplesSet.size());
	copy(samplesSet.begin(), samplesSet.end(),samples.begin());

    for(unsigned int i=0; i < samples.size(); i++ ) {
        samples[i]->saveOriginalRetentionTimes();
    }

    saveFit();
	double R2_before = checkFit();

    cerr << "Max Itterations: " << maxIterations << endl;
    for(int iter=0; iter < maxIterations; iter++) {
        cerr << iter << endl;

        /** polyfit algo starts **/ 
        
        polyFit(polynomialDegree);


        /** polyfit algo ends **/

        double R2_after = checkFit();
        cerr << "Itteration:" << iter << " R2_before" << R2_before << " R2_after=" << R2_after << endl;

        if (R2_after > R2_before) {
            cerr << "done...restoring previous fit.." << endl;
            restoreFit();
            break;
        } else {
            saveFit();
        }
        R2_before = R2_after;
    }

}

void PolyFit::restoreFit() {
	cerr << "restoreFit() " << endl;
	for(unsigned int i=0; i < samples.size(); i++ ) {
		for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
			samples[i]->scans[ii]->rt = fit[i][ii];
		}
	}
}

void PolyFit::polyFit(int poly_align_degree) {

	if (groups.size() < 2 ) return;
	cerr << "Align: " << groups.size() << endl;

	vector<double> GroupsMeansRt  = groupMeanRt();

	for (unsigned int s=0; s < samples.size(); s++ ) {
        mzSample* sample = samples[s];
        if (sample == NULL) continue;

        StatisticsVector<float>subj;
        StatisticsVector<float>ref;
        int n=0;

        map<int,int>duplicates;
        for(unsigned int j=0; j < groups.size(); j++ ) {
            Peak* p = groups[j]->getPeak(sample);
            if (!p) continue;
            if (!p || p->rt <= 0 || GroupsMeansRt[j] <=0 ) continue;

            int intTime = (int) p->rt*100;
            duplicates[intTime]++;
            if ( duplicates[intTime] > 5 ) continue;

            ref.push_back(GroupsMeansRt[j]);
            subj.push_back(p->rt);
            n++; 
        }
        if ( n < 10 ) continue;

		polyAlignerInit(subj,ref);
        AlignmentStats* stats = optimalPolynomial(1,poly_align_degree,10);

        sampleDegree[sample] = stats->poly_align_degree;
        sampleCoefficient[sample] = stats->getCoeffients();

        if (stats->transformImproved()) {

            bool failedTransformation=false;
            for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
                double newrt =  stats->predict(sample->scans[ii]->rt);
                if (std::isnan(newrt) || std::isinf(newrt))  failedTransformation = true;
                break;
            }

            if (!failedTransformation) {
                for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
                    sample->scans[ii]->rt = stats->predict(sample->scans[ii]->rt);
                }

                for(unsigned int ii=0; ii < groups.size(); ii++ ) {
                    Peak* p = groups[ii]->getPeak(sample);
                    if (p)  p->rt = stats->predict(p->rt);
                }
            }
        } else 	{
            cerr << "APPLYTING TRANSFORM FAILED! " << endl;
        }
    }
}

void PolyFit::polyAlignerInit(StatisticsVector<float>& subj, StatisticsVector<float>& ref) {

	if( subj.size() != ref.size()) { 
		cerr << "alignVector failed.. vectors are different size\n";
		exit(-1);
	}

	//copy values that are presents in both vectors
	for(unsigned int indx=0; indx < subj.size(); indx++ ) {
		if (subj[indx]> 0 and ref[indx] > 0)  { 
			subjVector.push_back(subj[indx]);
			refVector.push_back(ref[indx]);
		}
	}
	mtRand = new MTRand(time(NULL));
	calculateOutliers(3);
}

double PolyFit::calculateR2(AlignmentStats* model) { 
	double R2=0;
	int N = subjVector.size();
	if (N == 0) return DBL_MAX;

	for(int i=0; i < N; i++ ) {
         double newrt = model->predict(subjVector[i]);
		 R2 += POW2(refVector[i] - newrt);
	}
	return sqrt(R2/N);
}

double PolyFit::countInliners(AlignmentStats* model, float zValueCutoff) { 

	int N = subjVector.size();
	if (N == 0) return 0;

	StatisticsVector<float>residuals;
	for(int i=0; i < N; i++ ) {
		 if(outlierVector[i] == true ) {
         	double newrt = model->predict(subjVector[i]);
		 	residuals.push_back(POW2(refVector[i] - newrt));
		 }
	}

	float meanResidual=residuals.mean();
	float stdResidual=sqrt(residuals.variance(meanResidual));

	int inlinerCount=0; 
	double R2=0;
	for(unsigned int i=0; i < residuals.size(); i++ ) {
		if (abs((residuals[i] - meanResidual)/stdResidual) < zValueCutoff ) {
			R2 += residuals[i];
			inlinerCount++;
		}
	}
	if (inlinerCount > 0 ) {
		return sqrt(R2/inlinerCount);
	} else {
		return DBL_MAX;
	}
}

void PolyFit::randomOutliers(double keepFrac) {

	int N = subjVector.size();
	if(!outlierVector.size()) outlierVector = vector<bool>(N,false);

	for(int i=0; i < N; i++ ) {
		 double r = mtRand->rand(); //random number from 0 to 1
		 if (r < keepFrac ) {
			 outlierVector[i]=false;
		 } else {
			 outlierVector[i]=true;
		 }
	}
}

PolyFit::AlignmentStats* PolyFit::align(int degree) {

	AlignmentStats* stats = new AlignmentStats();
	stats->poly_align_degree = degree; 
	if (stats->poly_align_degree >= 100) stats->poly_align_degree=99;
	
	int knownInBoth=subjVector.size();
	double* x  =   new double[subjVector.size()];
	double* ref =  new double[subjVector.size()];

	//copy values that are presents in both vectors to c arrays
	for(unsigned int indx=0; indx < subjVector.size(); indx++ ) {
		x[indx]  =subjVector[indx];
		ref[indx]=refVector[indx];
	}

	stats->N = knownInBoth;

	//remove outliers?
	if(outlierVector.size() > 0 ) {
		int j=0;
		stats->N=0;
		for(unsigned int i=0; i< outlierVector.size();i++) { 
			if(outlierVector[i] == false) { 
				x[j]= x[i];
				ref[j]=ref[i];
				stats->N++;
				j++;
			}
		}
		//cerr << "Removed " << knownInBoth - stats->N << endl;
	}

	if(stats->poly_align_degree > stats->N/3 ) { stats->poly_align_degree = stats->N/3; }

	double* result = new double[(stats->poly_align_degree+1)];
	double* w = new double[(stats->poly_align_degree+1)*(stats->poly_align_degree+1)];

	
	if(stats->N > 0) {
		stats->R_before=0; 
		stats->R_after=0;

		sort_xy(x, ref, stats->N, 1, 0);
        leasqu(stats->N, x, ref, stats->poly_align_degree, w, (stats->poly_align_degree+1), result);	//polynomial fit

		stats->transformedFailed=0;
		for(int ii=0; ii < stats->N; ii++)  { 
            double newrt = leasev(result, stats->poly_align_degree, x[ii]);
            if (newrt != newrt || std::isinf(newrt)) {
				cerr << "Transform failed: " << ii << "\t" << x[ii] << "-> " << newrt << endl;
				stats->transformedFailed++;
			} else { 
				stats->R_before += POW2(ref[ii] - x[ii]);
				stats->R_after  += POW2(ref[ii] - newrt);
			}
		}

		if(stats->transformedFailed == 0) {
			stats->poly_transform_result = vector<double>(stats->poly_align_degree+1,0);
			for(int ii=0; ii <= stats->poly_align_degree; ii++ ) stats->poly_transform_result[ii]=result[ii]; 
		}
	}

    delete[] result;
	delete[] w;
	delete[] x;
	delete[] ref;
	return stats;
}

void PolyFit::calculateOutliers(int initDegree) { 
	if (subjVector.size() <= 2) return;

	AlignmentStats* stats = this->align(initDegree); 
	int N = subjVector.size();

	StatisticsVector<float> distVector(N,0);
	for(int i=0; i < N; i++ ) {
		if(refVector[i]>0) {
			distVector[i] = POW2(refVector[i] - stats->predict(subjVector[i]));
		}
	}

	double meanDelta = distVector.mean();
	double stddev = sqrt(distVector.variance(meanDelta));

	outlierVector = vector<bool>(N,false);
	for(int i=0; i < N; i++ ) {
		distVector[i] = abs((distVector[i] - meanDelta)/stddev);
		if (distVector[i] > 1.5) {
			outlierVector[i] = true;
		} else {
			outlierVector[i] = false;
		}
    }

	/*(
	cerr << "OUTLIERS: " << endl;
	for(int i=0; i < N; i++ ) {
		cerr << outlierVector[i] << " r=" << refVector[i] << " pred=" << stats->predict(subjVector[i]) << " dist=" << distVector[i] << endl;
	}
	*/

	delete stats;
}


PolyFit::AlignmentStats* PolyFit::optimalPolynomial(int fromDegree, int toDegree, int sampleSize=0) { 

	float R_best = FLT_MAX;
    AlignmentStats* bestModel = NULL;

	for(int deg=fromDegree; deg<toDegree; deg++) { 
		for(int attempt=0; attempt <= sampleSize; attempt++ ) { 

			AlignmentStats* thisModel = this->align(deg); 
			double R_thismodel = this->calculateR2(thisModel);
			//double R_thismodel = this->countInliners(thisModel,2.0);

			randomOutliers(0.35); //new random subset of data

			if (R_thismodel < R_best ) { 
                //cerr << "optimalPolynomial() \t attempt=" << attempt << " R2=" << R_thismodel << endl;
				if (bestModel) delete(bestModel);
				bestModel = thisModel;
				R_best = R_thismodel; 
				continue;
			}  else {
				delete(thisModel);
			}

		}
	}

	if ( bestModel ) return bestModel;
	else return this->align(1);
}

void PolyFit::test() { 

	vector<float> x(100);
	vector<float> y(100);

	for(float i=0; i < 100; i++ ) { 
		float xi = i/100;
		x[i] = xi;
		y[i] = 3+(2*xi)-0.5*(xi*xi)+0.25*(xi*xi*xi)-0.1*(xi*xi*xi*xi)+0.01*(xi*xi*xi*xi*xi)-0.001*(xi*xi*xi*xi*xi);
	}
	
	this->subjVector = x;
	this->refVector  = y;
	this->calculateOutliers(2);
	this->optimalPolynomial(0,10);

	//for(float i=0; i < 100; i++ ) { 
	//	cerr << x[i] << " " << y[i] << " " << stats->predict(x[i]) << endl;
	//}

	//stats->summary(); 

	exit(-1);
}

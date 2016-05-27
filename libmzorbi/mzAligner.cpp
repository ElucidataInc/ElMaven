#include "mzSample.h"
#include "mzMassSlicer.h"
#include "math.h"


Aligner::Aligner() {
	maxItterations=10;
	polynomialDegree=3;
}

void Aligner::doAlignment(vector<PeakGroup*>& peakgroups) {
	if (peakgroups.size() == 0) return;

	//store groups into private variable
	allgroups = peakgroups;
	//sort(allgroups.begin(), allgroups.end(), PeakGroup::compRt);
	samples.clear();

	set<mzSample*> samplesSet;
	for (unsigned int i=0; i < peakgroups.size();  i++ ) {
			for ( unsigned int j=0; j < peakgroups[i]->peakCount(); j++ ) {
					Peak& p = peakgroups[i]->peaks[j];
					mzSample* sample = p.getSample();
					if (sample) samplesSet.insert(sample);
			}
	}

	//unique list of samples
	samples.resize(samplesSet.size());
	copy(samplesSet.begin(), samplesSet.end(),samples.begin());

    for(unsigned int i=0; i < samples.size(); i++ ) {
        samples[i]->saveOriginalRetentionTimes();
    }

	 saveFit();
	 double R2_before = checkFit();

	 int iter=0;
	 while (++iter < maxItterations) {
	 	Fit();
	 	double R2_after = checkFit();

		if (R2_after > R2_before) {
			restoreFit();
			break;
		} else {
			saveFit();
		}
		R2_before = R2_after;
	 }
}


void Aligner::saveFit() {
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

void Aligner::restoreFit() {
	cerr << "restoreFit() " << endl;
	for(unsigned int i=0; i < samples.size(); i++ ) {
		for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
			samples[i]->scans[ii]->rt = fit[i][ii];
		}
	}
}
vector<double> Aligner::groupMeanRt() {
		//find retention time deviation
		vector<double> groupRt(allgroups.size());
		for (unsigned int i=0; i < allgroups.size(); i++ ) groupRt[i]=allgroups[i]->medianRt();
		return(groupRt);
}

double Aligner::checkFit() { 
	vector<double> groupRt  = groupMeanRt();

	double sumR2=0;
	for(unsigned int i=0; i < allgroups.size(); i++ ) {
		for(unsigned int j=0; j < allgroups[i]->peakCount(); j++ ) {
			sumR2 += POW2(groupRt[i]-allgroups[i]->peaks[j].rt);
		}
	}
	cerr << "groups=" << allgroups.size() << " checkFit() " << sumR2 << endl;
	return sumR2;
}

void Aligner::Fit() {

	if (allgroups.size() < 2 ) return;
	cerr << "Align: " << allgroups.size() << endl;

	double* x  = new double[allgroups.size()];
	double* ref =  new double[allgroups.size()];
	double* b =  new double[allgroups.size()];
	double* c =  new double[allgroups.size()];
	double* d =  new double[allgroups.size()];

//	ofstream temp;
//	temp.open("/tmp/fit.csv");


	int ideg=polynomialDegree;

	//polynomial fit with maximum possible degree 10
	int maxdeg=10;
	double* result = new double[maxdeg];
	double* w = new double[maxdeg*maxdeg];

	//vector<double> groupRt  = groupMeanRt();

	for (unsigned int s=0; s < samples.size(); s++ ) {
			mzSample* sample = samples[s];
			if (sample == NULL) continue;
			map<int,int>duplicates;

			int n=0;
			StatisticsVector<float>diff;
			for(unsigned int j=0; j < allgroups.size(); j++ ) {
				Peak* p = allgroups[j]->getPeak(sample);
				if (!p) continue;
				if (p->rt <= 0) continue;
				if (p->quality < 0.5 ) continue;

				int intTime = (int) p->rt*10000;
				duplicates[intTime]++;
				if ( duplicates[intTime] > 30 ) continue;

				ref[n]=allgroups[j]->medianRt();
				x[n]=p->rt; 

				diff.push_back(x[n]-ref[n]);
				n++; 
			}
			if ( n == 0 ) continue;
					
			float meanDiv = diff.mean();
			float stdDiv  = diff.stddev();

			if ( stdDiv == 0) continue;

			int removedCount=0;
			for(int ii=0; ii < n; ii++ ) {
				float z = (diff[ii]-meanDiv)/stdDiv;
				if ( abs(z) > 10) {
					//cerr << z << " " << x[ii] << " " << ref[ii] << " " << meanDiv << " " << stdDiv << endl;
					x[ii]=0; 
					ref[ii]=0; 
					removedCount++;
				}
			}

			if (n - removedCount < 10) {
				cerr << "\t Can't align.. too few peaks n=" << n << " removed=" << removedCount << endl;
				continue;
			}

			//for(int ii=0; ii < ideg; ii++ ) { cerr << "Fit: " << ii << " " << result[ii] << endl; }

			double R_before=0;
			for(int ii=0; ii < n; ii++)  R_before += POW2(ref[ii] - x[ii]);

			double R_after=0;   
			bool transformedFailed=false;
			double maxRt = sample->maxRt;

			sort_xy(x, ref, n, 1, 0);
			leasqu(n, x, ref, ideg, w, maxdeg, result);	//polynomial fit

			//for(int ii=0; ii < n; ii++) d[ii]=c[ii]=b[ii]=0;
			//spline(n, x, ref, b, c, d);

			for(int ii=0; ii < n; ii++)  { 
				float newrt = leasev(result, ideg, x[ii]);
				//float newrt = seval(n, x[ii], x, ref, b, c, d);
				//temp << s << ", " << x[ii] << "," << ref[ii] << "," <<  newrt << endl;
				if (newrt != newrt || ISINF(newrt) || newrt < -0.2 || newrt > maxRt*1.2 || abs(newrt-x[ii])/x[ii] > .2) {
                    cerr << "Polynomical transform failed! (A) " << x[ii] << "-->" << newrt <<  endl;
                    transformedFailed=true;
				}  else {
					R_after  += POW2(ref[ii] - newrt);
				}
			}

			cerr << "Alignment Sample: n=" << n << " mean=" << meanDiv << " std=" << stdDiv;
			cerr << "\t delta R" << R_before-R_after << endl;

			if (transformedFailed == false &&  R_after < R_before ) { 
					for(int ii=0; ii < n; ii++) {
						//float newrt = seval(n, x[ii], x, ref, b, c, d);
						float newrt = leasev(result,ideg, x[ii]);
							x[ii] = newrt;
					}

					for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
						//float newrt = seval(n, sample->scans[ii]->rt, x, ref, b, c, d);
						float newrt =  leasev(result, ideg, sample->scans[ii]->rt);
						if (!isnan(newrt) && !ISINF(newrt)) { //nan check
							sample->scans[ii]->rt = newrt;
						}
					}

					for(unsigned int ii=0; ii < allgroups.size(); ii++ ) {
						Peak* p = allgroups[ii]->getPeak(sample);
						if (p) { 
							//float newrt = seval(n, p->rt, x, ref, b, c, d);
							float newrt = leasev(result, ideg, p->rt);
							if (!isnan(newrt) && !ISINF(newrt)) { //nan check
								p->rt = newrt;
							} else {
								cerr << "Polynomial transformed failed! (peak)" << p->rt << endl;
							}
						}
						/*
							if (allgroups[ii]->hasCompoundLink()) { 
								Compound* c = allgroups[ii]->compound;
								Peak* p2 = c->getPeakGroup()->getPeak(sample);
								if (p2) { p2->rt = leasev(result, ideg, p2->rt); }
							}
						*/
					}

			} else {
					cerr << sample->sampleName << "Not Aligning :) R2 increased" << endl;
			}
	}

	delete[] result;
	delete[] w;
	delete[] x;
	delete[] ref;
	//temp.close();
	delete[] b;
	delete[] c;
	delete[] d;
}

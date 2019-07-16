#include <QJsonArray>
#include <QJsonValue>

#include "mzAligner.h"
#include "mzMassSlicer.h"
#include "mzSample.h"
#include "PolyAligner.h"
#include "mzSample.h"
#include "Compound.h"
#include "obiwarp.h"
#include "mavenparameters.h"
#include "Peak.h"
#include "Scan.h"

mzSample* Aligner::refSample = nullptr;

Aligner::Aligner() {
       maxIterations=10;
       polynomialDegree=3;
}

void Aligner::doAlignment(vector<PeakGroup*>& peakgroups)
{
	if (peakgroups.size() == 0) return;

	//store groups into private variable
	allgroups = peakgroups;

	for (unsigned int ii = 0; ii < allgroups.size(); ii++) {
		PeakGroup* grp = allgroups.at(ii);
		for (unsigned int jj = 0; jj < grp->getPeaks().size(); jj++) {
			Peak peak = grp->getPeaks().at(jj);
			deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] = peak.rt;
		}
	}

    samples.clear();
	set<mzSample*> samplesSet;
	for (unsigned int i = 0; i < peakgroups.size(); i++) {
		for ( unsigned int j = 0; j < peakgroups[i]->peakCount(); j++) {
			Peak& p = peakgroups[i]->peaks[j];
			mzSample* sample = p.getSample();
			if (sample) samplesSet.insert(sample);
		}
	}

	//unique list of samples
	samples.resize(samplesSet.size());
	copy(samplesSet.begin(), samplesSet.end(),samples.begin());

	saveFit();
	double R2_before = checkFit();

    cerr << "Max Iterations: " << maxIterations << endl;
    for(int iter = 0; iter < maxIterations; iter++) {
		cerr << iter << endl;

        PolyFit(polynomialDegree);
        double R2_after = checkFit();
        cerr << "Iteration:" << iter << " R2_before" << R2_before << " R2_after" << R2_after << endl;

		if (R2_after > R2_before) {
            cerr << "done..restoring previous fit.." << endl;
			restoreFit();
			break;
		} else {
			saveFit();
		}
		
        R2_before = R2_after;
	}

	for (unsigned int ii = 0; ii < allgroups.size(); ii++) {
		PeakGroup* grp = allgroups.at(ii);
		for (unsigned int jj = 0; jj < grp->getPeaks().size(); jj++) {
			Peak peak = grp->getPeaks().at(jj);
			deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] -= peak.rt;
		}
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

void Aligner::PolyFit(int poly_align_degree) {

	if (allgroups.size() < 2 ) return;
	cerr << "Align: " << allgroups.size() << endl;

	vector<double> allGroupsMeansRt  = groupMeanRt();

	for (unsigned int s=0; s < samples.size(); s++ ) {
			mzSample* sample = samples[s];
			if (sample == NULL) continue;

			StatisticsVector<float>subj;
			StatisticsVector<float>ref;
			int n=0;

            map<int,int>duplicates;
			for(unsigned int j=0; j < allgroups.size(); j++ ) {
				Peak* p = allgroups[j]->getPeak(sample);
				if (!p) continue;
                if (!p || p->rt <= 0 || allGroupsMeansRt[j] <=0 ) continue;

                int intTime = (int) p->rt*100;
                duplicates[intTime]++;
                if ( duplicates[intTime] > 5 ) continue;

                ref.push_back(allGroupsMeansRt[j]);
				subj.push_back(p->rt);
				n++; 
			}
			if ( n < 10 ) continue;

			PolyAligner polyAligner(subj,ref);
            AlignmentStats* stats = polyAligner.optimalPolynomial(1,poly_align_degree,10);

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

                    for(unsigned int ii=0; ii < allgroups.size(); ii++ ) {
                        Peak* p = allgroups[ii]->getPeak(sample);
                        if (p)  p->rt = stats->predict(p->rt);
                    }
                }
            } else 	{
                cerr << "APPLYTING TRANSFORM FAILED! " << endl;
            }
    }
}

void Aligner::Fit(int ideg) {

	if (allgroups.size() < 2 ) return;
	cerr << "Align: " << allgroups.size() << endl;

	double* x  = new double[allgroups.size()];
	double* ref =  new double[allgroups.size()];
    double* b =  new double[allgroups.size()];
	double* c =  new double[allgroups.size()];
	double* d =  new double[allgroups.size()];

//	ofstream temp;
//	temp.open("/tmp/fit.csv");

    //polynomial fit with maximum possible degree 5
    int maxdeg=5;
    if(ideg > maxdeg) ideg=maxdeg;
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
                //if (p->quality < 0.5 ) continue;
                int intTime = (int) p->rt*100;
                duplicates[intTime]++;
                if ( duplicates[intTime] > 5 ) continue;

				ref[n]=allgroups[j]->medianRt();
				x[n]=p->rt; 

                diff.push_back(POW2(x[n]-ref[n]));
				n++; 
			}
			if ( n == 0 ) continue;

            //double meanDiv = diff.mean();
            double stdDiv  = diff.stddev();
            if ( stdDiv == 0) continue;

            //REMOVE OUTLIERS
            int cutpos = n*0.95;
            double cut=diff[n-1]; if(cutpos>=0) cut=diff[cutpos];

			int removedCount=0;
			for(int ii=0; ii < n; ii++ ) {
                double deltaX = POW2(x[ii]-ref[ii]);
                if(deltaX > cut) {
                    //cerr << deltaX << " " << x[ii] << " " << ref[ii] << " " << meanDiv << " " << stdDiv << endl;
					x[ii]=0; 
					ref[ii]=0; 
					removedCount++;
				}
			}
			if (n - removedCount < 10) {
				cerr << "\t Can't align.. too few peaks n=" << n << " removed=" << removedCount << endl;
				continue;
			}

            //SORT AND ALIGN
			double R_before=0;
			for(int ii=0; ii < n; ii++)  R_before += POW2(ref[ii] - x[ii]);

			double R_after=0;   
            int transformedFailed=0;
            sort_xy(x, ref, n, 1, 0);
            leasqu(n, x, ref, ideg, w, maxdeg, result);	//polynomial fit
            //for(int ii=0; ii < ideg; ii++ ) { cerr << "Fit: " << ii << " " << result[ii] << endl; }


            //for(int ii=0; ii < n; ii++) d[ii]=c[ii]=b[ii]=0;
            //spline(n, x, ref, b, c, d);

			for(int ii=0; ii < n; ii++)  { 
                double newrt = leasev(result, ideg, x[ii]);
                //float newrt = seval(n, x[ii], x, ref, b, c, d);
				//temp << s << ", " << x[ii] << "," << ref[ii] << "," <<  newrt << endl;
                if (newrt != newrt || !std::isinf(newrt)) {
                    //cerr << "Polynomical transform failed! (A) " << x[ii] << "-->" << newrt <<  endl;
                    transformedFailed++;
				}  else {
					R_after  += POW2(ref[ii] - newrt);
				}
			}

            if(R_after > R_before ) {
                cerr << "Skipping alignment of " << sample->sampleName << " failed=" << transformedFailed << endl;
                 continue;
            }

            int failedTransformation=0;
            double zeroOffset =  leasev(result, ideg, 0);

            //cerr << "Alignment Sample: n=" << n << " mean=" << meanDiv << " std=" << stdDiv;
            //cerr << "\t improved R2" << R_before-R_after << " BAD=" << transformedFailed << endl;
            // cerr << "zeroOffset=" << zeroOffset << endl;
            for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
                //float newrt = seval(n, sample->scans[ii]->rt, x, ref, b, c, d);
                double newrt =  leasev(result, ideg, sample->scans[ii]->rt)-zeroOffset;
                if (!std::isnan(newrt) && !std::isinf(newrt)) { //nan check
                    sample->scans[ii]->rt = newrt;
                } else {
                    cerr << "error: " << sample->scans[ii]->rt << " " << newrt << endl;
                    failedTransformation++;
                }
            }

            for(unsigned int ii=0; ii < allgroups.size(); ii++ ) {
                Peak* p = allgroups[ii]->getPeak(sample);
                if (p) {
                    //float newrt = seval(n, p->rt, x, ref, b, c, d);
                    double newrt = leasev(result, ideg, p->rt)-zeroOffset;
                    if (!std::isnan(newrt) && !std::isinf(newrt)) { //nan check
                        p->rt = newrt;
                    } else {
                        //cerr << "Polynomial transformed failed! (peak)" << p->rt << endl;
                    }
                }
            }

            if (failedTransformation) {
                cerr << "APPLYTING TRANSFORM FAILED: " << failedTransformation << endl;
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
bool Aligner::alignSampleRts(mzSample* sample,
                             vector<float> &mzPoints,
                             ObiWarp& obiWarp,
                             bool setAsReference,
                             const MavenParameters* mp)
{
    vector<float> rtPoints;
    vector<vector<float> > mxn;

    int mxnCount = 0;
    for(auto scan: sample->scans) {
        if(mp->stop) return (true);
        if(scan->mslevel == 1 ) {
            rtPoints.push_back(scan->originalRt);
            mxn.push_back(vector<float> (mzPoints.size()));
        }
    }

    for(auto scan: sample->scans) {
        if(scan->mslevel == 1) {
            mxnCount++;
            for(int i = 0; i <  scan->mz.size(); i++) {
                if (mp->stop) return (true);
                if (scan->mz.at(i) < mzPoints.front() || scan->mz.at(i) > mzPoints.back())
                    continue;
                int index = upper_bound(mzPoints.begin(), mzPoints.end(), scan->mz.at(i)) - mzPoints.begin() -1;
                mxn[mxnCount - 1][index] = max(mxn[mxnCount -1][index], scan->intensity.at(i));
            }
        }
    }

    
    if (setAsReference) {
        if (mp->stop) return (true);
        obiWarp.setReferenceData(rtPoints, mzPoints, mxn);
    }
    else {

        rtPoints = obiWarp.align(rtPoints, mzPoints, mxn);
        if (rtPoints.empty()) return(true);
        for(int j = 0, i=0 ; j < rtPoints.size(); i++) {
            if(sample->scans.at(i)->mslevel ==1) {
                sample->scans.at(i)->rt = rtPoints.at(j);
                j++;
            }
        }
    }
    return (false);
}

void Aligner::setRefSample(mzSample* sample)
{
    if(sample != nullptr)
        std::cerr << "reference sample :  " << sample->sampleName << std::endl;
    refSample = sample;
}

bool Aligner::alignWithObiWarp(vector<mzSample*> samples,
                              ObiParams* obiParams,
                              const MavenParameters* mp)
{
    if (refSample == nullptr) {
        srand(time(NULL));
        refSample = samples[rand()%samples.size()];
    }

    //save current retention times
    for (auto sample : samples) {
        sample->saveCurrentRetentionTimes();
    }

    ObiWarp* obiWarp = new ObiWarp(obiParams);

    float binSize = obiParams->binSize;
    float minMzRange = 1e9;
    float maxMzRange = 0;

    for(const auto scan: refSample->scans) {
        // PRM/DDA data have both mslevel 1 and mslevel 2 scans. We only want to align mslevel 1 scans
        if(scan->mslevel == 1) {
            for(const auto mz: scan->mz) {
                minMzRange = min(minMzRange, mz);
                maxMzRange = max(maxMzRange, mz);
            }
        }
    }

    maxMzRange += 10;
    minMzRange -= 10;
    if(minMzRange < 0.f)
        minMzRange = 0.f;
    minMzRange = floor(minMzRange);
    maxMzRange = ceil(maxMzRange);
    vector<float> mzPoints;
    for (float bin = minMzRange; bin <= maxMzRange; bin += binSize)
        mzPoints.push_back(bin);

    bool stopped = false;
    stopped = alignSampleRts(refSample, mzPoints, *obiWarp, true, mp);

    if (mp->stop || stopped) {
        delete obiWarp;
        return (true);
    }

    int samplesAligned = 0;
    #pragma omp parallel for shared(samplesAligned)
    for (int i = 0; i < samples.size(); ++i) {
        if (samples[i] == refSample)
            continue;
        if (mp->stop || stopped) {
            stopped = true;
            #pragma omp cancel for
        }
        #pragma omp cancellation point for
        if (alignSampleRts(samples[i], mzPoints, *obiWarp, false, mp)) {
            stopped = true;
        } else {
            samplesAligned++;
            setAlignmentProgress("Aligning samples", samplesAligned, samples.size()-1);
        }
    }

    cerr << "Samples modified: " << samplesAligned << endl;
    delete obiWarp;
    return(stopped);
}

float AlignmentSegment::updateRt(float oldRt)
{
    // fractional distance from start of a segement
    if (oldRt >= segStart and oldRt <= segEnd) {
        float frac = (oldRt - segStart) / (segEnd - segStart);
        return newStart + frac * (newEnd - newStart);
    } else {
        cerr << "Bad Map: "
             << oldRt << "\t"
             << segStart
             << "\t"
             << segEnd
             << endl;

        // could not correct return old rt
        return oldRt;
    }
}

void Aligner::addSegment(string sampleName, AlignmentSegment* seg) {
    if (_alignmentSegments.count(sampleName) == 0) {
        _alignmentSegments[sampleName] = {};
    }
    _alignmentSegments.at(sampleName).push_back(seg);
}

void Aligner::performSegmentedAlignment()
{
    for (auto sample : samples) {
        if (sample == nullptr)
            continue;

        string sampleName = sample->sampleName;
        if (_alignmentSegments.count(sampleName) == 0) {
            cerr << "Cannot find alignment information for sample "
                 << sampleName
                 << endl;
            continue;
        }

        for (auto scan : sample->scans) {
            AlignmentSegment* seg = nullptr;
            for (auto segment : _alignmentSegments[sampleName]) {
                if (scan->rt >= segment->segStart
                    && scan->rt <= segment->segEnd) {
                    seg = segment;
                    break;
                }
            }

            if (seg) {
                double newRt = seg->updateRt(scan->rt);
                scan->rt = newRt;
            } else {
                cerr << "Cannot find segment for: "
                     << sampleName
                     << "\t"
                     << scan->rt
                     << endl;
            }
        }
    }
}

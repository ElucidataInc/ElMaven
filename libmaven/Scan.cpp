#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "mzSample.h"
#include "mzUtils.h"
#include "SavGolSmoother.h"

Scan::Scan(mzSample* sample, int scannum, int mslevel, float rt,
		float precursorMz, int polarity) {
	this->sample = sample;
	this->rt = rt;
	this->scannum = scannum;
	this->precursorMz = precursorMz;
	this->mslevel = mslevel;
	this->polarity = polarity;
	this->productMz = 0;
	this->collisionEnergy = 0;
	this->centroided = 0;

	/*if ( polarity != 1 && polarity != -1 ) {
	 cerr << "Warning: polarity of scan is not 1 or -1 " << polarity << endl;
	 }*/
}

int Scan::findHighestIntensityPos(float _mz, float ppm) {
	float mzmin = _mz - _mz / 1e6 * ppm;
	float mzmax = _mz + _mz / 1e6 * ppm;

	vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin - 1);
	int lb = itr - mz.begin();
	int bestPos = -1;
	float highestIntensity = 0;
	for (int k = lb; k < nobs(); k++) {
		if (mz[k] < mzmin)
			continue;
		if (mz[k] > mzmax)
			break;
		if (intensity[k] > highestIntensity) {
			highestIntensity = intensity[k];
			bestPos = k;
		}
	}
	return bestPos;
}

vector<int> Scan::findMatchingMzs(float mzmin, float mzmax) {
	vector<int> matches;
	vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin - 1);
	int lb = itr - mz.begin();
	for (int k = lb; k < nobs(); k++) {
		if (mz[k] < mzmin)
			continue;
		if (mz[k] > mzmax)
			break;
		matches.push_back(k);
	}
//	cerr << "matches:" << mzmin << " " << mzmax << " " << matches.size() << endl;
	return matches;
}

//removes intensities from scan that lower than X
void Scan::quantileFilter(int minQuantile) {
	if (intensity.size() == 0)
		return;
	if (minQuantile <= 0 || minQuantile >= 100)
		return;

	int vsize = intensity.size();
	vector<float> dist = quantileDistribution(this->intensity);
	vector<float> cMz;
	vector<float> cIntensity;
	for (int i = 0; i < vsize; i++) {
		if (intensity[i] > dist[minQuantile]) {
			cMz.push_back(mz[i]);
			cIntensity.push_back(intensity[i]);
		}
	}
	vector<float>(cMz).swap(cMz);
	vector<float>(cIntensity).swap(cIntensity);
	mz.swap(cMz);
	intensity.swap(cIntensity);
}

//removes intensities from scan that lower than X
void Scan::intensityFilter(int minIntensity) {
	if (intensity.size() == 0)
		return;

	//first pass.. find local maxima in intensity space
	int vsize = intensity.size();
	vector<float> cMz;
	vector<float> cIntensity;
	for (int i = 0; i < vsize; i++) {
		if (intensity[i] > minIntensity) { //local maxima
			cMz.push_back(mz[i]);
			cIntensity.push_back(intensity[i]);
		}
	}
	vector<float>(cMz).swap(cMz);
	vector<float>(cIntensity).swap(cIntensity);
	mz.swap(cMz);
	intensity.swap(cIntensity);
}

void Scan::simpleCentroid() { //centroid data

	if (intensity.size() < 5)
		return;

	//pass zero smooth..
	int smoothWindow = intensity.size() / 20;
	int order = 2;

	if (smoothWindow < 1) {
		smoothWindow = 2;
	}
	if (smoothWindow > 10) {
		smoothWindow = 10;
	}

	mzUtils::SavGolSmoother smoother(smoothWindow, smoothWindow, order);
	//smooth once
	vector<float> spline = smoother.Smooth(intensity);
	//smooth twice
	spline = smoother.Smooth(spline);

	//find local maxima in intensity space
	int vsize = spline.size();
	vector<float> cMz;
	vector<float> cIntensity;
	for (int i = 1; i < vsize - 2; i++) {
		if (spline[i] > spline[i - 1] && spline[i] > spline[i + 1]) { //local maxima in spline space
		//local maxima in real intensity space
			float maxMz = mz[i];
			float maxIntensity = intensity[i];
			for (int j = i - 1; j < i + 1; j++) {
				if (intensity[i] > maxIntensity) {
					maxIntensity = intensity[i];
					maxMz = mz[i];
				}
			}
			cMz.push_back(maxMz);
			cIntensity.push_back(maxIntensity);
		}
	}

	vector<float>(cMz).swap(cMz);
	vector<float>(cIntensity).swap(cIntensity);
	mz.swap(cMz);
	intensity.swap(cIntensity);

	centroided = true;
}

bool Scan::hasMz(float _mz, float ppm) {
	float mzmin = _mz - _mz / 1e6 * ppm;
	float mzmax = _mz + _mz / 1e6 * ppm;
	vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin);
	//cerr << _mz  << " k=" << lb << "/" << mz.size() << " mzk=" << mz[lb] << endl;
	for (int k = itr - mz.begin(); k < nobs(); k++) {
		if (mz[k] >= mzmin && mz[k] <= mzmax)
			return true;
		if (mz[k] > mzmax)
			return false;
	}
	return false;
}

void Scan::summary() {

	cerr << "Polarity=" << getPolarity() << " msLevel=" << mslevel << " rt="
			<< rt << " m/z size=" << mz.size() << " ints size="
			<< intensity.size() << " precursorMz=" << precursorMz
			<< " productMz=" << productMz << " srmID=" << filterLine
			<< " totalIntensty=" << this->totalIntensity() << endl;

}

//generate multi charges series..endingin in change Zx,Mx
vector<float> Scan::chargeSeries(float Mx, unsigned int Zx) {
	//Mx  = observed m/z
	//Zz  = charge of Mx
	//n =  number of charge states to g
	vector<float> chargeStates(Zx + 20, 0);
	double M = (Mx * Zx) - Zx;
	for (int z = 1; z < Zx + 20; z++)
		chargeStates[z] = (M + z) / z;
	return (chargeStates);
}

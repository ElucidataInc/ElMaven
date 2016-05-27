#ifndef MASSSLICES_H
#define MASSSLICES_H

#include <climits>
#include <map>
#include <vector>

#include "mzSample.h"
#include "mzUtils.h"

class mzSample;
using namespace std;

class MassSlices {

public:
	MassSlices() {
		_maxSlices = INT_MAX;
	}
	~MassSlices() {
		delete_all(slices);
		cache.clear();
	}

	vector<mzSlice*> slices;
	mzSlice* sliceExists(float mz, float rt);
	void algorithmA();
	void algorithmB(float ppm, float minIntensity, int step);
	void setMaxSlices(int x) {
		_maxSlices = x;
	}
	void setSamples(vector<mzSample*> samples) {
		this->samples = samples;
	}

private:
	int _maxSlices;
	vector<mzSample*> samples;
	multimap<int, mzSlice*> cache;

};
#endif

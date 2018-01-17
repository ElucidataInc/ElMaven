#ifndef PEAKFILTERING_H
#define PEAKFILTERING_H

#include <iostream>

#include "Peak.h"
#include "mavenparameters.h"

using namespace std;

class PeakFiltering
{
  public:
	PeakFiltering(MavenParameters *mavenParameters);
	void filter(vector<EIC*> &eics);
	void filter(vector<Peak> &peaks);
	bool filter(Peak &peak);

  private:
	MavenParameters *_mavenParameters;
};

#endif //PEAKFILTERING_H
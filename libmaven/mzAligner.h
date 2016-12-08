#include <cmath>
#include <cstddef>
#include <float.h>
#include <limits>
#include <climits>
#include <vector>
#include "mzSample.h"
#include "Compound.h"

class Aligner;
using namespace std;
class Aligner {
   public:
    Aligner();
    void doAlignment(vector<PeakGroup*>& peakgroups);
    vector<double> groupMeanRt();
    double checkFit();
    void Fit(int ideg);
    void saveFit();
    void restoreFit();
    void setMaxItterations(int x) { maxItterations = x; }
    void setPolymialDegree(int x) { polynomialDegree = x; }
    vector<vector<float> > fit;
    vector<mzSample*> samples;

    int medianRt;
    int compoundDataRt;
    float tolerance;
   private:
    vector<PeakGroup*> allgroups;
    int maxItterations;
    int polynomialDegree;
};

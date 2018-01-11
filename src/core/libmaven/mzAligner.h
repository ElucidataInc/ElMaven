#include <cmath>
#include <cstddef>
#include <float.h>
#include <limits>
#include <climits>
#include <vector>
#include "mzSample.h"
#include "Compound.h"
#include <QJsonObject>

using namespace std;
class Aligner {
   public:
    Aligner();
    void preProcessing(vector<PeakGroup*>& peakgroups,bool alignWrtExpectedRt);
    void doAlignment(vector<PeakGroup*>& peakgroups);
    vector<double> groupMeanRt();
    double checkFit();
    void Fit(int ideg);
    void saveFit();
    void PolyFit(int poly_align_degree);
    void restoreFit();
    void setMaxItterations(int x) { maxItterations = x; }
    void setPolymialDegree(int x) { polynomialDegree = x; }
    map<pair<string,string>, double> getDeltaRt() {return deltaRt; }
	map<pair<string, string>, double> deltaRt;
    vector<vector<float> > fit;
    vector<mzSample*> samples;

    int medianRt;
    int compoundDataRt;
    float tolerance;
    map<mzSample*, int> sampleDegree;
    map<mzSample*, vector<double> > sampleCoefficient;

    void updateRts(QJsonObject& parentObj);
    void updateSampleRts(QJsonObject& sampleRts);
    void updateGroupsRts(QJsonObject& groupsRts);

    QJsonObject groupsJson;
    QJsonObject rtsJson;


   private:
    vector<PeakGroup*> allgroups;
    int maxItterations;
    int polynomialDegree;

};

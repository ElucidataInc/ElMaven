#ifndef MZALIGNER_H
#define MZALIGNER_H

#include <cmath>
#include <cstddef>
#include <float.h>
#include <limits>
#include <climits>
#include <vector>
#include "mzSample.h"
#include "Compound.h"
#include "obiwarp.h"
#include "mavenparameters.h"
#include <QJsonObject>

#include <boost/signals2.hpp>
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
    int alignWithObiWarp(vector<mzSample*> samples,
                         ObiParams* obiParams,
                         const MavenParameters* mp);
    int alignSampleRts(mzSample* sample,
                        vector<float> &mzPoints,
                        ObiWarp& obiWarp,
                        bool setAsReference,
                        const MavenParameters* mp);
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

    static mzSample* refSample;
    static void setRefSample(mzSample* sample);

public:
    boost::signals2::signal< void (const string&,unsigned int , int ) > setAlignmentProgress;

   private:
    vector<PeakGroup*> allgroups;
    int maxItterations;
    int polynomialDegree;

};


#endif

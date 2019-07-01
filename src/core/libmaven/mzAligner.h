#ifndef MZALIGNER_H
#define MZALIGNER_H

#include <QJsonObject>
#include <boost/signals2.hpp>

#include "standardincludes.h"

class PeakGroup;
class mzSample;
class ObiParams;
class ObiWarp;
class MavenParameters;

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
    void setMaxIterations(int x) { maxIterations = x; }
    void setPolymialDegree(int x) { polynomialDegree = x; }
    bool alignWithObiWarp(vector<mzSample*> samples,
                         ObiParams* obiParams,
                         const MavenParameters* mp);
    bool alignSampleRts(mzSample* sample,
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
    int maxIterations;
    int polynomialDegree;

};


#endif

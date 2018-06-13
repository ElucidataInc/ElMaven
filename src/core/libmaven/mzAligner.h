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
#include <QJsonObject>

using namespace std;

class Aligner {
   public:
    // Aligner();
    // void preProcessing(vector<PeakGroup*>& peakgroups,bool alignWrtExpectedRt);
    // void doAlignment(vector<PeakGroup*>& peakgroups);
    // vector<double> groupMeanRt();
    // double checkFit();
    // void Fit(int ideg);
    // void saveFit();
    // void PolyFit(int poly_align_degree);
    // void restoreFit();
    // void setMaxItterations(int x) { maxItterations = x; }
    // void setPolymialDegree(int x) { polynomialDegree = x; }
    void alignWithObiWarp(vector<mzSample*> samples , ObiParams* obiParams, int referenceSampleIndex = -1);
    void alignSampleRts(mzSample* sample, vector<float> &mzPoints,ObiWarp& obiWarp, bool setAsReference);
    // map<pair<string,string>, double> getDeltaRt() {return deltaRt; }
	// map<pair<string, string>, double> deltaRt;
    // vector<vector<float> > fit;
    // vector<mzSample*> samples;

    int medianRt;
    int compoundDataRt;
    float tolerance;
    map<mzSample*, int> sampleDegree;
    map<mzSample*, vector<double> > sampleCoefficient;

    // void updateRts(QJsonObject& parentObj);
    // void updateSampleRts(QJsonObject& sampleRts);
    // void updateGroupsRts(QJsonObject& groupsRts);

    // QJsonObject groupsJson;
    // QJsonObject rtsJson;


//    private:
//     vector<PeakGroup*> allgroups;
//     int maxItterations;
//     int polynomialDegree;

};


// new aligner starts here


class Aligner {
    public:
        Aligner();
        void setGroups(vector<PeakGroups*> x) { groups = new(x); }
        // vector<PeakGroups*> getGroups() { return groups; }

    private:
        vector<PeakGroup*> groups;
        map<pair<string, string>, double> deltaRt;

        vector<double> groupMeanRt();
};

class PolyFit : public Aligner {
    public:
        PolyFit(vector<peakGroup*> group);
        void polyFitAlgo();
	   
    private:
        int maxIterations;
        int polynomialDegree;

        vector<mzSample*> samples;
        vector<vector<float> > fit;

        void setPolymialDegree (int x) { polynomialDegree = x; }
        void setMaxItterations (int x) { maxIterations = x; }

        void saveFit();
        double checkFit();
        void restoreFit();

        // map<pair<string,string>, double> getDeltaRt() { return deltaRt; }


};



class LoessFit : public Aligner {
    public:
        LoessFit(vector<peakGroup*> x, bool y) { groups = new (x); alignWrtExpectedRt = y; pythonProg = new QProcess(); };
        void loessFit();
    private:
        QProcess* pythonProg;
        QJsonObject groupsJson;
        QJsonObject rtsJson;

        bool alignWrtExpectedRt;

        void preProcessing();
        void runPythonProg();
        // other helper functions
}

class ObiWarp : public Aligner {
    public:
        void obiWarp();
    private:
        void preProcessing();
        // other helper functions
};


#endif
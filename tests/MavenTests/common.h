#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "databases.h"
#include "mzSample.h"
#include "mavenparameters.h"
#include "classifierNeuralNet.h"
#include "PeakDetector.h"

extern Databases DBS;

using namespace std;

struct Samples
{
    vector<mzSample*> alignmentSamples;
    Samples() {

        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);


        alignmentSamples[0]->loadSample("bin/methods/091215_120i.mzXML");
        alignmentSamples[1]->loadSample("bin/methods/091215_120M.mzXML");
        alignmentSamples[2]->loadSample("bin/methods/091215_240i.mzXML");
        alignmentSamples[3]->loadSample("bin/methods/091215_240M.mzXML");

    }
};

class common {

    private:
        common();
    public:
        static bool floatCompare(float a, float b);
        static bool compareMaps(const map<string,int> & l, const map<string,int> & k);
        static vector<Compound*> getCompoudDataBaseWithRT();
        static vector<Compound*> getCompoudDataBaseWithNORT();
        static vector<Compound*> getFaltyCompoudDataBase();
        static vector<PeakGroup> getGroupsFromProcessCompounds();
        static void loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                             MavenParameters* mavenparameters);
};

#endif // COMMON_H

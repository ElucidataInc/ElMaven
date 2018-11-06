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

struct Samples {
    mzSample* smallSample;
    vector<mzSample*> regularSamples;
    vector<mzSample*> ms2Samples;
    vector<mzSample*> alignmentSamples;

    Samples() {
        smallSample = new mzSample();
        smallSample->loadSample("bin/methods/testsample_1.mzxml");

        auto sample1 = new mzSample();
        auto sample2 = new mzSample();
        sample1->loadSample("bin/methods/testsample_2.mzxml");
        sample2->loadSample("bin/methods/testsample_3.mzxml");
        regularSamples.push_back(sample1);
        regularSamples.push_back(sample2);

        auto ms2Sample1 = new mzSample();
        auto ms2Sample2 = new mzSample();
        ms2Sample1->loadSample("bin/methods/ms2test1.mzML");
        ms2Sample2->loadSample("bin/methods/ms2test2.mzML");
        ms2Samples.push_back(ms2Sample1);
        ms2Samples.push_back(ms2Sample2);

        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);

        alignmentSamples[0]->loadSample("bin/methods/091215_120i.mzXML");
        alignmentSamples[1]->loadSample("bin/methods/091215_120M.mzXML");
        alignmentSamples[2]->loadSample("bin/methods/091215_240i.mzXML");
        alignmentSamples[3]->loadSample("bin/methods/091215_240M.mzXML");
    }

    ~Samples() {
        delete smallSample;
        delete regularSamples[0];
        delete regularSamples[1];
        delete ms2Samples[0];
        delete ms2Samples[1];
        delete alignmentSamples[0];
        delete alignmentSamples[1];
        delete alignmentSamples[2];
        delete alignmentSamples[3];
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

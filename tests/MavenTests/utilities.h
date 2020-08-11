#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "database.h"
#include "mzSample.h"

class PeakGroup;
class MavenParameters;

using namespace std;

struct TestSamples {
    mzSample* smallSample;
    vector<mzSample*> ms1TestSamples;
    vector<mzSample*> ms2TestSamples;
    vector<mzSample*> alignmentSamples;

    TestSamples() {
        smallSample = new mzSample();
        smallSample->loadSample("bin/methods/testsample_1.mzxml");

        auto sample1 = new mzSample();
        auto sample2 = new mzSample();
        sample1->loadSample("bin/methods/testsample_2.mzxml");
        sample2->loadSample("bin/methods/testsample_3.mzxml");
        ms1TestSamples.push_back(sample1);
        ms1TestSamples.push_back(sample2);

        auto ms2Sample1 = new mzSample();
        auto ms2Sample2 = new mzSample();
        ms2Sample1->loadSample("bin/methods/ms2test1.mzML");
        ms2Sample2->loadSample("bin/methods/ms2test2.mzML");
        ms2TestSamples.push_back(ms2Sample1);
        ms2TestSamples.push_back(ms2Sample2);

        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);
        alignmentSamples.push_back(new mzSample);

        alignmentSamples[0]->loadSample("bin/methods/091215_120i.mzXML");
        alignmentSamples[1]->loadSample("bin/methods/091215_120M.mzXML");
        alignmentSamples[2]->loadSample("bin/methods/091215_240i.mzXML");
        alignmentSamples[3]->loadSample("bin/methods/091215_240M.mzXML");
    }

    ~TestSamples() {
        delete smallSample;
        delete ms1TestSamples[0];
        delete ms1TestSamples[1];
        delete ms2TestSamples[0];
        delete ms2TestSamples[1];
        delete alignmentSamples[0];
        delete alignmentSamples[1];
        delete alignmentSamples[2];
        delete alignmentSamples[3];
    }
};

class TestUtils {

    private:
        TestUtils();

    public:
        static bool floatCompare(float a, float b);
        static float roundTo(float a, int numPlaces);
        static double roundTo(double a, int numPlaces);
        static bool compareMaps(const map<string,int> & l, const map<string,int> & k);
        static vector<Compound*> getCompoudDataBaseWithRT();
        static vector<Compound*> getCompoudDataBaseWithNORT();
        static vector<Compound*> getFaltyCompoudDataBase();
        static vector<PeakGroup> getGroupsFromProcessCompounds();
        static void loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                             MavenParameters* mavenparameters);
};

namespace maventests {
    extern Database database;
    extern TestSamples samples;
}

#endif // COMMON_H

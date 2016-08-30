#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "../libmaven/databases.h"
#include "../libmaven/mzSample.h"
#include "../libmaven/mavenparameters.h"
#include "../libmaven/classifierNeuralNet.h"
#include "../libmaven/PeakDetector.h"

extern Databases DBS;

using namespace std;

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
};

#endif // COMMON_H
#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>

using namespace std;

class common {

    public:
        static bool floatCompare(float a, float b);
        static bool compareMaps(const map<string,int> & l, const map<string,int> & k);
};

#endif // COMMON_H
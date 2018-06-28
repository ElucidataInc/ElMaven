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
#include "PeakGroup.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include "vec.h"
#include "mat.h"
#include "dynprog.h"


using namespace std;

class Aligner {
    public:
        // Aligner();
        void setGroups(vector<PeakGroup*> x) { groups = x; }
        // vector<PeakGroup*> getGroups() { return groups; }
        map<pair<string, string>, double> getDeltaRt() { return deltaRt; }
        vector<PeakGroup*> groups;
        map<pair<string, string>, double> deltaRt;
        void alignmentError(QString);

        vector<double> groupMeanRt();
};

#endif
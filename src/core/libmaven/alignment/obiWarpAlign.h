#ifndef obiWarpAlign_H
#define obiWarpAlign_H


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
#include "mzAligner.h"



class ObiWarpAlign : public Aligner {
    

    public:
        ObiWarpAlign(ObiParams *obiParams, vector<mzSample*> samples, int referenceSampleIndex = -1) {
            *params = *obiParams;

            this->samples = samples;
            this->referenceSampleIndex = referenceSampleIndex;
        }

        void obiWarpAlign ();


        
    
    private:
        ObiParams *params;
        vector<mzSample*> samples;
        int referenceSampleIndex;

        void alignSampleRts (mzSample* sample, vector<float> &mzPoints, ObiWarp& obiWarp, bool setAsReference);
};

#endif
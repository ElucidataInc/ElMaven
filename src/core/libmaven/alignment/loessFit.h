#ifndef loessFit_H
#define loessFit_H

#include <cstddef>
#include <float.h>
#include <limits>
#include <climits>
#include <vector>
#include "mzSample.h"
#include "Compound.h"
#include "PeakGroup.h"
#include <QJsonObject>
#include <QJsonArray>
#include "vec.h"
#include "mat.h"
#include "dynprog.h"
#include "mzAligner.h"


class LoessFit : public Aligner {
    public:
        LoessFit(vector<PeakGroup*> x, bool y, vector <mzSample*> sample) { setGroups(x); alignWrtExpectedRt = y; pythonProg = new QProcess(); samples = sample; }
        void loessFit();
    private:
        QProcess* pythonProg;
        QJsonObject groupsJson;
        QJsonObject rtsJson;

        vector<mzSample*> samples;

        bool alignWrtExpectedRt;
        QByteArray processedDataFromPython;

        void preProcessing();
        void updateSampleRts(QJsonObject &sampleRts);
        void updateGroupsRts(QJsonObject &groupsRts);
        void updateRts(QJsonObject &parentObj);
        void readDataFromPython();
        void writeToPythonProcess(QByteArray data);
        void sendDataToPython();
        void runPythonProg();
        // other helper functions
};

#endif
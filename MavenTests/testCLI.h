#ifndef TESTCLI_H
#define TESTCLI_H

// #define _GLIBCXX_USE_CXX11_ABI 0

#include <string>
#include "common.h"

#include <QtTest>

#include "../libmaven/databases.h"
#include "../libmaven/csvreports.h"
#include "../libmaven/PeakDetector.h"
#include "../libmaven/classifierNeuralNet.h"
#include "../peakdetector/PeakDetectorCLI.h"



class PeakDetectorCLI;

using namespace std;

class TestCLI : public QObject {
    Q_OBJECT

    public:
        TestCLI();
        ~TestCLI();

    private:
        const char* clsfPath;
        const char* dbPath;
        const char* normalSample;
        const char* blankSample;
        const char* xmlPath;
        const char* createXmlPath;

    private Q_SLOTS:
        void testLoadClassificationModel();
        void testLoadCompoundsFile();
        void testLoadSamples();
        void testProcessXml();
        void testCreateXMLFile();
        void testReduceGroups();

};

#endif
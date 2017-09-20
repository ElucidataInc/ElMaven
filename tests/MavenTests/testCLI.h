#ifndef TESTCLI_H
#define TESTCLI_H

// #define _GLIBCXX_USE_CXX11_ABI 0

#include <string>
#include "common.h"

#include <QtTest>

#include "databases.h"
#include "csvreports.h"
#include "PeakDetector.h"
#include "classifierNeuralNet.h"
#include "PeakDetectorCLI.h"



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
        void testWriteReport();

};

#endif

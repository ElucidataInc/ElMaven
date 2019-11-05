#ifndef TESTCLI_H
#define TESTCLI_H

// #define _GLIBCXX_USE_CXX11_ABI 0

#include <string>

#include <QtTest>

class PeakDetectorCLI;
class Logger;
class Analytics;

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
        Logger* _log;
        Analytics* _analytics;

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

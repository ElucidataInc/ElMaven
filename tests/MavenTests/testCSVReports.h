#ifndef TESTCSVREPORTS_H
#define TESTCSVREPORTS_H

#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>

#include "EIC.h"
#include "utilities.h"
#include "mzSample.h"
#include "csvreports.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "isotopeDetection.h"
#include "classifierNeuralNet.h"


class TestCSVReports : public QObject {
    Q_OBJECT

    public:
        TestCSVReports();
    private:

        mzSample* mzsample1;
        mzSample* mzsample2;
        vector<mzSample*> mzsamples;

        string outputfile;

    private Q_SLOTS:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testopenGroupReport();
        void testopenPeakReport();
        void testaddGroup();

        // tests that are called by "testaddGroup" method
        void verifyTargetedGroupReport(vector<mzSample*>& samplesToLoad,
                                       MavenParameters* mavenparameters);
        void verifyUntargetedGroupReport(vector<mzSample*>& samplesToLoad,
                                         MavenParameters* mavenparameters);
        void verifyTargetedPeakReport(vector<mzSample*>& samplesToLoad,
                                      MavenParameters* mavenparameters);
        void verifyUntargetedPeakReport(vector<mzSample*>& samplesToLoad,
                                        MavenParameters* mavenparameters);

};

#endif // TESTCSVREPORTS_H

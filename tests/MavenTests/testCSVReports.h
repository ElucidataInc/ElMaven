#ifndef TESTCSVREPORTS_H
#define TESTCSVREPORTS_H

#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>

#include "EIC.h"
#include "common.h"
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

        void testExport();
};

#endif // TESTCSVREPORTS_H

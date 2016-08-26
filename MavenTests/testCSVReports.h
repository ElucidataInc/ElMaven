#ifndef TESTCSVREPORTS_H
#define TESTCSVREPORTS_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include "common.h"
#include "../libmaven/mzSample.h"
#include "../libmaven/csvreports.h"
#include "../libmaven/mavenparameters.h"
#include "../libmaven/PeakDetector.h"
#include "../libmaven/EIC.h"
#include "../libmaven/classifierNeuralNet.h"


class TestCSVReports : public QObject {
    Q_OBJECT

    public:
        TestCSVReports();
    private:

        mzSample* mzsample1;
        mzSample* mzsample2;
        vector<mzSample*> mzsamples;



        string outputfile;

    private slots:
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
        void testaddGroups();
};

#endif // TESTCSVREPORTS_H

#ifndef TESTSCAN_H
#define TESTSCAN_H
#include <iostream>
#include <vector>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include "common.h"
#include "../libmaven/mzSample.h"


class TestScan : public QObject {
    Q_OBJECT

    public:
        TestScan();
    private:
        mzSample* sample;
        void initScan (Scan*);
    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testdeepcopy();
        void testfindHighestIntensityPos();
        void testfindMatchingMzs();
        void testquantileFilter();
        void testintensityFilter();
        void testsimpleCentroid();
        void testhasMz();
        void testchargeSeries();
        void testdeconvolute();
        void testgetTopPeaks();

};

#endif // TESTSCAN_H

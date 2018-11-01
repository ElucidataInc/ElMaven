#ifndef TESTEIC_H
#define TESTEIC_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include "common.h"
#include "EIC.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "mzSample.h"

class TestEIC : public QObject {
    Q_OBJECT

    public:
        TestEIC();
    private:
        const char* loadFile;
        const char* loadGoodSample;
        QStringList files;
        QStringList files_ms2;

    private Q_SLOTS:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testgetEIC();
        void testgetEICms2();
        void testcomputeSpline();
        void testgetPeakPositions();
        void testcomputeBaseline();
        void testfindPeakBounds();
        void testGetPeakDetails();
        void testgroupPeaks();
        void testeicMerge();
};

#endif // TESTEIC_H

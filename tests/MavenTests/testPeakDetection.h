#ifndef TESTPEAKDETECTION_H
#define TESTPEAKDETECTION_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"
#include "mzSample.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "EIC.h"
#include "classifierNeuralNet.h"

class TestPeakDetection : public QObject {
    Q_OBJECT

    public:
        TestPeakDetection();
    private:
        const char* loadCompoundDB;
        const char* loadCompoundDB1;
        QStringList files;

    private Q_SLOTS:
        // functions executed by QtTest before and after tuest suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testProcessCompound();
        void testPullEICs();
        void testprocessSlices();
        void testpullIsotopes();
};

#endif // TESTPEAKDETECTION_H

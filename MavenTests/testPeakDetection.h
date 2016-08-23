#ifndef TESTPEAKDETECTION_H
#define TESTPEAKDETECTION_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"
#include "../libmaven/mzSample.h"
#include "../libmaven/PeakDetector.h"
#include "../libmaven/mavenparameters.h"
#include "../libmaven/EIC.h"
#include "../libmaven/mavenparameters.h"
#include "../libmaven/classifierNeuralNet.h"

class TestPeakDetection : public QObject {
    Q_OBJECT

    public:
        TestPeakDetection();
    private:
        const char* loadCompoundDB;
        QStringList files;

    private slots:
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

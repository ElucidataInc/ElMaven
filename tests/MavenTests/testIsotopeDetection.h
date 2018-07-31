#ifndef TESTISOTOPEDETECTION_H
#define TESTISOTOPEDETECTION_H

#include <iostream>
#include <QtTest>
#include <string>

#include "common.h"
#include "mzSample.h"
#include "PeakDetector.h"
#include "mavenparameters.h"
#include "isotopeDetection.h"
#include "classifierNeuralNet.h"

class TestIsotopeDetection : public QObject {
    Q_OBJECT

    public:
        TestIsotopeDetection();
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
        void testpullIsotopes();
};

#endif // TESTISOTOPEDETECTION_H

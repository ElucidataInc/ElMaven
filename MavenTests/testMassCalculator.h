#ifndef TESTMASSCALCULATOR_H
#define TESTMASSCALCULATOR_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"
#include "../libmaven/databases.h"

extern Databases DBS;

class TestMassCalculator : public QObject {
    Q_OBJECT

    public:
        TestMassCalculator();
    private:

    private Q_SLOTS:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testGetComposition();
        void testNeutralMass();
        void testComputeMass();
        void testComputeIsotopes();
        void testenumerateMasses();
};

#endif // TESTMASSCALCULATOR_H
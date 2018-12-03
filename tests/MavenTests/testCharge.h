#ifndef TESTCHARGE_H
#define TESTCHARGE_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "utilities.h"
#include "Compound.h"
#include "mzSample.h"
#include "mavenparameters.h"


class TestCharge : public QObject {
    Q_OBJECT

    public:
        TestCharge();
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
        void testChargeWithFormulaNoCompound();
        void testChargeWithNoCompoundNoFormula();
        void testChargeWithCompoundNoCharge();
        void testChargeWithCompoundNoFormula();
        void testChargeWithFormulaAndCompound();

};

#endif // TESTLOADDM_H

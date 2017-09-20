#ifndef TESTMZSLICE_H
#define TESTMZSLICE_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"
#include "mzSample.h"
#include "mavenparameters.h"

class TestMzSlice : public QObject {
    Q_OBJECT

    public:
        TestMzSlice();
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
        void testCalculateMzMaxMinWithCF();
        void testCalculateMzMaxMinWithNOCF();
        void testCalculateMzMaxMinWithNOCFNOMass();
        void testcalculateRTMinMaxWithRTandEnabled();
        void testcalculateRTMinMaxWithNORTandEnabled();
        void testcalculateRTMinMaxWithNORTandDisabled();
        void testcalculateRTMinMaxWithRTandDisabled();
};

#endif // TESTMZSLICE_H

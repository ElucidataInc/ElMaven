#ifndef TESTMZFIT_H
#define TESTMZFIT_H
#include <iostream>
#include <vector>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include "common.h"
#include "../libmaven/mzFit.h"


class TestMzFit : public QObject {
    Q_OBJECT

    public:
        TestMzFit();
    private:

    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testSort_xy();
        //void testCxfree();
        void testGauss();
        void testStasum();
        void testLeasqu();
        void testLeasev();
};

#endif // TESTMZFIT_H

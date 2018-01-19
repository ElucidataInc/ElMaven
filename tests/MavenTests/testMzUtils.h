#ifndef TESTMZUTILS_H
#define TESTMZUTILS_H

#include <QtTest>
#include "common.h"
#include "mzUtils.h"

class TestMzUtils: public QObject {
    Q_OBJECT

    public:
        TestMzUtils();
    private Q_SLOTS:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testmedian();

};

#endif
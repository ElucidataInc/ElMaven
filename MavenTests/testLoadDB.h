#ifndef TESTLOADDM_H
#define TESTLOADDM_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"
#include "../libmaven/databases.h"
#include "../libmaven/mzSample.h"

extern Databases DB;

class TestLoadDB : public QObject {
    Q_OBJECT

    public:
        TestLoadDB();
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
        void testExtractCompoundfromEachLineNormal();
        void testExtractCompoundfromEachLineWithNoMz();
        void testExtractCompoundfromEachLineWithNoMzandFormula();
        void testExtractCompoundfromEachLineWithExpRTandRT();
        void testExtractCompoundfromEachLineWithCompoundField();
        void testloadCompoundCSVFileWithIssues();
        void testloadCompoundCSVFileWithRep();
        //void testloadCompoundCSVFileWithRepNoId();
};

#endif // TESTLOADDM_H
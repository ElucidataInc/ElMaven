#ifndef TESTEIC_H
#define TESTEIC_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include "common.h"

class TestEIC : public QObject {
    Q_OBJECT

    public:
        TestEIC();
    private:
        const char* loadFile;
        const char* blankSample;

    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testFileLoad();
        void testIsAllScansParsed();
        void testScanParsing();
        void testSrmScan();
        void testMinMaxMz();
        void testMinMaxRT();
        void testSampleName();
        void testBlankSample();
};

#endif // TESTEIC_H
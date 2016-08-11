#ifndef TESTCSVREPORTS_H
#define TESTCSVREPORTS_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include "common.h"

class TestCSVReports : public QObject {
    Q_OBJECT

    public:
        TestCSVReports();
    private:
        

        string outputfile;

    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testopenGroupReport();
        
};

#endif // TESTCSVREPORTS_H

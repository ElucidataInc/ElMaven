#ifndef TESTBASE64_H
#define TESTBASE64_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>

class Testbase64 : public QObject {
    Q_OBJECT

    public:
        Testbase64();
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
        void testdecodeBase64();
        void testdecodeString();
};

#endif // TESTBASE64_H

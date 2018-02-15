#ifndef TESTISOTOPELOGIC_H
#define TESTISOTOPELOGIC_H
#include <iostream>
#include <QtTest>
#include <string>
#include <sstream>


class TestIsotopeLogic : public QObject {
    Q_OBJECT

    public:
        TestIsotopeLogic();
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
};

#endif // TESTISOTOPELOGIC_H

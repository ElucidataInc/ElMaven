#ifndef TESTEIC_H
#define TESTEIC_H

#include <QtTest>

class TestEIC : public QObject {
    Q_OBJECT

    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testHello();
};

#endif // TESTEIC_H
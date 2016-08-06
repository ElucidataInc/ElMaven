#include "testEIC.h"
#include "../libmaven/mzSample.h"

void TestEIC::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
}

void TestEIC::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestEIC::init() {
    // This function is executed before each test
}

void TestEIC::cleanup() {
    // This function is executed after each test
    qDebug() << "TestEIC::cleanup()";
}

void TestEIC::testHello() {
    EIC eic;
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}


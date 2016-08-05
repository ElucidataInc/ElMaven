#include "testEIC.h"

void TestEIC::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    qDebug() << "TestEIC::initTestCase()";
}

void TestEIC::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
    qDebug() << "TestEIC::cleanupTestCase()";
}

void TestEIC::init() {
    // This function is executed before each test
    qDebug() << "TestEIC::init()";
}

void TestEIC::cleanup() {
    // This function is executed after each test
    qDebug() << "TestEIC::cleanup()";
}

void TestEIC::testHello() {
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}


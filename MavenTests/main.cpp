#include <QtTest>
#include <QApplication>
#include "testEIC.h"
#include "testMassCalculator.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    TestEIC testEIC;
    TestMassCalculator testMassCalculator;
    // multiple test suites can be ran like this
    return QTest::qExec(&testEIC, argc, argv) | 
           QTest::qExec(&testMassCalculator, argc, argv);
}
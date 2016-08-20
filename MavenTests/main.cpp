#include <QtTest>
#include <QApplication>
#include "testLoadSamples.h"
#include "testMassCalculator.h"
#include "testCSVReports.h"
#include "testPeakDetection.h"
#include "testMzSlice.h"
#include "testLoadDB.h"
#include "testScan.h"
#include "testEIC.cpp"
#include "testbase64.h"
#include "testMzFit.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    int result = 0;

    freopen("testLoadSamples.xml", "w", stdout);
    result |= QTest::qExec(new TestLoadSamples, argc, argv);

    freopen("testMassCalculator.xml",  "w", stdout);
    result |= QTest::qExec(new TestMassCalculator, argc, argv);
    
    freopen("testCSVReports.xml",  "w", stdout);
    result |= QTest::qExec(new TestCSVReports, argc, argv);

    freopen("testPeakDetection.xml",  "w", stdout);
    result |= QTest::qExec(new TestPeakDetection, argc, argv);

    freopen("testLoadDB.xml",  "w", stdout);
    result |= QTest::qExec(new TestLoadDB, argc, argv);

    freopen("testMzSlice.xml",  "w", stdout);
    result |= QTest::qExec(new TestMzSlice, argc, argv);

    freopen("testScan.xml",  "w", stdout);
    result |= QTest::qExec(new TestScan, argc, argv);

    freopen("testEIC.xml",  "w", stdout);
    result |= QTest::qExec(new TestEIC, argc, argv);

    freopen("testbase64.xml",  "w", stdout);
    result |= QTest::qExec(new Testbase64, argc, argv);

    freopen("testMzFit.xml",  "w", stdout);
    result |= QTest::qExec(new TestMzFit, argc, argv);

    return result;
}

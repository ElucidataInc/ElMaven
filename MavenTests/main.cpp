#include <QtTest>
#include <QCoreApplication>
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
#include "testMzAligner.h"
#include "testCLI.h"
#include "testCharge.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    int result = 0;

    if (freopen("testLoadSamples.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadSamples, argc, argv);

    if (freopen("testMassCalculator.xml", "w", stdout))
        result |= QTest::qExec(new TestMassCalculator, argc, argv);

    if(freopen("testCLI.xml",  "w", stdout))
        result |= QTest::qExec(new TestCLI, argc, argv);

    if (freopen("testPeakDetection.xml", "w", stdout))
        result |= QTest::qExec(new TestPeakDetection, argc, argv);

    if(freopen("testCharge.xml",  "w", stdout))
        result |= QTest::qExec(new TestCharge, argc, argv);

    if (freopen("testLoadDB.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadDB, argc, argv);

    if (freopen("testMzSlice.xml", "w", stdout))
        result |= QTest::qExec(new TestMzSlice, argc, argv);

    if (freopen("testScan.xml", "w", stdout))
        result |= QTest::qExec(new TestScan, argc, argv);

    if(freopen("testEIC.xml",  "w", stdout))
        result |= QTest::qExec(new TestEIC, argc, argv);

    if(freopen("testbase64.xml",  "w", stdout))
        result |= QTest::qExec(new Testbase64, argc, argv);

    if(freopen("testMzFit.xml",  "w", stdout))
        result |= QTest::qExec(new TestMzFit, argc, argv);

    if (freopen("testCSVReports.xml", "w", stdout))
        result |= QTest::qExec(new TestCSVReports, argc, argv);

    // freopen("testMzAligner.xml",  "w", stdout);
    // result |= QTest::qExec(new TestMzAligner, argc, argv);

    return result;
}

#include <QtTest>
#include <QApplication>
#include "testLoadSamples.h"
#include "testMassCalculator.h"
#include "testCSVReports.h"
#include "testPeakDetection.h"
#include "testMzSlice.h"
#include "testLoadDB.h"
#include "testScan.h"
#include "testMzFit.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    int result = 0;

    freopen("test1.xml", "w", stdout);
    result |= QTest::qExec(new TestLoadSamples, argc, argv);

    freopen("test2.xml",  "w", stdout);
    result |= QTest::qExec(new TestMassCalculator, argc, argv);
    
    freopen("test3.xml",  "w", stdout);
    result |= QTest::qExec(new TestCSVReports, argc, argv);

    freopen("test4.xml",  "w", stdout);
    result |= QTest::qExec(new TestPeakDetection, argc, argv);

    freopen("test5.xml",  "w", stdout);
    result |= QTest::qExec(new TestLoadDB, argc, argv);

    freopen("test6.xml",  "w", stdout);
    result |= QTest::qExec(new TestMzSlice, argc, argv);

    freopen("test7.xml",  "w", stdout);
    result |= QTest::qExec(new TestScan, argc, argv);

    freopen("test9.xml",  "w", stdout);
    result |= QTest::qExec(new TestMzFit, argc, argv);

    return result;
}

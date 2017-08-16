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

void readLog(QString);

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    int result = 0;

    if (freopen("testLoadSamples.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadSamples, argc, argv);
    readLog("testLoadSamples.xml");

    if (freopen("testMassCalculator.xml", "w", stdout))
        result |= QTest::qExec(new TestMassCalculator, argc, argv);
    readLog("testMassCalculator.xml");
    
    if(freopen("testCLI.xml",  "w", stdout))
        result |= QTest::qExec(new TestCLI, argc, argv);
    readLog("testCLI.xml");

    if (freopen("testPeakDetection.xml", "w", stdout))
        result |= QTest::qExec(new TestPeakDetection, argc, argv);
    readLog("testPeakDetection.xml");

    if(freopen("testCharge.xml",  "w", stdout))
        result |= QTest::qExec(new TestCharge, argc, argv);
    readLog("testCharge.xml");

    if (freopen("testLoadDB.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadDB, argc, argv);
    readLog("testLoadDB.xml");

    if (freopen("testMzSlice.xml", "w", stdout))
        result |= QTest::qExec(new TestMzSlice, argc, argv);
    readLog("testMzSlice.xml");

    if (freopen("testScan.xml", "w", stdout))
        result |= QTest::qExec(new TestScan, argc, argv);
    readLog("testScan.xml");

    if(freopen("testEIC.xml",  "w", stdout))
        result |= QTest::qExec(new TestEIC, argc, argv);
    readLog("testEIC.xml");

    if(freopen("testbase64.xml",  "w", stdout))
        result |= QTest::qExec(new Testbase64, argc, argv);
    readLog("testbase64.xml");

    if(freopen("testMzFit.xml",  "w", stdout))
        result |= QTest::qExec(new TestMzFit, argc, argv);
    readLog("testMzFit.xml");

    if (freopen("testCSVReports.xml", "w", stdout))
        result |= QTest::qExec(new TestCSVReports, argc, argv);
    readLog("testCSVReports.xml");
    
    // if(freopen("testMzAligner.xml",  "w", stdout))
    //     result |= QTest::qExec(new TestMzAligner, argc, argv);
    // readLog("testMzAligner.xml");

    return result;
}


void readLog(QString filename){

    QFile file(filename);
    if(file.open(QFile::ReadOnly | QFile::Text)){
        QXmlStreamReader xsr(&file);
        xsr.readNext();

        while(!xsr.atEnd()){
            if(xsr.attributes().value("type")=="fail"){
                QString file=xsr.attributes().value("file").toString();
                int line=xsr.attributes().value("line").toInt();
                while(xsr.name()!="Description"){
                    xsr.readNext();
                }
                qDebug()<<"test case failed: file-"<<file<<", line-"<<line<<", error info: "<<xsr.readElementText();
            }
            xsr.readNext();
        }

        file.close();
    }
}
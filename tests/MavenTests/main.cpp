#include <QtTest>
#include <QCoreApplication>
#include "testLoadSamples.h"
#include "testMassCalculator.h"
#include "testCSVReports.h"
#include "testPeakDetection.h"
#include "testIsotopeDetection.h"
#include "testMzSlice.h"
#include "testLoadDB.h"
#include "testScan.h"
#include "testEIC.cpp"
#include "testbase64.h"
#include "testMzFit.h"
#include "testMzAligner.h"
#include "testCLI.h"
#include "testCharge.h"
#include "testSRMList.h"
#include "testIsotopeLogic.h"

int readLog(QString);

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    int result = 0;

    auto timer = mzUtils::startTimer();
    if (freopen("testLoadSamples.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadSamples, argc, argv);
    result|=readLog("testLoadSamples.xml");
    mzUtils::stopTimer(timer, "testLoadSamples");

    timer = mzUtils::startTimer();
    if (freopen("testMassCalculator.xml", "w", stdout))
        result |= QTest::qExec(new TestMassCalculator, argc, argv);
    result|=readLog("testMassCalculator.xml");
    mzUtils::stopTimer(timer, "testMassCalculator");

    timer = mzUtils::startTimer();
    if(freopen("testCLI.xml",  "w", stdout))
        result |= QTest::qExec(new TestCLI, argc, argv);
    result|=readLog("testCLI.xml");
    mzUtils::stopTimer(timer, "testCLI");

    timer = mzUtils::startTimer();
    if (freopen("testPeakDetection.xml", "w", stdout))
        result |= QTest::qExec(new TestPeakDetection, argc, argv);
    result|=readLog("testPeakDetection.xml");
    mzUtils::stopTimer(timer, "testPeakDetection");

    timer = mzUtils::startTimer();
    if (freopen("testIsotopeDetection.xml", "w", stdout))
        result |= QTest::qExec(new TestIsotopeDetection, argc, argv);
    result|=readLog("testIsotopeDetection.xml");
    mzUtils::stopTimer(timer, "testIsotopeDetection");

    timer = mzUtils::startTimer();
    if(freopen("testCharge.xml",  "w", stdout))
        result |= QTest::qExec(new TestCharge, argc, argv);
    result|=readLog("testCharge.xml");
    mzUtils::stopTimer(timer, "testCharge");

    timer = mzUtils::startTimer();
    if (freopen("testLoadDB.xml", "w", stdout))
        result |= QTest::qExec(new TestLoadDB, argc, argv);
    result|=readLog("testLoadDB.xml");
    mzUtils::stopTimer(timer, "testLoadDB");

    timer = mzUtils::startTimer();
    if (freopen("testMzSlice.xml", "w", stdout))
        result |= QTest::qExec(new TestMzSlice, argc, argv);
    result|=readLog("testMzSlice.xml");
    mzUtils::stopTimer(timer, "testMzSlice");

    timer = mzUtils::startTimer();
    if (freopen("testScan.xml", "w", stdout))
        result |= QTest::qExec(new TestScan, argc, argv);
    result|=readLog("testScan.xml");
    mzUtils::stopTimer(timer, "testScan");

    timer = mzUtils::startTimer();
    if(freopen("testEIC.xml",  "w", stdout))
        result |= QTest::qExec(new TestEIC, argc, argv);
    result|=readLog("testEIC.xml");
    mzUtils::stopTimer(timer, "testEIC");

    timer = mzUtils::startTimer();
    if(freopen("testbase64.xml",  "w", stdout))
        result |= QTest::qExec(new Testbase64, argc, argv);
    result|=readLog("testbase64.xml");
    mzUtils::stopTimer(timer, "testbase64");

    timer = mzUtils::startTimer();
    if(freopen("testMzFit.xml",  "w", stdout))
        result |= QTest::qExec(new TestMzFit, argc, argv);
    result|=readLog("testMzFit.xml");
    mzUtils::stopTimer(timer, "testMzFit");

    timer = mzUtils::startTimer();
    if (freopen("testCSVReports.xml", "w", stdout))
        result |= QTest::qExec(new TestCSVReports, argc, argv);
    result|=readLog("testCSVReports.xml");
    mzUtils::stopTimer(timer, "testCSVReports");

    timer = mzUtils::startTimer();
    if (freopen("testSRMList.xml", "w", stdout))
        result |= QTest::qExec(new TestSRMList, argc, argv);
    result|=readLog("testSRMList.xml");
    mzUtils::stopTimer(timer, "testSRMList");

    timer = mzUtils::startTimer();
    if (freopen("testIsotopeLogic.xml", "w", stdout))
        result |= QTest::qExec(new TestIsotopeLogic, argc, argv);
    result|=readLog("testIsotopeLogic.xml");
    mzUtils::stopTimer(timer, "testIsotopeLogic");

    timer = mzUtils::startTimer();
    if (freopen("testMzAligner.xml", "w", stdout)) {
        result |= QTest::qExec(new TestMzAligner, argc, argv);
    }
    result|=readLog("testMzAligner.xml");
    mzUtils::stopTimer(timer, "testMzAligner");

    return result;
}


int readLog(QString filename){

    QFile file(filename);
    int result=0;
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
                result=1;
                std::cerr<<"\n\ntest case failed: file-"<<file.toStdString()<<", line-"<<line<<", error info: "<<xsr.readElementText().toStdString()<<"\n";
            }
            xsr.readNext();
            
        }
        file.close();
    }
    return result;
}

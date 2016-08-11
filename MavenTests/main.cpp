#include <QtTest>
#include <QApplication>
#include "testEIC.h"
#include "testMassCalculator.h"
#include "testCSVReports.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    int result = 0;

    freopen("test1.xml", "w", stdout);
    result |= QTest::qExec(new TestEIC, argc, argv);
    freopen("test2.xml",  "w", stdout);
    result |= QTest::qExec(new TestMassCalculator, argc, argv);
    freopen("test3.xml",  "w", stdout);
    result |= QTest::qExec(new TestCSVReports, argc, argv);
    return result;
}

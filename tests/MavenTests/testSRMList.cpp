#include "testSRMList.h"

TestSRMList::TestSRMList() {

    filterline1 = "- SRM SIC Q1=124 Q3=80 sample=157 period=1 experiment=1 transition=0";
    filterline2 = "FTMS + p ESI Full ms2 209.19@hcd35.00 [50.00-230.00]";
    filterline3 = "FTMS - p ESI Full ms [70.00-800.00]";
}

void TestSRMList::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
}

void TestSRMList::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestSRMList::init() {
    // This function is executed before each test
}

void TestSRMList::cleanup() {
    // This function is executed after each test
}

void TestSRMList::testGetPrecursorOfSrm() {

    SRMList* srmList;

    double precursorMz1 = srmList->getPrecursorOfSrm(filterline1);
    double precursorMz2 = srmList->getPrecursorOfSrm(filterline2);
    double precursorMz3 = srmList->getPrecursorOfSrm(filterline3);
    
    QVERIFY(precursorMz1 == 124);
    QVERIFY(precursorMz2 == 209.19);
    QVERIFY(precursorMz3 == 0);
    
}

void TestSRMList::testGetProductOfSrm() {

    SRMList* srmList;

    double productMz1 = srmList->getProductOfSrm(filterline1);
    double productMz2 = srmList->getProductOfSrm(filterline2);
    double productMz3 = srmList->getProductOfSrm(filterline3);
    
    QVERIFY(productMz1 == 80);
    QVERIFY(productMz2 == 140);
    QVERIFY(productMz3 == 435);
}

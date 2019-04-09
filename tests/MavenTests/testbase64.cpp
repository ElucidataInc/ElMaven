#include "testbase64.h"
#include "base64.h"
#include "utilities.h"

Testbase64::Testbase64() {

}

void Testbase64::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void Testbase64::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void Testbase64::init() {
    // This function is executed before each test
}

void Testbase64::cleanup() {
    // This function is executed after each test
}

void Testbase64::testdecodeBase64()
{
    string b64String="Qowh+kUQcBVCjCYG";
    vector<float> decodedArray = base64::decodeBase64(b64String, 4, true, false);

    QVERIFY(decodedArray.size()==3);
    QVERIFY(TestUtils::floatCompare(decodedArray[0],70.0663604736328));
    QVERIFY(TestUtils::floatCompare(decodedArray[1],2311.00512695312));
    QVERIFY(TestUtils::floatCompare(decodedArray[2],70.0742645263672));
}

void Testbase64::testdecodeString()
{
    string b64String="bWF2ZW4gaXMgYXdlc29tZQ==";

    string dest = base64::decodeString(b64String.c_str(), b64String.size());

    QVERIFY((unsigned char)dest[0]=='m');
    QVERIFY((unsigned char)dest[1]=='a');
    QVERIFY((unsigned char)dest[2]=='v');
    QVERIFY((unsigned char)dest[3]=='e');
    QVERIFY((unsigned char)dest[4]=='n');
    QVERIFY((unsigned char)dest[5]==' ');
    QVERIFY((unsigned char)dest[6]=='i');
    QVERIFY((unsigned char)dest[7]=='s');
    QVERIFY((unsigned char)dest[8]==' ');
    QVERIFY((unsigned char)dest[9]=='a');
    QVERIFY((unsigned char)dest[10]=='w');
    QVERIFY((unsigned char)dest[11]=='e');
    QVERIFY((unsigned char)dest[12]=='s');
    QVERIFY((unsigned char)dest[13]=='o');
    QVERIFY((unsigned char)dest[14]=='m');
    QVERIFY((unsigned char)dest[15]=='e');

}

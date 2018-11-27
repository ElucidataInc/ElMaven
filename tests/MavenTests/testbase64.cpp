#include "testbase64.h"



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

void Testbase64::testdecode_base64() {

    string b64String="Qowh+kUQcBVCjCYG";
    vector<float> decodedArray =
        base64::decode_base64(b64String, 4, true, false);

    QVERIFY(decodedArray.size()==3);
    QVERIFY(TestUtils::floatCompare(decodedArray[0],70.0663604736328));
    QVERIFY(TestUtils::floatCompare(decodedArray[1],2311.00512695312));
    QVERIFY(TestUtils::floatCompare(decodedArray[2],70.0742645263672));
}

void Testbase64::testencode_base64() {

    vector<float> decodedArray(3);
    decodedArray[0]=70.0663604736328;
    decodedArray[1]=2311.00512695312;
    decodedArray[2]=70.0742645263672;

    unsigned char* out=base64::encode_base64(decodedArray);

    QVERIFY(out[0]=='+');
    QVERIFY(out[1]=='i');
    QVERIFY(out[2]=='G');
    QVERIFY(out[3]=='M');
    QVERIFY(out[4]=='Q');
    QVERIFY(out[5]=='h');
    QVERIFY(out[6]=='V');
    QVERIFY(out[7]=='w');
    QVERIFY(out[8]=='E');
    QVERIFY(out[9]=='E');
    QVERIFY(out[10]=='U');
    QVERIFY(out[11]=='G');
    QVERIFY(out[12]=='J');
    QVERIFY(out[13]=='o');
    QVERIFY(out[14]=='x');
    QVERIFY(out[15]=='C');

}

void Testbase64::testdecodeString() {

    string b64String="bWF2ZW4gaXMgYXdlc29tZQ==";

    char *dest = base64::decodeString(b64String);

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

void Testbase64::testencodeString() {

    unsigned char arr[17]={'m','a','v','e','n',' ','i','s',' ','a','w','e','s','o','m','e'};
    unsigned char *src=arr;

    unsigned char *dest=base64::encodeString(src,16);

    QVERIFY((unsigned char)dest[0]=='b');
    QVERIFY((unsigned char)dest[1]=='W');
    QVERIFY((unsigned char)dest[2]=='F');
    QVERIFY((unsigned char)dest[3]=='2');
    QVERIFY((unsigned char)dest[4]=='Z');
    QVERIFY((unsigned char)dest[5]=='W');
    QVERIFY((unsigned char)dest[6]=='4');
    QVERIFY((unsigned char)dest[7]=='g');
    QVERIFY((unsigned char)dest[8]=='a');
    QVERIFY((unsigned char)dest[9]=='X');
    QVERIFY((unsigned char)dest[10]=='M');
    QVERIFY((unsigned char)dest[11]=='g');
    QVERIFY((unsigned char)dest[12]=='Y');
    QVERIFY((unsigned char)dest[13]=='X');
    QVERIFY((unsigned char)dest[14]=='d');
    QVERIFY((unsigned char)dest[15]=='l');
    QVERIFY((unsigned char)dest[16]=='c');
    QVERIFY((unsigned char)dest[17]=='2');
    QVERIFY((unsigned char)dest[18]=='9');
    QVERIFY((unsigned char)dest[19]=='t');
    QVERIFY((unsigned char)dest[20]=='Z');
    QVERIFY((unsigned char)dest[21]=='Q');

}

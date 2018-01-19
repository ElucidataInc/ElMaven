#include "testMzUtils.h"

TestMzUtils::TestMzUtils(){

}

void TestMzUtils::initTestCase(){

}
void TestMzUtils::cleanupTestCase(){
    
}
void TestMzUtils::init(){
    
}
void TestMzUtils::cleanup(){
    
}

void TestMzUtils::testmedian(){

    float a[]={3, 13, 7, 5, 21, 23, 23, 40, 23, 14, 12, 56, 23, 29};
    float median=22;
    vector<float> points(a,a+sizeof(a)/sizeof(float));

    QVERIFY(mzUtils::median(points)==median);
    QVERIFY(mzUtils::median(a,sizeof(a)/sizeof(float))==median);
}


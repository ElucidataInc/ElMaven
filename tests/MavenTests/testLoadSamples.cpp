#include "testLoadSamples.h"
#include "mavenparameters.h"
#include "mzSample.h"

TestLoadSamples::TestLoadSamples() {
    loadFile = "bin/methods/testsample_1.mzxml";
    blankSample = "bin/methods/blank_1.mzxml";
}

void TestLoadSamples::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestLoadSamples::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestLoadSamples::init() {
    // This function is executed before each test
}

void TestLoadSamples::cleanup() {
    // This function is executed after each test
}

void TestLoadSamples::testFileLoad() {
    mzSample mzsample;
    mzsample.loadSample(loadFile);
    QVERIFY(true);
}

void TestLoadSamples:: testIsAllScansParsed() {
    mzSample mzsample;
    unsigned int numberOfScans = 1603;
    mzsample.parseMzXML(loadFile);
    QVERIFY(mzsample.scanCount() == numberOfScans);
}

void TestLoadSamples:: testScanParsing() {
    mzSample mzsample;
    unsigned int scanNum = 1;
    int scannum = 1;
    float rt = 0.493428 / 60.0;
    int msLevel = 1;
    int polarity = 1;
    float basePeakMz = 258.96;
    float precursorMz = 0.0;
    float collisionEnergy = 0.0;
    bool centroided = false;
    string filterLine = "FTMS + p ESI Full ms [70.00-770.00]";
    string scanType = "Full";
    unsigned int peakCount  = 918;

    mzsample.parseMzXML(loadFile);
    Scan* scan = mzsample.getScan(scanNum);

    QVERIFY(scan->rt == rt);

    QVERIFY(scan->scannum == scannum);

    QVERIFY(scan->mslevel == msLevel);

    QVERIFY(scan->polarity == polarity);

    QVERIFY(scan->precursorMz == precursorMz);

    QVERIFY(scan->productMz == basePeakMz);

    QVERIFY(scan->collisionEnergy == collisionEnergy);

    QVERIFY(scan->centroided == centroided);

    QVERIFY(scan->filterLine == filterLine);

    QVERIFY(scan->scanType == scanType);

    QVERIFY(scan->mz.size() == peakCount);

    QVERIFY(scan->intensity.size() == peakCount);

}

void TestLoadSamples:: testSrmScan() {
    mzSample mzsample;
    unsigned int numberOfScans = 1603;
    mzsample.loadSample(loadFile);

    map<string,vector<int> >::iterator itr = mzsample.srmScans.begin();

    QVERIFY(itr->first.compare("FTMS + p ESI Full ms [70.00-770.00]") == 0 );

    unsigned int k = mzsample.srmScans.find(itr->first)->second.size();

    QVERIFY(k == numberOfScans);
}

void TestLoadSamples:: testMinMaxMz() {
    mzSample mzsample;
    float minMz = 70.0012;
    float maxMz = 769.953;
    mzsample.loadSample(loadFile);

    QVERIFY(common::floatCompare(mzsample.minMz, minMz));

    QVERIFY(common::floatCompare(mzsample.maxMz, maxMz));

}

void TestLoadSamples:: testMinMaxRT() {
    mzSample mzsample;
    float minRT = 0.00201776666;
    float maxRT = 7.20475;
    mzsample.loadSample(loadFile);

    QVERIFY(common::floatCompare(mzsample.minRt, minRT));

    QVERIFY(common::floatCompare(mzsample.maxRt, maxRT));

}

#ifndef WIN32
void TestLoadSamples:: testSampleName() {
    mzSample mzsample;
    string sampleName = "testsample_1";
    mzsample.loadSample(loadFile);

    QVERIFY(sampleName.compare(mzsample.sampleName) == 0);
}
#endif

void TestLoadSamples:: testBlankSample() {
    mzSample mzsample;
    mzsample.loadSample(blankSample);
    QVERIFY(mzsample.isBlank);

    mzsample.loadSample(loadFile);
    QVERIFY(!mzsample.isBlank);
}

void TestLoadSamples::testParseMzMLInjectionTimeStamp() {

    // Different format of time stamps
    vector<string> test_cases{
        "2001-10-26T19:32:52Z",
        "2001-10-26T21:32:52",
        "2001-10-26T21:32:52+02:00",
        "2001-10-26T19:32:52+00:00",
        "2001-10-26T21:32:52.12679"
    };

    // Expected output - epoch time
    vector<long unsigned int> expected_output{
        1004124772,
        1004131972,
        1004131972,
        1004124772,
        1004131972
    };

    // check if input and expected output vector size is same
    if (test_cases.size() != expected_output.size()) QVERIFY(0);


    for (vector<string>::iterator it = test_cases.begin(); it != test_cases.end(); it++) {

        // creating xml document from scratch
        pugi::xml_document doc;

        // add node with name mzML
        pugi::xml_node node = doc.append_child("mzML");

        // add node with name run with parent node mzML
        pugi::xml_node run = node.append_child("run");
        run.append_child(pugi::node_pcdata).set_value("experiment run");

        // add attribute & corresponding value startTimeStamp to node
        string str = *it;
        run.append_attribute("startTimeStamp") = str.c_str();

        // using the "parseMzMLInjectionTimeStamp" function to get time stamp
        mzSample* sample = new mzSample();
        xml_node experimentRun = doc.first_child().first_element_by_path("run");
        sample->parseMzMLInjectionTimeStamp(experimentRun.attribute("startTimeStamp"));

        // comparing expected output with observed output
        unsigned int index_itr = it - test_cases.begin();
        QVERIFY(sample->injectionTime == expected_output[index_itr]);

    }

}

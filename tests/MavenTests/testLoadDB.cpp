#include "testLoadDB.h"
#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "utilities.h"

TestLoadDB::TestLoadDB() {

}

void TestLoadDB::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestLoadDB::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestLoadDB::init() {
    // This function is executed before each test
}

void TestLoadDB::cleanup() {
    // This function is executed after each test
}

void TestLoadDB::testExtractCompoundfromEachLineNormal() {
    string name = "ADP-D-glucose";
    string formula = "C16H25N5O15P2";
    string id = "C00498";
    string db = "qe3_v11_2016_04_29";
    static const string fieldsarr[] = {"588.07523818", "14.14", "-1", name, formula, id};
    vector<string> fields (fieldsarr, fieldsarr + sizeof(fieldsarr) / sizeof(fieldsarr[0]) );
    map<string, int> header;
    header["mz"] = 0;
    header["rt"] = 1;
    header["charge"] = 2;
    header["name"] = 3;
    header["formula"] = 4;
    header["id"] = 5;

    int loadCount = 4;
    string filename = "/bin/methods/qe3_v11_2016_04_29.csv";
    Compound* compound = maventests::database.extractCompoundfromEachLine(fields, header, loadCount, filename);

    QVERIFY(TestUtils::floatCompare(compound->mz(), 588.07523818) && \
    TestUtils::floatCompare(compound->expectedRt(), 14.14) && \
    TestUtils::floatCompare(compound->charge(), -1) && \
    name.compare(compound->name()) == 0 && \
    formula.compare(compound->formula()) == 0 && \
    id.compare(compound->id()) == 0 && \
    db.compare(compound->db()) == 0);
}

void TestLoadDB::testExtractCompoundfromEachLineWithNoMz() {
    string name = "ADP-D-glucose";
    string formula = "C16H25N5O15P2";
    string id = "C00498";
    string db = "qe3_v11_2016_04_29";
    static const string fieldsarr[] = {"", "14.14", "-1", name, formula, id};
    vector<string> fields (fieldsarr, fieldsarr + sizeof(fieldsarr) / sizeof(fieldsarr[0]) );
    map<string, int> header;
    header["mz"] = 0;
    header["rt"] = 1;
    header["charge"] = 2;
    header["name"] = 3;
    header["formula"] = 4;
    header["id"] = 5;

    int loadCount = 4;
    string filename = "/bin/methods/qe3_v11_2016_04_29.csv";
    Compound* compound = maventests::database.extractCompoundfromEachLine(fields, header, loadCount, filename);

    QVERIFY(TestUtils::floatCompare(compound->mz(), 589.082238 - 1.007) && \
    TestUtils::floatCompare(compound->expectedRt(), 14.14) && \
    TestUtils::floatCompare(compound->charge(), -1) && \
    name.compare(compound->name()) == 0 && \
    formula.compare(compound->formula()) == 0 && \
    id.compare(compound->id()) == 0 && \
    db.compare(compound->db()) == 0);
}

void TestLoadDB::testExtractCompoundfromEachLineWithNoMzandFormula() {
    string name = "ADP-D-glucose";
    string formula = "C16H25N5O15P2";
    string id = "C00498";
    string db = "qe3_v11_2016_04_29";
    static const string fieldsarr[] = {"", "14.14", "-1", name, "", id};
    vector<string> fields (fieldsarr, fieldsarr + sizeof(fieldsarr) / sizeof(fieldsarr[0]) );
    map<string, int> header;
    header["mz"] = 0;
    header["rt"] = 1;
    header["charge"] = 2;
    header["name"] = 3;
    header["formula"] = 4;
    header["id"] = 5;

    int loadCount = 4;
    string filename = "/bin/methods/qe3_v11_2016_04_29.csv";
    Compound* compound = maventests::database.extractCompoundfromEachLine(fields, header, loadCount, filename);

    QVERIFY(compound == NULL);
}

void TestLoadDB::testExtractCompoundfromEachLineWithExpRTandRT() {
    string name = "ADP-D-glucose";
    string formula = "C16H25N5O15P2";
    string id = "C00498";
    string db = "qe3_v11_2016_04_29";
    static const string fieldsarr[] = {"", "14.14", "-1", name, formula, id, "12.14"};
    vector<string> fields (fieldsarr, fieldsarr + sizeof(fieldsarr) / sizeof(fieldsarr[0]) );
    map<string, int> header;
    header["mz"] = 0;
    header["rt"] = 1;
    header["charge"] = 2;
    header["name"] = 3;
    header["formula"] = 4;
    header["id"] = 5;
    header["expectedrt"] = 6;

    int loadCount = 4;
    string filename = "/bin/methods/qe3_v11_2016_04_29.csv";
    Compound* compound = maventests::database.extractCompoundfromEachLine(fields, header, loadCount, filename);

    QVERIFY(TestUtils::floatCompare(compound->mz(), 589.082238 - 1.007) && \
    TestUtils::floatCompare(compound->expectedRt(), 12.14) && \
    TestUtils::floatCompare(compound->charge(), -1) && \
    name.compare(compound->name()) == 0 && \
    formula.compare(compound->formula()) == 0 && \
    id.compare(compound->id()) == 0 && \
    db.compare(compound->db()) == 0);
}

void TestLoadDB::testExtractCompoundfromEachLineWithCompoundField() {
    string name = "ADP-D-glucose";
    string compoundname = "ADP-D-glucose";
    string formula = "C16H25N5O15P2";
    string id = "C00498";
    string db = "qe3_v11_2016_04_29";
    static const string fieldsarr[] = {"588.07523818", "14.14", "-1", name, formula, id, compoundname};
    vector<string> fields (fieldsarr, fieldsarr + sizeof(fieldsarr) / sizeof(fieldsarr[0]) );
    map<string, int> header;
    header["mz"] = 0;
    header["rt"] = 1;
    header["charge"] = 2;
    header["name"] = 3;
    header["formula"] = 4;
    header["id"] = 5;
    header["compound"] = 6;

    int loadCount = 4;
    string filename = "/bin/methods/qe3_v11_2016_04_29.csv";
    Compound* compound = maventests::database.extractCompoundfromEachLine(fields, header, loadCount, filename);

    QVERIFY(TestUtils::floatCompare(compound->mz(), 588.07523818) && \
    TestUtils::floatCompare(compound->expectedRt(), 14.14) && \
    TestUtils::floatCompare(compound->charge(), -1) && \
    compoundname.compare(compound->name()) == 0 && \
    formula.compare(compound->formula()) == 0 && \
    id.compare(compound->id()) == 0 && \
    db.compare(compound->db()) == 0);
}

void TestLoadDB::testloadCompoundCSVFileWithIssues() {
        int numberofCompounds = maventests::database.loadCompoundCSVFile("bin/methods/compoundlist.csv");
        QVERIFY(numberofCompounds == 7);
}

void TestLoadDB::testloadCompoundCSVFileWithRep() {
        int numberofCompounds = maventests::database.loadCompoundCSVFile("bin/methods/compoundlist_rep.csv");
        QVERIFY(numberofCompounds == 7);
}

/* void TestLoadDB::testloadCompoundCSVFileWithRepNoId() {
        int numberofCompounds = maventests::database.loadCompoundCSVFile("bin/methods/compoundlist_rep_with_noId.csv");
        QVERIFY(numberofCompounds == 7);
} */


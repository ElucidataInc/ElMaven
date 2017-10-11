#ifndef TESTSRMLIST_H
#define TESTSRMLIST_H

class TestSRMList : public QObject {
    Q_OBJECT

    public:
        TestSRMList();

    private Q_SLOTS:

        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        void testGetPrecursorOfSrm();
        void testGetProductOfSrm();
        void testGetMatchedCompounds();

};

#endif // TESTSRMLIST_H
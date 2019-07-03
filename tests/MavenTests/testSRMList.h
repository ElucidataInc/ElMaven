#ifndef TESTSRMLIST_H
#define TESTSRMLIST_H

#include <QtTest>

using namespace std;

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

        /**
         * @see SRMList
         */
        void testGetPrecursorOfSrm();

        /**
         * @see SRMList
         */
        void testGetProductOfSrm();

    private:
        string filterline1;
        string filterline2;
        string filterline3;
        
};

#endif // TESTSRMLIST_H

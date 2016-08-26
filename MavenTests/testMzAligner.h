#ifndef TESTMZALIGNER_H
#define TESTMZALIGNER_H
#include <QtTest>
#include <string>
#include <sstream>
#include <iostream>
#include "../libmaven/mzSample.h"
#include "../libmaven/PeakDetector.h"
#include "../libmaven/mavenparameters.h"
#include "../libmaven/classifierNeuralNet.h"

using namespace std;

class TestMzAligner : public QObject {
    Q_OBJECT

    public:
        TestMzAligner();
        MavenParameters* mavenparameters = new MavenParameters();

    private:
        QStringList files;

    private slots:
        // functions executed by QtTest before and after test suite
        void initTestCase();
        void cleanupTestCase();

        // functions executed by QtTest before and after each test
        void init();
        void cleanup();

        // test functions - all functions prefixed with "test" will be ran as tests
        // this is automatically detected thanks to Qt's meta-information about QObjects
        void testInputGroups();
        void testDoAlignment();
        void testSaveFit();

};

#endif // TESTMZALIGNER_H

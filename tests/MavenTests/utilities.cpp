#include "classifierNeuralNet.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "PeakDetector.h"
#include "PeakGroup.h"
#include "utilities.h"

namespace maventests {
    Database database;
    TestSamples samples;
}

TestUtils::TestUtils() {}

bool TestUtils::floatCompare(float a, float b)
{
    float EPSILON = 0.001;
    return fabs(a - b) < EPSILON;
}

float TestUtils::roundTo(float value, int numPlaces)
{
    float factor = powf(10.0f, numPlaces);
    return roundf(value * factor) / factor;
}

double TestUtils::roundTo(double value, int numPlaces)
{
    double factor = pow(10.0, numPlaces);
    return round(value * factor) / factor;
}

bool TestUtils::compareMaps(const map<string, int>& l,
                            const map<string, int>& k)
{
    // same types, proceed to compare maps here

    if (l.size() != k.size())
        return false;  // differing sizes, they are not the same

    map<string, int>::const_iterator i, j;
    for (i = l.begin(), j = k.begin(); i != l.end(); ++i, ++j) {
        if (*i != *j)
            return false;
    }

    return true;
}

vector<Compound*> TestUtils::getCompoudDataBaseWithRT()
{
    const char* loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    ;

    maventests::database.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds =
        maventests::database.getCompoundsSubset("qe3_v11_2016_04_29");

    return compounds;
}

vector<Compound*> TestUtils::getCompoudDataBaseWithNORT()
{
    const char* loadCompoundDB = "bin/methods/KNOWNS.csv";
    ;

    maventests::database.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds =
        maventests::database.getCompoundsSubset("KNOWNS");

    return compounds;
}

vector<Compound*> TestUtils::getFaltyCompoudDataBase()
{
    const char* loadCompoundDB = "bin/methods/compoundlist.csv";

    maventests::database.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds =
        maventests::database.getCompoundsSubset("compoundlist");

    return compounds;
}

void TestUtils::loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                         MavenParameters* mavenparameters)
{
    QStringList files;
    files << "bin/methods/testsample_2.mzxml"
          << "bin/methods/testsample_3.mzxml";
    for (int i = 0; i < files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    string loadmodel = "bin/default.model";
    clsf->loadModel(loadmodel);
    mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10, "ppm");
    mavenparameters->clsf = clsf;
    mavenparameters->ionizationMode = -1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 1;
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    // limiting time and mz range for speedier testing
    mavenparameters->maxRt = 6.0f;
    mavenparameters->maxMz = 200.0f;
}

vector<PeakGroup> TestUtils::getGroupsFromProcessCompounds()
{
    const char* loadCompoundDB = "bin/methods/qe3_v11_2016_04_29.csv";
    maventests::database.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds =
        maventests::database.getCompoundsSubset("qe3_v11_2016_04_29");

    vector<mzSample*> samplesToLoad;
    MavenParameters* mavenparameters = new MavenParameters();
    loadSamplesAndParameters(samplesToLoad, mavenparameters);

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = peakDetector.processCompounds(compounds,
                                                            "compounds");
    peakDetector.processSlices(slices, "compounds");

    return mavenparameters->allgroups;
}

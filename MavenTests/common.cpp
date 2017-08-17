#include "common.h"
Databases DBS;

common::common() {
}

bool common::floatCompare(float a, float b) {
    float EPSILON = 0.001;
    return fabs(a - b) < EPSILON;
}

bool common::compareMaps(const map<string,int> & l, const map<string,int> & k) {
  // same types, proceed to compare maps here

  if(l.size() != k.size())
    return false;  // differing sizes, they are not the same

 map<string,int>::const_iterator i, j;
  for(i = l.begin(), j = k.begin(); i != l.end(); ++i, ++j)
  {
    if(*i != *j)
      return false;
  }

  return true;
}

vector<Compound*> common::getCompoudDataBaseWithRT() {
    const char* loadCompoundDB = \
    "bin/methods/qe3_v11_2016_04_29.csv";;

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("qe3_v11_2016_04_29");

    return compounds;
}

vector<Compound*> common::getCompoudDataBaseWithNORT() {
    const char* loadCompoundDB = \
    "bin/methods/KNOWNS.csv";;

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("KNOWNS");

    return compounds;
}

vector<Compound*> common::getFaltyCompoudDataBase() {
    const char* loadCompoundDB = \
    "bin/methods/compoundlist.csv";

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("compoundlist");

    return compounds;
}

vector<PeakGroup> common::getGroupsFromProcessCompounds(){

    const char* loadCompoundDB = \
    "bin/methods/qe3_v11_2016_04_29.csv";;

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("qe3_v11_2016_04_29");

    vector<mzSample*> samplesToLoad;

    QStringList files;
    files << "bin/methods/testsample_2.mzxml" 
          << "bin/methods/testsample_3.mzxml";

    for (int i = 0; i < files.size(); ++i) {
        mzSample* mzsample = new mzSample();
        mzsample->loadSample(files.at(i).toLatin1().data());
        samplesToLoad.push_back(mzsample);
    }

    MavenParameters* mavenparameters = new MavenParameters();
    ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
    string loadmodel = "bin/default.model";
    clsf->loadModel(loadmodel);
    mavenparameters->clsf = clsf;
    mavenparameters->compoundPPMWindow = 10;
    mavenparameters->ionizationMode = -1;
    mavenparameters->matchRtFlag = true;
    mavenparameters->compoundRTWindow = 2;
    mavenparameters->samples = samplesToLoad;
    mavenparameters->eic_smoothingWindow = 10;
    mavenparameters->eic_smoothingAlgorithm = 1;
    mavenparameters->amuQ1 = 0.25;
    mavenparameters->amuQ3 = 0.30;
    mavenparameters->baseline_smoothingWindow = 5;
    mavenparameters->baseline_dropTopX = 80;

    PeakDetector peakDetector;
    peakDetector.setMavenParameters(mavenparameters);
    vector<mzSlice*> slices = 
	    peakDetector.processCompounds(compounds, "compounds");
    peakDetector.processSlices(slices, "compounds");

    return mavenparameters->allgroups;

}

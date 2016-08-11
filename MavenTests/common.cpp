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
    "/home/rajat/elucidata/maven/maven_opensource/bin/methods/qe3_v11_2016_04_29.csv";;

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("qe3_v11_2016_04_29");

    return compounds;
}

vector<Compound*> common::getCompoudDataBaseWithNORT() {
    const char* loadCompoundDB = \
    "/home/rajat/elucidata/maven/maven_opensource/bin/methods/KNOWNS.csv";;

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("KNOWNS");

    return compounds;
}

vector<Compound*> common::getFaltyCompoudDataBase() {
    const char* loadCompoundDB = \
    "/home/rajat/elucidata/maven/maven_opensource/bin/methods/compoundlist.csv";

    DBS.loadCompoundCSVFile(loadCompoundDB);
    vector<Compound*> compounds = DBS.getCopoundsSubset("compoundlist");

    return compounds;
}



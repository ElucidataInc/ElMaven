#ifndef JSONREPORTS_H
#define JSONREPORTS_H

#include <boost/algorithm/string.hpp>

#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"

// #include "../mzroll/tabledockwidget.h"

using namespace std;


class mzSample;
class EIC;
class PeakGroup;
class MavenParameters;

class JSONReports{

public:
    MavenParameters* mavenParameters;
    JSONReports();
    JSONReports(MavenParameters* _mp);
    ~JSONReports();
    void saveMzEICJson(string filename,vector<PeakGroup> allgroups,vector<mzSample*> vsampleNames);
    void writeGroupMzEICJson(PeakGroup& grp,ofstream& myfile, vector<mzSample*> vsampleNames);
    string sanitizeJSONstring(string s);
    float outputRtWindow = 2.0;
};


#endif
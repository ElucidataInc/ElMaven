#ifndef JSONREPORTS_H
#define JSONREPORTS_H

#include <boost/algorithm/string.hpp>
#include "standardincludes.h"

// #include "../mzroll/tabledockwidget.h"

using namespace std;

class mzSample;
class EIC;
class PeakGroup;
class MavenParameters;

class JSONReports{

public:
    JSONReports();
    JSONReports(MavenParameters* _mp, bool pollyUpload = false);
    ~JSONReports();
    void save(string filename,vector<PeakGroup> allgroups,vector<mzSample*> vsampleNames);
    //void writeGroupMzEICJson(PeakGroup& grp,ofstream& myfile, vector<mzSample*> vsampleNames);
private:  
    void _writeGroup(PeakGroup& grp,ofstream& myfile);
    void _writePeak(PeakGroup& grp,ofstream& myfile, vector<mzSample*> vsampleNames);
    void _writeEIC(PeakGroup& grp,ofstream& myfile, mzSample*);
    void _writeCompoundLink(PeakGroup& grp,ofstream& myfile);
    string _sanitizeJSONstring(string s);
    
    float _outputRtWindow = 2.0;
    bool _uploadToPolly;
    MavenParameters* _mavenParameters;
};


#endif

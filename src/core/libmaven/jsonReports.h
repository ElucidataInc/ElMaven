#ifndef JSONREPORTS_H
#define JSONREPORTS_H

#include <boost/algorithm/string.hpp>
#include "standardincludes.h"

using namespace std;

class mzSample;
class EIC;
class PeakGroup;
class MavenParameters;

class JSONReports{

public:
    JSONReports();
    JSONReports(MavenParameters* mp);
    ~JSONReports();

    /**
     * @brief save Stores the compounds information in a json file.
     * @param filename Output filename.
     * @param allgroups Formed after processing the samples.
     * @param vsampleNames  vector of samples uploaded.
     */
    void save(string filename, vector<PeakGroup> allgroups, vector<mzSample*> vsampleNames);

private:  
    /**
     * @brief _writeGroup write specific group information to the file.
     * @param grp Group to be written.
     * @param myfile filename.
     */
    void _writeGroup(PeakGroup& grp, ofstream& filename);

    /**
     * @brief _writeGroup write peak information to the file.
     * @param grp PeakGroup to be written.
     * @param myfile filename.
     * @param samples uploaded.
     */
    void _writePeak(PeakGroup& grp, ofstream& filename, vector<mzSample*> vsampleNames);

    /**
     * @brief _writeEIC EIC infomation exported to csv file.
     * @param grp
     * @param myfile
     */
    void _writeEIC(PeakGroup& grp, ofstream& filename, mzSample*);

    /**
     * @brief _writeCompoundLink writes Compound Link of group
     * @param grp
     * @param myfile
     */
    void _writeCompoundLink(PeakGroup& grp, ofstream& filename);
    string _sanitizeJSONstring(string s);
    
    float _outputRtWindow = 2.0;
    bool _uploadToPolly;
    MavenParameters* _mavenParameters;
};

#endif

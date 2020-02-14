#ifndef MZALIGNER_H
#define MZALIGNER_H

#include <QJsonObject>
#include <boost/signals2.hpp>

#include "standardincludes.h"

class PeakGroup;
class mzSample;
class ObiParams;
class ObiWarp;
class MavenParameters;

using namespace std;

struct AlignmentSegment {
    string sampleName;
    float segStart;
    float segEnd;
    float newStart;
    float newEnd;
    float updateRt(float oldRt);
};

class Aligner {
   public:
    Aligner();
    void doAlignment(vector<PeakGroup*>& peakgroups);
    vector<double> groupMeanRt();
    double checkFit();
    void Fit(int ideg);
    void saveFit();
    void PolyFit(int poly_align_degree);
    void restoreFit();
    void setMaxIterations(int x) { maxIterations = x; }
    void setPolymialDegree(int x) { polynomialDegree = x; }
    bool alignWithObiWarp(vector<mzSample*> samples,
                         ObiParams* obiParams,
                         const MavenParameters* mp);
    bool alignSampleRts(mzSample* sample,
                        vector<float> &mzPoints,
                        ObiWarp& obiWarp,
                        bool setAsReference,
                        const MavenParameters* mp,
                        map<string,vector<AlignmentSegment>>& alignmentSegment_private);
    map<pair<string,string>, double> getDeltaRt() {return deltaRt; }
	map<pair<string, string>, double> deltaRt;
    vector<vector<float> > fit;
    vector<mzSample*> samples;

    int medianRt;
    int compoundDataRt;
    float tolerance;
    map<mzSample*, int> sampleDegree;
    map<mzSample*, vector<double> > sampleCoefficient;

    static mzSample* refSample;
    static void setRefSample(mzSample* sample);

    /**
     * @brief Add an AlignmentSegment that will be used when performing
     * segmented alignment on the next call to `performSegmentedAlignment`.
     * @param sampleName Name of the sample associated with this segment.
     * @param seg The AlignmentSegment to be added.
     * @param alignmentSegment_private Private map, function makes
     * use of multiprocessing. Each Iteration must make edits private maps
     * and then merged into final map.
     */
    void addSegment(string sampleName,
                    AlignmentSegment seg,
                    map<string,vector<AlignmentSegment>>& alignmentSegment_private);

    /**
     * @brief Perform alignment using segments of known retention times, where
     * the rt values in-between these known (aligned) segments will be simply
     * interpolated.
     */
    void performSegmentedAlignment();

    /**
     * @brief Set the samples for the next alignment operation.
     * @param set A vector of pointers to samples.
     */
    void setSamples(vector<mzSample*> set) { samples = set; }

    map<string,vector<AlignmentSegment>> alignmentSegments()
    {
        return _alignmentSegments;
    }

    void setAlignmentSegment(map<string,vector<AlignmentSegment>> mp)
    {
        _alignmentSegments = mp;
    }

public:
    boost::signals2::signal< void (const string&,unsigned int , int ) > setAlignmentProgress;

   private:
    vector<PeakGroup*> allgroups;
    int maxIterations;
    int polynomialDegree;
    map<string,vector<AlignmentSegment>> _alignmentSegments;
};


#endif

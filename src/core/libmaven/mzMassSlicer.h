#ifndef MASSSLICES_H
#define MASSSLICES_H

#include <omp.h>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "standardincludes.h"

class MassCutoff;
class mzSample;
class mzSlice;

using namespace std;

/**
 * @class MassSlices
 * @ingroup libmaven
 * @brief Mass Slices class.
 * @author Elucidata
 */
class MassSlices {

    public:
        MassSlices();
        ~MassSlices();

        void sendSignal(const string& progressText,
                        unsigned int completed_samples,
                        int total_samples);

        vector<mzSlice*> slices;

        /**
         * @brief This function finds the `mzSlice`, present in cache, whose area
         * overlaps with the given area (in mz-rt space) and either of the
         * centers of these areas is contained in the other.
         * @details This function will pick a slice from cache if either of the
         * following conditions are satisfied:
         *  1. If the center of an existing slice is contained within the given
         *     area.
         *  2. If the center of the given area is contained within the area of
         *     and existing slice.
         *
         * If multiple such slices exist that satisfy either of the conditions,
         * then the one with the shortest distance from the center of given area
         * is returned.
         * @param mzMinBound Lower m/z bound.
         * @param mzMaxBound Upper m/z bound.
         * @param rtMinBound Lower rt bound.
         * @param rtMaxBound Upper rt bound.
         * @return The `mzSlice` that best "contains" the given m/z and rt
         * value. If no such slice exists, a nullptr is returned.
         */
        mzSlice* sliceExists(float mzMinBound,
                             float mzMaxBound,
                             float rtMinBound,
                             float rtMaxBound);

        void removeDuplicateSlices(MassCutoff *massCutoff, float threshold);
        
        /**
         * [This is function is called when mass Slicing using 
         * AlgorithmB returns no slices. The slices here are created using the filterLine
         * in Mzml and Mzxml files.]
         * @method algorithmA
         */
        void algorithmA();

        /**
         * This is the main function that does the untargeted peak detection.
         * @details This function does not need a DB to check the peaks. The
         * function essentially loops over every observation in every scan in
         * every sample. Every observation is checked if it is already present
         * in a slice or not. If present in a slice MZmax, MZmin, RTmin, RTmax,
         * intensity, MZ and RT are modified and the  slice then put back into
         * cache. If not then then a new slice is created and added to the
         * slice.
         * @param massCutoff The user defined mass tolerance for MZ range
         * @param rtStep Minimum RT range for RT window
         */
        void algorithmB( MassCutoff *massCutoff, int rtStep);

        void algorithmC(float ppm, float minIntensity, float rtStep);
        /**
         * [setMaxSlices ]
         * @method setMaxSlices
         * @param  x            []
         */
        void setMaxSlices( int x) { _maxSlices=x; }

        /**
         * [setSamples ]
         * @method setSamples
         * @param  samples    []
         */
        void setSamples(vector<mzSample*> samples)  { this->samples = samples; }

        void setMaxIntensity( float v) {  _maxIntensity=v; }
        void setMinIntensity( float v) {  _minIntensity=v; }
        void setMinRt       ( float v) {  _minRt = v; }
        void setMaxRt	    ( float v) {  _maxRt = v; }
        void setMaxMz       ( float v) {  _maxMz = v; }
        void setMinMz	    ( float v) {  _minMz = v; }
        void setMinCharge   ( float v) {  _minCharge = v; }
        void setMaxCharge   ( float v) {  _maxCharge = v; }
        void setPrecursorPPMTolr (MassCutoff* v) { massCutoff = v; }
	    void setMavenParameters(MavenParameters* mp) { mavenParameters = mp;}
        void stopSlicing();

    private:
        unsigned int _maxSlices;
        float _minRt;
        float _maxRt;
        float _minMz;
        float _maxMz;
        float _maxIntensity;
        float _minIntensity;
        int _minCharge;
        int _maxCharge;
        MassCutoff *massCutoff;

        vector<mzSample*> samples;
        multimap<int,mzSlice*>cache;
        MavenParameters* mavenParameters;

};
#endif

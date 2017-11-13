#ifndef MASSSLICES_H
#define MASSSLICES_H

#include "mavenparameters.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "Matrix.h"

#ifndef __APPLE__
#include <omp.h>
#endif

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

class mzSample;
using namespace std;

/**
 * @class MassSlices
 * @ingroup libmaven
 * @brief Mass Slices class.
 * @author Elucidata
 */
class MassSlices {

    public:

        MassSlices()  {
            _maxSlices=INT_MAX; 
            _minRt=FLT_MIN; _minMz=FLT_MIN; _minIntensity=FLT_MIN;
            _maxRt=FLT_MAX; _maxMz=FLT_MAX; _maxIntensity=FLT_MAX;
            _minCharge=0; _maxCharge=INT_MAX;
            massCutoff=NULL;
        }
        ~MassSlices() { delete_all(slices); cache.clear(); }

        void sendSignal( const string& progressText, unsigned int completed_samples, int total_samples)
        {
            mavenParameters->sig(progressText, completed_samples, total_samples);
        }

        vector<mzSlice*> slices;
        mzSlice* sliceExists(float mz,float rt);

        void removeDuplicateSlices(MassCutoff *massCutoff, float threshold);
        
        /**
         * [This is function is called when mass Slicing using 
         * AlgorithmB returns no slices. The slices here are created using the filterLine
         * in Mzml and Mzxml files.]
         * @method algorithmA
         */
        void algorithmA();

        /**
         * [This is the main function that does the peakdetection
         * This does not need a DB to check the peaks. The function essentially loops over
         * every observation in every scan in every sample. Every observation is checked if
         * it is already present in a slice or not. If present in a slice MZmax, MZmin, RTmin,
         * RTmax, intensity, MZ and RT are modified and the  slice then put back into cache. If 
         * not then then a new slice is created and added to the slice.]
         * @method AlgorithmB
         * @param userPPM      The user defined PPM for MZ range
         * @param rtStep       Minimum RT range for RT window
         */
        void algorithmB( MassCutoff *massCutoff, int step);


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

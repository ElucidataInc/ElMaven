#ifndef MASSSLICES_H
#define MASSSLICES_H

#include "mavenparameters.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "Matrix.h"

#include <omp.h>
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
            _precursorMassAccPair=make_pair(ppm,1000);
        }
        ~MassSlices() { delete_all(slices); cache.clear(); }

        void sendSignal( const string& progressText, unsigned int completed_samples, int total_samples)
        {
            mavenParameters->sig(progressText, completed_samples, total_samples);
        }

        vector<mzSlice*> slices;
        mzSlice* sliceExists(float mz,float rt);

        /**
         * [algorithmA ]
         * @method algorithmA
         */
        void algorithmA();

        /**
         * [algorithmB ]
         * @method algorithmB
         * @param  ppm          []
         * @param  minIntensity []
         * @param  step         []
         */
        void algorithmB(pair<massAccType,double> pr, int step);


        void algorithmC(pair<massAccType,double> pr, float minIntensity, float rtStep);
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
        pair<massAccType,double> _precursorMassAccPair;

        vector<mzSample*> samples;
        multimap<int,mzSlice*>cache;
        MavenParameters* mavenParameters;

};
#endif

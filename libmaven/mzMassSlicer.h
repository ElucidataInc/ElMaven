#ifndef MASSSLICES_H
#define MASSSLICES_H

#include "mzSample.h"
#include "mzUtils.h"
#include "Matrix.h"

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
			MassSlices()  { _maxSlices=INT_MAX; }
			~MassSlices() { delete_all(slices); cache.clear(); }

			vector<mzSlice*> slices;
			mzSlice* sliceExists(float mz,float rt);

			/**
			 * [algorithmA description]
			 * @method algorithmA
			 */
			void algorithmA();

			/**
			 * [algorithmB description]
			 * @method algorithmB
			 * @param  ppm          [description]
			 * @param  minIntensity [description]
			 * @param  step         [description]
			 */
    	void algorithmB(float ppm, float minIntensity, int step);

			/**
			 * [setMaxSlices description]
			 * @method setMaxSlices
			 * @param  x            [description]
			 */
			void setMaxSlices( int x) { _maxSlices=x; }

			/**
			 * [setSamples description]
			 * @method setSamples
			 * @param  samples    [description]
			 */
			void setSamples(vector<mzSample*> samples)  { this->samples = samples; }

		private:
      unsigned int _maxSlices;
			vector<mzSample*> samples;
			multimap<int,mzSlice*>cache;


};
#endif

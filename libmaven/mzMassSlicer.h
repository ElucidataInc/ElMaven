#ifndef MASSSLICES_H
#define MASSSLICES_H

#include "mzSample.h"
#include "mzUtils.h"
#include "Matrix.h"


#include <omp.h>

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
    	void algorithmB(float ppm, float minIntensity, int step);

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

		private:
      unsigned int _maxSlices;
			vector<mzSample*> samples;
			multimap<int,mzSlice*>cache;


};
#endif

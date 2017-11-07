#include "pullEics.h"

//TODO: Refactor this function. Too many parameters - Sahil
vector<EIC*> PullEICs::pullEICs(mzSlice* slice,
								 std::vector<mzSample*>&samples,
								 int peakDetect,
								 int smoothingWindow,
								 int smoothingAlgorithm,
								 float amuQ1,
								 float amuQ3,
								 int baseline_smoothingWindow,
								 int baseline_dropTopX,
								 double minSignalBaselineDifference,
								 int eicType,
								 string filterline)
{

	vector<EIC*> eics;
	vector<mzSample*> vsamples;
	#ifndef __APPLE__
	#pragma omp parallel default(shared)
	#endif
	{

		#ifndef __APPLE__
		#pragma omp for
		#endif
		for (unsigned int i = 0; i < samples.size(); i++)
		{
			if (samples[i] == NULL)
				continue;
			if (samples[i]->isSelected == false)
				continue;
			#ifndef __APPLE__
			#pragma omp critical
			#endif
			vsamples.push_back(samples[i]);
		}

		// single threaded version - getting EICs of selected samples.
		// #pragma omp parallel for ordered

		#ifndef __APPLE__
		#pragma omp for
		#endif
		for (unsigned int i = 0; i < vsamples.size(); i++)
		{
			//Samples been selected
			mzSample* sample = vsamples[i];
			//getting the slice with which EIC has to be pulled
			Compound* c = slice->compound;

			EIC* e = NULL;

			if (!slice->srmId.empty())
			{

				e = sample->getEIC(slice->srmId, eicType);
			}
			else if (c && c->precursorMz > 0 && c->productMz > 0)
			{

				e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz, eicType,
								   filterline, amuQ1, amuQ3);
			}
			else
			{

				e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin,
								   slice->rtmax, 1, eicType, filterline);
			}

			if (e)
			{

				EIC::SmootherType smootherType =
					(EIC::SmootherType) smoothingAlgorithm;
				e->setSmootherType(smootherType);
				e->setBaselineSmoothingWindow(baseline_smoothingWindow);
				e->setBaselineDropTopX(baseline_dropTopX);
				e->setFilterSignalBaselineDiff(minSignalBaselineDifference);
				e->getPeakPositions(smoothingWindow);

				#ifndef __APPLE__
				#pragma omp critical
				#endif
				eics.push_back(e);
			}
		}
	}
	return eics;
}
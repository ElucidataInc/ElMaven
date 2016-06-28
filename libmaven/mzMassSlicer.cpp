#include "mzMassSlicer.h"

void MassSlices::algorithmA() {
    delete_all(slices);
    cache.clear();
    map< string, int> seen;
    
    #pragma omp parallel for ordered
    for(unsigned int i=0; i < samples.size(); i++) {
        for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {
            Scan* scan = samples[i]->scans[j];
            if ( scan->filterLine.empty() ) continue;
            if ( seen.count( scan->filterLine ) ) continue;
            mzSlice* s = new mzSlice(scan->filterLine);
            slices.push_back(s);
            seen[ scan->filterLine ]=1;
        }
    }
    cerr << "#algorithmA" << slices.size() << endl;
}


void MassSlices::algorithmB(float userPPM, float minIntensity, int rtStep) {
	delete_all(slices);
	cache.clear();

        float ppm=userPPM;	//centering of ppm values for +/- mz calculation
	float rtWindow=2.0;
	if (samples.size() > 0 ) rtWindow = (samples[0]->getAverageFullScanTime()*rtStep);

	cerr << "#algorithmB:" << " PPM=" << userPPM << " rtWindow=" << rtWindow << " rtStep=" << rtStep << endl;
	
	//#pragma omp parallel for ordered      
	for(unsigned int i=0; i < samples.size(); i++) {
		//if (slices.size() > _maxSlices) break;
		Scan* lastScan = NULL;
		cerr << samples[i]->sampleName << endl;
  
		for(unsigned int j=0; j < samples[i]->scans.size(); j++ ) {
			Scan* scan = samples[i]->scans[j];
			if (scan->mslevel != 1 ) continue;

			if ( lastScan ) {
				for(unsigned int k=0; k < scan->nobs(); k++ ){
					if (scan->intensity[k] < minIntensity) continue;
					float mz = scan->mz[k];
					float mzmax = mz + mz/1e6*ppm;
					float mzmin = mz - mz/1e6*ppm;
					float rt = scan->rt;

					mzSlice* Z;
				//	#pragma omp declare target critical
					Z = sliceExists(mz,rt);
				//	#pragma omp end declare target critical
					
					if (Z) {
						//cerr << "Merged Slice " <<  Z->mzmin << " " << Z->mzmax << " " << scan->intensity[k] << "  " << Z->ionCount << endl;

						Z->ionCount = std::max((float) Z->ionCount, (float ) scan->intensity[k]);
						Z->rtmax = std::max((float)Z->rtmax, rt+2*rtWindow);
						Z->rtmin = std::min((float)Z->rtmin, rt-2*rtWindow);
						Z->mzmax = std::max((float)Z->mzmax, mzmax);
						Z->mzmin = std::min((float)Z->mzmin, mzmin);


						//make sure that mz windown doesn't get out of control
						if (Z->mzmin < mz-(mz/1e6*userPPM)) Z->mzmin =  mz-(mz/1e6*userPPM);
						if (Z->mzmax > mz+(mz/1e6*userPPM)) Z->mzmax =  mz+(mz/1e6*userPPM);
						Z->mz =(Z->mzmin+Z->mzmax)/2; Z->rt=(Z->rtmin+Z->rtmax)/2;
						//cerr << Z->mz << " " << Z->mzmin << " " << Z->mzmax << " " << ppmDist((float)Z->mzmin,mz) << endl;
						continue;
					}

					if ( lastScan->hasMz(mz, userPPM) ) {
						//cerr << "\t" << rt << "  " << mzmin << "  "  << mzmax << endl;
						mzSlice* s = new mzSlice(mzmin,mzmax, rt-2*rtWindow, rt+2*rtWindow);
						s->ionCount = scan->intensity[k];
						s->rt=scan->rt;
						s->mz=mz;
						//cerr << "New Slice " <<  s->mzmin << " " << s->mzmax << " " << s->ionCount << endl;
						slices.push_back(s);

						int mzRange = mz;
						cache.insert( pair<int,mzSlice*>(mzRange, s));

						if ( slices.size() % 1000 == 0) { cerr << "MassSlices count=" << slices.size() << endl; }
					}
					}
				}

				lastScan = scan;
			}
		}
		cerr << "Found=" << slices.size() << " slices" << endl;
		sort(slices.begin(),slices.end(), mzSlice::compIntensity);
	}


mzSlice*  MassSlices::sliceExists(float mz, float rt) {
	pair< multimap<int, mzSlice*>::iterator,  multimap<int, mzSlice*>::iterator > ppp;
	ppp = cache.equal_range( (int) mz );
	multimap<int, mzSlice*>::iterator it2 = ppp.first;
	float bestDist=10000000; mzSlice* best=NULL;
	
	for( ;it2 != ppp.second; ++it2 ) {
		mzSlice* x = (*it2).second;
		if (mz > x->mzmin && mz < x->mzmax && rt > x->rtmin && rt < x->rtmax) {
			float d = (mz-x->mzmin) + (x->mzmax-mz);
			if ( d < bestDist ) { best=x; bestDist=d; }
		}
	}
	return best;
}

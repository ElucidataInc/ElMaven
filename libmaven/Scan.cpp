#include "mzSample.h"

Scan::Scan(mzSample* sample, int scannum, int mslevel, float rt, float precursorMz, int polarity) {
    this->sample = sample;
    this->rt = rt;
    this->scannum = scannum;
    this->precursorMz = precursorMz;
    this->mslevel = mslevel;
    this->polarity = polarity;
    this->productMz=0;
    this->collisionEnergy=0;
    this->centroided=0;

    /*if ( polarity != 1 && polarity != -1 ) {
        cerr << "Warning: polarity of scan is not 1 or -1 " << polarity << endl;
    }*/
}

void Scan::deepcopy(Scan* b) {
    this->sample = b->sample;
    this->rt = b->rt;
    this->scannum = b->scannum;
    this->precursorMz = b->precursorMz;
    this->mslevel = b->mslevel;
    this->polarity = b->polarity;
    this->productMz= b->productMz;
    this->collisionEnergy= b->collisionEnergy;
    this->centroided= b->centroided;
    this->intensity = b->intensity;
    this->mz    = b->mz;
    this->scanType = b->scanType;
    this->filterLine = b->filterLine;
    this->setPolarity( b->getPolarity() );

}

int Scan::findHighestIntensityPos(float _mz, float ppm) {
        float mzmin = _mz - _mz/1e6*ppm;
        float mzmax = _mz + _mz/1e6*ppm;

        vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin-1);
        int lb = itr-mz.begin();
        int bestPos=-1;  float highestIntensity=0;
        for(unsigned int k=lb; k < nobs(); k++ ) {
                if (mz[k] < mzmin) continue;
                if (mz[k] > mzmax) break;
                if (intensity[k] > highestIntensity ) {
                        highestIntensity=intensity[k];
                        bestPos=k;
                }
        }
        return bestPos;
}

vector<int> Scan::findMatchingMzs(float mzmin, float mzmax) {
	vector<int>matches;
	vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin-1);
	int lb = itr-mz.begin();
	for(unsigned int k=lb; k < nobs(); k++ ) {
		if (mz[k] < mzmin) continue;
		if (mz[k] > mzmax) break;
		matches.push_back(k);
	}
//	cerr << "matches:" << mzmin << " " << mzmax << " " << matches.size() << endl;
	return matches;
}

void Scan::quantileFilter(int minQuantile) {
        if (intensity.size() == 0 ) return;
        if( minQuantile <= 0 || minQuantile >= 100 ) return;

        int vsize=intensity.size();
        vector<float>dist = quantileDistribution(this->intensity);
        vector<float>cMz;
        vector<float>cIntensity;
        for(int i=0; i<vsize; i++ ) {
            if ( intensity[i] > dist[ minQuantile ]) {
                cMz.push_back(mz[i]);
                cIntensity.push_back(intensity[i]);
            }
        }
        vector<float>(cMz).swap(cMz);
        vector<float>(cIntensity).swap(cIntensity);
        mz.swap(cMz);
        intensity.swap(cIntensity);
}

void Scan::intensityFilter(int minIntensity) {
        if (intensity.size() == 0 ) return;

        //first pass.. find local maxima in intensity space
        int vsize=intensity.size();
        vector<float>cMz;
        vector<float>cIntensity;
        for(int i=0; i<vsize; i++ ) {
           if ( intensity[i] > minIntensity) { //local maxima
                cMz.push_back(mz[i]);
                cIntensity.push_back(intensity[i]);
            }
        }
        vector<float>(cMz).swap(cMz);
        vector<float>(cIntensity).swap(cIntensity);
        mz.swap(cMz);
        intensity.swap(cIntensity);
}

void Scan::simpleCentroid() {

        if (intensity.size() < 5 ) return;

        //pass zero smooth..
		int smoothWindow = intensity.size() / 20;
		int order=2;

		if (smoothWindow < 1 )  { smoothWindow = 2; }
		if (smoothWindow > 10 ) { smoothWindow = 10; }

        mzUtils::SavGolSmoother smoother(smoothWindow,smoothWindow,order);
		//smooth once
        vector<float>spline = smoother.Smooth(intensity);
		//smooth twice
        spline = smoother.Smooth(spline);

        //find local maxima in intensity space
        int vsize=spline.size();
        vector<float>cMz;
        vector<float>cIntensity;
        for(int i=1; i<vsize-2; i++ ) {
            if ( spline[i] > spline[i-1] &&  spline[i] > spline[i+1] ) { //local maxima in spline space
					//local maxima in real intensity space
					float maxMz=mz[i]; float maxIntensity=intensity[i];
					for(int j=i-1; j<i+1; j++) {
							if (intensity[i] > maxIntensity) { maxIntensity=intensity[i]; maxMz=mz[i]; }
					}
                	cMz.push_back(maxMz);
                	cIntensity.push_back(maxIntensity);
            }
        }

        vector<float>(cMz).swap(cMz);
        vector<float>(cIntensity).swap(cIntensity);
        mz.swap(cMz);
        intensity.swap(cIntensity);

        centroided = true;
}

bool Scan::hasMz(float _mz, float ppm) {
    float mzmin = _mz - _mz/1e6*ppm;
    float mzmax = _mz + _mz/1e6*ppm;
	vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin);
	//cerr << _mz  << " k=" << lb << "/" << mz.size() << " mzk=" << mz[lb] << endl;
	for(unsigned int k=itr-mz.begin(); k < nobs(); k++ ) {
        if (mz[k] >= mzmin && mz[k] <= mzmax )  return true;
		if (mz[k] > mzmax ) return false;
    }
    return false;
}

void Scan::summary() {

    cerr << "Polarity=" << getPolarity()
         << " msLevel="  << mslevel
         << " rt=" << rt
         << " m/z size=" << mz.size()
         << " ints size=" << intensity.size()
         << " precursorMz=" << precursorMz
         << " productMz=" << productMz
         << " srmID=" << filterLine
         << " totalIntensty=" << this->totalIntensity()
         << endl;

}

vector<float> Scan::chargeSeries(float Mx, unsigned int Zx) {
    //Mx  = observed m/z
    //Zz  = charge of Mx
    //n =  number of charge states to g
    vector<float>chargeStates(Zx+20,0);
    double M = (Mx*Zx)-Zx;
    for(unsigned int z=1; z<Zx+20; z++) chargeStates[z]=(M+z)/z;
    return(chargeStates);
}

void Scan::initaliseBrotherData(int z, float mzfocus) {
        brotherdata->expectedMass = (mzfocus*z)-z;     //predict what M ought to be
        brotherdata->countMatches=0;
        brotherdata->totalIntensity=0;
        brotherdata->upCount=0;
        brotherdata->downCount=0;
        brotherdata->minZ=z;
        brotherdata->maxZ=z;
}

bool Scan::updateBrotherDataIfPeakFound(ChargedSpecies* x, int ii, bool lastMatched, float lastIntensity, float mzfocus, float noiseLevel, float ppmMerge) {
            
            float brotherMz = (brotherdata->expectedMass+ii)/ii;
            int pos = this->findHighestIntensityPos(brotherMz,ppmMerge);
            float brotherIntensity = pos>=0?this->intensity[pos]:0;
            float snRatio = brotherIntensity/noiseLevel;
            if (brotherIntensity < 1.1*lastIntensity && snRatio > 2 && withinXppm(this->mz[pos]*ii-ii,brotherdata->expectedMass,ppmMerge)) {
                brotherdata->maxZ = ii;
                brotherdata->countMatches++;
                brotherdata->upCount++;
                brotherdata->totalIntensity += brotherIntensity;
                lastMatched=true;
                lastIntensity=brotherIntensity;
                return true;
                //cout << "up.." << ii << " pos=" << pos << " snRa=" << snRatio << "\t"  << " T=" << totalIntensity <<  endl;
            } else if (lastMatched == true) {   //last charge matched ..but this one didn't..
                return false;
            }
            return true;
}

void Scan::findError(ChargedSpecies* x) {
            float totalError=0; brotherdata->totalIntensity=0;
            for(unsigned int i=0; i < x->observedCharges.size(); i++ ) {
                    float My = (x->observedMzs[i]*x->observedCharges[i]) - x->observedCharges[i];
                    float deltaM = abs(x->deconvolutedMass - My);
                    totalError += deltaM*deltaM;
                    brotherdata->totalIntensity += x->observedIntensities[i];
            }
            //cout << "\t" << mzfocus << " matches=" << x->countMatches << " totalInts=" << x->totalIntensity << " Score=" << x->qscore << endl;
            x->error = sqrt(totalError/x->countMatches);
            //cout << "-------- total Error= " << sqrt(totalError/x->countMatches) << " total Intensity=" << totalIntensity << endl;
}

void Scan::updateChargedSpeciesDataAndFindQScore(ChargedSpecies* x,float mzfocus, float noiseLevel, float ppmMerge, int minChargedStates) {
        if (x->totalIntensity < brotherdata->totalIntensity && brotherdata->countMatches>minChargedStates && brotherdata->upCount >= 2 && brotherdata->downCount >= 2 ) {
                x->totalIntensity = brotherdata->totalIntensity;
                x->countMatches=brotherdata->countMatches;
                x->deconvolutedMass = (mzfocus*z)-z;
                x->minZ = brotherdata->minZ;
                x->maxZ = brotherdata->maxZ;
                x->scan = this;
                x->observedCharges.clear();
                x->observedMzs.clear();
                x->observedIntensities.clear();
                x->upCount = brotherdata->upCount;
                x->downCount = brotherdata->downCount;

                float qscore=0;
                for(int ii=brotherdata->minZ; ii <= brotherdata->maxZ; ii++ ) {
                        int pos = this->findHighestIntensityPos( (brotherdata->expectedMass+ii)/ii, ppmMerge );
                        if (pos > 0 ) {
                                x->observedCharges.push_back(ii);
                                x->observedMzs.push_back( this->mz[pos] );
                                x->observedIntensities.push_back( this->intensity[pos] );
                                float snRatio = this->intensity[pos]/noiseLevel;
                                qscore += log(pow(0.97,(int)snRatio));
                        //      if(ii == z) cout << '*';
                        //      cout << setprecision(2) << snRatio << ",";
                        }
                }
                x->qscore = -20*qscore;
                //cout << " upC=" << x->upCount << " downC=" << x->downCount << " qscore=" << -qscore <<  " M=" << x->deconvolutedMass << endl;
        }
}

void Scan::findBrotherPeaks(ChargedSpecies* x, int z, float mzfocus, float noiseLevel, float ppmMerge, int minDeconvolutionCharge, int maxDeconvolutionCharge) {
    bool flag=true;
    bool lastMatched=false;
    float lastIntensity=parentdata->parentPeakIntensity;
    for(int ii=z; ii < z+50 && ii<maxDeconvolutionCharge; ii++ ) {
        flag=updateBrotherDataIfPeakFound(x,ii,lastMatched,lastIntensity,mzfocus,noiseLevel,ppmMerge);
        if (flag==false)
            break;
    }

    lastMatched = false;
    lastIntensity=parentdata->parentPeakIntensity;
    for(int ii=z-1; ii > z-50 && ii>minDeconvolutionCharge; ii--) {
        flag=updateBrotherDataIfPeakFound(x,ii,lastMatched,lastIntensity,mzfocus,noiseLevel,ppmMerge);
        if (flag==false)
            break;
    }
}

bool Scan::setParentData(float mzfocus, float noiseLevel, float ppmMerge, float minSigNoiseRatio) {
    parentdata->flag=true;
    int mzfocus_pos = this->findHighestIntensityPos(mzfocus,ppmMerge);
    if (mzfocus_pos < 0 ) { cout << "ERROR: Can't find parent " << mzfocus << endl; parentdata->flag=false; return parentdata->flag; }
    parentdata->parentPeakIntensity=this->intensity[mzfocus_pos];
    float parentPeakSN=parentdata->parentPeakIntensity/noiseLevel;
    if(parentPeakSN <=minSigNoiseRatio) {parentdata->flag=false; return parentdata->flag;}
    return parentdata->flag;
}

ChargedSpecies* Scan::deconvolute(float mzfocus, float noiseLevel, float ppmMerge, float minSigNoiseRatio, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates ) {
    parentdata=&p;
    brotherdata=&b;

    //cout << "Deconvolution of " << mzfocus << " pSN=" << parentPeakSN << endl;
    bool flag=setParentData {mzfocus, noiseLevel, ppmMerge, minSigNoiseRatio};
    if (flag==false)
        return NULL;

    int scanTotalIntensity=0;
    for(unsigned int i=0; i<this->nobs();i++) scanTotalIntensity+=this->intensity[i];

    ChargedSpecies* x = new ChargedSpecies();

    for(int z=minDeconvolutionCharge; z <= maxDeconvolutionCharge; z++ ) {
        
        initaliseBrotherData(z, mzfocus);

        if (brotherdata->expectedMass >= maxDeconvolutionMass || brotherdata->expectedMass <= minDeconvolutionMass ) continue;
        
        findBrotherPeaks(x, z, mzfocus, noiseLevel, ppmMerge, minDeconvolutionCharge, maxDeconvolutionCharge);

        updateChargedSpeciesDataAndFindQScore(x, mzfocus,noiseLevel,ppmMerge,minChargedStates);

    }
    // done..
    if ( x->countMatches > minChargedStates ) {
            findError(x);
            return x;
    } 
    
    else {
            delete(x);
            x=NULL;
            return(x);
    }
}

vector <pair<float,float> > Scan::getTopPeaks(float minFracCutoff) {
    vector<pair<float,float> > mzarray(nobs());
    float maxI = 0;
    for(unsigned int i=0; i < nobs(); i++ ) {
        mzarray[i] = make_pair(intensity[i], mz[i]);
        if(intensity[i] > maxI) maxI=intensity[i];
    }

    vector< pair<float,float> > selected;
    sort(mzarray.begin(), mzarray.end());
    //TODO: why does i not go down to 0? is it beacuse, when sorted it gives the smallest value?
    //Does that mean the values doesnt have to be pushed in selected?
    for(int i=mzarray.size()-1; i > 0; i-- ) {
        if (mzarray[i].first/maxI > minFracCutoff) {
            mzarray[i].first = (mzarray[i].first/maxI)*100;
            selected.push_back(mzarray[i]);
        } else {
            break;
        }
    }
    return selected;
}

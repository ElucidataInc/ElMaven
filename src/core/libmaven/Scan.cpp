#include "Scan.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "constants.h"
#include "SavGolSmoother.h"

Scan::Scan(mzSample* sample, int scannum, int mslevel, float rt, float precursorMz, int polarity) {
    this->sample = sample;
    this->rt = rt;
    this->originalRt = rt;
    this->scannum = scannum;
    this->precursorMz = precursorMz;
    this->mslevel = mslevel;
    this->polarity = polarity;
    this->productMz = 0;
    this->collisionEnergy = 0;
    this->centroided = 0;
	this->precursorCharge = 0;
	this->precursorIntensity = 0;
    this->_isolationWindow = 1.0f;
    this->_msType = MsType::MS1;
    this->_swathWindowMin = 0.0f;
    this->_swathWindowMax = 0.0f;
}

void Scan::deepcopy(Scan* b) {
    this->sample = b->sample;
    this->rt = b->rt;
    this->scannum = b->scannum;
    this->precursorMz = b->precursorMz;
    this->precursorIntensity= b->precursorIntensity;
    this->precursorCharge= b->precursorCharge;
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
    this->originalRt = b->originalRt;
    this->setIsolationWindow(b->isolationWindow());
}

int Scan::findHighestIntensityPos(float _mz, MassCutoff *massCutoff) {
        float mzmin = _mz - massCutoff->massCutoffValue(_mz);
        float mzmax = _mz + massCutoff->massCutoffValue(_mz);

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

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging point
int Scan::findClosestHighestIntensityPos(float _mz, MassCutoff *massCutoff) {
			float mzmin = _mz - massCutoff->getMassCutoff()-0.001;
			float mzmax = _mz + massCutoff->getMassCutoff()+0.001;

			vector<float>::iterator itr = lower_bound(mz.begin(), mz.end(), mzmin-0.1);
			int lb = itr-mz.begin();
			float highestIntensity=0; 
			for(unsigned int k=lb; k < mz.size(); k++ ) {
				if (mz[k] < mzmin) continue; 
				if (mz[k] > mzmax) break;
				if (intensity[k] > highestIntensity) highestIntensity=intensity[k];
			}
				
			int bestPos=-1; float bestScore=0;
			for(unsigned int k=lb; k < mz.size(); k++ ) {
				if (mz[k] < mzmin) continue; 
				if (mz[k] > mzmax) break;
				float deltaMz = (mz[k]-_mz); 
				float alignScore = sqrt(intensity[k] / highestIntensity)-(deltaMz*deltaMz);
			//	cerr << _mz << "\t" << k << "\t" << deltaMz << " " << alignScore << endl;
				if (bestScore < alignScore) { bestScore=alignScore; bestPos=k; }
			}
			//if(bestPos>=0) cerr << "best=" << bestPos << endl;
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

        vector<float> spline=smoothenIntensitites();

        //find local maxima in intensity space
        int vsize=spline.size();
        vector<float>cMz;
        vector<float>cIntensity;

        findLocalMaximaInIntensitySpace(vsize, &cMz, &cIntensity, &spline);

        updateIntensityWithTheLocalMaximas(&cMz, &cIntensity);

        centroided = true;
}

vector<float> Scan::smoothenIntensitites() {
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

    return spline;
}

void Scan::findLocalMaximaInIntensitySpace(int vsize, vector<float> *cMz, vector<float> *cIntensity, vector<float> *spline) {
        for(int i=1; i<vsize-2; i++ ) {
            if ( (*spline)[i] > (*spline)[i-1] &&  (*spline)[i] > (*spline)[i+1] ) { //local maxima in spline space
                    //local maxima in real intensity space
                    float maxMz=mz[i]; float maxIntensity=intensity[i];
                    for(int j=i-1; j<i+1; j++) {
                            if (intensity[i] > maxIntensity) { maxIntensity=intensity[i]; maxMz=mz[i]; }
                    }
                    cMz->push_back(maxMz);
                    cIntensity->push_back(maxIntensity);
            }
        }
}

void Scan::updateIntensityWithTheLocalMaximas(vector<float> *cMz, vector<float> *cIntensity){

    vector<float>(*cMz).swap(*cMz);
    vector<float>(*cIntensity).swap(*cIntensity);
    mz.swap(*cMz);
    intensity.swap(*cIntensity);
}

bool Scan::hasMz(float _mz, MassCutoff *massCutoff) {
    float mzmin = _mz - massCutoff->massCutoffValue(_mz);
    float mzmax = _mz + massCutoff->massCutoffValue(_mz);
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


bool Scan::setParentPeakData(float mzfocus,  float noiseLevel, MassCutoff *massCutoffMerge,float minSigNoiseRatio) {
    bool flag=true;
    int mzfocus_pos = this->findHighestIntensityPos(mzfocus,massCutoffMerge);
    if (mzfocus_pos < 0 ) { cout << "ERROR: Can't find parent " << mzfocus << endl; flag=false; return flag; }
    parentPeakIntensity=this->intensity[mzfocus_pos];
    float parentPeakSN=parentPeakIntensity/noiseLevel;
    if(parentPeakSN <=minSigNoiseRatio){ flag=false; return flag;}
    return flag;
}

void Scan::initialiseBrotherData(int z, float mzfocus) {
        brotherdata->expectedMass = (mzfocus*z)-z;     //predict what M ought to be
        brotherdata->countMatches=0;
        brotherdata->totalIntensity=0;
        brotherdata->upCount=0;
        brotherdata->downCount=0;
        brotherdata->minZ=z;
        brotherdata->maxZ=z;
}

void Scan::updateBrotherDataIfPeakFound(int loopdirection, int ii, bool *flag, bool *lastMatched, float *lastIntensity, float noiseLevel,  MassCutoff *massCutoffMerge) {

            float brotherMz = (brotherdata->expectedMass+ii)/ii;
            int pos = this->findHighestIntensityPos(brotherMz, massCutoffMerge);
            float brotherIntensity = pos>=0?this->intensity[pos]:0;
            float snRatio = brotherIntensity/noiseLevel;
            if (brotherIntensity < 1.1*(*lastIntensity) && snRatio > 2 && withinXMassCutoff(this->mz[pos]*ii-ii,brotherdata->expectedMass,massCutoffMerge)) {
                if (loopdirection==1) {
                    brotherdata->maxZ = ii;
                    brotherdata->upCount++;
                }
                else if (loopdirection==-1) {
                    brotherdata->minZ = ii;
                    brotherdata->downCount++;
                }
                brotherdata->countMatches++;
                brotherdata->totalIntensity += brotherIntensity;
                *lastMatched=true;
                *lastIntensity=brotherIntensity;
                //cout << "up.." << ii << " pos=" << pos << " snRa=" << snRatio << "\t"  << " T=" << totalIntensity <<  endl;
            } else if (*lastMatched == true) {   //last charge matched ..but this one didn't..
                *flag=false;
                return;
            }

}

void Scan::findBrotherPeaks (ChargedSpecies* x, float mzfocus, float noiseLevel,  MassCutoff *massCutoffMerge,int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates) {
    brotherdata=&b;
    for(int z=minDeconvolutionCharge; z <= maxDeconvolutionCharge; z++ ) {

        initialiseBrotherData(z,mzfocus);

        if (brotherdata->expectedMass >= maxDeconvolutionMass || brotherdata->expectedMass <= minDeconvolutionMass ) continue;
        bool flag=true;
        bool lastMatched=false;
        int loopdirection;
        loopdirection=1;
        float lastIntensity=parentPeakIntensity;
        for(int ii=z; ii < z+50 && ii<maxDeconvolutionCharge; ii++ ) {
            updateBrotherDataIfPeakFound(loopdirection,ii,&flag, &lastMatched,&lastIntensity,noiseLevel,massCutoffMerge);
            if (flag==false)
               break;
        }

        flag=true;
        lastMatched = false;
        loopdirection=-1;
        lastIntensity=parentPeakIntensity;
        for(int ii=z-1; ii > z-50 && ii>minDeconvolutionCharge; ii--) {
             updateBrotherDataIfPeakFound(loopdirection,ii,&flag, &lastMatched,&lastIntensity,noiseLevel,massCutoffMerge);
             if (flag==false)
                 break;
        }

        updateChargedSpeciesDataAndFindQScore(x, z, mzfocus,noiseLevel,massCutoffMerge,minChargedStates);

    }
    // done..
}


void Scan::updateChargedSpeciesDataAndFindQScore(ChargedSpecies* x, int z,float mzfocus, float noiseLevel,  MassCutoff *massCutoffMerge, int minChargedStates) {
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
                        int pos = this->findHighestIntensityPos( (brotherdata->expectedMass+ii)/ii, massCutoffMerge );
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

ChargedSpecies* Scan::deconvolute(float mzfocus, float noiseLevel,  MassCutoff *massCutoffMerge, float minSigNoiseRatio, int minDeconvolutionCharge, int maxDeconvolutionCharge, int minDeconvolutionMass, int maxDeconvolutionMass, int minChargedStates ) {


    bool flag=setParentPeakData(mzfocus,noiseLevel,massCutoffMerge,minSigNoiseRatio);

        if (flag==false)
            return NULL;
    //cout << "Deconvolution of " << mzfocus << " pSN=" << parentPeakSN << endl;

    int scanTotalIntensity=0;
    for(unsigned int i=0; i<this->nobs();i++) scanTotalIntensity+=this->intensity[i];

    ChargedSpecies* x = new ChargedSpecies();
    findBrotherPeaks (x, mzfocus, noiseLevel, massCutoffMerge, minDeconvolutionCharge, maxDeconvolutionCharge, minDeconvolutionMass, maxDeconvolutionMass, minChargedStates);


    if ( x->countMatches > minChargedStates ) {
            findError(x);
            return x;
    } else {
            delete(x);
            x=NULL;
            return(x);
    }
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
vector<int> Scan::intensityOrderDesc() {
    vector<pair<float,int> > mzarray(nobs());
    vector<int>position(nobs());
    for(unsigned int pos=0; pos < nobs(); pos++ ) {
        mzarray[pos] = make_pair(intensity[pos],pos);
    }

   //reverse sort first key [ ie intensity ]
   sort(mzarray.rbegin(), mzarray.rend());

   //return positions in order from highest to lowest intenisty
   for(unsigned int i=0; i < mzarray.size(); i++) { position[i] = mzarray[i].second; }
   return position;
}

vector <pair<float,float> > Scan::getTopPeaks(float minFracCutoff,float minSNRatio=3,int dropTopX=40)
{
    vector<pair<float,float>> selected;
    if (nobs() == 0)
        return selected;

    unsigned int N = nobs();

	vector<int> positions = this->intensityOrderDesc();
	float maxI = intensity[positions[0]];

   //compute baseline intensity.. 
   float cutvalueF = (100.0-(float) dropTopX)/101;
   float baseline=1; 
   unsigned int mid = N * cutvalueF;
   if(mid < N) baseline = intensity[positions[mid]];

   for(unsigned int i=0; i<N; i++) {
		   int pos = positions[i];
		   if (intensity[pos]/baseline > minSNRatio && intensity[pos]/maxI > minFracCutoff) {
				   selected.push_back( make_pair(intensity[pos], mz[pos]));
		   } else {
				   break;
		   }
   }
    return selected;
}

string Scan::toMGF() { 
    //Merged with Maven776 - Kiran
    std::stringstream buffer;
    buffer << "BEGIN IONS" << endl;
    if (sample) { buffer << "TITLE=" <<  sample->sampleName << "." << scannum << "." << scannum << "." << precursorCharge << endl; }
    buffer << "PEPMASS=" << setprecision(8) << precursorMz << " " << setprecision(3) << precursorIntensity << endl;
    buffer << "RTINSECONDS=" << setprecision(9) << rt*60 << "\n";
    buffer << "CHARGE=" << precursorCharge; if(polarity < 0) buffer << "-"; else buffer << "+"; buffer << endl;
    for(unsigned int i=0; i < mz.size(); i++) {
        buffer << setprecision(8) << mz[i] << " " << setprecision(3) << intensity[i] << endl;
    }
    buffer << "END IONS" << endl;
    //cout << buffer;
    return buffer.str();
}

vector<int> Scan::assignCharges(MassCutoff *massCutoffTolr) {
    if ( nobs() == 0) {
        vector<int>empty;
        return empty;
    }

    int N = nobs(); //current scan size
    vector<int>chargeStates (N,0);
    vector<int>peakClusters = vector<int>(N,0);
    vector<int>parentPeaks = vector<int>(N,0);
    int clusterNumber=0;

    //order intensities from high to low
    vector<int>intensityOrder = intensityOrderDesc();
    double NMASS=C13_MASS-12.00;

    //a little silly, required number of peaks in a series in already to call a charge
                          //z=0,   z=1,    z=2,   z=3,    z=4,   z=5,    z=6,     z=7,   z=8,
    int minSeriesSize[9] = { 1,     2,     3,      3,      3,     4,      4,       4,     5  } ;

    //for every position in a scan
    for(int i=0; i < N; i++ ) {
        int pos=intensityOrder[i];
        float centerMz = mz[pos];
        float centerInts = intensity[pos];
       // float ppm = (0.125/centerMz)*1e6;
       MassCutoff massCutoff=*massCutoffTolr;
       massCutoff.setMassCutoffAndType(2*massCutoffTolr->getMassCutoff(),massCutoffTolr->getMassCutoffType());
       // cerr << pos << " " <<  centerMz << " " << centerInts << " " << clusterNumber << endl;
        if (chargeStates[pos] != 0) continue;  //charge already assigned

        //check for charged peak groups
        int bestZ=0; int maxSeriesIntenisty=0;
        vector<int>bestSeries;


        //determine most likely charge state
        for(int z=5; z>=1; z--) {
            float delta = NMASS/z;
            int zSeriesIntensity=centerInts;
            vector<int>series;


            for(int j=1; j<6; j++) { //forward
                float mz=centerMz+(j*delta);

                int matchedPos = findHighestIntensityPos(mz,&massCutoff);
                if (matchedPos>0 && intensity[matchedPos]<centerInts) {
                    series.push_back(matchedPos);
                    zSeriesIntensity += intensity[matchedPos];
                } else break;
            }

            for(int j=1; j<3; j++) {  //back
                float mz=centerMz-(j*delta);
                int matchedPos = findHighestIntensityPos(mz,&massCutoff);
                if (matchedPos>0 && intensity[matchedPos]<centerInts) {
                    series.push_back(matchedPos);
                    zSeriesIntensity += intensity[matchedPos];
                } else break;
            } 
            //cerr << endl;
            if (zSeriesIntensity>maxSeriesIntenisty) { bestZ=z; maxSeriesIntenisty=zSeriesIntensity; bestSeries=series; }
        }

        //if ( i < 50) cerr << centerMz << " " << bestZ << " " << bestSeries.size() << " " << minSeriesSize[bestZ] << endl;

        //series with highest intensity is taken to be be the right one
        if(bestZ > 0 and bestSeries.size() >= minSeriesSize[bestZ] ) {
            clusterNumber++;
            int parentPeakPos=pos;
            for(unsigned int j=0; j<bestSeries.size();j++) {
                int brother_pos =bestSeries[j];
                if(bestZ > 1 and mz[brother_pos] < mz[parentPeakPos]
                        and intensity[brother_pos] < intensity[parentPeakPos]
                        and intensity[brother_pos] > intensity[parentPeakPos]*0.25)
                        parentPeakPos=brother_pos;
                chargeStates[brother_pos]=bestZ;
                peakClusters[brother_pos]=clusterNumber;
             }

           //if ( i < 50 ) cerr << "c12parent: " << mz[parentPeakPos] << endl;
            peakClusters[parentPeakPos]=clusterNumber;
            parentPeaks[parentPeakPos]=bestZ;


            //cerr << "z-series: " <<  mz[pos] << endl;
            //cerr << "parentPeak=" << mz[parentPeakPos] << " " << bestZ << endl;
        }
    }
    return parentPeaks;
}

Scan* Scan::getLastFullScan(int historySize)
{
	if (!this->sample) return 0;
    int scanNum = this->scannum;
    for(int i = scanNum; i > (scanNum - historySize); i--) {
        Scan* lscan = this->sample->getScan(i);
        if (!lscan or lscan->mslevel > 1) continue;
		return lscan; // found ms1 scan, all is good
	}
	return 0;
}

void Scan::recalculatePrecursorMz(float ppm)
{
    if (mslevel != 2)
        return;
    
    Scan* fullScan = getLastFullScan(50);
    if (!fullScan)
        return;
    
    MassCutoff* massCutoff = new MassCutoff();
    massCutoff->setMassCutoffAndType(ppm, "ppm");

    //find highest intensity precursor for this ms2 scan
    //increase the error range till a precursor is found
    for (int i : {1, 2, 3, 4, 5}) {
        massCutoff->setMassCutoff(ppm * i);
        int pos = fullScan->findHighestIntensityPos(this->precursorMz, massCutoff);
        if (pos > 0 && pos < fullScan->nobs()) {
            this->precursorMz = fullScan->mz[pos];
            break;
        }
    }
}

vector<mzPoint> Scan::getIsolatedRegion(float isolationWindowAmu)
{
	vector<mzPoint> isolatedSegment;
	if(! this->sample) return isolatedSegment;

	//find last ms1 scan or get out
	Scan* lastFullScan = this->getLastFullScan();
	if (!lastFullScan) return isolatedSegment;

	//no precursor information
	if (this->precursorMz <= 0) return isolatedSegment;

	//extract isolated region 
	float minMz = this->precursorMz - (isolationWindowAmu / 2.0f);
	float maxMz = this->precursorMz + (isolationWindowAmu / 2.0f);
	
	for(int i = 0; i < lastFullScan->nobs(); i++ ) {
		if (lastFullScan->mz[i] < minMz) continue;
		if (lastFullScan->mz[i] > maxMz) break;
		isolatedSegment.push_back(mzPoint(lastFullScan->rt,
                                          lastFullScan->intensity[i],
                                          lastFullScan->mz[i]));
	}
	return isolatedSegment;
}

double Scan::getPrecursorPurity(float ppm)
{
    if (this->precursorMz <= 0 ) return 0;
    if (this->sample == 0 ) return 0;

    //extract isolated window
    vector<mzPoint> isolatedSegment = this->getIsolatedRegion(_isolationWindow);
    if (isolatedSegment.size() == 0) return 0;

    //get last full scan
    Scan* lastFullScan = this->getLastFullScan();
    if (!lastFullScan) return 0;

    //locate intensity of isolated mass
    MassCutoff* massCutoff = new MassCutoff();
    massCutoff->setMassCutoffAndType(ppm, "ppm");
    int pos = lastFullScan->findHighestIntensityPos(this->precursorMz, massCutoff);
    if (pos < 0) return 0;
    double targetInt = lastFullScan->intensity[pos];

    //calculate total intensity in isolated segment
    double totalInt = 0;
    for (mzPoint& point: isolatedSegment)
        totalInt += point.y;

    delete massCutoff;
    if (totalInt > 0) {
        return (targetInt / totalInt);
    } else {
        return 0;
    }
}

void Scan::setIsolationWindow(const float window)
{
    _isolationWindow = window;
    if (_isolationWindow > 1.0f) {
        // this assumption is okay, right?
        _msType = MsType::DIA;
        _swathWindowMin = precursorMz - (isolationWindow() / 2.0f);
        _swathWindowMax = precursorMz + (isolationWindow() / 2.0f);
    } else {
        _msType = MsType::DDA;
    }
}

Scan::MsType Scan::msType()
{
    return _msType;
}

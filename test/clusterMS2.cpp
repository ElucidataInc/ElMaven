#include "mzSample.h"
#include <iomanip>

const int maxFragments=25;
const float minFractionalIntenisty=0.05;
const double minScore=0.2;
const double precursorPPM=10;
const double productAmuToll=0.35;

double spearmanRankCorrelation(const vector<int>& X) {
	double d2=0; int n=X.size();
	for(int i=0; i<X.size();i++ ) {	
	//	cerr << X[i] << " ";
		if (X[i] == -1 ) d2 += (i-n)*(i-n);	//mising values set to average distance
		else 		 d2 += (i-X[i])*(i-X[i]);
	}
	//double p = 1.00-(6.0*d2)/(n*((n*n)-1));
	//cerr << " cor=" << p << endl;
	return 1.00-(6.0*d2)/(n*((n*n)-1));
}

vector<int> ordered( vector<float>& values) {
	vector<int> indices(values.size(),0);
	for(int i=0; i<values.size();i++) { indices[i]=i;}
	std::sort(begin(indices),end(indices),[&](int a, int b) {return values[a]>values[b];});
	return indices;
}

class Fragment { 
	public: 
		double precursorMz;	
		int polarity;
		vector<float> mzs;
		vector<float> intensities;
		vector<int>obscount;
		vector<Fragment*> brothers;
		Fragment* consensus;

	Fragment(float Mz ) { precursorMz = Mz; }

	//copy constructor
	Fragment( Fragment* other) { 
		this->precursorMz = other->precursorMz;
		this->polarity = other->polarity;
		this->mzs = other->mzs;
		this->intensities = other->intensities;
		this->obscount = other->obscount;
		this->consensus = other->consensus;
	}

	Fragment(Scan* scan) {
		this->precursorMz = scan->precursorMz;
		this->polarity = scan->getPolarity();
		vector<pair<float,float> >mzarray = scan->getTopPeaks(minFractionalIntenisty);

		for(unsigned int j=0; j<mzarray.size() && j < maxFragments; j++ ) {
			this->mzs.push_back(mzarray[j].second);
			this->intensities.push_back(mzarray[j].first);
		}
		this->obscount = vector<int>( this->mzs.size(), 1);
		this->consensus =NULL;
	}

	void printFragment() { 
		cerr << setprecision(10) << precursorMz << " " << polarity << endl; 

		cerr << "\t";
		for(unsigned int i=0; i<mzs.size(); i++ ) { 
			cerr << setprecision(3) << mzs[i];
			//cerr  << "[" << (int) intensities[i] << "] ";
			cerr  << "[" << obscount[i] << "] ";
		}
		cerr << endl;

		for(unsigned int i=0; i<brothers.size(); i++) {
			cerr << "\t";
			for(unsigned int j=0; j<brothers[i]->mzs.size(); j++ ) { 
				cerr << setprecision(3) << brothers[i]->mzs[j];
				//ycerr << "[" << (int) brothers[i]->intensities[j] << "] ";
				cerr  << "[" << brothers[i]->obscount[j] << "] ";
			}
			cerr << endl;
		}
	
}
	void printConsensus() { 
		if(this->consensus == NULL) return; 

		cout << "Name:" << "fragmentx" << endl;
		cout << "PrecursorMZ:" << setprecision(10) << consensus->precursorMz << endl;
		cout << "Status: Normal" << endl;
		cout << "NumPeaks:" << consensus->mzs.size() << endl;
		for(unsigned int i=0; i<consensus->mzs.size(); i++ ) { 
			cout << setprecision(5) << consensus->mzs[i] << "\t";
			cout << consensus->intensities[i] << "\t";
			cout << consensus->obscount[i] << endl;
		}
		cout << endl;
	}


	double compareToFragment(Fragment* other) { 
		if (mzs.size() < 2 or other->mzs.size() < 2) return 0; 	
		//which one is smaller;
		Fragment* a = this;
		Fragment* b =  other;
		if (b->mzs.size() < a->mzs.size() ) { a=other; b=this; }
		return spearmanRankCorrelation( compareRanks(a,b) );
	}

	static vector<int> compareRanks( Fragment* a, Fragment* b) { 
		vector<int> ranks (a->mzs.size(),-1);	//missing value == -1
		for(unsigned int i=0; i<a->mzs.size(); i++ ) {
		    for( unsigned int j=0; j < b->mzs.size(); j++ ) {
		    	if (abs(a->mzs[i]-b->mzs[j])<productAmuToll) { ranks[i] = j; break; }
		    }
		}
		return ranks;
	}

	void mergeFragment(Fragment* b) { 
		brothers.push_back(b);
	}

	void buildConsensus() { 
		Fragment* a = new Fragment(this);  //make a copy

		for(unsigned int i=0; i<brothers.size(); i++) {
			Fragment* brother = brothers[i];
			vector<int> ranks = compareRanks(brother,a);	//location 

			for(unsigned int j=0; j<ranks.size(); j++ ) {
				int   posA = ranks[j];	
				float mzB = brother->mzs[j];
				float intB = brother->intensities[j];
				if (posA >= 0)  {
					a->intensities[ posA ] += intB;
					a->obscount[ posA ] += 1;
				} else if ( posA == -1 ) {
					a->mzs.push_back(mzB);
					a->intensities.push_back(intB);
					a->obscount.push_back(1);
				}
			}
			//cerr << "cons" << i  << " "; a->printFragment();
		}

		//average values
		int N = brothers.size();
		for(unsigned int i=0; i<a->intensities.size(); i++) { a->intensities[i] /= N; }
		a->sortByIntensity();

		float maxValue = a->intensities[0];
		for(unsigned int i=0; i<a->intensities.size(); i++) { a->intensities[i] = a->intensities[i]/maxValue*100; }
		this->consensus = a; 
		//cerr << "cons" ; a->printFragment();
	}

        void sortByIntensity() { 
		vector<int>order = ordered(this->intensities);
		vector<float> a(mzs.size());
		vector<float> b(intensities.size());
		vector<int> c(obscount.size());
		for(unsigned int i=0; i<order.size(); i++) {
			b[i] = intensities[order[i]];
			a[i] = mzs[order[i]];
			c[i] = obscount[order[i]];
			
		};
		mzs = a;
		intensities = b;
		obscount = c;
	}	

	void buildConsensusAvg() { 
		map<float,double> mz_intensity_map;
		map<float,double> mz_bin_map;
		map<float,int> mz_count;

		vector<Fragment*> fragmentList = brothers;
		fragmentList.push_back(this);

		for(unsigned int i=0; i<fragmentList.size(); i++) {
			Fragment* brother = fragmentList[i];
			for(unsigned int j=0; j < brother->mzs.size(); j++) {
				float bin = (round(brother->mzs[j]+0.5)*10)/10;
				mz_intensity_map[bin] += ((double) brother->intensities[j]);
				mz_bin_map[bin] += ((double)(brother->intensities[j])*(brother->mzs[j]));
				mz_count[bin]++;
			}
		}
		map<float,double>::iterator itr;
		for(itr = mz_intensity_map.begin(); itr != mz_intensity_map.end(); ++itr ) {
			float bin = (*itr).first;
			double totalIntensity=(*itr).second;
			double avgMz =  mz_bin_map[bin] / totalIntensity;
			cerr << "\t" << setprecision(3) << avgMz << " " << totalIntensity/mz_count[bin] << endl;
			//avgScan->mz.push_back((float)avgMz);
			//avgScan->intensity.push_back((float) totalIntensity / mz_count[bin]);
		}
	}


	static bool compPrecursorMz(const Fragment* a, const Fragment* b) { return a->precursorMz<b->precursorMz; }
        bool operator<(const Fragment* b) const{ return this->precursorMz < b->precursorMz; }
        bool operator==(const Fragment* b) const{ return abs(this->precursorMz-b->precursorMz)<0.001; }
};

struct compPrecursor {
        bool operator() (const Fragment* a, const Fragment* b) const { return a->precursorMz<b->precursorMz; }
};


multiset<Fragment*,compPrecursor> fragmentsSet;

void printALL() {
    multiset<Fragment*>::iterator itr;
    for(itr = fragmentsSet.begin(); itr != fragmentsSet.end(); itr++) {
	if ((*itr)->brothers.size() > 5 ) {
		(*itr)->buildConsensus();
		//(*itr)->printFragment();
		(*itr)->printConsensus();
		//(*itr)->buildConsensusAvg();
	}
    }
}


//return index of matching fragments;
vector<Fragment*> findNearestFragmentSet(const Fragment* f, float mzmin, float mzmax) { 
    vector<Fragment*>matches;

    multiset<Fragment*>::iterator itr = lower_bound(fragmentsSet.begin(), fragmentsSet.end(), f, Fragment::compPrecursorMz);
    for(; itr != fragmentsSet.end(); itr++) {
	    if ((*itr)->precursorMz > mzmax) break;
		matches.push_back((*itr));
    } 

   return matches;
}

vector<Scan*> getMS2Scans( mzSample* sample) { 
	//get all ms2 scans
	vector<Scan*> ms2scans;
	for(unsigned int i=0; i < sample->scans.size(); i++ ) {
		Scan* scan = sample->scans[i];
		if (scan->mslevel != 2) continue;
		ms2scans.push_back(scan);
	}

	//sort scans by precursorMz
	cerr << "MS2 Scans=" << ms2scans.size() << endl;
	sort(ms2scans.begin(), ms2scans.end(), Scan::compPrecursor);
	return ms2scans;
}


int main(int argc, char *argv[]) {

//vector<Fragment*> fragments;

    for(int i=1; i<argc;i++) {
	    string fileIn(argv[i]);
	    mzSample* sample = new mzSample();
	    sample->loadSample(fileIn.c_str());    //load file

           vector<Scan*>ms2scans = getMS2Scans(sample);
		
           //run time clustering
	   Fragment*  searchf = new Fragment(0.0);

	   for(unsigned int i=0; i < ms2scans.size(); i++ ) {
		Scan* scan = ms2scans[i];
		//if ( scan->precursorMz < 300 || scan->precursorMz > 305 ) continue;
		//cerr << "serching: " << setprecision(5) << scan->precursorMz << endl;

		float mzseek = scan->precursorMz; 
		float range=mzseek/1e6*precursorPPM;
		searchf->precursorMz = mzseek-range;
		vector<Fragment*> matches = findNearestFragmentSet(searchf, mzseek-range, mzseek+range);
		//if (f->mzs.size() < 5) continue; 

		//nothing matches.
		if ( matches.size() == 0 ) { 
			Fragment* f = new Fragment(scan);
  			fragmentsSet.insert(f); //add new fragment
		} 

		//matched precursorMz,, check if fragments are correlated
		if ( matches.size() >  0 ) {
			Fragment* f = new Fragment(scan);
			Fragment* bestmatch=NULL; 
			double bestScore=-1;

			for(int j=0; j < matches.size(); j++ ) {
				double score = matches[j]->compareToFragment(f);
				if(score > bestScore) { 
					bestScore=score; 
					bestmatch=matches[j];
				}
			}

			//cerr << "\t matches=" << matches.size() << " bestscore=" << bestScore << endl;
			if(bestScore>minScore) {
				bestmatch->mergeFragment(f);
				//f->printFragment();
				//bestmatch->printFragment();
			} else {
				fragmentsSet.insert(f);
			}
		}

	  }
	 // cerr << "Size=" << fragments.size() << endl;
	  cerr << "Set Size=" << fragmentsSet.size() << endl;
	
	//cleanup
	 delete(sample);
    }
    printALL();

    /*for(int i=0; i < fragments.size(); i++ ) {
 	fragments[i]->printFragment();
    }*/
}

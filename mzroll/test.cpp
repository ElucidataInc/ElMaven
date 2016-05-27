#include<iostream>
#include<vector>
#include<math.h>

using namespace std;

long nchoosek(int n, int k) {  
	int n_k = n - k; 
	if (k < n_k) { k = n_k; n_k = n - k; } 
	int  nchsk = 1; 
	for ( int i = 1; i <= n_k; i++) { nchsk *= (++k); nchsk /= i; } 
	return nchsk; 
}

int naturalAbundanceCorrection(int nC, vector<double>& M) {
	//inputs
	int n = nC;		//number of carbonms
	int nr_13C=n-1; //number of labeled carbons
			
	//uncorrected values
	vector<double>C(n,0); // output vector with corrected values

	for(int k=0; k < nr_13C-1; k++ ) { 
		double contamination=0;
		
		for(int i=0; i<k; i++ ) {
			contamination += pow( 0.011,k-i) * pow(0.989, n-k) * nchoosek(n-i,k-i) * C[i];
		}

		C[k] = (M[k]-contamination) / pow(0.989,n-k);
		if(C[k] < 1e-4) C[k]=0;
	}

	for(int i=0; i<n; i++ ) cerr << M[i] << "\t\t" << C[i] << endl;
	return 0;
}

int labeledNutrient() {
	//inputs
	int n = 6;		//number of carbonms
	int nr_13C=n-1; //number of labeled carbons
	double A=0.99;	//purity of labeled nutrient

	vector<double>M(n+1,0);
	M[0]=0.187158;
	M[1]=1.24898e-2;
	M[2]=3.47407e-4;
	M[3]=2.06750e-5;
	M[4]=1.15276e-3;
	M[5]=4.56475e-2;
	M[6]=0.753184;
			
	//uncorrected values
	vector<double>C(n,0); // output vector with corrected values

	for(int k=0; k < nr_13C-1; k++ ) { 
		double contamination=0;
		double labeledcontamination=0;
		
		for(int i=0; i<k; i++ ) {
			contamination += pow( 0.011,k-i) * pow(0.989, n-k) * nchoosek(n-i,k-i) * C[i];
		}

		for(int i=k+1; i<n; i++ ) {
			labeledcontamination += pow(1-A,i-k) * pow(A, k) * nchoosek(i,i-k) * C[i];
		}

		C[k] = (M[k]-contamination-labeledcontamination) / (pow(0.989,n-k) * pow(A,k));
		if(C[k] < 1e-4) C[k]=0;
	}

	for(int i=0; i<n; i++ ) cerr << M[i] << "\t\t" << C[i] << endl;
	//end
	return 0;
}

int main() {

	//inputs
	int n = 12;		//number of carbonms
	vector<double>M(n+1,0);
	M[0]=0.8757; M[1]=1.16878e-1; M[2]=7.14977e-3; M[3]=2.65074e-4;
	M[4]=6.63355e-6;  M[5]=1.18049e-7;
			
	
	//labeledNutrient();
	naturalAbundanceCorrection(n,M);
	
}

#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "statistics.h"
#include "SavGolSmoother.h"

#ifdef MINGW
#include <io.h> 
#endif  

#define POW2(x) ((x)*(x))
#define MAX(x,y) ((x)>(y)) ? (x) : (y)
#define MIN(x,y) ((x)<(y)) ? (x) : (y)
#define SIGN(x) ((x < 0) ? -1 : 1)
#define ISINF(x) (!isnan(x) && (x >= FLT_MAX || x <= FLT_MIN))


#define C13_MASS	 13.0033548378
#define HMASS  		 1.007825032

#if defined _WIN32 && !defined __CYGWIN__
   /* Use Windows separators on all _WIN32 defining
	*       environments, except Cygwin. */
#  define DIR_SEPARATOR_CHAR		'\\'
#  define DIR_SEPARATOR_STR		"\\"
#  define PATH_SEPARATOR_CHAR		';'
#  define PATH_SEPARATOR_STR		";"
#endif
#ifndef DIR_SEPARATOR_CHAR
   /* Assume that not having this is an indicator that all
	*       are missing. */
#  define DIR_SEPARATOR_CHAR		'/'
#  define DIR_SEPARATOR_STR		"/"
#  define PATH_SEPARATOR_CHAR		':'
#  define PATH_SEPARATOR_STR		":"
#endif /* !DIR_SEPARATOR_CHAR */


using namespace std;

namespace mzUtils {

inline float round(float val) {
	if (val - long(val) >= 0.5 ) return (ceil(val));
	return (floor(val));
}

//spline fitting
float spline_eval( int n, float *x, float *f, float *b, float *c, float *d, float t );
void tridiagonal ( int n, float *c, float *a, float *b, float *r );
void cubic_nak   ( int n, float *x, float *f, float *b, float *c, float *d );

/*smoothing functions */
void gaussian1d_smoothing (int ns, int nsr, float *data);
void smoothAverage(float *y, float* s, int points, int n);
void conv(int lx, int ifx, float *x, int ly, int ify, float *y, int lz, int ifz, float *z);	//convolutio

/*statistical functions*/
float ttest(StatisticsVector<float>& groupA, StatisticsVector<float>& groupB );
float median(vector <float> y);
float median(float* y, int n);
float kth_smallest(float a[], int n, int k);
float torben_median(const vector<float> &m);
vector<float> quantileDistribution( vector<float> y );
int countBelow(vector<float>& y, float ymax);
float correlation(const vector<float>&a, const vector<float>&b);
void gaussFit(const vector<float>& yobs, float* sigmal, float* R2);
inline unsigned long factorial(int n);
long nchoosek(int n, int k); 
double beta(double x, double y);
double gamma(double z);
double betaPDF(double x, double a, double b);
double pertPDF(double x, double min, double mode, double max);

vector<double> naturalAbundanceCorrection(int nC, vector<double>& M);


/* string functions */
int string2integer(const std::string& s);
float string2float(const std::string& s);
string integer2string(int x);
string float2string(float f, int p);
string makeLowerCase(string &s);
void split(const string& s, char c, vector<string>& v);
char *mystrcasestr(const char *s1, const char *s2);
int strincmp(char *s1, char* s2,int n);
string cleanFilename(const string& s);  
string substituteInQuotedString(const string& s, const string& chars, const string& substitutions );

/* rounding and ppm functions */
float ppmDist(const float mz1, const float mz2);
double ppmDist(const double mz1, const double mz2);
float ppmround(const float mz1, const float resolution);
bool withinXppm( float mz1, float mz2, int ppmWindow );

/* file system functions */
bool fileExists(string strFilename);
int createDir(const char* path);
int isFile(const char* path);
int isDir(const char* path);

//fractional overlap between two line segments
inline float checkOverlap(float a, float b, float c, float d) { 
	 if( (a<c && b<c) || (d<a && d<b) )    return(0.0);  //no overalp
     if( (c<=a && b<=d) || (a<=c && d<=b) ) return(1.0); //100% overlap
	 if  (c<=a) return(1/abs((c-b)/(d-a)));
	 if  (a<=c) return(abs((c-b)/(d-a)));
	 return 0.0;
}

//print vector
template <typename T>
void shuffle(vector<T>& my_vector) {
    int N = my_vector.size()-1; if (N <= 0) return;
	for (unsigned int i = 0; i < my_vector.size(); i++ ) { 
		int j = ((double) rand()/RAND_MAX)*N;
		if (i == j) continue;
		T tmp = my_vector[i];
		my_vector[i] = my_vector[j];
		my_vector[j] = tmp; 
	}
}


//print vector
template <typename T>
void printF(vector<T>& my_vector) {
       for (unsigned int i = 0; i < my_vector.size(); i++ ) { 
			cerr << setprecision(6) << my_vector[i] << " ";
		}
	   cerr << endl;
}


//deallocated elements fo array from memory
template <typename T>
void delete_all(vector<T>& my_vector) {
		if (my_vector.empty()) return;
        for (unsigned int i = 0; i < my_vector.size(); i++ ) { 
              if (  my_vector[i] != NULL ) { 
					  try {
						  delete(my_vector[i]);  
						  my_vector[i] = NULL; 
							  
					  }
					  catch(...) {
						  cerr << "delete_all() sigfaulting.. ";
					  }
			  }
        }
        my_vector.clear(); 
}  

//deallocated elements fo array from memory
template <typename T>
inline void delete_all(deque<T>& my_vector) {
		if (my_vector.empty()) return;
        for (unsigned int i = 0; i < my_vector.size(); i++ ) { 
              if (  my_vector[i] != NULL ) { delete(my_vector[i]);  my_vector[i] = NULL; }
        }
        my_vector.clear(); 
}  

}
//deallocated single element
template <typename T>
void delete_one(vector<T> & my_vector, unsigned int idx) {
		if ( my_vector.size() == 0 ) return;
        if ( idx < 0 || idx > my_vector.size()-1) return; 
        if ( my_vector[idx] != NULL ) delete(my_vector[idx]); my_vector[idx] = NULL;
        my_vector.erase( my_vector.begin()+idx );
}

//deallocated single element
template <typename T>
void delete_one(deque<T> & my_vector, unsigned int idx) {
		if ( my_vector.size() == 0 ) return;
        if ( idx > my_vector.size()-1) return; 
        if ( my_vector[idx] != NULL ) delete(my_vector[idx]); my_vector[idx] = NULL;
        my_vector.erase( my_vector.begin()+idx );
}


//resize vector in order to free up memory
template<typename vector>
void shrink_vector(vector& v) {
    vector(v.begin(), v.end()).swap(v);

}


#endif

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
#include <stdexcept>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include "statistics.h"
#include "SavGolSmoother.h"

#ifdef ZLIB
#include <zlib.h>
#endif


#ifdef MINGW
#include <io.h>
#endif

#define POW2(x) ((x)*(x))
#define MAX(x,y) ((x)>(y)) ? (x) : (y)
#define MIN(x,y) ((x)<(y)) ? (x) : (y)
#define SIGN(x) ((x < 0) ? -1 : 1)
#define ISINF(x) (!isnan(x) && (x >= FLT_MAX || x <= FLT_MIN))
#define FLOATROUND(x,sd)(int(x*sd+0.5)/sd)

#define C13_MASS	 13.0033548378
#define HMASS  		 1.007825032
#define EMASS 		 0.00054857971 //(6.022141×10^23 ⋅ 9.10938×10^-28 )
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

/**
 * [round description]
 * @method round
 * @param  val   [description]
 * @return       [description]
 */
inline float round(float val) {
	if (val - long(val) >= 0.5 ) return (ceil(val));
	return (floor(val));
}

/**
 * [spline fitting]
 * @method spline_eval
 * @param  n           [description]
 * @param  x           [description]
 * @param  f           [description]
 * @param  b           [description]
 * @param  c           [description]
 * @param  d           [description]
 * @param  t           [description]
 * @return             [description]
 */
float spline_eval( int n, float *x, float *f, float *b, float *c, float *d, float t );

/**
 * [tridiagonal  description]
 * @method tridiagonal
 * @param  n           [description]
 * @param  c           [description]
 * @param  a           [description]
 * @param  b           [description]
 * @param  r           [description]
 */
void tridiagonal ( int n, float *c, float *a, float *b, float *r );

/**
 * [cubic_nak    description]
 * @method cubic_nak
 * @param  n         [description]
 * @param  x         [description]
 * @param  f         [description]
 * @param  b         [description]
 * @param  c         [description]
 * @param  d         [description]
 */
void cubic_nak   ( int n, float *x, float *f, float *b, float *c, float *d );

/*smoothing functions */

/**
 * [gaussian1d_smoothing  description]
 * @method gaussian1d_smoothing
 * @param  ns                   [description]
 * @param  nsr                  [description]
 * @param  data                 [description]
 */
void gaussian1d_smoothing (int ns, int nsr, float *data);

/**
 * [smoothAverage description]
 * @method smoothAverage
 * @param  y             [description]
 * @param  s             [description]
 * @param  points        [description]
 * @param  n             [description]
 */
void smoothAverage(float *y, float* s, int points, int n);

/**
 * [conv description]
 * @method conv
 * @param  lx   [description]
 * @param  ifx  [description]
 * @param  x    [description]
 * @param  ly   [description]
 * @param  ify  [description]
 * @param  y    [description]
 * @param  lz   [description]
 * @param  ifz  [description]
 * @param  z    [description]
 */
void conv(int lx, int ifx, float *x, int ly, int ify, float *y, int lz, int ifz, float *z);	//convolutio

/*statistical functions*/
/**
 * [ttest description]
 * @method ttest
 * @param  groupA [description]
 * @param  groupB [description]
 * @return        [description]
 */
float ttest(StatisticsVector<float>& groupA, StatisticsVector<float>& groupB );

/**
 * [median description]
 * @method median
 * @param  y      [description]
 * @return        [description]
 */
float median(vector <float> y);

/**
 * [median description]
 * @method median
 * @param  y      [description]
 * @param  n      [description]
 * @return        [description]
 */
float median(float* y, int n);

/**
 * [kth_smallest description]
 * @method kth_smallest
 * @param  a            [description]
 * @param  n            [description]
 * @param  k            [description]
 * @return              [description]
 */
float kth_smallest(float a[], int n, int k);

/**
 * [torben_median description]
 * @method torben_median
 * @param  m             [description]
 * @return               [description]
 */
float torben_median(const vector<float> &m);


vector<float> quantileDistribution(vector<float> y);

/**
 * [countBelow description]
 * @method countBelow
 * @param  y          [description]
 * @param  ymax       [description]
 * @return            [description]
 */
int countBelow(vector<float>& y, float ymax);

/**
 * [correlation description]
 * @method correlation
 * @param  a           [description]
 * @param  b           [description]
 * @return             [description]
 */
float correlation(const vector<float>&a, const vector<float>&b);

/**
 * [gaussFit description]
 * @method gaussFit
 * @param  yobs     [description]
 * @param  sigmal   [description]
 * @param  R2       [description]
 */
void gaussFit(const vector<float>& yobs, float* sigmal, float* R2);

/**
 * [factorial description]
 * @method factorial
 * @param  n         [description]
 * @return [description]
 */
inline unsigned long factorial(int n);

/**
 * [nchoosek description]
 * @method nchoosek
 * @param  n        [description]
 * @param  k        [description]
 * @return [description]
 */
long nchoosek(int n, int k);

/**
 * [beta description]
 * @method beta
 * @param  x    [description]
 * @param  y    [description]
 * @return [description]
 */
double beta(double x, double y);

/**
 * [gamma description]
 * @method gamma
 * @param  z     [description]
 * @return [description]
 */
double gamma(double z);

/**
 * [betaPDF description]
 * @method betaPDF
 * @param  x       [description]
 * @param  a       [description]
 * @param  b       [description]
 * @return [description]
 */
double betaPDF(double x, double a, double b);

/**
 * [pertPDF description]
 * @method pertPDF
 * @param  x       [description]
 * @param  min     [description]
 * @param  mode    [description]
 * @param  max     [description]
 * @return [description]
 */
double pertPDF(double x, double min, double mode, double max);

/**
 *
 */
vector<double> naturalAbundanceCorrection(int nC, vector<double>& M);


/* string functions */
/**
 * [string2integer description]
 * @method string2integer
 * @param  s              [description]
 * @return [description]
 */
int string2integer(const std::string& s);

/**
 * [string2float description]
 * @method string2float
 * @param  s            [description]
 * @return [description]
 */
float string2float(const std::string& s);

/**
 * [integer2string description]
 * @method integer2string
 * @param  x              [description]
 * @return [description]
 */
string integer2string(int x);

/**
 * [float2string description]
 * @method float2string
 * @param  f            [description]
 * @param  p            [description]
 * @return [description]
 */
string float2string(float f, int p);

/**
 * [makeLowerCase description]
 * @method makeLowerCase
 * @param  s             [description]
 * @return [description]
 */
string makeLowerCase(string &s);

/**
 * [split description]
 * @method split
 * @param  s     [description]
 * @param  c     [description]
 * @param  v     [description]
 */
void split(const string& s, char c, vector<string>& v);

/**
 * [mystrcasestr description]
 * @method mystrcasestr
 * @param  s1           [description]
 * @param  s2           [description]
 * @return [description]
 */
char *mystrcasestr(const char *s1, const char *s2);

/**
 * [strincmp description]
 * @method strincmp
 * @param  s1       [description]
 * @param  s2       [description]
 * @param  n        [description]
 * @return [description]
 */
int strincmp(char *s1, char* s2,int n);

/**
 * [cleanFilename description]
 * @method cleanFilename
 * @param  s             [description]
 * @return [description]
 */
string cleanFilename(const string& s);

/**
 * [substituteInQuotedString description]
 * @method substituteInQuotedString
 * @param  s                        [description]
 * @param  chars                    [description]
 * @param  substitutions            [description]
 * @return [description]
 */
string substituteInQuotedString(const string& s, const string& chars, const string& substitutions );

/**
 * [gzipInflate description]
 * @method gzipInflate
 * @param  compressedBytes   [description]
 * @param  uncompressedBytes [description]
 * @return [description]
 */
bool gzipInflate( const std::string& compressedBytes, std::string& uncompressedBytes );

/**
 * [decompress_string description]
 * @method decompress_string
 * @param  str               [description]
 * @return [description]
 */
std::string decompress_string(const std::string& str);

/* rounding and ppm functions */
/**
 * [ppmDist description]
 * @method ppmDist
 * @param  mz1     [description]
 * @param  mz2     [description]
 * @return [description]
 */
float ppmDist(const float mz1, const float mz2);

/**
 * [ppmDist description]
 * @method ppmDist
 * @param  mz1     [description]
 * @param  mz2     [description]
 * @return [description]
 */
double ppmDist(const double mz1, const double mz2);

/**
 * [ppmround description]
 * @method ppmround
 * @param  mz1        [description]
 * @param  resolution [description]
 * @return [description]
 */
float ppmround(const float mz1, const float resolution);

/**
 * [withinXppm description]
 * @method withinXppm
 * @param  mz1        [description]
 * @param  mz2        [description]
 * @param  ppmWindow  [description]
 * @return [description]
 */
bool withinXppm( float mz1, float mz2, int ppmWindow );

/* file system functions */
/**
 * [fileExists description]
 * @method fileExists
 * @param  strFilename [description]
 * @return [description]
 */
bool fileExists(string strFilename);

/**
 * [createDir description]
 * @method createDir
 * @param  path      [description]
 * @return [description]
 */
int createDir(const char* path);

/**
 * [isFile description]
 * @method isFile
 * @param  path   [description]
 * @return [description]
 */
int isFile(const char* path);

/**
 * [isDir description]
 * @method isDir
 * @param  path  [description]
 * @return [description]
 */
int isDir(const char* path);


/**
 * [fractional overlap between two line segments]
 * @method checkOverlap
 * @param  a            [description]
 * @param  b            [description]
 * @param  c            [description]
 * @param  d            [description]
 * @return [description]
 */
inline float checkOverlap(float a, float b, float c, float d) {
	 if( (a<c && b<c) || (d<a && d<b) )    return(0.0);  //no overalp
     if( (c<=a && b<=d) || (a<=c && d<=b) ) return(1.0); //100% overlap
	 if  (c<=a) return(1/abs((c-b)/(d-a)));
	 if  (a<=c) return(abs((c-b)/(d-a)));
	 return 0.0;
}

//print vector
template <typename T>

/**
 * [shuffle description]
 * @method shuffle
 * @param  my_vector [description]
 */
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


template <typename T>
/**
 * [print vector]
 * @method printF
 * @param  my_vector [description]
 */
void printF(vector<T>& my_vector) {
       for (unsigned int i = 0; i < my_vector.size(); i++ ) {
			cerr << setprecision(6) << my_vector[i] << " ";
		}
	   cerr << endl;
}


template <typename T>
/**
 * [delete_all deallocated elements fo array from memory]
 * @method delete_all
 * @param  my_vector  [description]
 */
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


template <typename T>
/**
 * [deallocated elements fo array from memory]
 * @method delete_all
 * @param  my_vector  [description]
 */
inline void delete_all(deque<T>& my_vector) {
		if (my_vector.empty()) return;
        for (unsigned int i = 0; i < my_vector.size(); i++ ) {
              if (  my_vector[i] != NULL ) { delete(my_vector[i]);  my_vector[i] = NULL; }
        }
        my_vector.clear();
}

}


template <typename T>
/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  [description]
 * @param  idx        [description]
 */
void delete_one(vector<T> & my_vector, unsigned int idx) {
		if ( my_vector.size() == 0 ) return;
        if ( idx < 0 || idx > my_vector.size()-1) return;
        if ( my_vector[idx] != NULL ) delete(my_vector[idx]); my_vector[idx] = NULL;
        my_vector.erase( my_vector.begin()+idx );
}


template <typename T>
/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  [description]
 * @param  idx        [description]
 */
void delete_one(deque<T> & my_vector, unsigned int idx) {
		if ( my_vector.size() == 0 ) return;
        if ( idx > my_vector.size()-1) return;
        if ( my_vector[idx] != NULL ) delete(my_vector[idx]); my_vector[idx] = NULL;
        my_vector.erase( my_vector.begin()+idx );
}


template<typename vector>
/**
 * [resize vector in order to free up memory]
 * @method shrink_vector
 * @param  v             [description]
 */
void shrink_vector(vector& v) {
    vector(v.begin(), v.end()).swap(v);
}


#endif

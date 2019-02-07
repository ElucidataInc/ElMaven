#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <fcntl.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <cmath>
#include <deque>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "SavGolSmoother.h"
#include "csvparser.h"
#include "statistics.h"
#include "masscutofftype.h"

#ifdef ZLIB
#include <zlib.h>
#endif

#ifdef MINGW
#include <io.h>
#endif

#define POW2(x) ((x) * (x))
#define MAX(x, y) ((x) > (y)) ? (x) : (y)
#define MIN(x, y) ((x) < (y)) ? (x) : (y)
#define SIGN(x) ((x < 0) ? -1 : 1)
#define ISINF(x) (!isnan(x) && (x >= FLT_MAX || x <= FLT_MIN))
#define FLOATROUND(x, sd) (int(x * sd + 0.5) / sd)

#if defined _WIN32 && !defined __CYGWIN__
/* Use Windows separators on all _WIN32 defining
 *       environments, except Cygwin. */
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR "\\"
#define PATH_SEPARATOR_CHAR ';'
#define PATH_SEPARATOR_STR ";"
#endif
#ifndef DIR_SEPARATOR_CHAR
/* Assume that not having this is an indicator that all
 *       are missing. */
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"
#define PATH_SEPARATOR_CHAR ':'
#define PATH_SEPARATOR_STR ":"
#endif /* !DIR_SEPARATOR_CHAR */

using namespace std;

namespace mzUtils {

    /**
     * [round ]
     * @method round
     * @param  val   []
     * @return       []
     */
    inline float round(float val) {
        if (val - long(val) >= 0.5) return (ceil(val));
        return (floor(val));
    }

    bool strcasecmp_withNumbers(const std::string& a, const std::string& b );

    void removeSpecialcharFromStartEnd(vector<string>& fields);
    /**
     * [spline fitting]
     * @method spline_eval
     * @param  n           []
     * @param  x           []
     * @param  f           []
     * @param  b           []
     * @param  c           []
     * @param  d           []
     * @param  t           []
     * @return             []
     */
    float spline_eval(int n, float* x, float* f, float* b, float* c, float* d,
            float t);

    /**
     * [tridiagonal  ]
     * @method tridiagonal
     * @param  n           []
     * @param  c           []
     * @param  a           []
     * @param  b           []
     * @param  r           []
     */
    void tridiagonal(int n, float* c, float* a, float* b, float* r);

    /**
     * [cubic_nak    ]
     * @method cubic_nak
     * @param  n         []
     * @param  x         []
     * @param  f         []
     * @param  b         []
     * @param  c         []
     * @param  d         []
     */
    void cubic_nak(int n, float* x, float* f, float* b, float* c, float* d);

    /*smoothing functions */

    /**
     * [gaussian1d_smoothing  ]
     * @method gaussian1d_smoothing
     * @param  ns                   []
     * @param  nsr                  []
     * @param  data                 []
     */
    void gaussian1d_smoothing(int ns, int nsr, float* data);

    /**
     * [smoothAverage ]
     * @method smoothAverage
     * @param  y             []
     * @param  s             []
     * @param  points        []
     * @param  n             []
     */
    void smoothAverage(float* y, float* s, int points, int n);

    /**
     * [conv ]
     * @method conv
     * @param  lx   []
     * @param  ifx  []
     * @param  x    []
     * @param  ly   []
     * @param  ify  []
     * @param  y    []
     * @param  lz   []
     * @param  ifz  []
     * @param  z    []
     */
    void conv(int lx, int ifx, float* x, int ly, int ify, float* y, int lz, int ifz,
            float* z);  // convolutio

    /*statistical functions*/
    /**
     * [ttest ]
     * @method ttest
     * @param  groupA []
     * @param  groupB []
     * @return        []
     */
    float ttest(StatisticsVector<float>& groupA, StatisticsVector<float>& groupB);

    /**
     * [median ]
     * @method median
     * @param  y      []
     * @return        []
     */
    float median(vector<float> y);

    /**
     * [median ]
     * @method median
     * @param  y      []
     * @param  n      []
     * @return        []
     */
    float median(float* y, int n);

    /**
     * [kth_smallest ]
     * @method kth_smallest
     * @param  a            []
     * @param  n            []
     * @param  k            []
     * @return              []
     */
    float kth_smallest(float a[], int n, int k);

    /**
     * [torben_median ]
     * @method torben_median
     * @param  m             []
     * @return               []
     */
    float torben_median(const vector<float>& m);

    vector<float> quantileDistribution(vector<float> y);

    /**
     * [countBelow ]
     * @method countBelow
     * @param  y          []
     * @param  ymax       []
     * @return            []
     */
    int countBelow(vector<float>& y, float ymax);

    /**
     * [correlation ]
     * @method correlation
     * @param  a           []
     * @param  b           []
     * @return             []
     */
    float correlation(const vector<float>& a, const vector<float>& b);

    /**
     * [gaussFit ]
     * @method gaussFit
     * @param  yobs     []
     * @param  sigmal   []
     * @param  R2       []
     */
    void gaussFit(const vector<float>& yobs, float* sigmal, float* R2);

    /**
     * [factorial ]
     * @method factorial
     * @param  n         []
     * @return []
     */
    inline unsigned long factorial(int n);

    /**
     * [nchoosek ]
     * @method nchoosek
     * @param  n        []
     * @param  k        []
     * @return []
     */
    long long nchoosek(int n, int k);

    /**
     * [beta ]
     * @method beta
     * @param  x    []
     * @param  y    []
     * @return []
     */
    double beta(double x, double y);

    /**
     * [gamma ]
     * @method gamma
     * @param  z     []
     * @return []
     */
    double gamma(double z);

    /**
     * [betaPDF ]
     * @method betaPDF
     * @param  x       []
     * @param  a       []
     * @param  b       []
     * @return []
     */
    double betaPDF(double x, double a, double b);

    /**
     * [pertPDF ]
     * @method pertPDF
     * @param  x       []
     * @param  min     []
     * @param  mode    []
     * @param  max     []
     * @return []
     */
    double pertPDF(double x, double min, double mode, double max);

    /**
     *
     */
    vector<double> naturalAbundanceCorrection(int nC, vector<double>& M, map<unsigned int, string> carbonIsotopeSpecies);

    /* string functions */
    /**
     * [string2integer ]
     * @method string2integer
     * @param  s              []
     * @return []
     */
    int string2integer(const std::string& s);

    /**
     * [string2float ]
     * @method string2float
     * @param  s            []
     * @return []
     */
    float string2float(const std::string& s);

    /**
     * [integer2string ]
     * @method integer2string
     * @param  x              []
     * @return []
     */
    string integer2string(int x);

    /**
     * [float2string ]
     * @method float2string
     * @param  f            []
     * @param  p            []
     * @return []
     */
    string float2string(float f, int p);

    /**
     * [makeLowerCase ]
     * @method makeLowerCase
     * @param  s             []
     * @return []
     */
    string makeLowerCase(string& s);

    /**
     * [split ]
     * @method split
     * @param  s     []
     * @param  c     []
     * @param  v     []
     */
    void split(const string& s, char c, vector<string>& v);

    void splitNew(const string& s, const string& c, vector<string>& v);

    /**
     * [mystrcasestr ]
     * @method mystrcasestr
     * @param  s1           []
     * @param  s2           []
     * @return []
     */
    char* mystrcasestr(const char* s1, const char* s2);

    /**
     * [strincmp ]
     * @method strincmp
     * @param  s1       []
     * @param  s2       []
     * @param  n        []
     * @return []
     */
    int strincmp(char* s1, char* s2, int n);

    /**
     * [cleanFilename ]
     * @method cleanFilename
     * @param  s             []
     * @return []
     */
    string cleanFilename(const string& s);

    /**
     * [substituteInQuotedString ]
     * @method substituteInQuotedString
     * @param  s                        []
     * @param  chars                    []
     * @param  substitutions            []
     * @return []
     */
    string substituteInQuotedString(const string& s, const string& chars,
            const string& substitutions);

    /**
     * [gzipInflate ]
     * @method gzipInflate
     * @param  compressedBytes   []
     * @param  uncompressedBytes []
     * @return []
     */
    bool gzipInflate(const std::string& compressedBytes,
            std::string& uncompressedBytes);

    /**
     * [decompress_string ]
     * @method decompress_string
     * @param  str               []
     * @return []
     */
    std::string decompress_string(const std::string& str);

    /* rounding and ppm functions */
    /**
     * [ppmDist ]
     * @method ppmDist
     * @param  mz1     []
     * @param  mz2     []
     * @return []
     */
    float massCutoffDist(const float mz1, const float mz2,MassCutoff *massCutoff);

    /**
     * [ppmDist ]
     * @method ppmDist
     * @param  mz1     []
     * @param  mz2     []
     * @return []
     */
    double massCutoffDist(const double mz1, const double mz2,MassCutoff *massCutoff);

    float ppmDist(const float mz1, const float mz2);

    double ppmDist(const double mz1, const double mz2);

    /**
     * [ppmround ]
     * @method ppmround
     * @param  mz1        []
     * @param  resolution []
     * @return []
     */
    float ppmround(const float mz1, const float resolution);

    /**
     * [withinXppm ]
     * @method withinXppm
     * @param  mz1        []
     * @param  mz2        []
     * @param  ppmWindow  []
     * @return []
     */
    bool withinXMassCutoff(float mz1, float mz2, MassCutoff *massCutoff);

    /* file system functions */
    /**
     * [fileExists ]
     * @method fileExists
     * @param  strFilename []
     * @return []
     */
    bool fileExists(string strFilename);

    /**
     * [createDir ]
     * @method createDir
     * @param  path      []
     * @return []
     */
    int createDir(const char* path);

    /**
     * [isFile ]
     * @method isFile
     * @param  path   []
     * @return []
     */
    int isFile(const char* path);

    /**
     * [isDir ]
     * @method isDir
     * @param  path  []
     * @return []
     */
    int isDir(const char* path);

    /**
     * [fractional overlap between two line segments]
     * @method checkOverlap
     * @param  a            []
     * @param  b            []
     * @param  c            []
     * @param  d            []
     * @return []
     */
    inline float checkOverlap(float a, float b, float c, float d) {
        if ((a < c && b < c) || (d < a && d < b)) return (0.0);  // no overalp
        if ((c <= a && b <= d) || (a <= c && d <= b))
            return (1.0);  // 100% overlap
        if (c <= a) return (1 / abs((c - b) / (d - a)));
        if (a <= c) return (abs((c - b) / (d - a)));
        return 0.0;
    }

    // print vector
    template <typename T>

        /**
         * [shuffle ]
         * @method shuffle
         * @param  my_vector []
         */
        void shuffle(vector<T>& my_vector) {
            int N = my_vector.size() - 1;
            if (N <= 0) return;
            for (unsigned int i = 0; i < my_vector.size(); i++) {
                int j = ((double)rand() / RAND_MAX) * N;
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
         * @param  my_vector []
         */
        void printF(vector<T>& my_vector) {
            for (unsigned int i = 0; i < my_vector.size(); i++) {
                cerr << setprecision(6) << my_vector[i] << " ";
            }
            cerr << endl;
        }

    template <typename T>

        /**
         * [delete_all deallocated elements fo array from memory]
         * @method delete_all
         * @param  my_vector  []
         */
        void delete_all(vector<T>& my_vector) {
            if (my_vector.empty()) return;
            for (unsigned int i = 0; i < my_vector.size(); i++) {
                if (my_vector[i] != NULL) {
                    try {
                        delete (my_vector[i]);
                        my_vector[i] = NULL;

                    } catch (...) {
                        cerr << "delete_all() segfaulting.. ";
                    }
                }
            }
            my_vector.clear();

            //	my_vector.clear();
            //	my_vector.shrink_to_fit();
        }

    template <typename T>

        /**
         * [deallocated elements fo array from memory]
         * @method delete_all
         * @param  my_vector  []
         */
        inline void delete_all(deque<T>& my_vector) {
            if (my_vector.empty()) return;
            for (unsigned int i = 0; i < my_vector.size(); i++) {
                if (my_vector[i] != NULL) {
                    try {
                        delete (my_vector[i]);
                        my_vector[i] = NULL;

                    } catch (...) {
                        cerr << "delete_all() segfaulting.. ";
                    }
                }
            }
            my_vector.clear();
            //	my_vector.clear();
            //	my_vector.shrink_to_fit();
        }
}

template <typename T>

/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  []
 * @param  idx        []
 */
void delete_one(vector<T>& my_vector, unsigned int idx) {
    if (my_vector.size() == 0) return;
    if (idx < 0 || idx > my_vector.size() - 1) return;
    if (my_vector[idx] != NULL) delete (my_vector[idx]);
    my_vector[idx] = NULL;
    my_vector.erase(my_vector.begin() + idx);
}

template <typename T>
/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  []
 * @param  idx        []
 */
void delete_one(deque<T>& my_vector, unsigned int idx) {
    if (my_vector.size() == 0) return;
    if (idx > my_vector.size() - 1) return;
    if (my_vector[idx] != NULL) delete (my_vector[idx]);
    my_vector[idx] = NULL;
    my_vector.erase(my_vector.begin() + idx);
}

template <typename T>
bool isBetweenInclusive(T x, T lb, T ub) {
    return (x>=lb && x<=ub) || (x>=ub && x <=lb);
}

// template<typename vector>
// /**
//  * [resize vector in order to free up memory]
//  * @method shrink_vector
//  * @param  v             []
//  */
// void shrink_vector(vector& v) {
// 	vector(v.begin(), v.end()).swap(v);
// }

#endif

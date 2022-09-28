#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

/**
 *  "standardincludes.h" library thas has the correct order
 *  of including "math.h" and "cmath" libraries which is why
 *  it needs to be added before any other standard library
 *  specially "iostream"
 */
#include "standardincludes.h"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <iostream>
#include <random>
#include "statistics.h"

#ifdef ZLIB
#include <zlib.h>
#endif

#ifdef MINGW
#include <io.h>
#endif

#define SQUARE(x) ((x) * (x))
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

class MassCutoff;

using namespace std;

namespace mzUtils
{
    /**
     * [round ]
     * @method round
     * @param  val   []
     * @return       []
     */
    inline float round(float val)
    {
        if (val - long(val) >= 0.5)
            return (ceil(val));
        return (floor(val));
    }

    /**
     *@brief Generates a random integer number in
     * the given range.
     *@param low  Lower inclusive bound of the range.
     *@param high Upper inclusive bound of the range.
     *@return An integer in range of closed lower and
     *closed upper bounds.
     */
    int randInt(int low, int high);

    /**
     *@brief Generates a long Random integer numbers
     * in the given range.
     *@param low  Lower inclusive bound of the range.
     *@param high Upper inclusive bound of the range.
     *@return A long value in range of closed lower
     *and closed upper bounds.
     */
    long randLong(long low, long high);

    /**
     *@brief Generates the Float Random Integer numbers
     * in the given range.
     *@param low  Lower inclusive bound of the range.
     *@param high Upper inclusicve bound of the range.
     *@return A float in range of closed lower and
     *closed upper bounds.
     */
    float randFloat(float low, float high);

    /**
     *@brief Generates Double Random Integer numbers in
     * the given range.
     *@param low  Lower inclusive bound of the range.
     *@param high Upper inclusive bound of the range.
     *@return A double in range of closed lower and
     *closed upper bounds.
     */
    double randDouble(double low, double high);

    /**
     * @brief compareStringsWithNumbers Compares the alphanumeric strings.
     * @param a First string to be compared against another string.
     * @param b Second string.
     * @return A boolean variable indicating if the two strings are equal
     * or not.
     */
    bool compareStringsWithNumbers(const std::string& a, const std::string& b);

    /**
     * @brief removeSpecialcharFromStartEnd Removes the ' "" ' or ' / ' from
     * the begining and end of string if they exist.
     * @param fields  Vector of strings. Special characters from string at
     * each index is removed.
     */
    void removeSpecialCharFromStartEnd(vector<string>& fields);

    /**
      PURPOSE:
      evaluate a cubic spline at a single value of
      the independent variable given the coefficients of
      the cubic spline interpolant (obtained from
      'cubic_nak' or 'cubic_clamped')


       CALLING SEQUENCE:
           y = spline_eval ( n, x, f, b, c, d, t );
       spline_eval ( n, x, f, b, c, d, t );


       INPUTS:
           n        number of interpolating points
           x        array containing interpolating points
           f        array containing the constant terms from
                    the cubic spline (obtained from 'cubic_nak'
                    or 'cubic_clamped')
           b        array containing the coefficients of the
                    linear terms from the cubic spline
                    (obtained from 'cubic_nak' or 'cubic_clamped')
           c        array containing the coefficients of the
                    quadratic terms from the cubic spline
                    (obtained from 'cubic_nak' or 'cubic_clamped')
           d        array containing the coefficients of the
                    cubic terms from the cubic spline
                    (obtained from 'cubic_nak' or 'cubic_clamped')
           t        value of independent variable at which
                    the interpolating polynomial is to be
                    evaluated
           OUTPUTS:
           y        value of cubic spline at the specified
           value of the independent variable
    */
    float spline_eval(int n,
                      float* x,
                      float* f,
                      float* b,
                      float* c,
                      float* d,
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
     *     PURPOSE:
     *    determine the coefficients for the 'not-a-knot'
     *      cubic spline for a given set of data
     *       CALLING SEQUENCE:
     *          cubic_nak ( n, x, f, b, c, d );
     *
     *       INPUTS:
     *          num                      number of interpolating points
     *          interpolatingPoints      array containing interpolating points
     *          functionValues           array containing function values to
     *                                   be interpolated;  functionValues[i] is
     *                                   the function value corresponding to
     *                                   interpolatingPoints[i]
     *           linearCoeff             array of size at least num; contents
     *                                   will be overwritten
     *          quadraticCoeff           array of size at least num;
     *                                   contents will be overwritten
     *          cubicCoeff               array of size at
     *                                  least num; contents will be overwritten
     *      OUTPUTS:
     *          linearCoeff             coefficients of linear terms in cubic
     *                                  spline
     *          quadraticCoeff           coefficients of quadratic terms in
     *                                   cubic spline
     *          cubicCoeff               coefficients of cubic terms in cubic
     *                                   spline
     *      REMARK:
     *           remember that the constant terms in the cubic spline
     *           are given by the function values being interpolated;
     *           i.e., the contents of the f array are the constant terms
     *           to evaluate the cubic spline, use the routine 'spline_eval'
     */

    void cubic_nak(int num,
                   float* interpolatingPoints,
                   float* functionValues,
                   float* linearCoeff,
                   float* quadraticCoeff,
                   float* cubicCoeff);

    /****************************************************************
    Input:
           ns      number of samples in the input data
           nsr     width (in samples) of the gaussian for which
                   amplitude > 0.5*max amplitude

    Output:
        data    1-D array[ns] of smoothed data
    ****************************************************************/
    void  gaussian1d_smoothing(int ns, int nsr, float* data);

    /**
     * [smoothAverage ]
     * @method smoothAverage
     * @param  y             []
     * @param  s             []
     * @param  points        []
     * @param  n             []
     */
    void smoothAverage(float* y, float* s, int points, int n);

    /********************************************************************
      Compute z = x convolved with y; i.e.,

      ifx+lx-1
      z[i] =   sum    x[j]*y[i-j]  ;  i = ifz,...,ifz+lz-1
      j=ifx
     *********************************************************************
     Input:
     xLen               length of x array
     indexFirstX        sample index of first x
     x                  array[xLen] to be convolved with input
     inputLen           length of input array
     indexFirstInput    sample index of first input array
     input              array[inputLen] with which x is to be convolved
     resultLen          length of result array
     indexFirstResult   sample index of first result

     Output:
     result		array[resultLen] containing x convolved with input
     ***********************************************************************
     Notes:
     The x samples are contained in x[0], x[1], ..., x[lx-1]; likewise
     for the y and z samples.  The sample indices of the first x, input,
     and result values determine the location of the origin for each array.
     For example, if z is to be a weighted average of the nearest 5 samples
     of input, one might use x[0] = x[1] = x[2] = x[3] = x[4] = 1.0/5.0;
     conv(5, -2, x, xLen, 0, input, inputLen, 0, result);
     In this example, the filter x is symmetric, with index of first sample = -2
     This function is optimized for architectures that can simultaneously
     perform a multiply, add, and one load from memory; e.g., the IBM RISC
     System/6000. Because, for each value of i, it accumulates the convolution
     sum z[i] in a scalar, this function is not likely to be optimal for vector
     architectures.
     **************************************************************************
     Author:  Dave Hale, Colorado School of Mines, 11/23/91
     **************************************************************************/
    void conv(int xLen,
              int indexFirstX,
              float* x,
              int inputLen,
              int indexFirstInput,
              float* input,
              int resultLen,
              int indexFirstResult,
              float* result);

    /*statistical functions*/
    /**
     * [ttest ]
     * @method ttest
     * @param  groupA []
     * @param  groupB []
     * @return        []
     */
    float ttest(StatisticsVector<float>& groupA,
                StatisticsVector<float>& groupB);

    /**
     * @brief median Calculates the median of vector of float
     * integer number.
     * @param y Vector of float integers.
     * @return  Returns calculated median.
     */
    float median(vector<float> y);

    vector<float> quantileDistribution(vector<float> y);

    /**
     * @brief correlation Calculates correlation between two floating
     * point vector.
     * @param a First vector.
     * @param b Second vector.
     * @return Returns Coorelation.
     */
    float correlation(const vector<float>& a, const vector<float>& b);

    /**
     * [gaussFit ]
     * @method gaussFit
     * @param  yobs     []
     * @param  sigmal   []
     * @param  R2       []
     */
    pair<float, float> gaussFit(const vector<float>& yobs);

    /**
     * @brief factorial Calculates the factorial of the given integer number.
     * @param n Integer number.
     * @return
     */
    inline unsigned long factorial(int n);

    /**
     * @brief nchoosek Calculates the number of items(k) chosen from n item
     * at a time.
     * @param n Total num of item.
     * @param k Number of items to be chosen.
     * @return
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
    vector<double> naturalAbundanceCorrection(
        int nC,
        vector<double>& M,
        map<unsigned int, string> carbonIsotopeSpecies);

    /**
     * @brief string2integer Converts string to an integer
     * @param s String to be converted.
     * @return
     */
    int string2integer(const std::string& s);

    /**
     * @brief string2float Converts string to floating point number.
     * @param s String to be converted.
     * @return
     */
    float string2float(const std::string& s);

    /**
     * @brief integer2string Converts interger to string.
     * @param x Integer to be converted.
     * @return
     */
    string integer2string(int x);

    /**
     * @brief float2string  Converts floating point number to string.
     * @param f     Floating Point number.
     * @param p     Precision of floats.
     * @return
     */
    string float2string(float f, int p);

    /**
     * @brief makeLowerCase Converts the string to lower case.
     * @param s String to be converted.
     * @return
     */
    string makeLowerCase(string& s);

    /**
     * @brief split Splits the string on basis of sep.
     * @param s String to be split.
     * @param c Seperator.
     * @param v Output. Vector of string
     */
    vector<string> split(const string& str, const string& sep);

    /**
     * @brief splitCSVFields split csv files which might have coma in 
     * the values of the field, where a simple coma split function would
     * give unexpected results. 
     * @param s String to be split. 
     * @param c Seperator.
     * @return vector of string.
     */ 
    vector<string> splitCSVFields(const string& s, const string& c);

    /**
     * @brief Join a vector of words into a single string with the given
     * separator.
     * @param words A vector of strings to be accumulated.
     * @param sep A separator that will be placed in between two words (but not
     * at the start or end) in the result string.
     * @return The string of joined words.
     */
    string join(const vector<string>& words, const string& sep);

    /**
     * @brief contains  Checks if s1 contains s2.
     * @param s1    string.
     * @param s2    string.
     * @return      Return s1, if it contains else an empty string.
     */
    bool contains(const string& first, const string& second, const bool ignoreCase = true);

    /**
     * @brief cleanFilename Gives the name of the file, removing the extension
     * and path.
     * @param s Path of the file.
     * @return
     */
    string cleanFilename(const string& s);

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
     * @method Decompress an STL string using zlib (deflate) filter of Boost
     * Iostream library and return uncompressed data.
     * @param str A STL string containing compressed zlib binary data.
     * @return An STL string containing uncompressed data.
     */
    std::string decompressString(const std::string& str);

    /* rounding and ppm functions */
    /**
     * [ppmDist ]
     * @method ppmDist
     * @param  mz1     []
     * @param  mz2     []
     * @return []
     */
    float massCutoffDist(const float mz1,
                         const float mz2,
                         MassCutoff* massCutoff);

    /**
     * [ppmDist ]
     * @method ppmDist
     * @param  mz1     []
     * @param  mz2     []
     * @return []
     */
    double massCutoffDist(const double mz1,
                          const double mz2,
                          MassCutoff* massCutoff);

    float ppmDist(const float mz1, const float mz2);

    double ppmDist(const double mz1, const double mz2);

    /**
     * resolution parameter =10  -> one digit after decimal point,
     *                      100 -> two digits after decimal point
     *                     etc..
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
    bool withinXMassCutoff(float mz1, float mz2, MassCutoff* massCutoff);

    /**
     * @brief fileExists Checks if file exists on a certain path or not.
     * @param strFilename   Path of the file.
     * @return
     */
    bool fileExists(string strFilename);

    /**
     * @brief createDir Creates the directory at a certain path.
     * @param path  Specifies path to create directory.
     * @return
     */
    int createDir(string path);

    /**
     * @brief isDir Checks if the specified path exists or not.
     * @param path  Specifies the path.
     * @return
     */
    int isDir(string path);

    /**
     * @brief Check whether two double precision fractions are almost equal to
     * each other.
     * @param a First double value to be compared.
     * @param b Second double value to be compared.
     * @return True if the values are relatively close to each other.
     */
    bool almostEqual(double a, double b,
                     double epsilon = std::numeric_limits<double>::epsilon());

    /**
     * @brief Check whether two single precision fractions are almost equal to
     * each other.
     * @param a First float value to be compared.
     * @param b Second float value to be compared.
     * @return True if the values are relatively close to each other.
     */
    bool almostEqual(float a, float b,
                     float epsilon = std::numeric_limits<float>::epsilon());

    /**
     * [fractional overlap between two line segments]
     * @method checkOverlap
     * @param  a            []
     * @param  b            []
     * @param  c            []
     * @param  d            []
     * @return []
     */
    inline float checkOverlap(float a, float b, float c, float d)
    {
        if ((a < c && b < c) || (d < a && d < b))
            return (0.0);  // no overalp
        if ((c <= a && b <= d) || (a <= c && d <= b))
            return (1.0);  // 100% overlap
        if (c <= a)
            return (1 / abs((c - b) / (d - a)));
        if (a <= c)
            return (abs((c - b) / (d - a)));
        return 0.0;
    }

    // print vector
    template<typename T>

    /**
     * @brief shuffle   Randomly shuffles the elements in the array.
     * @param vect Vector to be shuflled.
     */
    void shuffle(vector<T>& vect)
    {
        int N = vect.size() - 1;
        if (N <= 0)
            return;
        for (int i = 0; i < vect.size(); i++) {
            int j = ((double)rand() / RAND_MAX) * N;
            if (i == j)
                continue;
            T tmp = vect[i];
            vect[i] = vect[j];
            vect[j] = tmp;
        }
    }

    template<typename T>

    /**
     * [print vector]
     * @method printF   Prints the vector.
     * @param vect
     */
    void printF(vector<T>& vect)
    {
        for (unsigned int i = 0; i < vect.size(); i++) {
            cerr << setprecision(6) << vect[i] << " ";
        }
        cerr << endl;
    }

    template<typename T>

    /**
     * [delete_all deallocated elements fo array from memory]
     * @method delete_all
     * @param  my_vector  []
     */
    void delete_all(vector<T>& my_vector)
    {
        if (my_vector.empty())
            return;
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
    }

    template<typename T>

    /**
     * [deallocated elements fo array from memory]
     * @method delete_all
     * @param  my_vector  []
     */
    inline void delete_all(deque<T>& my_vector)
    {
        if (my_vector.empty())
            return;
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
    }

    /**
     * @brief Removes elements at given indexes from a vector. Please note, that
     * this function will disturb the order of elements in the vector being
     * filtered.
     * @param vec The vector that has to be filtered.
     * @param indexesToErase A vector of indexes to be removed from `vec`. No
     * element should be greater than or equal to the size of `vec`.
     */
    template<typename T>
    inline void eraseIndexes(vector<T>& vec, vector<size_t>& indexesToErase)
    {
        sort(begin(indexesToErase), end(indexesToErase), greater<size_t>());
        for (size_t index : indexesToErase) {
            vec[index] = vec.back();
            vec.pop_back();
        }
    }

    /**
     * @brief Inserts an element into a sorted vector, at a position such that
     * the sort order is preserved.
     * @param vec The container vector to be inserted into.
     * @param item The item to be inserted.
     * @param pred The predicate that will be used to compare two items in the
     * vector. This predicate should represent the order logic itself.
     */
    template<typename T, typename Predicate>
    typename vector<T>::iterator
    insertSorted(vector<T>& vec, T const& item, Predicate pred)
    {
        return vec.insert(upper_bound(begin(vec), end(vec), item, pred), item);
    }

    /**
     * @brief Zeroth-order modified bessel function of the first kind.
     * @param x Argument for the modified bessel function.
     * @return Solution to the bessel equation for the given argument.
     */
    double besseli0(double x);

    /**
     * @brief Create a Kaiser window of given size.
     * @details Kaiser-Bessel is from the one-parameter family of window
     * functions used in finite impulse response (FIR) filter design and
     * spectral analysis. It is considered to be the optimum window for
     * low-pass filtering required when resampling digital signals.
     * @param N Desired size for the window.
     * @param beta Beta factor parametrizing the Kaiser window.
     * @return A vector of double precision values forming a Kaiser window.
     */
    std::vector<double> kaiser(size_t N, double beta);

    /**
     * @brief A sine cardinal function.
     * @details In information theory, the nomalized sinc function is
     * commonly defined as:
     *   sinc(x) = sin(π•x) / (π•x)
     * @param x The value to be passed to the sinc function.
     * @return Value of sinc(x) as a double.
     */
    double sinc(double x);

    /**
     * @brief Compute the coefficients of a finite impulse response (FIR)
     * filter using a Kaiser window.
     * @param n Length of the desired filter. Must be greater than 0.
     * @param fc Cutoff frequency of the filter. Must be in range (0, 0.5)
     * @param beta Beta parameter value for Kaiser window. By default the
     * value is set to 5.0.
     * @param scale Optional parameter to scale the filter coefficients
     * by the specified amount.
     * @return A vector of coefficients that can be used to create an FIR
     * filter optimal for resampling.
     */
    std::vector<double> firDesignKaiser(size_t n,
                                        double fc,
                                        double beta = 5.0,
                                        double scale = 1.0);
    /**
     * @brief An abstracted function that returns filter coefficients that
     * can be used for resampling a digital signal with given interpolation
     * and decimation rate.
     * @param interpRate Rate of interpolation.
     * @param decimRate Rate of decimation.
     * @return A vector of coefficients that be used for resampling a signal
     * by FIR filtering.
     */
    std::vector<double> computeFilterCoefficients(int interpRate,
                                                  int decimRate);

    /**
     * @brief This function can be used to approximate a resampling rate for
     * a given data size.
     * @details It should be noted that this function is largely ad hoc and
     * meant for resampling of EIC intensity values. For a function to truly
     * estimate an optimal resampling factor, without losing information, it
     * would have to consider the nature of the signal itself.
     * @param dataSize Length of the digital signal.
     * @param lowerSizeLimit Length which the returned resampling factor is
     * guaranteed to not cross if used to resize data of given size.
     * @return A integer resampling factor (always ≥ 1).
     */
    int approximateResamplingFactor(size_t dataSize, int lowerSizeLimit = 100);

    /**
     * @brief Resample an input signal according to the given interpolation
     * and decimation rates.
     * @details The underlying method that is used to resample this signal,
     * first interpolates the signal, low-pass filters it and then decimates
     * it. It claims to be more efficient than two separate steps of
     * interpolation and decimation.
     * @param inputData Vector containing the data to be resampled.
     * @param interpRate Rate of interpolation.
     * @param decimRate Rate of decimation.
     * @return A vector containing the resampled data.
     */
    std::vector<double> resample(const std::vector<double>& inputData,
                                 int interpRate,
                                 int decimRate);

    /**
     * @brief Create a clock that can be used to indicate the start of an
     * operation which needs to be timed.
     * @return A high resolution `time_point` value representing the current
     * point in time.
     */
    chrono::time_point<chrono::high_resolution_clock> startTimer();

    /**
     * @brief Given a high resolution time point, prints the difference
     * between the given time point and the current point in time.
     * @details Despite its name, this function does not really stop
     * anything but exists only to print a difference between given and
     * current time points. Wrapping an operation with `startTimer` and
     * `stopTimer` to record its runtime makes idiomatic sense.
     * @param clock A high resolution time point object.
     * @param name The name of the operation, which elapsed time will be
     * attributed to.
     */
    void stopTimer(chrono::time_point<chrono::high_resolution_clock>& clock,
                   string name);

    /**
     * @brief Query total memory available on the current system. This is not
     * the same as "free memory".
     * @return A 64-bit unsigned integer representing the amount of memory in
     * bytes.
     */
    unsigned long long availableSystemMemory();

    /**
     * @brief Query the number of processing units available on the system.
     * This is also the cap on the number of parallel threads that can be worked
     * with on the system.
     * @return An integer denoting the number of physical/virtual processors.
     */
    int numSystemCpus();

    /**
     * @brief Compare dates of format yyyy-mm-dd. 
     * @return Returns -1 if date1 is smaller, 0 if
     * two dates are equal and 1 is date1 is bigger.
     */ 
    int compareDates(string date1, string date2);

    /**
     * @brief Compare time in 24hr format like - 15:39:40.
     * @return Returns true, if time1 < time2.
     */
    bool compareTime(string time1, string time2); 

}  // namespace mzUtils

template<typename T>

/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  []
 * @param  idx        []
 */
void delete_one(vector<T>& my_vector, unsigned int idx)
{
    if (my_vector.size() == 0)
        return;
    if (idx < 0 || idx > my_vector.size() - 1)
        return;
    if (my_vector[idx] != NULL)
        delete (my_vector[idx]);
    my_vector[idx] = NULL;
    my_vector.erase(my_vector.begin() + idx);
}

template<typename T>
/**
 * [deallocated single element]
 * @method delete_one
 * @param  my_vector  []
 * @param  idx        []
 */
void delete_one(deque<T>& my_vector, unsigned int idx)
{
    if (my_vector.size() == 0)
        return;
    if (idx > my_vector.size() - 1)
        return;
    if (my_vector[idx] != NULL)
        delete (my_vector[idx]);
    my_vector[idx] = NULL;
    my_vector.erase(my_vector.begin() + idx);
}

template<typename T>
bool isBetweenInclusive(T x, T lb, T ub)
{
    return (x >= lb && x <= ub) || (x >= ub && x <= lb);
}

#endif

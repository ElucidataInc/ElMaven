#include "plsutility.h"
#include <iostream>

using std::string;
using std::vector;
using std::pair;
using std::make_pair;

Row col_stdev( Mat2D mat, Row means ) {
    Row stdevs = Row::Zero(mat.cols());
    const float_type N = mat.rows();
    if ( N < 2 ) return stdevs;

    const float_type N_inv = 1.0/(N-1); // N-1 for unbiased sample variance
    for (int i=0; i<mat.cols(); i++) {
        stdevs[i] = sqrt( (mat.col(i).array()-means[i]).square().sum() * N_inv );
    }
    return stdevs;
}


float_type dominant_eigenvalue( EigenSolver<Mat2Dc> es ){
    Colc  ev = es.eigenvalues();
    float_type m = 0;

    for (int i = 0; i<ev.size(); i++) {
        if (imag(ev[i]) == 0) {
            if (abs(ev[i]) > m) m = abs(ev[i]);
        }
    }
    return m;
}


Colc dominant_eigenvector( EigenSolver<Mat2D> es ){
    Colc eig_val = es.eigenvalues();
    float_type m = 0;
    int idx = 0;

    for (int i = 0; i<eig_val.size(); i++) {
        if (imag(eig_val[i]) == 0) {
            if (abs(eig_val[i]) > m) {
                m = abs(eig_val[i]);
                idx = i;
            }
        }
    }
    return es.eigenvectors().col(idx);
}

Mat2D colwise_z_scores( const Mat2D& mat ) {
    // Standardize values by column, i.e. convert to Z-scores
    Row means = col_means( mat );
    Row stdev = col_stdev( mat, means );
    Mat2D zmat = Mat2D::Zero(mat.rows(), mat.cols());
    for (int r = 0; r<mat.rows(); r++) { zmat.row(r) = (mat.row(r) - means).cwiseQuotient(stdev); }
    return zmat;
}

/*
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// -- OR --
template <typename T> inline constexpr
int signum(T x, std::false_type is_signed) {
    return T(0) < x;
}

template <typename T> inline constexpr
int signum(T x, std::true_type is_signed) {
    return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr
int signum(T x) {
    return signum(x, std::is_signed<T>());
}*/

/*template <typename T>
vector<size_t> ordered(vector<T> const& values) {
    vector<size_t> indices(values.size());
    std::iota(begin(indices), end(indices), static_cast<size_t>(0));

    std::sort(
        begin(indices), end(indices),
        [&](size_t a, size_t b) { return values[a] < values[b]; }
    );
    return indices;
}*/

vector<int> ordered(Col const& values) {

    vector<pair<float_type,int> > pairedvalues(values.size());
    for(unsigned int pos=0; pos < values.size(); pos++ ) {
        pairedvalues[pos] = make_pair(values[pos],pos);
    }

   // sort first key [ ie intensity ]
   sort(pairedvalues.begin(), pairedvalues.end());

   //return positions in order from highest to lowest intenisty
   vector<int>indices(values.size());
   for(int i=0; i < pairedvalues.size(); i++) { indices[i] = pairedvalues[i].second; }
   return indices;
}


//
// WILCOXON SIGNED RANK TEST FOR EVALUATING RELATIVE QUALITY OF TWO
// COMPETING METHODS
//
// DESCRIPTION: Pairwise comparison between sets of model predictions
// Competing models: model#1, model#2
//
// REFERENCE: Lehmann E. L. Nonparamtrics: Statistical Methods Based on Ranks.
// Holden-Day: San Francisco, 1975, 120-132.
//
// Let: U=sum of postive ranks, V=sum of negative ranks
// (V>U) is evidence that the model#1 is better)
// Define: d=U-V and t=U+V=n(n+1)/2
// Then V=(t-d)/2
//
// Asymptotic Theory: Suppose n is the number of samples.
// Then, E(V)=n(n+1)/4 and Var(V)=n(n+1)(2n+1)/24.
// It follows that (V-E(V))/Std(V) is approx. normally distributed.
//
// INPUT: err_1=prediction errors from model#1
//        err_2=prediction errors from model#2
//
// OUTPUT: probw=Prob{V is larger than observed}
// If probw is small enough, conclude that model#1 is better
//
// Based on Matlab code from
// Thomas E. V. Non-parametric statistical methods for multivariate calibration 
// model selection and comparison. J. Chemometrics 2003; 17: 653–659
//
float_type wilcoxon(const Col err_1, const Col err_2) {
    int n = err_1.rows();
    Col del = err_1.cwiseAbs() - err_2.cwiseAbs();
    Rowi sdel;
    sdel.setZero(del.size());
    //Matrix<int, Dynamic, 1> sdel = del.unaryExpr(std::ptr_fun(_sgn)); // can't get this to work
    for (int i=0; i<del.size(); i++)  sdel(i) = (0 < del(i)) - (del(i) < 0); // get the sign of each element
    Col adel = del.cwiseAbs();
    // 's' gives the original positions (indices) of the sorted values
    vector<int> s = ordered(adel); 
    float d = 0;
    for (int i=0; i<n; i++) d += (i+1)*sdel(s[i]);
    float t  = n*(n+1)/2.0;
    float v  = (t-d)/2.0;
    float ev = t/2.0;
    double sv = sqrt((double) n*(n+1)*(2*n+1)/24.0);
    float_type z = (v-ev)/sv;
    float_type probw = 1.0 - normalcdf(z);

    return probw;
}

//
// Numerical Approximation to Normal Cumulative Distribution Function
//
// DESCRIPTION:
// REFERENCE: Handbook of Mathematical Functions with Formulas, Graphs, and Mathematical
// Tables, U.S. Dept of Commerce - National Bureau of Standards, Editors: M. Abramowitz and I. A. Stegun
// December 1972, p. 932
// INPUT: z=computed Z-value
// OUTPUT: probn=cumulative probability from -infinity to z
//
//
float_type normalcdf(float_type z){
    const double c1 = 0.196854;
    const double c2 = 0.115194;
    const double c3 = 0.000344;
    const double c4 = 0.019527;
    float_type p;
    if (z < 0) {
        z = -z;
        p = 1 - 0.5 / pow(1 + c1*z + c2*z*z + c3*z*z*z + c4*z*z*z*z,4);
    } else {
        p = 0.5 / pow(1 + c1*z + c2*z*z + c3*z*z*z + c4*z*z*z*z,4);
    }
    float_type probn = 1.0 - p;
    return probn;
}

/*
double normal_pdf(double x, double mu, double var) {
    long double PI = 3.1415926535897932384;
    return exp(-pow(x-mu,2) / (2.0*var)) / sqrt(2*PI*var);
}

double normal_cdf(double x, double mu, double var) {
    x = (x-mu)/sqrt(var);
    // Abramowitz & Stegun (1964) approximation
    long double b0 = 0.2316419;
    double b1 = 0.319381530;
    double b2 = -0.356563782;
    double b3 = 1.781477937;
    double b4 = -1.821255978;
    double b5 = 1.330274429;
    if (x >= 0.0) {
        long double t = 1.0/(1.0+b0*x);
        return 1.0 - normal_pdf(x, 0, 1)*(b1*t + b2*pow(t,2) + b3*pow(t,3) + b4*pow(t,4) + b5*pow(t,5));
    } else {
        long double t = 1.0/(1.0-b0*x);
        return normal_pdf(x, 0, 1)*(b1*t + b2*pow(t,2) + b3*pow(t,3) + b4*pow(t,4) + b5*pow(t,5));
    }
}*/




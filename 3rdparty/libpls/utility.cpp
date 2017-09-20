#include "utility.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::ifstream;

void split(const string& s, char c, vector<string>& v) {
    string::size_type i = 0;
    string::size_type j = s.find(c);

    while (j != string::npos) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);
    }
    if (j == string::npos) v.push_back(s.substr(i, s.length( )));
}


Mat2D read_matrix_file(string filename, char sep) {
    cerr << "Loading " << filename << endl;
    ifstream myfile(filename.c_str());
    stringstream ss;

    vector<vector<double> > M;
    if (myfile.is_open()) {
        string line;

        while ( getline(myfile,line) ) {
            //split string based on "," and store results into vector
            vector<string> fields;
            split(line, sep, fields);

            vector<double>row(fields.size());
            for( int i=0; i < fields.size(); i++ ) {
                row[i] = string2double(fields[i]);
            }
            M.push_back(row);
        }
    }

    Mat2D X( (int) M.size(), (int) M[0].size() );
    for(int i=0; i < M.size(); i++ ) {
        for(int j=0; j < M[i].size(); j++ ) {  
            X(i,j)=M[i][j]; 
        }
    }
    return X;
}


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
    vector<int> indices(values.size());
    std::iota(begin(indices), end(indices), static_cast<size_t>(0));

    std::sort(
        begin(indices), end(indices),
        [&](int a, int b) { return values[a] < values[b]; }
    );
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

void rand_nchoosek(int N, vector<int>& sample) {
    if (sample.size() == 0 ) return;
    int k = sample.size();       // k is specified by size of requested vector

    assert( k <= N );

    int top = N-k;
    double Nreal = (double) N;

    int newidx=0;
    int lastidx=0;
    int i=0;

    while ( k >= 2 ) {
        double V = runif(RNG);
        //double V = mtrand->rand();
        int S=0;
        double quot = top/Nreal;
        while( quot > V ) {
            S++; top-=1; Nreal-=1;
            quot =(quot*top)/Nreal;
        }
        //skip over the next S records and select the following one for the sample
        newidx = lastidx + S;
        sample[i]=newidx; lastidx = newidx+1; i++;
        Nreal -= 1.0; k -= 1;
    }

    if ( k == 1 ) {
        // the following line had (newidx+1) instead of lastidx before, which
        // produced incorrect results when N == 1; this, I believe, is correct
        std::uniform_int_distribution<int> randInt(0,(int) Nreal - 1);
        sample[i++] = lastidx + randInt(RNG); // truncated float on [0, Nreal]
    }
}



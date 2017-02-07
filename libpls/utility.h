#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <fstream>
#include <../Eigen/Core>
#include <../Eigen/Eigenvalues>
#include <random>
#include <chrono>


const int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::default_random_engine RNG(seed);
static std::uniform_real_distribution<double> runif(0,1);

using namespace Eigen;

#ifdef MPREAL_SUPPORT
#include "mpreal.h"
#include <unsupported/Eigen/MPRealSupport>
    using namespace mpfr;
    typedef mpreal float_type;
#else
    typedef long double float_type;
#endif 

typedef Matrix<float_type,Dynamic,Dynamic> Mat2D;
typedef Matrix<float_type, Dynamic, 1>  Col;
typedef Matrix<float_type, 1, Dynamic>  Row;
typedef Matrix<int, 1, Dynamic>  Rowi;
typedef Matrix<std::complex<float_type>,Dynamic,Dynamic> Mat2Dc;
typedef Matrix<std::complex<float_type>, Dynamic, 1>  Colc;

void split(const std::string& s, char c, std::vector<std::string>& v);

inline double string2double(const std::string& s){ std::istringstream i(s); double x = 0; i >> x; return x; }

inline Row col_means( Mat2D mat ) { return mat.colwise().sum() / mat.rows(); }

Mat2D read_matrix_file(std::string filename, char sep); 

Row col_stdev( Mat2D mat, Row means );

float_type dominant_eigenvalue( EigenSolver<Mat2Dc> es );

Colc dominant_eigenvector( EigenSolver<Mat2D> es );

Mat2D colwise_z_scores( const Mat2D& mat );

std::vector<int> ordered(Col const& values);

float_type wilcoxon(const Col err_1, const Col err_2);

float_type normalcdf(float_type z);

// N is the size of the sample space--which includes 0, so the int "N" itself will never get
// returned in the sample.  sample is an empty vector that needs to be of size k
void rand_nchoosek(int N, std::vector<int>& sample);


#endif

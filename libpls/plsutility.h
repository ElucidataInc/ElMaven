#ifndef PLSUTILITY_H
#define PLSUTILITY_H

#include <vector>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
using namespace Eigen;
typedef long double float_type;

typedef Matrix<float_type,Dynamic,Dynamic> Mat2D;
typedef Matrix<float_type, Dynamic, 1>  Col;
typedef Matrix<float_type, 1, Dynamic>  Row;
typedef Matrix<int, 1, Dynamic>  Rowi;
typedef Matrix<std::complex<float_type>,Dynamic,Dynamic> Mat2Dc;
typedef Matrix<std::complex<float_type>, Dynamic, 1>  Colc;

inline Row col_means( Mat2D mat ) { return mat.colwise().sum() / mat.rows(); }

//int _sgn(float_type val) { return (0 < val) - (val < 0); }

Row col_stdev( Mat2D mat, Row means );

float_type dominant_eigenvalue( EigenSolver<Mat2Dc> es );

Colc dominant_eigenvector( EigenSolver<Mat2D> es );

Mat2D colwise_z_scores( const Mat2D& mat );

std::vector<int> ordered(Col const& values);

float_type wilcoxon(const Col err_1, const Col err_2);

float_type normalcdf(float_type z);

#endif

#ifndef PLS_H
#define PLS_H

#include "utility.h"

//using namespace std;
using namespace Eigen;

using std::cout;
using std::cerr;
using std::complex;
using std::endl;
using std::vector;

typedef enum { KERNEL_TYPE1, KERNEL_TYPE2 } REGRESSION_ALGORITHM;
typedef enum { LOO, LSO } VALIDATION_METHOD;
typedef enum { PRESS, MSEP, RMSEP } VALIDATION_OUTPUT;

/*
    Variables and their interpretation from 
    Dayal and MacGregor (1997) "Improved PLS Algorithms" J. of Chemometrics. 11,73-85.

            X      predictor variables matrix (N × K)
            Y      response variables matrix (N × M)
            B_PLS  PLS regression coefficients matrix (K × M)
            W      PLS weights matrix for X (K × A)
            P      PLS loadings matrix for X (K × A)
            Q      PLS loadings matrix for Y (M × A)
            R      PLS weights matrix to compute scores T directly from original X (K × A)
            T      PLS scores matrix of X (N × A)
            w_a    a column vector of W
            p_a    a column vector of P
            q_a    a column vector of Q
            r_a    a column vector of R
            t_a    a column vector of T
            K      number of X-variables
            M      number of Y-variables
            N      number of objects
            A      number of components in PLS model
            a      integer counter for latent variable dimension.
*/

class PLS_Model {
  public:
    Mat2Dc P, W, R, Q, T;
    int A;
    REGRESSION_ALGORITHM algorithm;
    void initialize(int num_predictors, int num_responses, int num_components) {
        A = num_components; 
        P.setZero(num_predictors, num_components);
        W.setZero(num_predictors, num_components);
        R.setZero(num_predictors, num_components);
        Q.setZero(num_responses, num_components);
        // T will be initialized if needed
        return;
    }

    //"Modified kernel algorithms 1 and 2"
    //from Dayal and MacGregor (1997) "Improved PLS Algorithms" J. of Chemometrics. 11,73-85.
    void plsr(const Mat2D X, const Mat2D Y, REGRESSION_ALGORITHM _algorithm) {
        algorithm = _algorithm;
        int M = Y.cols(); // Number of response variables == columns in Y

        if (algorithm == KERNEL_TYPE1) T.setZero(X.rows(), A);

        Mat2D XY = X.transpose() * Y;
        Mat2D XX;
        if (algorithm == KERNEL_TYPE2) XX = X.transpose() * X;

        for (int i=0; i<A; i++) {
            Colc w, p, q, r, t; 
            complex<float_type> tt;
            if (M==1) {
                w = XY.cast<complex<float_type> >();
            } else {
                EigenSolver<Mat2D> es( (XY.transpose() * XY) );
                q = dominant_eigenvector(es);
                w = (XY*q);
            }

            w /= sqrt((w.transpose()*w)(0,0)); // use normalize function from eigen?
            r=w;
            for (int j=0; j<=i-1; j++) {
                r -= (P.col(j).transpose()*w)(0,0)*R.col(j);
            }
            if (algorithm == KERNEL_TYPE1) {
                t = X*r;
                tt = (t.transpose()*t)(0,0);
                p.noalias() = (X.transpose()*t);
            } else if (algorithm == KERNEL_TYPE2) {
                tt = (r.transpose()*XX*r)(0,0);
                p.noalias() = (r.transpose()*XX).transpose();
            }
            p /= tt;
            q.noalias() = (r.transpose()*XY).transpose(); q /= tt;
            XY -= ((p*q.transpose())*tt).real(); // is casting this to 'real' always safe?
            W.col(i)=w;
            P.col(i)=p;
            Q.col(i)=q;
            R.col(i)=r;
            if (algorithm == KERNEL_TYPE1) T.col(i) = t;
        }
        if (algorithm == KERNEL_TYPE2) T = X*R; // not part of the algorithm; allows users to retrieve scores
        return; 
    }

    Mat2Dc scores() { return scores(A); }
    Mat2Dc scores(int comp) { 
        assert (comp <= A);
        assert (comp > 0);
        return T.leftCols(comp);
    }

    Mat2Dc loadingsX() { return loadingsX(A); }
    Mat2Dc loadingsX(int comp) { 
        assert (comp <= A);
        assert (comp > 0);
        return P.leftCols(comp);
    }

    Mat2Dc loadingsY() { return loadingsY(A); }
    Mat2Dc loadingsY(int comp) { 
        assert (comp <= A);
        assert (comp > 0);
        return Q.leftCols(comp);
    }
 
    // compute the regression coefficients (aka 'beta')
    Mat2Dc coefficients() { return coefficients(A); }
    Mat2Dc coefficients(int comp) {
        assert (comp <= A);
        assert (comp > 0);
        return R.leftCols(comp)*Q.leftCols(comp).transpose();
    }

    // predicted Y values, given X values and pls model
    Mat2D fitted_values(const Mat2D& X) { return fitted_values(X, A); }
    Mat2D fitted_values(const Mat2D& X, int comp) {
        return X*coefficients(comp).real();
    }

    // unexplained portion of Y values
    Mat2D residuals(const Mat2D& X, const Mat2D& Y) { return residuals(X, Y, A); }
    Mat2D residuals(const Mat2D& X, const Mat2D& Y, int comp) {
        return Y - fitted_values(X, comp);
    }
    
    // Sum of squared errors
    Row SSE(const Mat2D& X, const Mat2D& Y) { return this->SSE(X, Y, A); }
    Row SSE(const Mat2D& X, const Mat2D& Y, int comp) {
        return residuals(X, Y, comp).colwise().squaredNorm();
    }

    // Total sum of squares
    Row SST(const Mat2D& Y) { 
        Row sst(Y.cols());
        for (int c = 0; c < Y.cols(); c++) {
            sst(c) = (Y.col(c).array() - (Y.col(c).sum()/Y.rows())).square().sum();
        }
        return sst;
    }

    // fraction of explainable variance
    Row explained_variance(const Mat2D& X, const Mat2D& Y) { return explained_variance(X, Y, A); }
    Row explained_variance(const Mat2D& X, const Mat2D& Y, int comp) {
        assert (comp <= A);
        assert (comp > 0);
    //    cerr << "ev: " << this->SSE(X, Y, comp).cwiseQuotient( SST(Y) ) << endl;
        return (1.0 - this->SSE(X, Y, comp).cwiseQuotient( SST(Y) ).array()).matrix(); 
    }

    std::vector<Mat2D> _loo_cv_residual_matrix(const Mat2D& X, const Mat2D& Y) { 
        Mat2D Xv = X.bottomRows(X.rows()-1);
        Mat2D Yv = Y.bottomRows(Y.rows()-1);
       
        // vector of error matrices(rows=Y.rows(), cols=Y.cols())
        // col = Y category #, row = obs #, tier = component
        std::vector<Mat2D> Ev(this->A, Mat2D::Zero(X.rows(), Y.cols()));

        PLS_Model plsm_v;
        plsm_v.initialize(Xv.cols(), Yv.cols(), this->A);
        for (int i = 0; i < X.rows(); i++) {
            plsm_v.plsr(Xv, Yv, this->algorithm);
            for (int j = 0; j < this->A; j++) {
                Row res = plsm_v.residuals(X.row(i), Y.row(i), j+1).row(0); // convert j to number of components
                for (int k = 0; k < res.size(); k++) Ev[j](i,k) = res(k);
            }
            if ( i < Xv.rows() ) {
                // we haven't run out of rows to swap out yet
                Xv.row(i) = X.row(i); 
                Yv.row(i) = Y.row(i); 
            }
        }
        return Ev;
    }


    // leave-one-out validation of model (i.e., are we overfitting?)
    Mat2D loo_validation(const Mat2D& X, const Mat2D& Y, VALIDATION_OUTPUT out_type) { 
        std::vector<Mat2D> Ev = _loo_cv_residual_matrix(X,Y);
        Mat2D SSEv = Mat2D::Zero(Y.cols(), this->A);

        for (int j = 0; j < this->A; j++) {
            Mat2D res = Ev[j];
            Mat2D SE  = res.cwiseProduct(res);
            // rows in SSEv correspond to different parameters
            // Collapse the squared errors so that we're summing over all predicted rows
            // then transpose, so that rows now represent different parameters
            SSEv.col(j) += SE.colwise().sum().transpose();
        }
        if ( out_type == PRESS ) {
            return SSEv;
        } else {
            SSEv /= X.rows();
            if ( out_type == MSEP ) { 
                return SSEv;
            } else {
                // RMSEP
                return SSEv.cwiseSqrt();
            }
        }
    }


    std::vector<Mat2D> _lso_cv_residual_matrix(const Mat2D& X, const Mat2D& Y, const float test_fraction, const int num_trials) { 
        const int N = X.rows();
        const int test_size = (int) (test_fraction * N + 0.5);
        const int train_size = N - test_size;
        std::vector<Mat2D> Ev(this->A, Mat2D::Zero(num_trials*test_size, Y.cols()));
        vector<int> sample(train_size);
        Mat2D Xv(train_size, X.cols()); // values we're training on
        Mat2D Yv(train_size, Y.cols());
        Mat2D Xp(test_size, X.cols());  // values we're predicting
        Mat2D Yp(test_size, Y.cols());

        PLS_Model plsm_v;
        plsm_v.initialize(Xv.cols(), Yv.cols(), this->A);
        for (int rep = 0; rep < num_trials; ++rep) {
            rand_nchoosek(N, sample);
            int j=0;
            int k=0;
            for (unsigned int i=0; i<N; ++i) {
                if( sample[j] == i ) { // in training set
                    Xv.row(j) = X.row(i);
                    Yv.row(j) = Y.row(i);
                    j++; 
                } else {               // in testing set
                    Xp.row(k) = X.row(i);
                    Yp.row(k) = Y.row(i);
                    k++; 
                }
            }

            plsm_v.plsr(Xv, Yv, this->algorithm);
            for (int j = 0; j < this->A; j++) {
                Mat2D res = plsm_v.residuals(Xp, Yp, j+1); // convert j to number of components
                Ev[j].middleRows(rep*test_size, test_size) = res; // write to submatrix; middleRows(startRow, numRows)
            }
        }

        return Ev;
    }


    // leave-some-out validation of model (i.e., are we overfitting?)
    Mat2D lso_validation(const Mat2D& X, const Mat2D& Y, VALIDATION_OUTPUT out_type, float test_fraction, int num_trials) { 
        const std::vector<Mat2D> Ev = _lso_cv_residual_matrix(X, Y, test_fraction, num_trials);
        assert(Ev.size() > 0);
        const int num_residuals = Ev[0].rows();
        Mat2D SSEv = Mat2D::Zero(Y.cols(), this->A);

        for (int j = 0; j < this->A; j++) {
            Mat2D res = Ev[j];
            // square all of the residuals
            Mat2D SE  = res.cwiseProduct(res);
            // rows in SSEv correspond to different parameters
            // Collapse the squared errors so that we're summing over all predicted rows
            // then transpose, so that rows now represent different parameters
            SSEv.col(j) += SE.colwise().sum().transpose();
        }
        if ( out_type == PRESS ) {
            return SSEv;
        } else {
            SSEv /= num_residuals;
            if ( out_type == MSEP ) { 
                return SSEv;
            } else {
                // RMSEP
                return SSEv.cwiseSqrt();
            }
        }
    }
    

    Rowi loo_optimal_num_components(const Mat2D& X, const Mat2D& Y) { 
        const int dummy = 0;
        return _optimal_num_components(X, Y, LOO, dummy, dummy);
    }


    Rowi lso_optimal_num_components(const Mat2D& X, const Mat2D& Y, const float test_fraction, const int num_trials) { 
        return _optimal_num_components(X, Y, LSO, test_fraction, num_trials);
    }


    Rowi _optimal_num_components(const Mat2D& X, const Mat2D& Y, const VALIDATION_METHOD vmethod, const float test_fraction, const int num_trials) { 
        // tier = component #, col = Y category, row = obs #
        std::vector<Mat2D> errors;
        if (vmethod == LOO) {
            errors = _loo_cv_residual_matrix(X,Y);
        } else if (vmethod == LSO) {
            errors = _lso_cv_residual_matrix(X, Y, test_fraction, num_trials);
        }
        Mat2D press = Mat2D::Zero(Y.cols(), this->A);
        Rowi min_press_idx = Rowi::Zero(Y.cols());
        Row  min_press_val(Y.cols());
        Rowi best_comp(Y.cols());
        
        // Determine PRESS values
        for (int j = 0; j < this->A; j++) {
            Mat2D resmat = errors[j];
            for (int i = 0; i < X.rows(); i++) {
                Row res = resmat.row(i);
                press.col(j) += res.cwiseProduct(res).transpose();
            }
        }
        
        min_press_val = press.col(0);
        // Find the component number that minimizes PRESS for each Y category
        for (int i=0; i<press.rows(); i++) {              // for each Y category
            for (int j = 0; j < this->A; j++) {
                if (press(i,j) < min_press_val(i)) {
                    min_press_val(i) = press(i,j);
                    min_press_idx(i) = j;
                }
            }
        }

        best_comp = min_press_idx.array() + 1;
        // Find the min number of components that is not significantly
        // different from the min PRESS at alpha = 0.1 for each Y category
        const float ALPHA = 0.1;
        for (int i=0; i<press.rows(); i++) {              // for each Y category
            for (int j=0; j<min_press_idx(i); j++) {      // for each smaller number of components
                Col err1 = errors[min_press_idx(i)].col(i);
                Col err2 = errors[j].col(i);
                if (wilcoxon(err1, err2) > ALPHA) {
                    best_comp(i) = j+1; // +1 to convert from index to component number
                    break;
                }
            }
        }

        return best_comp;
    }

};


#endif

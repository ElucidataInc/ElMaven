#include "pls.h"

//http://eigen.tuxfamily.org/dox/QuickRefPage.html

int main(int argc, char *argv[]) { 
    if (argc < 4) { fprintf(stderr, "Usage: ./pls X_data.csv Y_data.csv num_components\n"); exit(100); }
    std::string x_filename(argv[1]);
    std::string y_filename(argv[2]);

    Mat2D X_orig  = read_matrix_file(x_filename, ',');
    Mat2D Y_orig  = read_matrix_file(y_filename, ',');

    //Mat2D X = X_orig ;
    //Mat2D Y = Y_orig ;
    Mat2D X = colwise_z_scores( X_orig );
    Mat2D Y = colwise_z_scores( Y_orig );

    PLS_Model plsm;
    int nobj  = X_orig.rows();
    int npred = X_orig.cols();
    int nresp = Y_orig.cols();
    int ncomp = atoi(argv[3]);
    plsm.initialize(npred, nresp, ncomp);
    plsm.plsr(X,Y, plsm, KERNEL_TYPE1);

    // A is number of components to use
    for (int A = 1; A<=ncomp; A++) { 
        // How well did we do with this many components?
        cerr << A << " components\t";
        cerr << "explained variance: " << plsm.explained_variance(X, Y, A);
        //cerr << "root mean squared error of prediction (RMSEP):" << plsm.rmsep(X, Y, A) << endl;
        cerr << " SSE: " << plsm.SSE(X,Y,A) <<  endl; 
    }

    
    cerr << "Validation (PRESS):\n";
    cerr << plsm.loo_validation(X, Y, PRESS) << endl;
    
    cerr << "Optimal number of components:\t" << plsm.optimal_num_components(X,Y) << endl;

    cerr << "Fitted Values:\n";
    //cerr << plsm.fitted_values(X,ncomp);
   
    cerr << "Regression coeeffientss:\n";
    cerr << plsm.coefficients(ncomp).transpose();
    cout << plsm.T;
   

/*
    cerr << "Validation (RMSEP):\n";
    cerr << plsm.loo_validation(X, Y, RMSEP) << endl;

    cerr << "W:\n";
    cerr << plsm.W.colwise().sum() << endl << endl;

    cerr << "P:\n";
    cerr << plsm.P.colwise().sum() << endl << endl;

    cerr << "Q:\n";
    cerr << plsm.Q.colwise().sum() << endl << endl;

    cerr << "R:\n";
    cerr << plsm.R.colwise().sum() << endl << endl;

    cerr << "X:\n";
    cerr << X.sum() << endl << endl;

    cerr << "Y:\n";
    cerr << Y.sum() << endl << endl;
*/

    return 0;
}

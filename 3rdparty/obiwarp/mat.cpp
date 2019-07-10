
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <numeric>
#include "mat.h"
#include "vec.h"

//#define JTP_BOUNDS_CHECK

//#define JTP_DEBUG

namespace VEC {

// BEGIN TEMPLATE

/****************************************************************
 * MatF
 ***************************************************************/

// Constructors:
MatF::MatF() : _m(0), _n(0), _dat(0) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatF()!");
#endif
}

MatF::MatF(int m, int n) : _m(m), _n(n), _dat(m*n) {
#ifdef JTP_BOUNDS_CHECK
    if (m < 0 || n < 0) { puts("m or n < 0"); exit(1); }
#endif
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatF(m,n)!");
#endif
}

MatF::MatF(int m, int n, const float &val) : _m(m), _n(n), _dat(m*n, val) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatF(m,n,val)!");
#endif
}

MatF::MatF(int m, int n, std::vector<float> arr) : _m(m), _n(n), _dat(m*n,arr) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatF(m,n,*arr,shallow) shallow=%d!\n", this->shallow());
#endif
}

MatF::MatF(const MatF &A) : _m(A._m), _n(A._n), _dat(A._dat) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatF(MatF &A,shallow) shallow=%d!\n", this->shallow());
#endif
}

std::vector<float> MatF::pointer(int m)
{
    return _dat.slice((m * _n), (m * _n) + _n);
}

void MatF::to_vec(VecF &outvec) {
    _dat.copy(outvec);
}

void MatF::set(int m, int n, std::vector<float> arr) {
    _dat.set(m*n,arr);
    _m = m;
    _n = n;
}

void MatF::set(MatF &A) {
    _dat.set(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("set called!");
#endif
}


void MatF::take(int m, int n, std::vector<float> arr) {
    _dat.take(m*n,arr);
    _m = m;
    _n = n;
}

void MatF::take(MatF &A) {
    // Checking is done in Vec to ensure we're not taking a shallow!
    _dat.take(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("take called!");
#endif
}

void MatF::row_vecs(int &cnt, VecF *vecs) {
    cnt = rows();
    int _cols = cols();
    for (int i = 0; i < cnt; ++i) {
        std::vector<float> ptr = this->pointer(i);
        vecs[i].set(_cols, ptr);  // shallow allocation
    }
}



bool MatF::operator==(const MatF &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n && A._m == _m) { // Same size
        return _dat == A._dat;
    }
    else {
        return false;
    }
}

void MatF::copy(MatF &receiver) const {
    receiver._m = _m;
    receiver._n = _n;
    _dat.copy(receiver._dat);
#ifdef JTP_DEBUG
    puts("copy called!");
#endif
}

MatF & MatF::operator=(const float &val) {
    _dat = val;
    return *this;
}

MatF & MatF::operator=(MatF &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _m = A._m;
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

MatF::~MatF( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

/*************************
 * MATH OPERATORS
 ************************/
void MatF::operator+=(const MatF &A) {
    if (A._n == _n && A._m == _m) {
        _dat += A._dat;
    }
}

void MatF::operator-=(const MatF &A) {
    if (A._n == _n && A._m == _m) {
        _dat -= A._dat;
    }
}

void MatF::operator*=(const MatF &A) {
    if (A._n == _n && A._m == _m) {
        _dat *= A._dat;
    }
}

void MatF::operator/=(const MatF &A) {
    if (A._n == _n && A._m == _m) {
        _dat /= A._dat;
    }
}

void MatF::add(const MatF &toadd, MatF &out) {
    if (_n == toadd._n && _m == toadd._m) {
        _dat.add(toadd._dat, out._dat);
    }
}

void MatF::sub(const MatF &tosub, MatF &out) {
    if (_n == tosub._n && _m == tosub._m) {
        _dat.sub(tosub._dat, out._dat);
    }
}

void MatF::mul(const MatF &tomul, MatF &out) {
    if (_n == tomul._n && _m == tomul._m) {
        _dat.mul(tomul._dat, out._dat);
    }
}

void MatF::div(const MatF &todiv, MatF &out) {
    if (_n == todiv._n && _m == todiv._m) {
        _dat.div(todiv._dat, out._dat);
    }
}

void MatF::transpose(MatF &out) {
    MatF me(*this);
    MatF tmp(me.nlen(), me.mlen());  // reverse m,n
    for (int m = 0; m < mlen(); ++m) {
        for (int n = 0; n < nlen(); ++n) {
            tmp(n,m) = me(m,n);
        }
    }
    out.take(tmp);
}

/*
MatF MatF::operator+(const MatF &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        MatF blank;
        return blank;
    }

else {
    MatF *C = new MatF(_n);
    MatF tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}

*/


    void MatF::expand(MatF &result, float match, int expand_x_lt, int expand_x_rt, int expand_y_up, int expand_y_dn, int expand_diag_lt_up, int expand_diag_rt_up, int expand_diag_lt_dn, int expand_diag_rt_dn ) {
    int i;
    int m_len = this->dim1();
    int n_len = this->dim2();
    this->copy(result);
    for (int m = 0; m < m_len; ++m) {
        for (int n = 0; n < n_len; ++n) {
            if ((*this)(m,n) == match) {
                for (i = 1; i <= expand_x_lt; ++i) {
                    if (n-i >= 0) {
                        result(m,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_x_rt; ++i) {
                    if (n+i < n_len) {
                        result(m,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_y_up; ++i) {
                    if (m-i >= 0) {
                        result(m-i,n) = match;
                    }
                }
                for (i = 1; i <= expand_y_dn; ++i) {
                    if (m+i < m_len) {
                        result(m+i,n) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_up; ++i) {
                    if (n-i >= 0 && m-i >=0) {
                        result(m-i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_up; ++i) {
                    if (n+i < n_len && m-i >= 0) {
                        result(m-i,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_dn; ++i) {
                    if (n-i >= 0 && m+i < m_len) {
                        result(m+i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_dn; ++i) {
                    if (n+i < n_len && m+i < m_len) {
                        result(m+i,n+i) = match;
                    }
                }
            }
        }
    }
}


void MatF::mask_as_vec(float return_val, MatI &mask, VecF &out) {
    _dat.mask_as_vec(return_val, mask._dat, out);
}


float MatF::sum(int m) {
    std::vector<float> ptr = pointer(m);
    float sum = std::accumulate(ptr.begin(), ptr.end(), 0.0f);
    return sum;
}


/****************************************************************
 * MatD
 ***************************************************************/

// Constructors:
MatD::MatD() : _m(0), _n(0), _dat(0) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatD()!");
#endif
}

MatD::MatD(int m, int n) : _m(m), _n(n), _dat(m*n) {
#ifdef JTP_BOUNDS_CHECK
    if (m < 0 || n < 0) { puts("m or n < 0"); exit(1); }
#endif
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatD(m,n)!");
#endif
}

MatD::MatD(int m, int n, const double &val) : _m(m), _n(n), _dat(m*n, val) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatD(m,n,val)!");
#endif
}

MatD::MatD(int m, int n, std::vector<double> arr) : _m(m), _n(n), _dat(m*n,arr) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatD(m,n,*arr,shallow) shallow=%d!\n", this->shallow());
#endif
}

MatD::MatD(const MatD &A) : _m(A._m), _n(A._n), _dat(A._dat) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatD(MatD &A,shallow) shallow=%d!\n", this->shallow());
#endif
}

std::vector<double> MatD::pointer(int m)
{
    return _dat.slice((m * _n), (m * _n) + _n);
}

void MatD::to_vec(VecD &outvec) {
    _dat.copy(outvec);
}

void MatD::set(int m, int n, std::vector<double> arr) {
    _dat.set(m*n,arr);
    _m = m;
    _n = n;
}

void MatD::set(MatD &A) {
    _dat.set(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("set called!");
#endif
}


void MatD::take(int m, int n, std::vector<double> arr) {
    _dat.take(m*n,arr);
    _m = m;
    _n = n;
}

void MatD::take(MatD &A) {
    // Checking is done in Vec to ensure we're not taking a shallow!
    _dat.take(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("take called!");
#endif
}

void MatD::row_vecs(int &cnt, VecD *vecs) {
    cnt = rows();
    int _cols = cols();
    for (int i = 0; i < cnt; ++i) {
        std::vector<double> ptr = this->pointer(i);
        vecs[i].set(_cols, ptr);  // shallow allocation
    }
}



bool MatD::operator==(const MatD &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n && A._m == _m) { // Same size
        return _dat == A._dat;
    }
    else {
        return false;
    }
}

void MatD::copy(MatD &receiver) const {
    receiver._m = _m;
    receiver._n = _n;
    _dat.copy(receiver._dat);
#ifdef JTP_DEBUG
    puts("copy called!");
#endif
}

MatD & MatD::operator=(const double &val) {
    _dat = val;
    return *this;
}

MatD & MatD::operator=(MatD &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _m = A._m;
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

MatD::~MatD( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

/*************************
 * MATH OPERATORS
 ************************/
void MatD::operator+=(const MatD &A) {
    if (A._n == _n && A._m == _m) {
        _dat += A._dat;
    }
}

void MatD::operator-=(const MatD &A) {
    if (A._n == _n && A._m == _m) {
        _dat -= A._dat;
    }
}

void MatD::operator*=(const MatD &A) {
    if (A._n == _n && A._m == _m) {
        _dat *= A._dat;
    }
}

void MatD::operator/=(const MatD &A) {
    if (A._n == _n && A._m == _m) {
        _dat /= A._dat;
    }
}

void MatD::add(const MatD &toadd, MatD &out) {
    if (_n == toadd._n && _m == toadd._m) {
        _dat.add(toadd._dat, out._dat);
    }
}

void MatD::sub(const MatD &tosub, MatD &out) {
    if (_n == tosub._n && _m == tosub._m) {
        _dat.sub(tosub._dat, out._dat);
    }
}

void MatD::mul(const MatD &tomul, MatD &out) {
    if (_n == tomul._n && _m == tomul._m) {
        _dat.mul(tomul._dat, out._dat);
    }
}

void MatD::div(const MatD &todiv, MatD &out) {
    if (_n == todiv._n && _m == todiv._m) {
        _dat.div(todiv._dat, out._dat);
    }
}

void MatD::transpose(MatD &out) {
    MatD me(*this);
    MatD tmp(me.nlen(), me.mlen());  // reverse m,n
    for (int m = 0; m < mlen(); ++m) {
        for (int n = 0; n < nlen(); ++n) {
            tmp(n,m) = me(m,n);
        }
    }
    out.take(tmp);
}

/*
MatD MatD::operator+(const MatD &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        MatD blank;
        return blank;
    }

else {
    MatD *C = new MatD(_n);
    MatD tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}

*/


    void MatD::expand(MatD &result, double match, int expand_x_lt, int expand_x_rt, int expand_y_up, int expand_y_dn, int expand_diag_lt_up, int expand_diag_rt_up, int expand_diag_lt_dn, int expand_diag_rt_dn ) {
    int i;
    int m_len = this->dim1();
    int n_len = this->dim2();
    this->copy(result);
    for (int m = 0; m < m_len; ++m) {
        for (int n = 0; n < n_len; ++n) {
            if ((*this)(m,n) == match) {
                for (i = 1; i <= expand_x_lt; ++i) {
                    if (n-i >= 0) {
                        result(m,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_x_rt; ++i) {
                    if (n+i < n_len) {
                        result(m,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_y_up; ++i) {
                    if (m-i >= 0) {
                        result(m-i,n) = match;
                    }
                }
                for (i = 1; i <= expand_y_dn; ++i) {
                    if (m+i < m_len) {
                        result(m+i,n) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_up; ++i) {
                    if (n-i >= 0 && m-i >=0) {
                        result(m-i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_up; ++i) {
                    if (n+i < n_len && m-i >= 0) {
                        result(m-i,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_dn; ++i) {
                    if (n-i >= 0 && m+i < m_len) {
                        result(m+i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_dn; ++i) {
                    if (n+i < n_len && m+i < m_len) {
                        result(m+i,n+i) = match;
                    }
                }
            }
        }
    }
}


void MatD::mask_as_vec(double return_val, MatI &mask, VecD &out) {
    _dat.mask_as_vec(return_val, mask._dat, out);
}


double MatD::sum(int m) {
    std::vector<double> ptr = pointer(m);
    double sum = std::accumulate(ptr.begin(), ptr.end(), 0.0);
    return sum;
}

/****************************************************************
 * MatI
 ***************************************************************/

// Constructors:
MatI::MatI() : _m(0), _n(0), _dat(0) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatI()!");
#endif
}

MatI::MatI(int m, int n) : _m(m), _n(n), _dat(m*n) {
#ifdef JTP_BOUNDS_CHECK
    if (m < 0 || n < 0) { puts("m or n < 0"); exit(1); }
#endif
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatI(m,n)!");
#endif
}

MatI::MatI(int m, int n, const int &val) : _m(m), _n(n), _dat(m*n, val) {
#ifdef JTP_DEBUG
    puts("CONSTRUCTOR MatI(m,n,val)!");
#endif
}

MatI::MatI(int m, int n, std::vector<int> arr) : _m(m), _n(n), _dat(m*n,arr) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatI(m,n,*arr,shallow) shallow=%d!\n", this->shallow());
#endif
}

MatI::MatI(const MatI &A) : _m(A._m), _n(A._n), _dat(A._dat) {
#ifdef JTP_DEBUG
    printf("CONSTRUCTOR MatI(MatI &A,shallow) shallow=%d!\n", this->shallow());
#endif
}

std::vector<int> MatI::pointer(int m)
{
    return _dat.slice((m * _n), (m * _n) + _n);
}

void MatI::to_vec(VecI &outvec) {
    _dat.copy(outvec);
}

void MatI::set(int m, int n, std::vector<int> arr) {
    _dat.set(m*n,arr);
    _m = m;
    _n = n;
}

void MatI::set(MatI &A) {
    _dat.set(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("set called!");
#endif
}


void MatI::take(int m, int n, std::vector<int> arr) {
    _dat.take(m*n,arr);
    _m = m;
    _n = n;
}

void MatI::take(MatI &A) {
    // Checking is done in Vec to ensure we're not taking a shallow!
    _dat.take(A._dat);
    _m = A._m;
    _n = A._n;
#ifdef JTP_DEBUG
    puts("take called!");
#endif
}

void MatI::row_vecs(int &cnt, VecI *vecs) {
    cnt = rows();
    int _cols = cols();
    for (int i = 0; i < cnt; ++i) {
        std::vector<int> ptr = this->pointer(i);
        vecs[i].set(_cols, ptr);  // shallow allocation
    }
}



bool MatI::operator==(const MatI &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n && A._m == _m) { // Same size
        return _dat == A._dat;
    }
    else {
        return false;
    }
}

void MatI::copy(MatI &receiver) const {
    receiver._m = _m;
    receiver._n = _n;
    _dat.copy(receiver._dat);
#ifdef JTP_DEBUG
    puts("copy called!");
#endif
}

MatI & MatI::operator=(const int &val) {
    _dat = val;
    return *this;
}

MatI & MatI::operator=(MatI &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _m = A._m;
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

MatI::~MatI( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

/*************************
 * MATH OPERATORS
 ************************/
void MatI::operator+=(const MatI &A) {
    if (A._n == _n && A._m == _m) {
        _dat += A._dat;
    }
}

void MatI::operator-=(const MatI &A) {
    if (A._n == _n && A._m == _m) {
        _dat -= A._dat;
    }
}

void MatI::operator*=(const MatI &A) {
    if (A._n == _n && A._m == _m) {
        _dat *= A._dat;
    }
}

void MatI::operator/=(const MatI &A) {
    if (A._n == _n && A._m == _m) {
        _dat /= A._dat;
    }
}

void MatI::add(const MatI &toadd, MatI &out) {
    if (_n == toadd._n && _m == toadd._m) {
        _dat.add(toadd._dat, out._dat);
    }
}

void MatI::sub(const MatI &tosub, MatI &out) {
    if (_n == tosub._n && _m == tosub._m) {
        _dat.sub(tosub._dat, out._dat);
    }
}

void MatI::mul(const MatI &tomul, MatI &out) {
    if (_n == tomul._n && _m == tomul._m) {
        _dat.mul(tomul._dat, out._dat);
    }
}

void MatI::div(const MatI &todiv, MatI &out) {
    if (_n == todiv._n && _m == todiv._m) {
        _dat.div(todiv._dat, out._dat);
    }
}

void MatI::transpose(MatI &out) {
    MatI me(*this);
    MatI tmp(me.nlen(), me.mlen());  // reverse m,n
    for (int m = 0; m < mlen(); ++m) {
        for (int n = 0; n < nlen(); ++n) {
            tmp(n,m) = me(m,n);
        }
    }
    out.take(tmp);
}

/*
MatI MatI::operator+(const MatI &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        MatI blank;
        return blank;
    }

else {
    MatI *C = new MatI(_n);
    MatI tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}

*/


    void MatI::expand(MatI &result, int match, int expand_x_lt, int expand_x_rt, int expand_y_up, int expand_y_dn, int expand_diag_lt_up, int expand_diag_rt_up, int expand_diag_lt_dn, int expand_diag_rt_dn ) {
    int i;
    int m_len = this->dim1();
    int n_len = this->dim2();
    this->copy(result);
    for (int m = 0; m < m_len; ++m) {
        for (int n = 0; n < n_len; ++n) {
            if ((*this)(m,n) == match) {
                for (i = 1; i <= expand_x_lt; ++i) {
                    if (n-i >= 0) {
                        result(m,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_x_rt; ++i) {
                    if (n+i < n_len) {
                        result(m,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_y_up; ++i) {
                    if (m-i >= 0) {
                        result(m-i,n) = match;
                    }
                }
                for (i = 1; i <= expand_y_dn; ++i) {
                    if (m+i < m_len) {
                        result(m+i,n) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_up; ++i) {
                    if (n-i >= 0 && m-i >=0) {
                        result(m-i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_up; ++i) {
                    if (n+i < n_len && m-i >= 0) {
                        result(m-i,n+i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_lt_dn; ++i) {
                    if (n-i >= 0 && m+i < m_len) {
                        result(m+i,n-i) = match;
                    }
                }
                for (i = 1; i <= expand_diag_rt_dn; ++i) {
                    if (n+i < n_len && m+i < m_len) {
                        result(m+i,n+i) = match;
                    }
                }
            }
        }
    }
}


void MatI::mask_as_vec(int return_val, MatI &mask, VecI &out) {
    _dat.mask_as_vec(return_val, mask._dat, out);
}


int MatI::sum(int m) {
    std::vector<int> ptr = pointer(m);
    int sum = std::accumulate(ptr.begin(), ptr.end(), 0);
    return sum;
}

// END TEMPLATE

} // End namespace VEC



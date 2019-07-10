#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "vec.h"


#ifndef min
	#define min(a,b) ( ( (a) < (b) ) ? (a) : (b) )
#endif
#ifndef max
	#define max(a,b) ( ( (a) > (b) ) ? (a) : (b) )
#endif

//#define JTP_BOUNDS_CHECK
//#define JTP_DEBUG

namespace VEC {

// BEGIN TEMPLATE

/****************************************************************
 * VecF
 ***************************************************************/

// Constructors:
VecF::VecF() : _n(0) {
#ifdef JTP_DEBUG
    puts("Creating DATA (NO ARGS)");
#endif
}

VecF::VecF(int n) : _n(n) {
#ifdef JTP_BOUNDS_CHECK
    if (n < 0) { puts("n < 0, exiting"); exit(1); }
#endif
    _dat.resize(_n);
#ifdef JTP_DEBUG
    puts("Creating DATA(N)");
#endif
}

VecF::VecF(int n, const float &val) : _n(n) {
    _dat.reserve(_n);
    for (int i = 0; i < _n; ++i) {
        _dat.push_back(val);
    }
#ifdef JTP_DEBUG
    puts("Creating DATA(N,float)");
#endif
}

VecF::VecF(int n, std::vector<float> arr) : _n(n), _dat(arr) {
#ifdef JTP_DEBUG
    puts("SHALLOW, (N,*ARR)");
#endif
}

VecF::VecF(const VecF &A) : _n(A._n) {
    _dat = A._dat;
#ifdef JTP_DEBUG
    puts("created with VecF(const VecF &A)");
#endif
}

void VecF::to_f(VecF &out) {
    VecF _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<float>(_dat[i]);
    }
    out.take(_tmp);
}

void VecF::to_i(VecI &out) {
    VecI _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<int>(_dat[i]);
    }
    out.take(_tmp);
}


void VecF::set(int n, std::vector<float> arr) {
    _dat = arr;
    _n = n;
}

void VecF::take(int n, std::vector<float> arr) {
    _dat = arr;
    _n = n;
}

void VecF::set(VecF &A) {
    _dat = A._dat;
    _n = A._n;
}


void VecF::take(VecF &A) {
    _dat = A._dat;
    _n = A._n;
}


bool VecF::operator==(const VecF &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n) { // Same size
        if (A._dat == _dat)
            return true; // Same data
    }
    return false;
}

void VecF::copy(VecF &receiver) const {
    receiver._n = _n;
    receiver._dat = _dat;
}

VecF & VecF::operator=(const float &val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] = val;
    }
    return *this;
}

VecF & VecF::operator=(VecF &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

VecF::~VecF( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

std::vector<float> VecF::slice(int start, int end)
{
    std::vector<float> out(&_dat[start], &_dat[end]);
    return out;
}

/*************************
 * MATH OPERATORS
 ************************/

void VecF::operator+=(float val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] += val;
    }
}

void VecF::operator-=(float val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= val;
    }
}

void VecF::operator*=(float val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= val;
    }
}

void VecF::operator/=(float val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= val;
    }
}

void VecF::operator+=(const VecF &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] += A[i];
    }
}

void VecF::operator-=(const VecF &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= A[i];
    }
}

void VecF::operator*=(const VecF &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= A[i];
    }
}

void VecF::operator/=(const VecF &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= A[i];
    }
}

void VecF::add(const VecF &toadd, VecF &out) {
    if (toadd._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] + toadd[i]);
        }
        out._n = _n;
    }
}

void VecF::sub(const VecF &tosub, VecF &out) {
    if (tosub._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] - tosub[i]);
        }
        out._n = _n;
    }
}

void VecF::mul(const VecF &tomul, VecF &out) {
    if (tomul._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] * tomul[i]);
        }
        out._n = _n;
    }
}

void VecF::div(const VecF &todiv, VecF &out) {
    if (todiv._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] / todiv[i]);
        }
        out._n = _n;
    }
}

void VecF::square_root() {
    std::transform(_dat.begin(),
                   _dat.end(),
                   _dat.begin(),
                   sqrtf);
}

/*
VecF VecF::operator+(const VecF &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        VecF blank;
        return blank;
    }

else {
    VecF *C = new VecF(_n);
    VecF tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}
*/

float VecF::sum() {
    return std::accumulate(_dat.begin(), _dat.end(), 0.0f);
}

char * VecF::class_name() {
    char *name = new char[7];
    strcpy(name, "VecF");
    return name;
}

void VecF::abs_val() {
    for (int n = 0; n < _n; ++n) {
        if (_dat[n] < 0) { _dat[n] *= -1; }
    }
}

void VecF::std_normal() {
    // @TODO: would like avg and stdev to stay double, even for floats!
    (*this) -= static_cast<float>(this->avg());
    double mean, stdev;
    this->sample_stats(mean, stdev);
    (*this) /= static_cast<float>(stdev);
}

void VecF::remove(int index) {
    _dat.erase(_dat.begin() + index);
    _n = _n - 1;
}

void VecF::sort() {
    std::sort(_dat.begin(), _dat.end());
}

int VecF::index(float val) {
    auto result = std::find(_dat.begin(), _dat.end(), val);
    if (result != _dat.end()) {
        return (result - _dat.begin());
    }
    return -1;
}

double VecF::avg() const {
    double total = std::accumulate(_dat.begin(), _dat.end(), 0.0);
    return total / static_cast<double>(_n);
}

//double VecF::prob_one_side_right(double x) {
//    double _mean;
//    double _sigma;
//    // zScore:
//    sample_stats( _mean, _sigma );
//    //printf("mean: %f sigma: %f\n", _mean, _sigma);
//
//    // @TODO: THIS IS BROKEN until I can figure out how to calc the normalCDF
//    // using public domain sources:
//    //return _normalCDF(_mean, _sigma, x);
//}

void VecF::sample_stats(double &mean, double &std_dev) {
    // Raw score method (below) is faster (~1.5X) than deviation score method
    // commonly used
    double _sum = 0.0;
    double _val = 0.0;
    double _sumSq = 0.0;
    int _len = this->dim();
    for( int i=0; i<_len; ++i ) {
        _val = static_cast<double>(_dat[i]);
        _sum += _val;
        _sumSq += _val *_val;
    }
    double tmp = _sumSq - ((_sum * _sum)/_len);
    tmp /= _len>1 ? _len-1 : 1;
#ifdef WIN32
    std_dev = sqrt( tmp );
#else
    std_dev = std::sqrt( tmp );
#endif
    mean = _sum/_len;
}

double VecF::_zScore(double mean, double sigma, double x) {
    return (x - mean)/(sigma == 0.0 ? 1E-20: sigma);
}

void VecF::mask_as_vec(float return_val, VecI &mask, VecF &out) {
    if (mask.size() != _n) { puts("mask.size() != this->length()"); exit(1); }
    std::vector<float> tmparr;
    tmparr.reserve(_n);
    int newcnt = 0;
    for (int i = 0; i < _n; ++i) {
        if (mask[i] == return_val) {
            tmparr.push_back(_dat[i]);
            ++newcnt;
        }
    }
    out.take(newcnt, tmparr);
}

void VecF::hist(int num_bins, VecD &bins, VecI &freqs) {
    // Create the scaling factor
    float _min = 0.0f;
    float _max = 0.0f;
    min_max(_min, _max);
    double dmin = static_cast<double>(_min);
    double conv = static_cast<double>(num_bins)/static_cast<double>(_max - _min);

    // initialize arrays
    VecD _bins(num_bins);
    VecI _freqs(num_bins, 0);
    int _len = this->dim();

    // Create the histogram:
    for (int i = 0; i < _len; ++i) {
        int index = static_cast<int>((_dat[i] - _min) * conv);
        if (index == num_bins) {
            --index;
        }
        _freqs[index]++;
    }

    // Create the bins:
    double iconv = 1.0/conv;
    for (int i = 0; i < num_bins; ++i) {
        _bins[i] = ((i+0.5) * iconv) + dmin;  // avg
        // _bins[i] = ((i+0.5) * iconv) + dmin; //min
    }

    bins.take(_bins);
    freqs.take(_freqs);
}

void VecF::logarithm(double base) {
    for (int i = 0; i < _n; ++i) {
        //printf("ME: %f\n", me[i]);
        _dat[i] = static_cast<float>(log(static_cast<double>(_dat[i]))
                                     / log(base));
        //printf("MELOGGED: %f\n", me[i]);
    }
}

void VecF::min_max(float &mn, float &mx) {
    mn = *(std::min_element(_dat.begin(), _dat.end()));
    mx = *(std::max_element(_dat.begin(), _dat.end()));
}

// Class functions:
// THIS MUST BE FOR float AND DOUBLE ONLY!!!
// This is a fairly precise Fortran->C translation of the SLATEC chim code
// Evaluate the deriv at each x point
// return 1 if less than 2 data points
// return 0 if no errors
// ASSUMES monotonicity of the X data points !!!!!
// ASSUMES that this->length() >= 2
// If length == 1 then derivs[0] is set to 0
// If length == 0 then prints message to STDERR and returns;
void VecF::chim(VecF &x, VecF &y, VecF &out_derivs) {
    std::vector<float> tmp_derivs;
    tmp_derivs.resize(x.length());
    // if they aren't the right type then warn

    //if (typeid(T) != typeid(float) || typeid(T) != typeid(double))
    //    printf("vec2 calling object must be of type float or double!\n");
    //    exit(2);
    // }

#ifdef JTP_BOUNDS_CHECK
    if (x.length() != y.length()) { puts("x.length() != y.length()"); exit(1); }
#endif
    int length = x.length();
    float del1;
    float del2;
    float h1;
    float h2;
    float hsum;
    float w1;
    float w2;
    float dmax;
    float dmin;
    float three = 3.0f;
    float dsave;
    float drat1;
    float drat2;
    float hsumt3;

    int ierr = 0;
    int lengthLess1 = length - 1;

    if (length < 2) {
        if (length == 1) {
            tmp_derivs[0] = 0;
            return;
        }
        else {
            std::cerr << "trying to chim with 0 data points!\n";
        }
    }

    // THIS can be done BEFORE this routine if someone cares to...
    //    // Check monotonicity
    //    for (int i = 2; i < length; i++) {
    //        if (x[i] <= x[i-1]) {
//            return 2;
    //        }
    //    }

    h1 = x[1] - x[0];
    del1 = (y[1] - y[0]) / h1;
    dsave = del1;

    // special case length=2 --use linear interpolation
    if (lengthLess1 < 2) {
        tmp_derivs[0] = del1;
        tmp_derivs[1] = del1;
        out_derivs.take(3, tmp_derivs);
        return;
    }

    // Normal case (length >= 3)
    // 10

    h2 = x[2] - x[1];
    del2 = (y[2] - y[1]) / h2;

    // SET D(1) VIA NON-CENTERED THREE-POINT FORMULA, ADJUSTED TO BE
    //     SHAPE-PRESERVING.

    hsum = h1 + h2;
    w1 = (h1 + hsum)/hsum;
    w2 = -h1/hsum;
    tmp_derivs[0] = (w1*del1) + (w2*del2);
    if ( pchst(tmp_derivs[0],del1) <= 0 ) {
        tmp_derivs[0] = 0.0f;
    }
    else if ( pchst(del1,del2) < 0 ) {
        // need to do this check only if monotonicity switches
        dmax = three * del1;
        if (fabs(tmp_derivs[0]) > fabs(dmax)) {
            tmp_derivs[0] = dmax;
        }
    }

    int pchstval;
    int ind;

    for (ind = 1; ind < lengthLess1; ind++) {
        if (ind != 1) {
            h1 = h2;
            h2 = x[ind+1] - x[ind];
            hsum = h1 + h2;
            del1 = del2;
            del2 = (y[ind+1] - y[ind])/h2;
        }
        // 40
        tmp_derivs[ind] = 0.0f;

        pchstval = pchst(del1,del2);

        // 45
        if (pchstval > 0) {
            hsumt3 = hsum+hsum+hsum;
            w1 = (hsum + h1)/hsumt3;
            w2 = (hsum + h2)/hsumt3;
            dmax = static_cast<float>(max(fabs(del1), fabs(del2)));
            dmin = static_cast<float>(min(fabs(del1), fabs(del2)));
            drat1 = del1/dmax;
            drat2 = del2/dmax;
            tmp_derivs[ind] = dmin/(w1*drat1 + w2*drat2);
        }
        // 42
        else if (pchstval < 0 ) {
            ierr = ierr + 1;
            dsave = del2;
            continue;
        }
        // 41
        else {  // equal to zero
            if (del2 == 0.0f) { continue; }
            if (VecF::pchst(dsave,del2) < 0) { ierr = ierr + 1; }
            dsave = del2;
            continue;
        }
    }

    // 50
    w1 = -h2/hsum;
    w2 = (h2 + hsum)/hsum;
    tmp_derivs[ind] = w1*del1 + w2*del2;
    if ( VecF::pchst(tmp_derivs[ind],del2) <= 0 ) {
        tmp_derivs[ind] = 0.0f;
    }
    else if ( VecF::pchst(del1, del2) < 0) {
        // NEED DO THIS CHECK ONLY IF MONOTONICITY SWITCHES.
        dmax = three*del2;
        if (fabs(tmp_derivs[ind]) > fabs(dmax)) {
            tmp_derivs[ind] = dmax;
        }
    }
    out_derivs.take(length, tmp_derivs);
    return;
}

void VecF::xy_to_x(VecF &x, VecF &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] - x._dat[i];
    }
}

void VecF::x_to_xy(VecF &x, VecF &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] + x._dat[i];
    }
}

void VecF::linear_derivs(VecF &x, VecF &y, VecF &out_derivs) {
    VecF tmp_d(x.size());
    for (int i = 0; i < x.size(); ++i) {
        tmp_d[i] = (y[i+1] - y[i]) / (x[i+1]-x[i]);
    }
    out_derivs.take(tmp_d);
}

void VecF::linear_interp(VecF &xin, VecF &yin, VecF &xe, VecF &out_ye, int sorted) {

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecF derivs;
    VecF::linear_derivs(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;
    float dt;

    if (sorted) {
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt*derivs[ifirst]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt * ((yin[ir] - yin[ifirst]) / (xin[ir]-xin[ifirst])) );
        }
    }
}

float VecF::sum_of_sq() {
    double total = accumulate(_dat.begin(),
                              _dat.end(),
                              0.0,
                              [](double s, double x) { return s + (x * x); });
    return static_cast<float>(total);
}

double VecF::pearsons_r(VecF &x, VecF &y) {

    // Preparation:
    double sum_xTy = VecF::dot_product(x,y);
    double sum_x = x.sum();
    double sum_y = y.sum();
    // Could this step be sped up?
    double sum_x2 = x.sum_of_sq();
    double sum_y2 = y.sum_of_sq();
    int N = x.dim();

    // Here it is:
    // 'E' is Capital Sigma
    // r = EXY - (EXEY/N)
    //    -----------------
    //    sqrt( (EX^2 - (EX)^2/N) * (EY^2 - (EY)^2/N) )

    double top = sum_xTy - ((sum_x * sum_y)/N);
    double fbot = sum_x2 - ((sum_x*sum_x)/N);  //first part of bottom
    double sbot = sum_y2 - ((sum_y*sum_y)/N);  //second part of bottom
    return top / sqrt(fbot * sbot);

}

double VecF::covariance(VecF &x, VecF &y) {
    int i;
    int len = x.size();
    double mean_x = 0;
    double mean_y = 0;
    // get the means and x * y
    for (i = 0; i < len; ++i) {
        mean_x += x[i];
        mean_y += y[i];
    }
    mean_x /= len;
    mean_y /= len;
    double cov = 0;
    for (i = 0; i < len; ++i) {
        cov += (x[i] - mean_x) * (y[i] - mean_y);
    }
    return cov/len;
}

double VecF::euclidean(VecF &x, VecF &y) {
    VecF diff(x.size());
    double sum_of_diffs = 0;
    for (int i = 0; i < x.size(); ++i) {
        sum_of_diffs += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sqrt(sum_of_diffs);
}

float VecF::dot_product(VecF &x, VecF &y) {
    //assert(x.dim() == y.dim());
    float sum = 0;
    for (int i = 0; i < x.dim(); i++) {
        sum += (x[i] * y[i]);
    }
    return sum;
}

void VecF::chfe(VecF &xin, VecF &yin, VecF &xe, VecF &out_ye, int sorted) {
    //xin.print(); yin.print();

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecF derivs;
    VecF::chim(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;


    if (sorted) {
        VecF c2(xin.size());
        VecF c3(xin.size());
        calc_cubic_coeff(xin, yin, derivs, c2, c3);
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev(xin[ifirst], yin[ifirst], derivs[ifirst], c2[ifirst], c3[ifirst], xe[j], out_ye[j]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev_all(xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j], out_ye[j]);
        }
    }
}

void VecF::calc_cubic_coeff(VecF &x, VecF &y, VecF &derivs, VecF &c2, VecF &c3) {

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    float DEL1, DEL2, DELTA, H;
    for (int i = 0; i < x.size() - 1; ++i) {
        H = x[i+1] - x[i];
        DELTA = (y[i+1] - y[i])/H;
        DEL1 = (derivs[i] - DELTA)/H;
        DEL2 = (derivs[i+1] - DELTA)/H;
        c2[i] = -(DEL1+DEL1 + DEL2);
        c3[i] = (DEL1 + DEL2)/H;
    }

}

void VecF::chfev_all(float X1, float X2, float F1, float F2, float D1, float D2, float XE, float &FE) {
    float C2, C3, DEL1, DEL2, DELTA, H, X;

    H = X2 - X1;

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    DELTA = (F2 - F1)/H;
    DEL1 = (D1 - DELTA)/H;
    DEL2 = (D2 - DELTA)/H;
    C2 = -(DEL1+DEL1 + DEL2);
    C3 = (DEL1 + DEL2)/H;

    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecF::chfev(float X1, float F1, float D1, float C2, float C3, float XE, float &FE) {
    float X;
    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecF::chfe_xy(VecF &x, VecF &y, VecF &new_x, VecF &out_new_y, int sorted) {
    VecF::xy_to_x(x,y);
    chfe(x,y,new_x, out_new_y, sorted);
    x_to_xy(new_x, out_new_y);
    VecF::x_to_xy(x,y);
}

double VecF::sum_sq_res_yeqx(VecF &x, VecF &y) {
    //// PDL way
    //return sum(0.5*(($y - $x)**2));
    double __sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        float diff = x[i] - y[i];
        __sum += 0.5*(diff*diff);
    }
    return __sum;
}

double VecF::avg_sq_res_yeqx(VecF &x, VecF &y) {
    return (sum_sq_res_yeqx(x,y))/x.length();
}

double VecF::avg_abs_diff(VecF &x, VecF &y) {
    double sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        sum += fabs(x[i] - y[i]);
    }
    return sum/x.length();
}

void VecF::rsq_slope_intercept(VecF &x, VecF &y, double &rsq, double &slope, double &y_intercept) {
    int i;
    double mean_x = x.avg();
    double mean_y = y.avg();
    double sum_sq_res_xx = 0.0;
    double sum_sq_res_yy = 0.0;
    double sum_sq_res_xy = 0.0;
    //    double *sq_res_xx = new double[x.length()];
    //    double *sq_res_yy = new double[y.length()];
    //    double *sq_res_xy = new double[x.length()];
    //VecD sq_res_xx(x.length());
    //VecD sq_res_yy(x.length());
    //VecD sq_res_xy(x.length());
    for (i = 0; i < x.length(); ++i) {
        double x_minus_mean_i, y_minus_mean_i;
        x_minus_mean_i = ( static_cast<double>(x[i]) ) - mean_x;
        y_minus_mean_i = ( static_cast<double>(y[i]) ) - mean_y;
        sum_sq_res_xx += x_minus_mean_i*x_minus_mean_i;
        sum_sq_res_yy += y_minus_mean_i*y_minus_mean_i;
        sum_sq_res_xy += x_minus_mean_i*y_minus_mean_i;
    }
    slope = sum_sq_res_xy/sum_sq_res_xx;
    y_intercept = mean_y - (slope * mean_x);
    rsq = (sum_sq_res_xy*sum_sq_res_xy)/(sum_sq_res_xx*sum_sq_res_yy);
    //    delete[] sq_res_xx;
    //    delete[] sq_res_yy;
    //    delete[] sq_res_xy;
}

/****************************************************************
 * VecD
 ***************************************************************/

// Constructors:
VecD::VecD() : _n(0) {
#ifdef JTP_DEBUG
    puts("Creating DATA (NO ARGS)");
#endif
}

VecD::VecD(int n) : _n(n) {
#ifdef JTP_BOUNDS_CHECK
    if (n < 0) { puts("n < 0, exiting"); exit(1); }
#endif
    _dat.resize(_n);
#ifdef JTP_DEBUG
    puts("Creating DATA(N)");
#endif
}

VecD::VecD(int n, const double &val) : _n(n) {
    _dat.reserve(_n);
    for (int i = 0; i < _n; ++i) {
        _dat.push_back(val);
    }
#ifdef JTP_DEBUG
    puts("Creating DATA(N,double)");
#endif
}

VecD::VecD(int n, std::vector<double> arr) : _n(n), _dat(arr) {
#ifdef JTP_DEBUG
    puts("SHALLOW, (N,*ARR)");
#endif
}

VecD::VecD(const VecD &A) : _n(A._n) {
    _dat = A._dat;
#ifdef JTP_DEBUG
    puts("created with VecD(const VecD &A)");
#endif
}

void VecD::to_f(VecD &out) {
    VecD _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<double>(_dat[i]);
    }
    out.take(_tmp);
}

void VecD::to_i(VecI &out) {
    VecI _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<int>(_dat[i]);
    }
    out.take(_tmp);
}


void VecD::set(int n, std::vector<double> arr) {
    _dat = arr;
    _n = n;
}

void VecD::take(int n, std::vector<double> arr) {
    _dat = arr;
    _n = n;
}

void VecD::set(VecD &A) {
    _dat = A._dat;
    _n = A._n;
}


void VecD::take(VecD &A) {
    _dat = A._dat;
    _n = A._n;
}


bool VecD::operator==(const VecD &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n) { // Same size
        if (A._dat == _dat)
            return true; // Same data
    }
    return false;
}

void VecD::copy(VecD &receiver) const {
    receiver._n = _n;
    receiver._dat = _dat;
}

VecD & VecD::operator=(const double &val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] = val;
    }
    return *this;
}

VecD & VecD::operator=(VecD &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

VecD::~VecD( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

std::vector<double> VecD::slice(int start, int end)
{
    std::vector<double> out(&_dat[start], &_dat[end]);
    return out;
}

/*************************
 * MATH OPERATORS
 ************************/

void VecD::operator+=(double val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] += val;
    }
}

void VecD::operator-=(double val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= val;
    }
}

void VecD::operator*=(double val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= val;
    }
}

void VecD::operator/=(double val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= val;
    }
}

void VecD::operator+=(const VecD &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] += A[i];
    }
}

void VecD::operator-=(const VecD &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= A[i];
    }
}

void VecD::operator*=(const VecD &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= A[i];
    }
}

void VecD::operator/=(const VecD &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= A[i];
    }
}

void VecD::add(const VecD &toadd, VecD &out) {
    if (toadd._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] + toadd[i]);
        }
        out._n = _n;
    }
}

void VecD::sub(const VecD &tosub, VecD &out) {
    if (tosub._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] - tosub[i]);
        }
        out._n = _n;
    }
}

void VecD::mul(const VecD &tomul, VecD &out) {
    if (tomul._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] * tomul[i]);
        }
        out._n = _n;
    }
}

void VecD::div(const VecD &todiv, VecD &out) {
    if (todiv._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] / todiv[i]);
        }
        out._n = _n;
    }
}

void VecD::square_root() {
    std::transform(_dat.begin(),
                   _dat.end(),
                   _dat.begin(),
                   sqrtf);
}

/*
VecD VecD::operator+(const VecD &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        VecD blank;
        return blank;
    }

else {
    VecD *C = new VecD(_n);
    VecD tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}
*/

double VecD::sum() {
    return std::accumulate(_dat.begin(), _dat.end(), 0.0f);
}

char * VecD::class_name() {
    char *name = new char[7];
    strcpy(name, "VecD");
    return name;
}

void VecD::abs_val() {
    for (int n = 0; n < _n; ++n) {
        if (_dat[n] < 0) { _dat[n] *= -1; }
    }
}

void VecD::std_normal() {
    // @TODO: would like avg and stdev to stay double, even for doubles!
    (*this) -= static_cast<double>(this->avg());
    double mean, stdev;
    this->sample_stats(mean, stdev);
    (*this) /= static_cast<double>(stdev);
}

void VecD::remove(int index) {
    _dat.erase(_dat.begin() + index);
    _n = _n - 1;
}

void VecD::sort() {
    std::sort(_dat.begin(), _dat.end());
}

int VecD::index(double val) {
    auto result = std::find(_dat.begin(), _dat.end(), val);
    if (result != _dat.end()) {
        return (result - _dat.begin());
    }
    return -1;
}

double VecD::avg() const {
    double total = std::accumulate(_dat.begin(), _dat.end(), 0.0);
    return total / static_cast<double>(_n);
}

//double VecD::prob_one_side_right(double x) {
//    double _mean;
//    double _sigma;
//    // zScore:
//    sample_stats( _mean, _sigma );
//    //printf("mean: %f sigma: %f\n", _mean, _sigma);
//
//    // @TODO: THIS IS BROKEN until I can figure out how to calc the normalCDF
//    // using public domain sources:
//    //return _normalCDF(_mean, _sigma, x);
//}

void VecD::sample_stats(double &mean, double &std_dev) {
    // Raw score method (below) is faster (~1.5X) than deviation score method
    // commonly used
    double _sum = 0.0;
    double _val = 0.0;
    double _sumSq = 0.0;
    int _len = this->dim();
    for( int i=0; i<_len; ++i ) {
        _val = static_cast<double>(_dat[i]);
        _sum += _val;
        _sumSq += _val *_val;
    }
    double tmp = _sumSq - ((_sum * _sum)/_len);
    tmp /= _len>1 ? _len-1 : 1;
#ifdef WIN32
    std_dev = sqrt( tmp );
#else
    std_dev = std::sqrt( tmp );
#endif
    mean = _sum/_len;
}

double VecD::_zScore(double mean, double sigma, double x) {
    return (x - mean)/(sigma == 0.0 ? 1E-20: sigma);
}

void VecD::mask_as_vec(double return_val, VecI &mask, VecD &out) {
    if (mask.size() != _n) { puts("mask.size() != this->length()"); exit(1); }
    std::vector<double> tmparr;
    tmparr.reserve(_n);
    int newcnt = 0;
    for (int i = 0; i < _n; ++i) {
        if (mask[i] == return_val) {
            tmparr.push_back(_dat[i]);
            ++newcnt;
        }
    }
    out.take(newcnt, tmparr);
}

void VecD::hist(int num_bins, VecD &bins, VecI &freqs) {
    // Create the scaling factor
    double _min = 0.0f;
    double _max = 0.0f;
    min_max(_min, _max);
    double dmin = static_cast<double>(_min);
    double conv = static_cast<double>(num_bins)/static_cast<double>(_max - _min);

    // initialize arrays
    VecD _bins(num_bins);
    VecI _freqs(num_bins, 0);
    int _len = this->dim();

    // Create the histogram:
    for (int i = 0; i < _len; ++i) {
        int index = static_cast<int>((_dat[i] - _min) * conv);
        if (index == num_bins) {
            --index;
        }
        _freqs[index]++;
    }

    // Create the bins:
    double iconv = 1.0/conv;
    for (int i = 0; i < num_bins; ++i) {
        _bins[i] = ((i+0.5) * iconv) + dmin;  // avg
        // _bins[i] = ((i+0.5) * iconv) + dmin; //min
    }

    bins.take(_bins);
    freqs.take(_freqs);
}

void VecD::logarithm(double base) {
    for (int i = 0; i < _n; ++i) {
        //printf("ME: %f\n", me[i]);
        _dat[i] = static_cast<double>(log(static_cast<double>(_dat[i]))
                                     / log(base));
        //printf("MELOGGED: %f\n", me[i]);
    }
}

void VecD::min_max(double &mn, double &mx) {
    mn = *(std::min_element(_dat.begin(), _dat.end()));
    mx = *(std::max_element(_dat.begin(), _dat.end()));
}

// Class functions:
// THIS MUST BE FOR double AND DOUBLE ONLY!!!
// This is a fairly precise Fortran->C translation of the SLATEC chim code
// Evaluate the deriv at each x point
// return 1 if less than 2 data points
// return 0 if no errors
// ASSUMES monotonicity of the X data points !!!!!
// ASSUMES that this->length() >= 2
// If length == 1 then derivs[0] is set to 0
// If length == 0 then prints message to STDERR and returns;
void VecD::chim(VecD &x, VecD &y, VecD &out_derivs) {
    std::vector<double> tmp_derivs;
    tmp_derivs.resize(x.length());
    // if they aren't the right type then warn

    //if (typeid(T) != typeid(double) || typeid(T) != typeid(double))
    //    printf("vec2 calling object must be of type double or double!\n");
    //    exit(2);
    // }

#ifdef JTP_BOUNDS_CHECK
    if (x.length() != y.length()) { puts("x.length() != y.length()"); exit(1); }
#endif
    int length = x.length();
    double del1;
    double del2;
    double h1;
    double h2;
    double hsum;
    double w1;
    double w2;
    double dmax;
    double dmin;
    double three = 3.0f;
    double dsave;
    double drat1;
    double drat2;
    double hsumt3;

    int ierr = 0;
    int lengthLess1 = length - 1;

    if (length < 2) {
        if (length == 1) {
            tmp_derivs[0] = 0;
            return;
        }
        else {
            std::cerr << "trying to chim with 0 data points!\n";
        }
    }

    // THIS can be done BEFORE this routine if someone cares to...
    //    // Check monotonicity
    //    for (int i = 2; i < length; i++) {
    //        if (x[i] <= x[i-1]) {
//            return 2;
    //        }
    //    }

    h1 = x[1] - x[0];
    del1 = (y[1] - y[0]) / h1;
    dsave = del1;

    // special case length=2 --use linear interpolation
    if (lengthLess1 < 2) {
        tmp_derivs[0] = del1;
        tmp_derivs[1] = del1;
        out_derivs.take(3, tmp_derivs);
        return;
    }

    // Normal case (length >= 3)
    // 10

    h2 = x[2] - x[1];
    del2 = (y[2] - y[1]) / h2;

    // SET D(1) VIA NON-CENTERED THREE-POINT FORMULA, ADJUSTED TO BE
    //     SHAPE-PRESERVING.

    hsum = h1 + h2;
    w1 = (h1 + hsum)/hsum;
    w2 = -h1/hsum;
    tmp_derivs[0] = (w1*del1) + (w2*del2);
    if ( pchst(tmp_derivs[0],del1) <= 0 ) {
        tmp_derivs[0] = 0.0f;
    }
    else if ( pchst(del1,del2) < 0 ) {
        // need to do this check only if monotonicity switches
        dmax = three * del1;
        if (fabs(tmp_derivs[0]) > fabs(dmax)) {
            tmp_derivs[0] = dmax;
        }
    }

    int pchstval;
    int ind;

    for (ind = 1; ind < lengthLess1; ind++) {
        if (ind != 1) {
            h1 = h2;
            h2 = x[ind+1] - x[ind];
            hsum = h1 + h2;
            del1 = del2;
            del2 = (y[ind+1] - y[ind])/h2;
        }
        // 40
        tmp_derivs[ind] = 0.0f;

        pchstval = pchst(del1,del2);

        // 45
        if (pchstval > 0) {
            hsumt3 = hsum+hsum+hsum;
            w1 = (hsum + h1)/hsumt3;
            w2 = (hsum + h2)/hsumt3;
            dmax = static_cast<double>(max(fabs(del1), fabs(del2)));
            dmin = static_cast<double>(min(fabs(del1), fabs(del2)));
            drat1 = del1/dmax;
            drat2 = del2/dmax;
            tmp_derivs[ind] = dmin/(w1*drat1 + w2*drat2);
        }
        // 42
        else if (pchstval < 0 ) {
            ierr = ierr + 1;
            dsave = del2;
            continue;
        }
        // 41
        else {  // equal to zero
            if (del2 == 0.0f) { continue; }
            if (VecD::pchst(dsave,del2) < 0) { ierr = ierr + 1; }
            dsave = del2;
            continue;
        }
    }

    // 50
    w1 = -h2/hsum;
    w2 = (h2 + hsum)/hsum;
    tmp_derivs[ind] = w1*del1 + w2*del2;
    if ( VecD::pchst(tmp_derivs[ind],del2) <= 0 ) {
        tmp_derivs[ind] = 0.0f;
    }
    else if ( VecD::pchst(del1, del2) < 0) {
        // NEED DO THIS CHECK ONLY IF MONOTONICITY SWITCHES.
        dmax = three*del2;
        if (fabs(tmp_derivs[ind]) > fabs(dmax)) {
            tmp_derivs[ind] = dmax;
        }
    }
    out_derivs.take(length, tmp_derivs);
    return;
}

void VecD::xy_to_x(VecD &x, VecD &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] - x._dat[i];
    }
}

void VecD::x_to_xy(VecD &x, VecD &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] + x._dat[i];
    }
}

void VecD::linear_derivs(VecD &x, VecD &y, VecD &out_derivs) {
    VecD tmp_d(x.size());
    for (int i = 0; i < x.size(); ++i) {
        tmp_d[i] = (y[i+1] - y[i]) / (x[i+1]-x[i]);
    }
    out_derivs.take(tmp_d);
}

void VecD::linear_interp(VecD &xin, VecD &yin, VecD &xe, VecD &out_ye, int sorted) {

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecD derivs;
    VecD::linear_derivs(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;
    double dt;

    if (sorted) {
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt*derivs[ifirst]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt * ((yin[ir] - yin[ifirst]) / (xin[ir]-xin[ifirst])) );
        }
    }
}

double VecD::sum_of_sq() {
    double total = accumulate(_dat.begin(),
                              _dat.end(),
                              0.0,
                              [](double s, double x) { return s + (x * x); });
    return static_cast<double>(total);
}

double VecD::pearsons_r(VecD &x, VecD &y) {

    // Preparation:
    double sum_xTy = VecD::dot_product(x,y);
    double sum_x = x.sum();
    double sum_y = y.sum();
    // Could this step be sped up?
    double sum_x2 = x.sum_of_sq();
    double sum_y2 = y.sum_of_sq();
    int N = x.dim();

    // Here it is:
    // 'E' is Capital Sigma
    // r = EXY - (EXEY/N)
    //    -----------------
    //    sqrt( (EX^2 - (EX)^2/N) * (EY^2 - (EY)^2/N) )

    double top = sum_xTy - ((sum_x * sum_y)/N);
    double fbot = sum_x2 - ((sum_x*sum_x)/N);  //first part of bottom
    double sbot = sum_y2 - ((sum_y*sum_y)/N);  //second part of bottom
    return top / sqrt(fbot * sbot);

}

double VecD::covariance(VecD &x, VecD &y) {
    int i;
    int len = x.size();
    double mean_x = 0;
    double mean_y = 0;
    // get the means and x * y
    for (i = 0; i < len; ++i) {
        mean_x += x[i];
        mean_y += y[i];
    }
    mean_x /= len;
    mean_y /= len;
    double cov = 0;
    for (i = 0; i < len; ++i) {
        cov += (x[i] - mean_x) * (y[i] - mean_y);
    }
    return cov/len;
}

double VecD::euclidean(VecD &x, VecD &y) {
    VecD diff(x.size());
    double sum_of_diffs = 0;
    for (int i = 0; i < x.size(); ++i) {
        sum_of_diffs += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sqrt(sum_of_diffs);
}

double VecD::dot_product(VecD &x, VecD &y) {
    //assert(x.dim() == y.dim());
    double sum = 0;
    for (int i = 0; i < x.dim(); i++) {
        sum += (x[i] * y[i]);
    }
    return sum;
}

void VecD::chfe(VecD &xin, VecD &yin, VecD &xe, VecD &out_ye, int sorted) {
    //xin.print(); yin.print();

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecD derivs;
    VecD::chim(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;


    if (sorted) {
        VecD c2(xin.size());
        VecD c3(xin.size());
        calc_cubic_coeff(xin, yin, derivs, c2, c3);
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev(xin[ifirst], yin[ifirst], derivs[ifirst], c2[ifirst], c3[ifirst], xe[j], out_ye[j]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev_all(xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j], out_ye[j]);
        }
    }
}

void VecD::calc_cubic_coeff(VecD &x, VecD &y, VecD &derivs, VecD &c2, VecD &c3) {

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    double DEL1, DEL2, DELTA, H;
    for (int i = 0; i < x.size() - 1; ++i) {
        H = x[i+1] - x[i];
        DELTA = (y[i+1] - y[i])/H;
        DEL1 = (derivs[i] - DELTA)/H;
        DEL2 = (derivs[i+1] - DELTA)/H;
        c2[i] = -(DEL1+DEL1 + DEL2);
        c3[i] = (DEL1 + DEL2)/H;
    }

}

void VecD::chfev_all(double X1, double X2, double F1, double F2, double D1, double D2, double XE, double &FE) {
    double C2, C3, DEL1, DEL2, DELTA, H, X;

    H = X2 - X1;

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    DELTA = (F2 - F1)/H;
    DEL1 = (D1 - DELTA)/H;
    DEL2 = (D2 - DELTA)/H;
    C2 = -(DEL1+DEL1 + DEL2);
    C3 = (DEL1 + DEL2)/H;

    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecD::chfev(double X1, double F1, double D1, double C2, double C3, double XE, double &FE) {
    double X;
    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecD::chfe_xy(VecD &x, VecD &y, VecD &new_x, VecD &out_new_y, int sorted) {
    VecD::xy_to_x(x,y);
    chfe(x,y,new_x, out_new_y, sorted);
    x_to_xy(new_x, out_new_y);
    VecD::x_to_xy(x,y);
}

double VecD::sum_sq_res_yeqx(VecD &x, VecD &y) {
    //// PDL way
    //return sum(0.5*(($y - $x)**2));
    double __sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        double diff = x[i] - y[i];
        __sum += 0.5*(diff*diff);
    }
    return __sum;
}

double VecD::avg_sq_res_yeqx(VecD &x, VecD &y) {
    return (sum_sq_res_yeqx(x,y))/x.length();
}

double VecD::avg_abs_diff(VecD &x, VecD &y) {
    double sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        sum += fabs(x[i] - y[i]);
    }
    return sum/x.length();
}

void VecD::rsq_slope_intercept(VecD &x, VecD &y, double &rsq, double &slope, double &y_intercept) {
    int i;
    double mean_x = x.avg();
    double mean_y = y.avg();
    double sum_sq_res_xx = 0.0;
    double sum_sq_res_yy = 0.0;
    double sum_sq_res_xy = 0.0;
    //    double *sq_res_xx = new double[x.length()];
    //    double *sq_res_yy = new double[y.length()];
    //    double *sq_res_xy = new double[x.length()];
    //VecD sq_res_xx(x.length());
    //VecD sq_res_yy(x.length());
    //VecD sq_res_xy(x.length());
    for (i = 0; i < x.length(); ++i) {
        double x_minus_mean_i, y_minus_mean_i;
        x_minus_mean_i = ( static_cast<double>(x[i]) ) - mean_x;
        y_minus_mean_i = ( static_cast<double>(y[i]) ) - mean_y;
        sum_sq_res_xx += x_minus_mean_i*x_minus_mean_i;
        sum_sq_res_yy += y_minus_mean_i*y_minus_mean_i;
        sum_sq_res_xy += x_minus_mean_i*y_minus_mean_i;
    }
    slope = sum_sq_res_xy/sum_sq_res_xx;
    y_intercept = mean_y - (slope * mean_x);
    rsq = (sum_sq_res_xy*sum_sq_res_xy)/(sum_sq_res_xx*sum_sq_res_yy);
    //    delete[] sq_res_xx;
    //    delete[] sq_res_yy;
    //    delete[] sq_res_xy;
}

/****************************************************************
 * VecI
 ***************************************************************/

// Constructors:
VecI::VecI() : _n(0) {
#ifdef JTP_DEBUG
    puts("Creating DATA (NO ARGS)");
#endif
}

VecI::VecI(int n) : _n(n) {
#ifdef JTP_BOUNDS_CHECK
    if (n < 0) { puts("n < 0, exiting"); exit(1); }
#endif
    _dat.resize(_n);
#ifdef JTP_DEBUG
    puts("Creating DATA(N)");
#endif
}

VecI::VecI(int n, const int &val) : _n(n) {
    _dat.reserve(_n);
    for (int i = 0; i < _n; ++i) {
        _dat.push_back(val);
    }
#ifdef JTP_DEBUG
    puts("Creating DATA(N,int)");
#endif
}

VecI::VecI(int n, std::vector<int> arr) : _n(n), _dat(arr) {
#ifdef JTP_DEBUG
    puts("SHALLOW, (N,*ARR)");
#endif
}

VecI::VecI(const VecI &A) : _n(A._n) {
    _dat = A._dat;
#ifdef JTP_DEBUG
    puts("created with VecI(const VecI &A)");
#endif
}

void VecI::to_f(VecI &out) {
    VecI _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<int>(_dat[i]);
    }
    out.take(_tmp);
}

void VecI::to_i(VecI &out) {
    VecI _tmp(_n);
    for (int i = 0; i < _n; ++i) {
        _tmp[i] = static_cast<int>(_dat[i]);
    }
    out.take(_tmp);
}


void VecI::set(int n, std::vector<int> arr) {
    _dat = arr;
    _n = n;
}

void VecI::take(int n, std::vector<int> arr) {
    _dat = arr;
    _n = n;
}

void VecI::set(VecI &A) {
    _dat = A._dat;
    _n = A._n;
}


void VecI::take(VecI &A) {
    _dat = A._dat;
    _n = A._n;
}


bool VecI::operator==(const VecI &A) {
    // We don't care if one is shallow and the A is not!
    if (A._n == _n) { // Same size
        if (A._dat == _dat)
            return true; // Same data
    }
    return false;
}

void VecI::copy(VecI &receiver) const {
    receiver._n = _n;
    receiver._dat = _dat;
}

VecI & VecI::operator=(const int &val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] = val;
    }
    return *this;
}

VecI & VecI::operator=(VecI &A) {
#ifdef JTP_DEBUG
    puts("IN ASSIGNMENT OP tOP");
#endif
    if (this != &A) {
#ifdef JTP_DEBUG
        puts("IN ASSIGNMENT OP MID");
#endif
        _n = A._n;
        _dat = A._dat;
    }
    return *this;
}

VecI::~VecI( ) {
#ifdef JTP_DEBUG
    puts("DESTRUCTOR");
#endif
}

std::vector<int> VecI::slice(int start, int end)
{
    std::vector<int> out(&_dat[start], &_dat[end]);
    return out;
}

/*************************
 * MATH OPERATORS
 ************************/

void VecI::operator+=(int val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] += val;
    }
}

void VecI::operator-=(int val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= val;
    }
}

void VecI::operator*=(int val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= val;
    }
}

void VecI::operator/=(int val) {
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= val;
    }
}

void VecI::operator+=(const VecI &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] += A[i];
    }
}

void VecI::operator-=(const VecI &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] -= A[i];
    }
}

void VecI::operator*=(const VecI &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] *= A[i];
    }
}

void VecI::operator/=(const VecI &A) {
    if (A.dim() != _n) {
        return;
    }
    for (int i = 0; i < _n; ++i) {
        _dat[i] /= A[i];
    }
}

void VecI::add(const VecI &toadd, VecI &out) {
    if (toadd._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] + toadd[i]);
        }
        out._n = _n;
    }
}

void VecI::sub(const VecI &tosub, VecI &out) {
    if (tosub._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] - tosub[i]);
        }
        out._n = _n;
    }
}

void VecI::mul(const VecI &tomul, VecI &out) {
    if (tomul._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] * tomul[i]);
        }
        out._n = _n;
    }
}

void VecI::div(const VecI &todiv, VecI &out) {
    if (todiv._n == _n) {
        out._dat.clear();
        out._dat.reserve(_n);
        for (int i = 0; i < _n; ++i) {
            out._dat.push_back(_dat[i] / todiv[i]);
        }
        out._n = _n;
    }
}

void VecI::square_root() {
    std::transform(_dat.begin(),
                   _dat.end(),
                   _dat.begin(),
                   sqrtf);
}

/*
VecI VecI::operator+(const VecI &A) {
    printf("Adim: %d selfdim: %d\n", A.dim(), _n);
    if (A.dim() != _n) {
        puts("**** NOT THE SAME *****!");
        VecI blank;
        return blank;
    }

else {
    VecI *C = new VecI(_n);
    VecI tmp = *C;
    tmp._to_pass_up = C;
        printf("TMPENEW %d\n", tmp.shallow());
    for (int i = 0; i < _n; ++i) {
        tmp[i] = _dat[i] + A[i];
    }
    return tmp;
}
}
*/

int VecI::sum() {
    return std::accumulate(_dat.begin(), _dat.end(), 0.0f);
}

char * VecI::class_name() {
    char *name = new char[7];
    strcpy(name, "VecI");
    return name;
}

void VecI::abs_val() {
    for (int n = 0; n < _n; ++n) {
        if (_dat[n] < 0) { _dat[n] *= -1; }
    }
}

void VecI::std_normal() {
    // @TODO: would like avg and stdev to stay double, even for ints!
    (*this) -= static_cast<int>(this->avg());
    double mean, stdev;
    this->sample_stats(mean, stdev);
    (*this) /= static_cast<int>(stdev);
}

void VecI::remove(int index) {
    _dat.erase(_dat.begin() + index);
    _n = _n - 1;
}

void VecI::sort() {
    std::sort(_dat.begin(), _dat.end());
}

int VecI::index(int val) {
    auto result = std::find(_dat.begin(), _dat.end(), val);
    if (result != _dat.end()) {
        return (result - _dat.begin());
    }
    return -1;
}

double VecI::avg() const {
    double total = std::accumulate(_dat.begin(), _dat.end(), 0.0);
    return total / static_cast<double>(_n);
}

//double VecI::prob_one_side_right(double x) {
//    double _mean;
//    double _sigma;
//    // zScore:
//    sample_stats( _mean, _sigma );
//    //printf("mean: %f sigma: %f\n", _mean, _sigma);
//
//    // @TODO: THIS IS BROKEN until I can figure out how to calc the normalCDF
//    // using public domain sources:
//    //return _normalCDF(_mean, _sigma, x);
//}

void VecI::sample_stats(double &mean, double &std_dev) {
    // Raw score method (below) is faster (~1.5X) than deviation score method
    // commonly used
    double _sum = 0.0;
    double _val = 0.0;
    double _sumSq = 0.0;
    int _len = this->dim();
    for( int i=0; i<_len; ++i ) {
        _val = static_cast<double>(_dat[i]);
        _sum += _val;
        _sumSq += _val *_val;
    }
    double tmp = _sumSq - ((_sum * _sum)/_len);
    tmp /= _len>1 ? _len-1 : 1;
#ifdef WIN32
    std_dev = sqrt( tmp );
#else
    std_dev = std::sqrt( tmp );
#endif
    mean = _sum/_len;
}

double VecI::_zScore(double mean, double sigma, double x) {
    return (x - mean)/(sigma == 0.0 ? 1E-20: sigma);
}

void VecI::mask_as_vec(int return_val, VecI &mask, VecI &out) {
    if (mask.size() != _n) { puts("mask.size() != this->length()"); exit(1); }
    std::vector<int> tmparr;
    tmparr.reserve(_n);
    int newcnt = 0;
    for (int i = 0; i < _n; ++i) {
        if (mask[i] == return_val) {
            tmparr.push_back(_dat[i]);
            ++newcnt;
        }
    }
    out.take(newcnt, tmparr);
}

void VecI::hist(int num_bins, VecD &bins, VecI &freqs) {
    // Create the scaling factor
    int _min = 0.0f;
    int _max = 0.0f;
    min_max(_min, _max);
    double dmin = static_cast<double>(_min);
    double conv = static_cast<double>(num_bins)/static_cast<double>(_max - _min);

    // initialize arrays
    VecD _bins(num_bins);
    VecI _freqs(num_bins, 0);
    int _len = this->dim();

    // Create the histogram:
    for (int i = 0; i < _len; ++i) {
        int index = static_cast<int>((_dat[i] - _min) * conv);
        if (index == num_bins) {
            --index;
        }
        _freqs[index]++;
    }

    // Create the bins:
    double iconv = 1.0/conv;
    for (int i = 0; i < num_bins; ++i) {
        _bins[i] = ((i+0.5) * iconv) + dmin;  // avg
        // _bins[i] = ((i+0.5) * iconv) + dmin; //min
    }

    bins.take(_bins);
    freqs.take(_freqs);
}

void VecI::logarithm(double base) {
    for (int i = 0; i < _n; ++i) {
        //printf("ME: %f\n", me[i]);
        _dat[i] = static_cast<int>(log(static_cast<double>(_dat[i]))
                                     / log(base));
        //printf("MELOGGED: %f\n", me[i]);
    }
}

void VecI::min_max(int &mn, int &mx) {
    mn = *(std::min_element(_dat.begin(), _dat.end()));
    mx = *(std::max_element(_dat.begin(), _dat.end()));
}

// Class functions:
// THIS MUST BE FOR int AND DOUBLE ONLY!!!
// This is a fairly precise Fortran->C translation of the SLATEC chim code
// Evaluate the deriv at each x point
// return 1 if less than 2 data points
// return 0 if no errors
// ASSUMES monotonicity of the X data points !!!!!
// ASSUMES that this->length() >= 2
// If length == 1 then derivs[0] is set to 0
// If length == 0 then prints message to STDERR and returns;
void VecI::chim(VecI &x, VecI &y, VecI &out_derivs) {
    std::vector<int> tmp_derivs;
    tmp_derivs.resize(x.length());
    // if they aren't the right type then warn

    //if (typeid(T) != typeid(int) || typeid(T) != typeid(double))
    //    printf("vec2 calling object must be of type int or double!\n");
    //    exit(2);
    // }

#ifdef JTP_BOUNDS_CHECK
    if (x.length() != y.length()) { puts("x.length() != y.length()"); exit(1); }
#endif
    int length = x.length();
    int del1;
    int del2;
    int h1;
    int h2;
    int hsum;
    int w1;
    int w2;
    int dmax;
    int dmin;
    int three = 3.0f;
    int dsave;
    int drat1;
    int drat2;
    int hsumt3;

    int ierr = 0;
    int lengthLess1 = length - 1;

    if (length < 2) {
        if (length == 1) {
            tmp_derivs[0] = 0;
            return;
        }
        else {
            std::cerr << "trying to chim with 0 data points!\n";
        }
    }

    // THIS can be done BEFORE this routine if someone cares to...
    //    // Check monotonicity
    //    for (int i = 2; i < length; i++) {
    //        if (x[i] <= x[i-1]) {
//            return 2;
    //        }
    //    }

    h1 = x[1] - x[0];
    del1 = (y[1] - y[0]) / h1;
    dsave = del1;

    // special case length=2 --use linear interpolation
    if (lengthLess1 < 2) {
        tmp_derivs[0] = del1;
        tmp_derivs[1] = del1;
        out_derivs.take(3, tmp_derivs);
        return;
    }

    // Normal case (length >= 3)
    // 10

    h2 = x[2] - x[1];
    del2 = (y[2] - y[1]) / h2;

    // SET D(1) VIA NON-CENTERED THREE-POINT FORMULA, ADJUSTED TO BE
    //     SHAPE-PRESERVING.

    hsum = h1 + h2;
    w1 = (h1 + hsum)/hsum;
    w2 = -h1/hsum;
    tmp_derivs[0] = (w1*del1) + (w2*del2);
    if ( pchst(tmp_derivs[0],del1) <= 0 ) {
        tmp_derivs[0] = 0.0f;
    }
    else if ( pchst(del1,del2) < 0 ) {
        // need to do this check only if monotonicity switches
        dmax = three * del1;
        if (fabs(tmp_derivs[0]) > fabs(dmax)) {
            tmp_derivs[0] = dmax;
        }
    }

    int pchstval;
    int ind;

    for (ind = 1; ind < lengthLess1; ind++) {
        if (ind != 1) {
            h1 = h2;
            h2 = x[ind+1] - x[ind];
            hsum = h1 + h2;
            del1 = del2;
            del2 = (y[ind+1] - y[ind])/h2;
        }
        // 40
        tmp_derivs[ind] = 0.0f;

        pchstval = pchst(del1,del2);

        // 45
        if (pchstval > 0) {
            hsumt3 = hsum+hsum+hsum;
            w1 = (hsum + h1)/hsumt3;
            w2 = (hsum + h2)/hsumt3;
            dmax = static_cast<int>(max(fabs(del1), fabs(del2)));
            dmin = static_cast<int>(min(fabs(del1), fabs(del2)));
            drat1 = del1/dmax;
            drat2 = del2/dmax;
            tmp_derivs[ind] = dmin/(w1*drat1 + w2*drat2);
        }
        // 42
        else if (pchstval < 0 ) {
            ierr = ierr + 1;
            dsave = del2;
            continue;
        }
        // 41
        else {  // equal to zero
            if (del2 == 0.0f) { continue; }
            if (VecI::pchst(dsave,del2) < 0) { ierr = ierr + 1; }
            dsave = del2;
            continue;
        }
    }

    // 50
    w1 = -h2/hsum;
    w2 = (h2 + hsum)/hsum;
    tmp_derivs[ind] = w1*del1 + w2*del2;
    if ( VecI::pchst(tmp_derivs[ind],del2) <= 0 ) {
        tmp_derivs[ind] = 0.0f;
    }
    else if ( VecI::pchst(del1, del2) < 0) {
        // NEED DO THIS CHECK ONLY IF MONOTONICITY SWITCHES.
        dmax = three*del2;
        if (fabs(tmp_derivs[ind]) > fabs(dmax)) {
            tmp_derivs[ind] = dmax;
        }
    }
    out_derivs.take(length, tmp_derivs);
    return;
}

void VecI::xy_to_x(VecI &x, VecI &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] - x._dat[i];
    }
}

void VecI::x_to_xy(VecI &x, VecI &y) {
    for (int i = 0; i < x.length(); i++) {
        y._dat[i] = y._dat[i] + x._dat[i];
    }
}

void VecI::linear_derivs(VecI &x, VecI &y, VecI &out_derivs) {
    VecI tmp_d(x.size());
    for (int i = 0; i < x.size(); ++i) {
        tmp_d[i] = (y[i+1] - y[i]) / (x[i+1]-x[i]);
    }
    out_derivs.take(tmp_d);
}

void VecI::linear_interp(VecI &xin, VecI &yin, VecI &xe, VecI &out_ye, int sorted) {

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecI derivs;
    VecI::linear_derivs(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;
    int dt;

    if (sorted) {
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt*derivs[ifirst]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            dt = xe[j] - xin[ifirst];  // diff in x, eval to input
            out_ye[j] = yin[ifirst] + (dt * ((yin[ir] - yin[ifirst]) / (xin[ir]-xin[ifirst])) );
        }
    }
}

int VecI::sum_of_sq() {
    double total = accumulate(_dat.begin(),
                              _dat.end(),
                              0.0,
                              [](double s, double x) { return s + (x * x); });
    return static_cast<int>(total);
}

double VecI::pearsons_r(VecI &x, VecI &y) {

    // Preparation:
    double sum_xTy = VecI::dot_product(x,y);
    double sum_x = x.sum();
    double sum_y = y.sum();
    // Could this step be sped up?
    double sum_x2 = x.sum_of_sq();
    double sum_y2 = y.sum_of_sq();
    int N = x.dim();

    // Here it is:
    // 'E' is Capital Sigma
    // r = EXY - (EXEY/N)
    //    -----------------
    //    sqrt( (EX^2 - (EX)^2/N) * (EY^2 - (EY)^2/N) )

    double top = sum_xTy - ((sum_x * sum_y)/N);
    double fbot = sum_x2 - ((sum_x*sum_x)/N);  //first part of bottom
    double sbot = sum_y2 - ((sum_y*sum_y)/N);  //second part of bottom
    return top / sqrt(fbot * sbot);

}

double VecI::covariance(VecI &x, VecI &y) {
    int i;
    int len = x.size();
    double mean_x = 0;
    double mean_y = 0;
    // get the means and x * y
    for (i = 0; i < len; ++i) {
        mean_x += x[i];
        mean_y += y[i];
    }
    mean_x /= len;
    mean_y /= len;
    double cov = 0;
    for (i = 0; i < len; ++i) {
        cov += (x[i] - mean_x) * (y[i] - mean_y);
    }
    return cov/len;
}

double VecI::euclidean(VecI &x, VecI &y) {
    VecI diff(x.size());
    double sum_of_diffs = 0;
    for (int i = 0; i < x.size(); ++i) {
        sum_of_diffs += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sqrt(sum_of_diffs);
}

int VecI::dot_product(VecI &x, VecI &y) {
    //assert(x.dim() == y.dim());
    int sum = 0;
    for (int i = 0; i < x.dim(); i++) {
        sum += (x[i] * y[i]);
    }
    return sum;
}

void VecI::chfe(VecI &xin, VecI &yin, VecI &xe, VecI &out_ye, int sorted) {
    //xin.print(); yin.print();

    if (out_ye.size() == 0) {
        out_ye._dat.clear();
        out_ye._dat.resize(xe.size());
    }

    // Calc the derivs:
    VecI derivs;
    VecI::chim(xin,yin,derivs);
    int i,j,ir;  // i indexes xin, j indexes xnew
    int ifirst = 0;

    // find the bounding points in xin
    int istart;


    if (sorted) {
        VecI c2(xin.size());
        VecI c3(xin.size());
        calc_cubic_coeff(xin, yin, derivs, c2, c3);
        istart = 0;
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            istart = i;
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev(xin[ifirst], yin[ifirst], derivs[ifirst], c2[ifirst], c3[ifirst], xe[j], out_ye[j]);
        }
    }
    else {

        // find the bounding points in xin
        for (j = 0; j < xe.size(); ++j) {
            ir = -1;
            istart = 0;
            // @TODO: This should be a binary search:
            for (i = istart; i < xin.size(); ++i) {
                // locate the interval
                if (xin[i] >= xe[j]) {
                    ir = i;
                    ifirst = i - 1;
                    break;
                }
            }
            if (ir == 0) { // left extrapolation
                ir = 1;
                ifirst = 0;
            }
            else if (ir == -1) { // right extrapolation
                ir = i - 1;
                ifirst = ir - 1;
            }
            //printf("ifirst: %d ir %d i %d\n",ifirst, ir, i); printf("xin[ifirst]%f, xin[ir]%f, yin[ifirst]%f, yin[ir]%f, derivs[ifirst]%f, derivs[ir]%f, xe[j]%f\n", xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j]);
            chfev_all(xin[ifirst], xin[ir], yin[ifirst], yin[ir], derivs[ifirst], derivs[ir], xe[j], out_ye[j]);
        }
    }
}

void VecI::calc_cubic_coeff(VecI &x, VecI &y, VecI &derivs, VecI &c2, VecI &c3) {

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    int DEL1, DEL2, DELTA, H;
    for (int i = 0; i < x.size() - 1; ++i) {
        H = x[i+1] - x[i];
        DELTA = (y[i+1] - y[i])/H;
        DEL1 = (derivs[i] - DELTA)/H;
        DEL2 = (derivs[i+1] - DELTA)/H;
        c2[i] = -(DEL1+DEL1 + DEL2);
        c3[i] = (DEL1 + DEL2)/H;
    }

}

void VecI::chfev_all(int X1, int X2, int F1, int F2, int D1, int D2, int XE, int &FE) {
    int C2, C3, DEL1, DEL2, DELTA, H, X;

    H = X2 - X1;

    //  COMPUTE CUBIC COEFFICIENTS (EXPANDED ABOUT X1).
    DELTA = (F2 - F1)/H;
    DEL1 = (D1 - DELTA)/H;
    DEL2 = (D2 - DELTA)/H;
    C2 = -(DEL1+DEL1 + DEL2);
    C3 = (DEL1 + DEL2)/H;

    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecI::chfev(int X1, int F1, int D1, int C2, int C3, int XE, int &FE) {
    int X;
    X = XE - X1;
    //printf("X: %f F1 %f D1 %f C2 %f C3 %f\n", X, F1, D1, C2, C3);
    FE = F1 + X*(D1 + X*(C2 + X*C3));
}

void VecI::chfe_xy(VecI &x, VecI &y, VecI &new_x, VecI &out_new_y, int sorted) {
    VecI::xy_to_x(x,y);
    chfe(x,y,new_x, out_new_y, sorted);
    x_to_xy(new_x, out_new_y);
    VecI::x_to_xy(x,y);
}

double VecI::sum_sq_res_yeqx(VecI &x, VecI &y) {
    //// PDL way
    //return sum(0.5*(($y - $x)**2));
    double __sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        int diff = x[i] - y[i];
        __sum += 0.5*(diff*diff);
    }
    return __sum;
}

double VecI::avg_sq_res_yeqx(VecI &x, VecI &y) {
    return (sum_sq_res_yeqx(x,y))/x.length();
}

double VecI::avg_abs_diff(VecI &x, VecI &y) {
    double sum = 0.0;
    for (int i = 0; i < x.length(); ++i) {
        sum += fabs(x[i] - y[i]);
    }
    return sum/x.length();
}

void VecI::rsq_slope_intercept(VecI &x, VecI &y, double &rsq, double &slope, double &y_intercept) {
    int i;
    double mean_x = x.avg();
    double mean_y = y.avg();
    double sum_sq_res_xx = 0.0;
    double sum_sq_res_yy = 0.0;
    double sum_sq_res_xy = 0.0;
    //    double *sq_res_xx = new double[x.length()];
    //    double *sq_res_yy = new double[y.length()];
    //    double *sq_res_xy = new double[x.length()];
    //VecD sq_res_xx(x.length());
    //VecD sq_res_yy(x.length());
    //VecD sq_res_xy(x.length());
    for (i = 0; i < x.length(); ++i) {
        double x_minus_mean_i, y_minus_mean_i;
        x_minus_mean_i = ( static_cast<double>(x[i]) ) - mean_x;
        y_minus_mean_i = ( static_cast<double>(y[i]) ) - mean_y;
        sum_sq_res_xx += x_minus_mean_i*x_minus_mean_i;
        sum_sq_res_yy += y_minus_mean_i*y_minus_mean_i;
        sum_sq_res_xy += x_minus_mean_i*y_minus_mean_i;
    }
    slope = sum_sq_res_xy/sum_sq_res_xx;
    y_intercept = mean_y - (slope * mean_x);
    rsq = (sum_sq_res_xy*sum_sq_res_xy)/(sum_sq_res_xx*sum_sq_res_yy);
    //    delete[] sq_res_xx;
    //    delete[] sq_res_yy;
    //    delete[] sq_res_xy;
}

// END TEMPLATE

} // End namespace VEC



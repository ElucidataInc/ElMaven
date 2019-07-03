#ifndef FIT_H
#define FIT_H

#define MAXFIT 12
#define errwin(x) {}

#include "standardincludes.h"

void sort_xy(double *tmp1, double *tmp2, int up, int sorton, int stype);
void cxfree(void *ptr);
void gauss(int n, double *a, int adim, double *b, double *x);
double leasev(double *c, int degree, double x);

void leasqu(int n, double *x, double *y, int degree, double *w, int wdim,
            double *r);
void stasum(double *x, int n, double *xbar, double *sd, int flag);

////kiran TODO:function not used
//int linear_regression(int n, double *x, double *y, double *fitted);
//
////kiran TODO:function not used
//void fitcurve(double *x, double *y, int n, int ideg, double *fitted);
//
////kiran TODO:function not used
//void runavg(double *x, double *y, double *ax, double *ay, int n, int ilen);
//
////kiran TODO:function not used
//void runstddev(double *x, double *y, double *ax, double *ay, int n, int ilen);
//
////kiran TODO:function not used
//void runmedian(double *x, double *y, double *ax, double *ay, int n, int ilen);
//
////kiran TODO:function not used
//void runminmax(double *x, double *y, double *ax, double *ay, int n, int ilen, int type);
//
////kiran TODO:function not used
//void filterser(int n, double *x, double *y, double *resx, double *resy, double *h, int len);
//
////kiran TODO:function not used
//void linearconv(double *x, double *h, double *y, int n, int m);
//
////kiran TODO:function not used
//int crosscorr(double *x, double *y, int n, int lag, int meth, double *xcov, double *xcor);
//
////kiran TODO:function not used
//void spline(int n, double *x, double *y, double *b, double *c, double *d);
//
////kiran TODO:function not used
//double seval(int n, double u, double *x, double *y, double *b, double *c, double *d);
//
////kiran TODO:function not used
//void ntiles(double *x, double *y, int n, int nt, double *resx, double *resy);
//
////kiran TODO:function not used
//void interp_pts(double *x, double *y, int n1, double *xr, double *yr, int n2);

#endif

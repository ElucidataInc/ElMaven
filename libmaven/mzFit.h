#ifndef FIT_H
#define FIT_H
#define MAXFIT 12
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define errwin(x) {}

void sort_xy(double *tmp1, double *tmp2, int up, int sorton, int stype);
void cxfree(void *ptr);
void gauss(int n, double *a, int adim, double *b, double *x);
int linear_regression(int n, double *x, double *y, double *fitted);
void stasum(double *x, int n, double *xbar, double *sd, int flag);
void leasqu(int n, double *x, double *y, int degree, double *w, int wdim,
		double *r);
double leasev(double *c, int degree, double x);
void fitcurve(double *x, double *y, int n, int ideg, double *fitted);
void runavg(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runstddev(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runmedian(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runminmax(double *x, double *y, double *ax, double *ay, int n, int ilen,
		int type);
void filterser(int n, double *x, double *y, double *resx, double *resy,
		double *h, int len);
void linearconv(double *x, double *h, double *y, int n, int m);
int crosscorr(double *x, double *y, int n, int lag, int meth, double *xcov,
		double *xcor);
void spline(int n, double *x, double *y, double *b, double *c, double *d);
double seval(int n, double u, double *x, double *y, double *b, double *c,
		double *d);
void ntiles(double *x, double *y, int n, int nt, double *resx, double *resy);
void interp_pts(double *x, double *y, int n1, double *xr, double *yr, int n2);

#endif


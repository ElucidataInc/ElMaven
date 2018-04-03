#ifndef OBIWARP_H
#define OBIWARP_H

#include <vector>
#include <iostream>
#include <assert.h>

#include "vec.h"
#include "mat.h"
#include "dynprog.h"

using namespace std;

class ObiWarp{
public:
    ObiWarp(string score="cor",bool local = false, float factor_diag = 2.f, float factor_gap = 1.f, 
            float gap_init = 0.2f,float gap_extend = 3.4f, float init_penalty = 0.f, 
            float response = 100.f, bool nostdnrm = false  );
    ~ObiWarp();
    void setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
    vector<float> align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
private:
    void tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals);
    void warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm);
    VecF _tm;
    VecF _mz;
    MatF _mat;
    int _tm_vals;
    int _mz_vals;
    float* tmPoint;
    float* mzPoint;
    DynProg dyn;

    char* score;
    bool local;
    float factor_diag;
    float factor_gap;
    float gap_init;
    float gap_extend;
    float init_penalty;
    float response;
    bool nostdnrm;

};

#endif 
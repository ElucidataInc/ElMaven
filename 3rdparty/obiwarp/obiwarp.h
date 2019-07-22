#ifndef OBIWARP_H
#define OBIWARP_H

#include <vector>
#include <iostream>
#include <assert.h>

#include "vec.h"
#include "mat.h"
#include "dynprog.h"

using namespace std;

struct ObiParams{
    ObiParams(string score,bool local, float factor_diag, float factor_gap, float gap_init,float gap_extend,
            float init_penalty, float response, bool nostdnrm, float binSize);

    string score;
    bool local;
    float factor_diag;
    float factor_gap;
    float gap_init;
    float gap_extend;
    float init_penalty;
    float response;
    bool nostdnrm;
    float binSize;
};

class ObiWarp{
public:
    ObiWarp(ObiParams *obiParams);
    ~ObiWarp();
    void setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
    vector<float> align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat);
private:
    bool tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals);
    void warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm);
    VecF _tm;
    VecF _mz;
    MatF _mat;
    int _tm_vals;
    int _mz_vals;
    std::vector<float> tmPoint;
    std::vector<float> mzPoint;
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

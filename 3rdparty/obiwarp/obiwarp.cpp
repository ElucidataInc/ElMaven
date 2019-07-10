#include "obiwarp.h"

ObiParams::ObiParams(string score,bool local, float factor_diag, float factor_gap, float gap_init,float gap_extend,
            float init_penalty, float response, bool nostdnrm, float binSize){

    this->score = score;
    this->local = local;
    this->factor_diag = factor_diag;
    this->factor_gap = factor_gap;
    this->gap_init = gap_init;
    this->gap_extend = gap_extend;
    this->init_penalty = init_penalty;
    this->response = response;
    this->nostdnrm = nostdnrm;
    this->binSize = binSize;
}

ObiWarp::ObiWarp(ObiParams *obiParams){

    this->score = &obiParams->score[0];
    this->local = obiParams->local;
    this->factor_diag = obiParams->factor_diag;
    this->factor_gap = obiParams->factor_gap;
    this->gap_init = obiParams->gap_init;
    this->gap_extend = obiParams->gap_extend;
    this->init_penalty = obiParams->init_penalty;
    this->response = obiParams->response;
    this->nostdnrm = obiParams->nostdnrm;
}

ObiWarp::~ObiWarp(){

}

void ObiWarp::setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    tmPoint = rtPoints;
    _tm_vals = tmPoint.size();
    _tm.take(_tm_vals, tmPoint);

    mzPoint = mzPoints;
    _mz_vals = mzPoint.size();
    _mz.take(_mz_vals, mzPoint);

    assert(_tm_vals == intMat.size());
    MatF mat(_tm_vals, _mz_vals);
    for(int i=0; i < _tm_vals; ++i){
        assert(_mz_vals == intMat[i].size());
        for(int j = 0; j < _mz_vals; ++j)
            mat(i,j) = intMat[i][j];
    }
    _mat.take(mat);

}

vector<float> ObiWarp::align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    
    VecF tm;
    vector<float> tmPoint(rtPoints);
    int tm_vals = tmPoint.size();
    tm.take(tm_vals, tmPoint);

    VecF mz;
    vector<float> mzPoint(mzPoints);
    int mz_vals = mzPoint.size();
    mz.take(mz_vals, mzPoint);

    assert(tm_vals = intMat.size());
    MatF mat(tm_vals, mz_vals);
    for(int i = 0; i < tm_vals; ++i){
        assert(mz_vals == intMat[i].size());
        for(int j = 0; j < mz_vals; ++j)
            mat(i,j)=intMat[i][j];
    }

    MatF smat;
    dyn.score(_mat, mat, smat, score);

    if (!nostdnrm) {
        if (!smat.all_equal()) { 
            smat.std_normal();
        }
    }

    int gp_length = smat.rows() + smat.cols();

    VecF gp_array;
    dyn.linear_less_before(gap_extend,gap_init,gp_length,gp_array);

    int minimize = 0;
    dyn.find_path(smat, gp_array, minimize, factor_diag, factor_gap, local, init_penalty);

    VecI mOut;
    VecI nOut;
    dyn.warp_map(mOut, nOut, response, minimize);

    VecF nOutF;
    VecF mOutF;
    vector<float> alignedRts;
    if (!tm_axis_vals(mOut, mOutF, _tm,_tm_vals) ||
        !tm_axis_vals(nOut, nOutF, tm,tm_vals))
        return alignedRts;
    warp_tm(nOutF, mOutF, tm);

    for(int i = 0; i < tm_vals; ++i)
        alignedRts.push_back(tm[i]);
    
    // delete[] tmPoint;
    // delete[] mzPoint;

    return alignedRts;
}


bool ObiWarp::tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals){
    VecF tmp(tmCoords.length());
    for (int i = 0; i < tmCoords.length(); ++i) {
        if (tmCoords[i] < _tm_vals) {
            tmp[i] = _tm[tmCoords[i]];
        }
        else {
            printf("asking for time value at index: %d (length: %d)\n", tmCoords[i], _tm_vals);
            return(false);
        }
    }
    tmVals.take(tmp);
    return(true);
}

void ObiWarp::warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm){
    VecF out;
    VecF::chfe(selfTimes, equivTimes, _tm, out, 1);  // run with sort option
    _tm.take(out);
}

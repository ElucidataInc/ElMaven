#include "obiwarp.h"


ObiWarp::ObiWarp(){
    std::cerr<<"===========mission successful============"<<std::endl;
}
ObiWarp::~ObiWarp(){
    //release memory from all places
}

void ObiWarp::setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    _tm_vals = rtPoints.size();
    float* tmPoint = new float[_tm_vals];
    for(int i=0; i < _tm_vals ; ++i)
        tmPoint[i] = rtPoints[i];
    _tm.take(_tm_vals, tmPoint);

    _mz_vals = mzPoints.size();
    float* mzPoint = new float[_mz_vals];
    for(int i = 0; i < _mz_vals; ++i)
        mzPoint[i] = mzPoints[i];
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

vector<float>& alignRt(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    VecF tm;
    int tm_vals = rtPoints.size();
    float* tmPoint = new float[tm_vals];
    for(int i = 0; i < tm_vals; ++i)
        tmPoint[i] = rtPoints[i];
    tm.take(tm_vals, tmPoint);

    VecF mz;
    int mz_vals = mzPoints.size();
    float* mzPoint = new float[mz_vals];
    for(int i = 0; i < mz_vals; ++i)
        mzPoint[i] = mzPoints[i];
    mz.take(mz_vals, mzPoint);

    assert(tm_vals = intMat.size());
    MatF mat(tm_vals, mz_vals);
    for(int i = 0; i < tm_vals; ++i){
        assert(mz_vals == intMat[i].size());
        for(int j = 0; j < mz_vals; ++j)
            mat(i,j)=intMat[i][j];
    }

}


void ObiWarp::tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals){

}

void ObiWarp::warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm){

}
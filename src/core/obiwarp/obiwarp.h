#ifndef OBIWARP_H
#define OBIWARP_H

#include "vec.h"
#include "mat.h"
#include "dynprog.h"

class ObiWarp{
public:
    ObiWarp();
    ~ObiWarp();
private:
    void tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals);
    void warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm);

};

#endif 
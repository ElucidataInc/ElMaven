#include "SRMList.h"

SRMLists::SRMList(vector<mzSample*>samples, deque<Compound*> compoundsDB){
    this->samples = samples;
    this->compoundsDB = compoundsDB;
}
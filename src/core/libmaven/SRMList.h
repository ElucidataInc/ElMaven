#ifndef SRMLIST_H
#define SRMLIST_H

#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "Scan.h"
#include <QMap>

class SRMList{
  public:

    SRMList(vector<mzSample*>samples, deque<Compound*> compoundsDB);
    vector<mzSample*>samples;

    vector<mzSlice*> getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool associateCompoundNames);
    deque<Compound*> compoundsDB;

    Compound* findSpeciesByPrecursor(float precursorMz, float productMz, float rt, int polarity,double amuQ1, double amuQ3);
    static double getPrecursorOfSrm(string srmId);
    static double getProductOfSrm(string srmId);
};

#endif
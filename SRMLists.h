#ifndef SRMLISTS_H
#define SRMLISTS_H

#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "Scan.h"
 #include <QMap> 

class SRMLists{
    public:
        SRMLists(vector<mzSample*>samples, deque<Compound*> compoundsDB);
        vector<mzSample*>samples;

        vector<mzSlice*> getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool rtMatch, bool associateCompoundNames);
        deque<Compound*> compoundsDB;

        Compound* findSpeciesByPrecursor(float precursorMz, float productMz, float rt, bool rtMatch, int polarity,double amuQ1, double amuQ3);
};

#endif

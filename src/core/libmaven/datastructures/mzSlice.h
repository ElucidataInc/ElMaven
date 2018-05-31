#ifndef MZSLICE_H
#define MZSLICE_H

#include <iostream>
#include <string>

#include "masscutofftype.h"

class Compound;

using namespace std;

/**
* @brief Stores a slice in mz and rt plane
*
* @details An mzSlice is a slice in mz and rt plane. This class
* stores an mzSlice and also provides couple of utility functions
* for a mzSlice
*
*/
class mzSlice
{
  public:
    /**
    * @brief Constructor for class mzSlice having rt and mz range
    * @param a mz minimum of a mzSlice
    * @param b mz maximum of a mzSlice
    * @param c rt minimum of a mzSlice
    * @param d rt maximum of a mzSlice
    */
    mzSlice(float a, float b, float c, float d)
    {
        mzmin = a;
        mzmax = b;
        rtmin = c;
        rtmax = d;
        mz = a + (b - a) / 2;
        rt = c + (d - c) / 2;
        compound = NULL;
        ionCount = 0;
    }

    /**
    * @brief Constructor for class mzSlice having a filterline
    * @param filterLine srm Id of a scan in a sample
    * @see Scan:filterLine
    */
    mzSlice(string filterLine)
    {
        mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
        compound = NULL;
        srmId = filterLine;
    }

    /**
    * @brief Constructor for class mzSlice having no inputs parameters
    */
    mzSlice()
    {
        mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
        compound = NULL;
    }

    mzSlice &operator=(const mzSlice &b)
    {
        mzmin = b.mzmin;
        mzmax = b.mzmax;
        rtmin = b.rtmin;
        rtmax = b.rtmax;
        ionCount = b.ionCount;
        compound = b.compound;
        srmId = b.srmId;
        mz = b.mz;
        rt = b.rt;
        return *this;
    }

    /**
    * @brief Average rt of a mzSlice. Zero in case mzSlice is based on filterline
    * or constructor is empty
    */
    float rt;

    /**
    * @brief Average mz of a mzSlice. Zero in case mzSlice is based on filterline
    * or constructor is empty
    */
    float mz;

    float mzmin;
    float mzmax;
    float rtmin;
    float rtmax;
    float ionCount;
    Compound *compound;
    string srmId;

    /**
    * @brief Compare total intensity (ion count) of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower intensity than mzSlice b
    */
    static bool compIntensity(const mzSlice *a, const mzSlice *b)
    {
        return b->ionCount < a->ionCount;
    }

    /**
    * @brief Compare average m/z of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower average m/z than mzSlice b
    */
    static bool compMz(const mzSlice *a, const mzSlice *b)
    {
        return a->mz < b->mz;
    }

    /**
    * @brief Compare average rt of two mzSlices
    * @param a object of class mzSlice
    * @param b object of class mzSlice
    * @return True if mzSlice a has lower average rt than mzSlice b
    */
    static bool compRt(const mzSlice *a, const mzSlice *b)
    {
        return a->rt < b->rt;
    }
    
    /**
    * @brief operator overloading for less than operator in class mzSlice
    * @param b object of class mzSlice
    * @return True if average m/z of input mzSlice object is greater than
    * current mzSlice object
    */
    bool operator<(const mzSlice *b) const
    {
        return mz < b->mz;
    }

    /**
    * @brief Calculate mzmin and mzmax of mzSlice using mass accuracy (in ppm)
    * @details mzmin and mzmax of an mzSlice are calculated using compound m/z
    * and mass accuracy (in ppm). Compound m/z is being calculated using formula
    * or taken from Compound class if formula is not present
    * @param CompoundppmWindow Mass accuracy in ppm (parts per million)
    * @param charge Charge of the compound
    * @return bool True if compound mass exists
    */
    bool calculateMzMinMax(MassCutoff *compoundMassCutoffWindow, int charge);

    /**
    * @brief Calculate rtmin and rtmax of mzSlice using rt window
    * @param matchRtFlag This flag is true if retention time in compound database
    * is being used
    * @param compoundRTWindow Retention time window for matching Rt from compound
    * database
    */
    void calculateRTMinMax(bool matchRtFlag, float compoundRTWindow);

    /**
    * @brief Set Srm Id of mzslice using the srm Id of compound
    */
    void setSRMId();
};

#endif //MZSLICE_H

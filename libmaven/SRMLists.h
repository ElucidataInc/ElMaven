#ifndef SRMLISTS_H
#define SRMLISTS_H

#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "Scan.h"
 #include <QMap> 

class SRMLists{
    public:

        /**
        * [Constructor for the class SRMLists.]
        * @method SRMLists
        * @param samples    [vector, with all the samples and repsective scans info]
        * @param compoundsDB[deque, a database with all the compounds and their databases] 
        */
        SRMLists(vector<mzSample*>samples, deque<Compound*> compoundsDB);
        vector<mzSample*>samples;

        /**
        * [Creates and returns slices created after finding best scan assiging the 
        *  best compound for each SRM with the help of findSpeciesByPrecursor().]
        * @param amuQ1      [tolerance level in Q1-value]
        * @param amuQ3      [tolerance level in Q3-value]
        * @param userPolarity
        * @param rtMatch    [Boolean - Whether to consider the variation in rt or not]
        * @param associateCompoundNames [Boolean - Whether to assign names to the compounds]
        * @return slices
        */
        vector<mzSlice*> getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool rtMatch, bool associateCompoundNames);
        deque<Compound*> compoundsDB;

        /**
        * [Finds the species/compound to which, a particular SRM belongs to(if exists).]
        * @param precursorMz    [SRM's precursorMz value - Q1]
        * @param productMz      [SRM's productMz value - Q3]
        * @param rt             [SRM's rt value]
        * @param rtMatch        [Boolean - Whether to consider the variation in rt or not]
        * @param polarity       [SRM's polarity]
        * @param amuQ1          [tolerance level in Q1-value]
        * @param amuQ3          [tolerance level in Q3-value]
        * @return x             [Compound that best fits the scan from database. Null, if none matches.]
        */
        Compound* findSpeciesByPrecursor(float precursorMz, float productMz, float rt, bool rtMatch, int polarity,double amuQ1, double amuQ3);
};

#endif
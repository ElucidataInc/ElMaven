#ifndef SRMLIST_H
#define SRMLIST_H

#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "Scan.h"
#include <QMap>

/**
 * @brief Calculate slices and annotate filterlines (srm-ids) 
 * 
 * @details Slices are calculated on the basis of filterline.
 * Each slice will have unique filterline. Slice/filterline is
 * annotated with compound based on precursor/product m/z and 
 * expected rt.
 * 
 */
class SRMList{
  public:

    /**
     * @brief Constructor of class SRMList
     * @param samples Samples used
     * @param compoundsDB Compounds from reference compound database
     */
    SRMList(vector<mzSample*>samples, deque<Compound*> compoundsDB);

    /**
     * @brief Calculates and annotate slices with compound names
     * @param amuQ1 Q1 mass tolerance
     * @param amuQ3 Q3 mass tolerance
     * @param userPolarity User selected polarity
     * @param associateCompoundNames If true, slices will be annotated
     * with compound names
     * @return vector<mzSlice*> vector of mzSlices objects
     * @see mzSlice
     */
    vector<mzSlice*> getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool associateCompoundNames);
    
    /**
     * @brief Get nearest compound to precursor m/z, product m/z and
     * expected rt
     * @param precursorMz Mass by charge ratio of precursor
     * @param productMz Mass by charge ratio of product
     * @param rt Rt of slice
     * @param polarity user polarity
     * @param amuQ1 Q1 mass tolerance
     * @param amuQ3 Q3 mass tolerance
     * @return Compound* Compound from compound database
     * @see Compound
     */
    Compound* findSpeciesByPrecursor(float precursorMz, float productMz, float rt, int polarity,double amuQ1, double amuQ3);

    /**
     * @brief Get precursor m/z from filterline (srm-id)
     * @param srmId Filterline of scan
     * @return double precursor m/z
     */
    static double getPrecursorOfSrm(string srmId);

    /**
     * @brief Get product m/z from filterline (srm-id)
     * @param srmId Filterline of scan
     * @return double product m/z
     */
    static double getProductOfSrm(string srmId);

    /**
     * @brief Get all the compounds which share same precursor
     * and product m/z as filterline
     * @param srmId filterline of scan
     * @param amuQ1 Q1 mass tolerance
     * @param amuQ3 Q3 mass tolerance
     * @return deque<Compound*> List of compounds
     * @see Compound
     */
    deque<Compound*> getMatchedCompounds(string srmId, double amuQ1, double amuQ3);

    /**
     * @brief store manual annotation
     * @see annotation
     */
    void setAnnotation(map<string, Compound*> annotationCompound) {
      annotation = annotationCompound;
    }

    vector<mzSample*>samples;

    deque<Compound*> compoundsDB;
    
  private:
    /**
     * @brief Manual annotation of filterline
     * @details Key is filterline and value is compound manually annotated
     */
    map<string, Compound*> annotation;

  };

#endif
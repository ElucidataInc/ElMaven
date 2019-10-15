#ifndef __MGFP_INCLUDE_MASSABUNDANCEPAIR_H__
#define __MGFP_INCLUDE_MASSABUNDANCEPAIR_H__

#include "config.h"

#include <utility>
#include <functional>

/** The library namespace that holds all library-specific types, classes and
 * functions.
 */
namespace mgf {

/** A type for representing a fragment ion m/z and abundance.
 */
typedef std::pair<double, double> MassAbundancePair;

/** Functor for m/z-based comparison of \c MassAbundancePairs.
 */
struct LessThanMz :  std::binary_function<bool, MassAbundancePair, 
  MassAbundancePair> {
    bool operator()(const MassAbundancePair& lhs, 
      const MassAbundancePair& rhs) {
        return lhs.first < rhs.first;
    }
    bool operator()(const double lhs, const MassAbundancePair& rhs) {
        return lhs < rhs.first;
    }
    bool operator()(const MassAbundancePair& lhs, const double rhs) {
        return lhs.first < rhs;
    }
};

/** Functor for abundance-based comparison of \c MassAbundancePairs.
 */
struct LessThanAbundance :  std::binary_function<bool, MassAbundancePair, 
  MassAbundancePair> {
    bool operator()(const MassAbundancePair& lhs, 
      const MassAbundancePair& rhs) {
        return lhs.second < rhs.second;
    }
};

} // namespace mgf

#endif


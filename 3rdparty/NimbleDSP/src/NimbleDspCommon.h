//
//  NimbleDspCommon.h
//  NimbleDSP
//
//  Created by Jim Clay on 11/20/14.
//
//

#ifndef NimbleDSP_NimbleDspCommon_h
#define NimbleDSP_NimbleDspCommon_h

namespace NimbleDSP {

enum FilterOperationType {STREAMING, ONE_SHOT_RETURN_ALL_RESULTS, ONE_SHOT_TRIM_TAILS};
typedef enum ParksMcClellanFilterType {PASSBAND_FILTER = 1, DIFFERENTIATOR_FILTER, HILBERT_FILTER} ParksMcClellanFilterType;

};

#endif

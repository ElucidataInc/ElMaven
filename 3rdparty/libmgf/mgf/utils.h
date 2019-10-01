#ifndef __MGFP_INCLUDE_UTILS_H__
#define __MGFP_INCLUDE_UTILS_H__

#include "config.h"

namespace mgf
{

/** Helper function to check if a value exists in a map. This is necessary
 * because std::map<A,B>::operator[] \a creates and returns an empty object if
 * the specified key dows not exist.
 */
template <class T>
typename T::mapped_type existsOrEmpty(T map, typename T::key_type key)
{
    typename T::iterator i = map.find(key);
    if (i != map.end()) {
        return i->second;
    }
    else {
        return typename T::mapped_type();
    }
}

}

#endif


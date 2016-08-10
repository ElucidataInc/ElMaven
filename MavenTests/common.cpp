#include "common.h"

bool common::floatCompare(float a, float b) {
    float EPSILON = 0.0001;
    return fabs(a - b) < EPSILON;
}

bool common::compareMaps(const map<string,int> & l, const map<string,int> & k)
{
  // same types, proceed to compare maps here

  if(l.size() != k.size())
    return false;  // differing sizes, they are not the same

 map<string,int>::const_iterator i, j;
  for(i = l.begin(), j = k.begin(); i != l.end(); ++i, ++j)
  {
    if(*i != *j)
      return false;
  }

  return true;
}

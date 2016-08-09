#include "common.h"

bool common:: floatCompare(float a, float b) {
    float EPSILON = 0.0001;
    return fabs(a - b) < EPSILON;
}

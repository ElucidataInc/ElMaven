#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#ifndef BASE64_H
#define BASE64_H
using namespace std;

namespace base64 {
    // borrowed from xmms GNU
    inline uint32_t swapbytes(uint32_t x)
    {
        return   ((x & 0x000000ffU) << 24)
               | ((x & 0x0000ff00U) <<  8)
               | ((x & 0x00ff0000U) >>  8)
               | ((x & 0xff000000U) >> 24);
    }

    // borrowed from xmms GNU
    inline uint64_t swapbytes64(uint64_t x)
    {
        return ((((uint64_t)swapbytes((uint32_t)(x & 0xffffffffU)) << 32) |
                 (uint64_t)swapbytes((uint32_t)(x >> 32))));
    }

    vector<float> decodeBase64(const string& src,
                               int float_size,
                               bool neworkorder,
                               bool decompress);

    string decodeString(const char* data, const size_t len);
}
#endif

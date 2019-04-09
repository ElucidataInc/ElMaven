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
    /**
     * @brief Reverse the order of bytes of a 32-bit word. Borrowed from xmms GNU.
     * @param x The 32-bit structure whose order of bytes is to be reversed.
     * @return A new 32-bit structure with bytes stored in a reverse order.
     */
    inline uint32_t swapbytes(uint32_t x)
    {
        return   ((x & 0x000000ffU) << 24)
               | ((x & 0x0000ff00U) <<  8)
               | ((x & 0x00ff0000U) >>  8)
               | ((x & 0xff000000U) >> 24);
    }

    /**
     * @brief Reverse the order of bytes of a 64-bit word. Borrowed from xmms GNU.
     * @param x The 64-bit structure whose order of bytes is to be reversed.
     * @return A new 64-bit structure with bytes stored in a reverse order.
     */
    inline uint64_t swapbytes64(uint64_t x)
    {
        return ((((uint64_t)swapbytes((uint32_t)(x & 0xffffffffU)) << 32) |
                 (uint64_t)swapbytes((uint32_t)(x >> 32))));
    }

    /**
     * @brief Decode a base64 encoded binary data string. If the string is
     * zlib compressed then it is uncompressed after decoding, and then
     * converted to an array of floating point values.
     * @param src A base64 encoded data string.
     * @param float_size Value denoting precision of floating point data.
     * @param neworkorder Boolean indication network order.
     * @param decompress Whether the string needs to be decompressed after
     * decoding step.
     * @return A vector of floating point values extracted from undecoded binary
     * data.
     */
    vector<float> decodeBase64(const string& src,
                               int float_size,
                               bool neworkorder,
                               bool decompress);

    /**
     * @brief Decode a plain base64-encoded string.
     * @param data A raw base64-encoded buffer.
     * @param len Length of the buffer containing base64 data.
     * @return A decoded form of input base64- encoded string.
     */
    string decodeString(const char* data, const size_t len);
}

#endif

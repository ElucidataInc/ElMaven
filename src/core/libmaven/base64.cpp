#include "base64.h"
#include "mzUtils.h"

using namespace std;

namespace base64 {
    string decodeString(const char *data, const size_t len)
    {
        unsigned char* p = (unsigned char*)data;
        int pad = len > 0 && (len % 4 || p[len - 1] == '=');
        const size_t L = ((len + 3) / 4 - pad) * 4;
        std::string str(L / 4 * 3 + pad, '\0');

        const int B64index[256] = {
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
            0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
            0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
        };

        for (size_t i = 0, j = 0; i < L; i += 4)
        {
            int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12
                    | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
            str[j++] = n >> 16;
            str[j++] = n >> 8 & 0xFF;
            str[j++] = n & 0xFF;
        }
        if (pad)
        {
            int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
            str[str.size() - 1] = n >> 16;

            if (len > L + 2 && p[L + 2] != '=')
            {
                n |= B64index[p[L + 2]] << 6;
                str.push_back(n >> 8 & 0xFF);
            }
        }
        return str;
    }

    vector<float> decodeBase64(const string& src,
                               int float_size,
                               bool neworkorder,
                               bool decompress)
    {
        string destStr = decodeString(src.c_str(), src.size());

        if (decompress) {
#ifdef ZLIB
            destStr = mzUtils::decompressString(destStr);
#endif
        }

#if (LITTLE_ENDIAN == 1)
         cerr << "INFO: little endian… inverted network order.";
         neworkorder=!neworkorder;
#endif

        int size = 1 + (destStr.size() - 4) / float_size;

        // we will cast everything as a float may be this is not wise,
        // but have not found a need for double precission yet.
        vector<float> decodedArray(size);
        const char* dest = destStr.c_str();

        if ( float_size == 8 ) {
            if ( neworkorder == false ) {
                for (int i=0; i<size; i++)
                    decodedArray[i] = (float) ((double*) dest)[i];
            } else {
                uint64_t *u = (uint64_t *) dest;
                double data=0;
                for (int i=0; i<size; i++) {
                    uint64_t t = swapbytes64(u[i]);
                    memcpy(&data,&t,8);
                    decodedArray[i] = (float) data;
                }
            }
        } else if (float_size == 4 ) {
            if ( neworkorder == false) {
                for (int i=0; i<size; i++) decodedArray[i] = ((float*)dest)[i];
            } else {
                uint32_t *u = (uint32_t *) dest;
                float data=0;
                for (int i=0; i<size; i++) {
                    uint32_t t = swapbytes(u[i]);
                    memcpy(&data,&t,4);
                    decodedArray[i] = data;
                }
            }
        }

        return decodedArray;
    }
} // namespace

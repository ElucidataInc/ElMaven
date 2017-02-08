#include "base64.h"
#include "mzUtils.h"

using namespace std;

namespace base64 {

    char encode(unsigned char u) {
        //Merged to 776
        if(u < 26) return 'A'+u;
        if(u < 52) return 'a'+(u-26);
        if(u < 62) return '0'+(u-52);
        if(u == 62) return '+';
        return '/';
    }

    unsigned char decode(char c){
        //Merged to 776
        if(c >= 'A' && c <= 'Z') return(c - 'A');
        if(c >= 'a' && c <= 'z') return(c - 'a' + 26);
        if(c >= '0' && c <= '9') return(c - '0' + 52);
        if(c == '+') return 62;
        return 63;
    }

    int is_base64(char c) {
        //Merged to 776
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') || (c == '+')             ||
                (c == '/')             || (c == '=')) {
            return true;
        }
        return false;
    }

    char *decodeString(const string &src) {
        // Merged to 776
        int k, l = src.length();
        char *buf = (char *)calloc(sizeof(char), l);
        char *dest = (char *)calloc(sizeof(char), l);
        char *p = dest;

        /* Ignore non base64 chars as per the POSIX standard */
        for (k = 0, l = 0; src[k]; k++) {
            if (is_base64(src[k])) {
                buf[l++] = src[k];
            }
        }

        for (k = 0; k < l; k += 4) {
            char c1 = 'A', c2 = 'A', c3 = 'A', c4 = 'A';
            unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0;

            c1 = buf[k];
            if (k + 1 < l) c2 = buf[k + 1];
            if (k + 2 < l) c3 = buf[k + 2];
            if (k + 3 < l) c4 = buf[k + 3];

            b1 = decode(c1);
            b2 = decode(c2);
            b3 = decode(c3);
            b4 = decode(c4);

            *p++ = ((b1 << 2) | (b2 >> 4));

            if (c3 != '=') *p++ = (((b2 & 0xf) << 4) | (b3 >> 2));
            if (c4 != '=') *p++ = (((b3 & 0x3) << 6) | b4);
        }
        free(buf);
        return dest;
    }

    vector<float> convertDecodedDataBackToFloat(char *dest, int float_size,
            bool neworkorder, int size) {
        //Merged to 776
#if (LITTLE_ENDIAN == 1)
        neworkorder = !neworkorder;
#endif

        // we will cast everything as a float may be this is not wise, but have not
        // found a need for double
        // precission yet
        vector<float> decodedArray(size);

        if (float_size == 8) {
            if (neworkorder == false) {
                for (int i = 0; i < size; i++)
                    decodedArray[i] = (float)((double *)dest)[i];
            } else {
                uint64_t *u = (uint64_t *)dest;
                double data = 0;
                for (int i = 0; i < size; i++) {
                    uint64_t t = swapbytes64(u[i]);
                    memcpy(&data, &t, 8);
                    decodedArray[i] = (float)data;
                }
            }
        } else if (float_size == 4) {
            if (neworkorder == false) {
                for (int i = 0; i < size; i++)
                    decodedArray[i] = ((float *)dest)[i];
            } else {
                uint32_t *u = (uint32_t *)dest;
                float data = 0;
                for (int i = 0; i < size; i++) {
                    uint32_t t = swapbytes(u[i]);
                    memcpy(&data, &t, 4);
                    decodedArray[i] = data;
                }
            }
        }

        free(dest);
        return decodedArray;
    }

    vector<float> decode_base64(const string& src, int float_size, bool neworkorder, bool decompress) {
        //Merged to 776
        int size = 1 + (src.length() * 3 / 4 - 4) / float_size;

        char *dest = decodeString(src);
        if (decompress) {
            decompressString(&dest, size, float_size);
        }

        vector<float> decodedArray =
            convertDecodedDataBackToFloat(dest, float_size, neworkorder, size);

        return decodedArray;

    }

    void decompressString(char **dest, int &size, int float_size) {
        //Merged to 776
        string decodedStr(*dest);
        string uncompStr(mzUtils::decompress_string(decodedStr));

        free(*dest);
        *dest = (char *)calloc(sizeof(char), uncompStr.size());
        for (unsigned int i = 0; i < uncompStr.size(); i++)
            *dest[i] = uncompStr[i];

        size = 1 + (uncompStr.size() * 3 / 4 - 4) / float_size;
    }

    unsigned char *convertFromFloatToCharacter(float *srcF,
            const vector<float> &farray) {
        //Merged to 776
        int i;
        int sizeF = farray.size();

        // copy vector to C array
        for (i = 0; i < sizeF; i++) {
            srcF[i] = farray[i];
        }

        unsigned char *src = (unsigned char *)srcF;
        return src;
    }

    unsigned char *encodeString(unsigned char *src, int size) {
        //Merge to 776
        unsigned char *out =
            (unsigned char *)calloc(sizeof(char), size * 4 / 3 + 4);
        unsigned char *p = out;
        int i;
        for (i = 0; i < size; i += 3) {

            unsigned char b1=0, b2=0, b3=0, b4=0, b5=0, b6=0, b7=0;

            b1 = src[i];

            if(i+1<size)
                b2 = src[i+1];

            if(i+2<size)
                b3 = src[i+2];

            b4 = b1>>2;
            b5 = ((b1&0x3)<<4)|(b2>>4);
            b6 = ((b2&0xf)<<2)|(b3>>6);
            b7 = b3&0x3f;

            *p++= encode(b4);
            *p++= encode(b5);

            if(i+1<size) {
                *p++= encode(b6);
            } else {
                *p++= '=';
            }

            if(i+2<size) {
                *p++= encode(b7);
            } else {
                *p++= '=';
            }

        }
        return out;
    }

    unsigned char *encode_base64(const vector<float>& farray) {
        //Merged to 776

        //copy vector t C array
        int sizeF = farray.size();
        int size = sizeF * 4;

        float* srcF= (float*) calloc(sizeof(float), sizeF);
        unsigned char *src = convertFromFloatToCharacter(srcF, farray);

        unsigned char* out = encodeString(src, size);

        free(srcF);
        return out;
    }

} //namespace

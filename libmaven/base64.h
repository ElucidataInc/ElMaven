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
		unsigned char decode(char c);
		char encode(unsigned char u);
		int is_base64(char c);
		vector<float> decode_base64(const string& src, int float_size,bool networkorder);
		unsigned char *encode_base64(const vector<float>& farray);

		/* swap bytes .. borrowed from xmms  GNU*/
		inline uint32_t swapbytes(uint32_t x) {
				return ((x & 0x000000ffU) << 24) |
						((x & 0x0000ff00U) <<  8) |
						((x & 0x00ff0000U) >>  8) |
						((x & 0xff000000U) >> 24);
		}

		inline uint64_t swapbytes64(uint64_t x) {
				return ((((uint64_t)swapbytes((uint32_t)(x & 0xffffffffU)) << 32) |
										(uint64_t)swapbytes((uint32_t)(x >> 32))));
		}

}
#endif



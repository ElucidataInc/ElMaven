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
		 * [Decode a base64 character]
		 * @method decode
		 * @param  c      [char to be decoded]
		 * @return [ASCII value]
		 */
		unsigned char decode(char c);

		/**
		 * [Base64 encode one byte]
		 * @method encode
		 * @param  u      [ASCII to be encoded]
		 * @return [base64 encoded character]
		 */
		char encode(unsigned char u);

		/**
		 * @return TRUE if 'c' is a valid base64 character, otherwise FALSE
		 */
		int is_base64(char c);

		/**
		 * [Decode a base64 string]
		 * @method decode
		 * @param  src      				[string to be decoded]
		 * @param  float_size      	[float size; 4 for 32 bit machines, 8 for 64 bit machines]
		 * @param  networkorder			[networkorder, LITTLE_ENDIAN or BIG_ENDIAN]
		 * @return [float array in binary representation]
		 */
		vector<float> decode_base64(const string& src, int float_size,bool networkorder);

		/**
		 * [Base64 encode a float array in binary representation]
		 * @method encode_base64
		 * @param  farray        [float array in binary representation]
		 * @return [Base64 encoded string]
		 */
		unsigned char *encode_base64(const vector<float>& farray);

		/**
		 * [swap bytes .. borrowed from xmms  GNU]
		 * @method swapbytes
		 * @param  x         []
		 * @return []
		 */
		inline uint32_t swapbytes(uint32_t x) {
				return ((x & 0x000000ffU) << 24) |
						((x & 0x0000ff00U) <<  8) |
						((x & 0x00ff0000U) >>  8) |
						((x & 0xff000000U) >> 24);
		}

		/**
		 * [swap bytes .. borrowed from xmms  GNU]
		 * @method swapbytes64
		 * @param  x           []
		 * @return []
		 */
		inline uint64_t swapbytes64(uint64_t x) {
				return ((((uint64_t)swapbytes((uint32_t)(x & 0xffffffffU)) << 32) |
										(uint64_t)swapbytes((uint32_t)(x >> 32))));
		}
}
#endif

#include "base64.h"
using namespace std;

namespace base64 { 
/**
 *  * Base64 encode one byte
 *   */
char encode(unsigned char u) {
		if(u < 26)  return 'A'+u;
		if(u < 52)  return 'a'+(u-26);
		if(u < 62)  return '0'+(u-52);
		if(u == 62) return '+';
		return '/';
}

/**
 *  * Decode a base64 character
 *   */


unsigned char decode(char c){
		if(c >= 'A' && c <= 'Z') return(c - 'A');
		if(c >= 'a' && c <= 'z') return(c - 'a' + 26);
		if(c >= '0' && c <= '9') return(c - '0' + 52);
		if(c == '+') return 62;
		return 63;
}

/**
 *  * Return TRUE if 'c' is a valid base64 character, otherwise FALSE
 *   */
int is_base64(char c) {
		if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9') || (c == '+')             ||
				(c == '/')             || (c == '=')) {
				return true;
		}
		return false;
}

vector<float> decode_base64(const string& src, int float_size, bool neworkorder) {

    int k, l= src.length();

    unsigned char *buf= (unsigned char*) calloc(sizeof(unsigned char), l);
    unsigned char *dest = (unsigned char*) calloc(sizeof(unsigned char), l);
    unsigned char *p= dest;

    /* Ignore non base64 chars as per the POSIX standard */
    for(k=0, l=0; src[k]; k++) {
        if(is_base64(src[k])) {
            buf[l++]= src[k];
        }
    }

    for(k=0; k<l; k+=4) {
        char c1='A', c2='A', c3='A', c4='A';
        unsigned char b1=0, b2=0, b3=0, b4=0;

        c1= buf[k];
        if(k+1<l) c2= buf[k+1];
        if(k+2<l) c3= buf[k+2];
        if(k+3<l) c4= buf[k+3];

        b1= decode(c1);
        b2= decode(c2);
        b3= decode(c3);
        b4= decode(c4);

        *p++=((b1<<2)|(b2>>4) );

        if(c3 != '=') *p++=(((b2&0xf)<<4)|(b3>>2) );
        if(c4 != '=') *p++=(((b3&0x3)<<6)|b4 );
    }

    int size = 1+(src.length() * 3/4 - 4)/float_size;

    /*
		cout << "Dest=" << dest << endl;
		cout << "src.length()=" << src.length() << endl;
		cout << "size=" << size << endl;
		*/

#if (LITTLE_ENDIAN == 1)
	 cerr << "WARNING: LITTLE_ENDIAN.. Inverted network order";
     neworkorder=!neworkorder;
#endif


    //we will cast everything as a float may be this is not wise, but have not found a need for double
    //precission yet
    vector<float> decodedArray(size);
    ////cerr << "Net=" << neworkorder << " float_size=" << float_size << endl;
    if ( float_size == 8 ) {
        if ( neworkorder == false ) {
            for (int i=0; i<size; i++) decodedArray[i] = (float) ((double*)dest)[i];
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

    //for debuging
    //for(int i=0; i < size; i++ ) { cout << "\tok.." << i << " " << size <<" " << decodedArray[i] << endl; }

    free(buf);
    free(dest);
    return decodedArray;
}

unsigned char *encode_base64(const vector<float>& farray) {


		int i;
		unsigned char *p;

		//copy vector to C array
		int sizeF = farray.size();
		float* srcF= (float*) calloc(sizeof(float), sizeF);
		for(i=0; i<sizeF; i++ ) { srcF[i]=farray[i]; } 
		
		int size = sizeF*4;
		unsigned char *src = (unsigned char*) srcF;
		unsigned char* out= (unsigned char*) calloc(sizeof(char), size*4/3+4);
		p= out;

		
		for(i=0; i<size; i+=3) {

				unsigned char b1=0, b2=0, b3=0, b4=0, b5=0, b6=0, b7=0;

				b1 = src[i];

				if(i+1<size)
						b2 = src[i+1];

				if(i+2<size)
						b3 = src[i+2];

				b4= b1>>2;
				b5= ((b1&0x3)<<4)|(b2>>4);
				b6= ((b2&0xf)<<2)|(b3>>6);
				b7= b3&0x3f;

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
		free(srcF);
		return out;
}

}//namespace

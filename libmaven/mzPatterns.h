#ifndef PATTERN_H
#define PATTERN_H

#include <string>

using namespace std;

/**
 * @class mzPattern
 * @ingroup libmaven
 * @brief Wrapper class for pattern.
 * @author Elucidata
 */
class mzPattern {

    public:

        string pattern;
        int lb;
        int rb;

        mzPattern(string pattern) {
            this->pattern = pattern;
            findNonZeroBounds();
        }

        int countItems(char c) {
            int count=0;
            for(unsigned int i=0; i < pattern.length(); i++) {
                if (pattern[i] == c ) count++;
            }
            return count;
        }

        int longestStratch(char c) {
            int stretch=0;
            int longest=0;
            for(unsigned int i=0; i < pattern.length(); i++ ) {
                if (pattern[i] == c ) {
                    stretch++;
                } else {
                    if (longest < stretch) longest = stretch;
                    stretch=0;
                }
            }
             if (longest < stretch) longest = stretch;
             return longest;
        }

        int countCharSwitches(char x, char y) {
            int count=0;
            for(unsigned int i=0; i < pattern.length()-1; i++ ) {
                if (pattern[i] == x && pattern[i+1] == y) {
                    count++;
                }
            }
            return count;
        }

        int longestSymmetry(char x, char y) {
            int longest=0;
            for(unsigned int i=0; i < pattern.length()-1; i++ ) {
                int xcount=0;
                int ycount=0;
                if (pattern[i] == x && pattern[i+1] == y) {
                    for(int j=i;j>=0;j--) if (pattern[j]==x)  xcount++; else break;
                    for(unsigned int j=i+1;j<pattern.length();j++) if (pattern[j]==y)  ycount++; else break;
                    int sym = xcount+ycount-abs(xcount-ycount);
                    if ( sym > longest) longest = sym;
                }
            }
            return longest;
        }

        int countAllSwitches() {
            int count=0;
            for(unsigned int i=0; i < pattern.length()-1; i++ ) {
                if (pattern[i] != pattern[i+1]) {
                    count++;
                }
            }
            return count;
        }

        int countSwitchesFrom(char x) {
            int count=0;
            for(unsigned int i=0; i < pattern.length()-1; i++ ) {
                if (pattern[i] == x && pattern[i+1] != x) count++;
            }
            return count;
        }

        void findNonZeroBounds() {
            lb = 0;
			rb = 0;
			if (pattern.length()== 0) return;

            for(unsigned int i=0; i < pattern.length(); i++ )
                if (pattern[i] == '0') lb++; else break;

            for(int i=pattern.length()-1; i>=0; i--)
                if (pattern[i] == '0') rb--; else break;
        }

        int lengthWithNonZeroEnds() {
            findNonZeroBounds();
            return(rb-lb);
        }

        float noiseness(){
			int length=0;
			int count=0;
			for(int i=lb; i<rb; i++ ) {
				length++;
				if (pattern[i] == '0' ) { count++; continue; }
				if (i+1 < rb && pattern[i] == '-' && pattern[i+1] == '+') { count++; continue; }
				if (i+1 < rb && pattern[i] == '+' && pattern[i+1] == '0') { count++; continue; }
			}
			return (float)count/length;
       }

       int countZeros() {
            return countItems('0');
       }

};

#endif

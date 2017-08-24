#include "mzPatterns.h"
#include <cstdlib>
mzPattern::mzPattern(string pattern) {
    this->pattern = pattern;
    findNonZeroBounds();
}

int mzPattern::countItems(char c) const {
    int count=0;
    for(unsigned int i=0; i < pattern.length(); i++) {
        if (pattern[i] == c ) count++;
    }
    return count;
}

int mzPattern::longestStratch(char c) const {
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

int mzPattern::countCharSwitches(char x, char y) const {
    int count=0;
    for(unsigned int i=0; i < pattern.length()-1; i++ ) {
        if (pattern[i] == x && pattern[i+1] == y) {
            count++;
        }
    }
    return count;
}

int mzPattern::longestSymmetry(char x, char y) const  {
    int longest=0;
    for(unsigned int i=0; i < pattern.length()-1; i++ ) {
        int xcount = 0; //naman scope can't be reduced, will change the logic.
        int ycount = 0; //naman scope can't be reduced, will change the logic.
        if (pattern[i] == x && pattern[i + 1] == y) {
            //naman reformatted 
            for (int j = i; j >= 0; j--) 
                if (pattern[j] == x)  
                    xcount++; 
                else 
                    break;

            for (unsigned int j = i + 1; j < pattern.length(); j++) 
                if (pattern[j] == y)  
                    ycount++; 
                else 
                    break;
            int sym = xcount + ycount - std::abs(xcount - ycount);
            if ( sym > longest) longest = sym;
        }
    }
    return longest;
}

int mzPattern::countAllSwitches() const {
    int count=0;
    for(unsigned int i=0; i < pattern.length()-1; i++ ) {
        if (pattern[i] != pattern[i+1]) {
            count++;
        }
    }
    return count;
}

int mzPattern::countSwitchesFrom(char x) const {
    int count=0;
    for(unsigned int i=0; i < pattern.length()-1; i++ ) {
        if (pattern[i] == x && pattern[i+1] != x) count++;
    }
    return count;
}

void mzPattern::findNonZeroBounds() {
    lb = 0;
    rb = 0;
    if (pattern.length()== 0) return;

    for(unsigned int i=0; i < pattern.length(); i++ )
        if (pattern[i] == '0') lb++; else break;

    for(int i=pattern.length()-1; i>=0; i--)
        if (pattern[i] == '0') rb--; else break;
}

int mzPattern::lengthWithNonZeroEnds() {
    findNonZeroBounds();
    return(rb-lb);
}

float mzPattern::noiseness() const {
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

int mzPattern::countZeros() {
    return countItems('0');
}

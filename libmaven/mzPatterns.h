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

        mzPattern(string pattern);
        int countItems(char c) const; 

        int longestStratch(char c) const;

        int countCharSwitches(char x, char y) const; 

        int longestSymmetry(char x, char y) const;

        int countAllSwitches() const;

        int countSwitchesFrom(char x) const;

        void findNonZeroBounds();

        int lengthWithNonZeroEnds();

        float noiseness() const;

       int countZeros();

};

#endif

/*
Copyright (c) 2014, James Clay

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


/**
 * @file Vector.h
 *
 * Definition of the template class Vector.
 */

#ifndef NimbleDSP_Vector_h
#define NimbleDSP_Vector_h


#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <complex>
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "NimbleDspCommon.h"


namespace NimbleDSP {

const unsigned DEFAULT_BUF_LEN = 0;

#ifndef SLICKDSP_FLOAT_TYPE
#define SLICKDSP_FLOAT_TYPE    double
#endif

#define VECTOR_TO_ARRAY(x)      (&((x)[0]))


/**
 * \brief Base class for NimbleDSP.
 *
 * Although you can instantiate objects of this type, that's not what this class is intended for.  It is the
 * base class that all of the other classes descend from which allows for a great deal of flexibility
 * through polymorphism.  It also reduces the amount of code because we don't have to replicate the same
 * functionality in each class.
 *
 * Derived classes: RealVector and ComplexVector.
 */
template <class T>
class Vector {

 public:
    /** 
     * \brief Buffer to store intermediate calculations when needed.
     */
    std::vector<T> *scratchBuf;

 protected:
    /** 
     * \brief Initializes vec to a given size and fills it with zeros.
     */
    void initSize(unsigned size) {vec = std::vector<T>(size);}
    
    /** 
     * \brief Initializes vec with the size and contents of "array".
     *
     * \param array Array to set vec equal to.
     * \param arrayLen Number of elements in array.
     */
    template <class U>
    void initArray(U *array, unsigned arrayLen);
    
 public:
    /*************************************************************************************//**
     * \brief Vector that holds the object's data.
     *
     * The class is built around this member.  Std::vector's were used because they handle the 
     * dynamic memory, have a rich set of support functions, are fast and efficient, and can
     * be accessed like a normal array when that is convenient.
     *****************************************************************************************/
	std::vector<T> vec;
    
    template <class U> friend class Vector;
    template <class U> friend class RealVector;
    template <class U> friend class ComplexVector;
    template <class U> friend class RealFirFilter;
    template <class U> friend class ComplexFirFilter;
    
    /*****************************************************************************************
                                        Constructors
    *****************************************************************************************/
    /**
     * \brief Basic constructor.
     *
     * Just sets the size of \ref vec and the pointer to the scratch buffer, if one is provided.
     * \param size Size of \ref vec.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    Vector<T>(unsigned size = 0, std::vector<T> *scratch = NULL) {initSize(size); scratchBuf = scratch;}
    
    /**
     * \brief Vector constructor.
     *
     * Sets vec equal to the input "data" parameter and sets the pointer to the scratch buffer,
     *      if one is provided.
     * \param data Vector that \ref vec will be set equal to.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    template <typename U>
    Vector<T>(std::vector<U> data, std::vector<T> *scratch = NULL) {initArray(VECTOR_TO_ARRAY(data), (unsigned) data.size()); scratchBuf = scratch;}
    
    /**
     * \brief Array constructor.
     *
     * Sets vec equal to the input "data" array and sets the pointer to the scratch buffer,
     *      if one is provided.
     * \param data Array that \ref vec will be set equal to.
     * \param dataLen Length of "data".
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    template <typename U>
    Vector<T>(U *data, unsigned dataLen, std::vector<T> *scratch = NULL) {initArray(data, dataLen); scratchBuf = scratch;}
    
    /**
     * \brief Copy constructor.
     */
    Vector<T>(const Vector<T>& other) {vec = other.vec; scratchBuf = other.scratchBuf;}

	/**
	 * \brief Virtual destructor.
	 */
	virtual ~Vector() {};

    /*****************************************************************************************
                                            Operators
    *****************************************************************************************/
    /**
     * \brief Index assignment operator.
     */
    T& operator[](unsigned index) {return vec[index];};
    
    /**
     * \brief Index operator.
     */
    const T& operator[](unsigned index) const {return vec[index];};
    
    /*****************************************************************************************
                                            Methods
    *****************************************************************************************/
    /**
     * \brief Returns the size of \ref vec.
     */
    const unsigned size() const {return (const unsigned) vec.size();};
    
    /**
     * \brief Finds the first instance of "val" in \ref vec.
     *
     * \param val The value to look for in \ref vec.
     * \return Index of first instance of "val".  If there aren't any elements equal to "val"
     *      it returns -1.
     */
    const int find(const T val) const;
    
    /**
     * \brief Returns the sum of all the elements in \ref vec.
     */
	T sum() const;

};


template <class T>
template <class U>
void Vector<T>::initArray(U *array, unsigned arrayLen) {
    vec = std::vector<T>(arrayLen);
    for (unsigned i=0; i<arrayLen; i++) {
        vec[i] = (T) array[i];
    }
}

template <class T>
inline bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    
    for (unsigned i=0; i<lhs.size(); i++) {
        if (lhs[i] != rhs[i])
            return false;
    }
    return true;
}

template <class T>
inline bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) {return !(lhs == rhs);}

template <class T>
const int Vector<T>::find(const T val) const {
    for (unsigned i=0; i<size(); i++) {
        if (vec[i] == val) {
            return (int) i;
        }
    }
    return -1;
}

/**
 * \brief Finds the first instance of "val" in \ref vec.
 *
 * \param vector Buffer to operate on.
 * \param val The value to look for in \ref vec.
 * \return Index of first instance of "val".  If there aren't any elements equal to "val"
 *      it returns -1.
 */
template <class T>
const int find(Vector<T> & vector, const T val) {
    return vector.find(val);
}

template <class T>
T Vector<T>::sum() const {
	assert(vec.size() > 0);
	T vectorSum = 0;
	for (unsigned i=0; i<vec.size(); i++) {
		vectorSum += vec[i];
	}
	return vectorSum;
}

/**
 * \brief Returns the sum of all the elements in \ref vec.
 *
 * \param vector Buffer to operate on.
 */
template <class T>
T sum(const Vector<T> & vector) {
	return vector.sum();
}

};

#endif

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
 * @file ComplexVector.h
 *
 * Definition of the template class ComplexVector.
 */

#ifndef NimbleDSP_ComplexVector_h
#define NimbleDSP_ComplexVector_h

#include <complex>
#include "Vector.h"
#include "kissfft.hh"



namespace NimbleDSP {

enum DomainType {TIME_DOMAIN, FREQUENCY_DOMAIN};

/**
 * \brief Vector class for complex numbers.
 *
 * The template type should be the "plain old data" type that you want to use, not "std::complex"
 * or your own custom complex class.  The object will automatically convert the buffer type to
 * std::complex<POD_type> for you.
 */
template <class T>
class ComplexVector : public Vector< std::complex<T> > {
 public:
    template <class U> friend class ComplexFirFilter;

    /**
     * \brief Indicates whether the data in \ref buf is time domain data or frequency domain.
     */
    DomainType domain;
    
    /*****************************************************************************************
                                        Constructors
    *****************************************************************************************/
    /**
     * \brief Basic constructor.
     *
     * Sets \ref domain to NimbleDSP::TIME_DOMAIN and calls Vector<T>::Vector(size, scratch).
     * \param size Size of \ref buf.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    ComplexVector<T>(unsigned size = DEFAULT_BUF_LEN, std::vector< std::complex<T> > *scratch = NULL) :
            Vector< std::complex<T> >(size, scratch) {domain = TIME_DOMAIN;}
            
    /**
     * \brief Vector constructor.
     *
     * Sets buf equal to the input "data" parameter and sets the pointer to the scratch buffer,
     *      if one is provided.  Also sets \ref domain to dataDomain.
     * \param data Vector that \ref buf will be set equal to.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     * \param dataDomain Indicates whether the data is time domain data or frequency domain.
     *      Valid values are NimbleDSP::TIME_DOMAIN and NimbleDSP::FREQUENCY_DOMAIN.
     */
    template <typename U>
    ComplexVector<T>(std::vector<U> data, DomainType dataDomain=TIME_DOMAIN,
                std::vector< std::complex<T> > *scratch = NULL) : Vector< std::complex<T> >(data, scratch)
                    {domain = dataDomain;}
    
    /**
     * \brief Array constructor.
     *
     * Sets buf equal to the input "data" array and sets the pointer to the scratch buffer,
     *      if one is provided.  Also sets \ref domain to dataDomain.
     * \param data Array that \ref buf will be set equal to.
     * \param dataLen Length of "data".
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     * \param dataDomain Indicates whether the data is time domain data or frequency domain.
     *      Valid values are NimbleDSP::TIME_DOMAIN and NimbleDSP::FREQUENCY_DOMAIN.
     */
    template <typename U>
    ComplexVector<T>(U *data, unsigned dataLen, DomainType dataDomain=TIME_DOMAIN,
                std::vector< std::complex<T> > *scratch = NULL) : Vector< std::complex<T> >(data, dataLen, scratch)
                    {domain = dataDomain;}
    
    /**
     * \brief Copy constructor.
     */
    ComplexVector<T>(const ComplexVector<T>& other)
            {this->vec = other.vec; domain = other.domain; this->scratchBuf = other.scratchBuf;}
    
    /*****************************************************************************************
                                            Operators
    *****************************************************************************************/
    /**
     * \brief Assignment operator from ComplexVector.
     * \return Reference to "this".
     */
    ComplexVector<T>& operator=(const ComplexVector<T>& rhs);
    
    /**
     * \brief Assignment operator from Vector.
     * \return Reference to "this".
     */
    ComplexVector<T>& operator=(const Vector<T>& rhs);
    
    /**
     * \brief Unary minus (negation) operator.
     * \return Reference to "this".
     */
    ComplexVector<T> & operator-();
    
    /**
     * \brief Add Buffer/Assignment operator.
     * \return Reference to "this".
     */
    template <class U>
    ComplexVector<T> & operator+=(const Vector<U> &rhs);
    
    /**
     * \brief Add Scalar/Assignment operator.
     * \return Reference to "this".
     */
    ComplexVector<T> & operator+=(const std::complex<T> &rhs);
    
    /**
     * \brief Subtract Buffer/Assignment operator.
     * \return Reference to "this".
     */
    template <class U>
    ComplexVector<T> & operator-=(const Vector<U> &rhs);
    
    /**
     * \brief Subtract Scalar/Assignment operator.
     * \return Reference to "this".
     */
    ComplexVector<T> & operator-=(const std::complex<T> &rhs);
    
    /**
     * \brief Multiply Buffer/Assignment operator.
     * \return Reference to "this".
     */
    template <class U>
    ComplexVector<T> & operator*=(const Vector<U> &rhs);
    
    /**
     * \brief Multiply Scalar/Assignment operator.
     * \return Reference to "this".
     */
    ComplexVector<T> & operator*=(const std::complex<T> &rhs);
    
    /**
     * \brief Divide Buffer/Assignment operator.
     * \return Reference to "this".
     */
    template <class U>
    ComplexVector<T> & operator/=(const Vector<U> &rhs);
    
    /**
     * \brief Divide Scalar/Assignment operator.
     * \return Reference to "this".
     */
    ComplexVector<T> & operator/=(const std::complex<T> &rhs);
    
    /*****************************************************************************************
                                            Methods
    *****************************************************************************************/
    /**
     * \brief Sets each element of \ref vec to e^(element).
     *
     * \return Reference to "this".
     */
    //virtual Vector< std::complex<T> > & exp();
    
    /**
     * \brief Sets each element of \ref buf equal to its value to the power of "exponent".
     *
     * \param exponent Exponent to use.
     * \return Reference to "this".
     */
    ComplexVector<T> & pow(const std::complex<SLICKDSP_FLOAT_TYPE> & exponent);

    /**
     * \brief Returns the element with the maximum real component in \ref buf.
     *
     * \param maxLoc If it isn't equal to NULL the index of the maximum element
     *      will be returned via this pointer.  If more than one element is equal
     *      to the maximum value the index of the first will be returned.
     *      Defaults to NULL.
     */
    const T max(unsigned *maxLoc = NULL) const;
    
    /**
     * \brief Returns the mean (average) of the data in \ref buf.
     */
    const std::complex<SLICKDSP_FLOAT_TYPE> mean() const;
    
    /**
     * \brief Returns the variance of the data in \ref buf.
     */
    const SLICKDSP_FLOAT_TYPE var() const;
    
    /**
     * \brief Returns the standard deviation of the data in \ref buf.
     */
    const SLICKDSP_FLOAT_TYPE stdDev() const {return std::sqrt(this->var());}
    
    /**
     * \brief Sets the upper and lower limit of the values in \ref buf.
     *
     * \param val Limiting value for the data in \ref buf.  Any values that
     *      are greater than "val" are made equal to "val", and
     *      any that are less than -val are made equal to -val.  This is done
     *      independently on the real and imaginary elements of \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & saturate(const std::complex<T> & val);

    /**
     * \brief Does a "ceil" operation on \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & ceil(void);

    /**
     * \brief Does a "ceil" operation on \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & floor(void);

    /**
     * \brief Does a "ceil" operation on \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & round(void);
    
    /**
     * \brief Conjugates the data in \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & conj();
    
    /**
     * \brief Sets each element of \ref buf equal to its magnitude squared.
     * \return Reference to "this".
     */
    ComplexVector<T> & magSq();
    
    /**
     * \brief Sets each element of \ref buf equal to its angle.
     *
     * The angle is held in the real portion of \ref buf.
     * \return Reference to "this".
     */
    ComplexVector<T> & angle();
    
    /**
     * \brief Sets \ref buf equal to the FFT of the data in \ref buf.
     *
     * Sets \ref domain equal to NimbleDSP::FREQUENCY_DOMAIN.
     * \return Reference to "this".
     */
    ComplexVector<T> & fft();
    
    /**
     * \brief Sets \ref buf equal to the inverse FFT of the data in \ref buf.
     *
     * Sets \ref domain equal to NimbleDSP::TIME_DOMAIN.
     * \return Reference to "this".
     */
    ComplexVector<T> & ifft();
    
    /**
     * \brief Changes the elements of \ref vec to their absolute value.
     *
     * \return Reference to "this".
     */
    ComplexVector<T> & abs();
    
    /**
     * \brief Sets each element of \ref vec to e^(element).
     *
     * \return Reference to "this".
     */
    ComplexVector<T> & exp();
    
    /**
     * \brief Sets each element of \ref vec to the natural log of the element.
     *
     * \return Reference to "this".
     */
    ComplexVector<T> & log();
    
    /**
     * \brief Sets each element of \ref vec to the base 10 log of the element.
     *
     * \return Reference to "this".
     */
    ComplexVector<T> & log10();
    
    /**
     * \brief Circular rotation.
     *
     * \param numToShift Number of positions to shift in the circular rotation.  numToShift
     *      can be positive or negative.  If you visualize the 0 index value at the left and
     *      the end of the array at the right, positive numToShift values shift the array to
     *      the left, and negative values shift it to the right.
     * \return Reference to "this".
     */
    ComplexVector<T> & rotate(int numToShift);
    
    /**
     * \brief Reverses the order of the elements in \ref vec.
     *
     * \return Reference to "this".
     */
    ComplexVector<T> & reverse();
    
    /**
     * \brief Sets the length of \ref vec to "len".
     *
     * \param len The new length for \ref vec.  If len is longer than vec's current size, the
     *      new elements will be set to "val".  If len is less than vec's current size the extra
     *      elements will be cut off and the other elements will remain the same.
     * \param val The value to set any new elements to.  Defaults to 0.
     * \return Reference to "this".
     */
    ComplexVector<T> & resize(unsigned len, T val = (T) 0) {this->vec.resize(len, val); return *this;}

    /**
     * \brief Reserves "len" elements for \ref vec without actually resizing it.
     *
     * \param len The number of elements to reserve for \ref vec.
     * \return Reference to "this".
     */
    ComplexVector<T> & reserve(unsigned len) {this->vec.reserve(len); return *this;}
    
    /**
     * \brief Lengthens \ref vec by "len" elements.
     *
     * \param len The number of elements to add to \ref vec.
     * \param val The value to set the new elements to.  Defaults to 0.
     * \return Reference to "this".
     */
    ComplexVector<T> & pad(unsigned len, T val = (T) 0) {this->vec.resize(this->size()+len, val); return *this;}

    /**
     * \brief Copies the elements in the range "lower" to "upper" (inclusive) to "destination"
     *
     * \param lower The index to start copying at.
     * \param upper The last index to copy from.
     * \destination The vector to copy the vector slice to.
     */
	void slice(unsigned lower, unsigned upper, ComplexVector<T> &destination);
    
    /**
     * \brief Inserts rate-1 zeros between samples.
     *
     * \param rate Indicates how many zeros should be inserted between samples.
     * \param phase Indicates how many of the zeros should be before the samples (as opposed to
     *      after).  Valid values are 0 to "rate"-1.  Defaults to 0.
     * \return Reference to "this".
     */
    ComplexVector<T> & upsample(int rate, int phase = 0);
    
    /**
     * \brief Removes rate-1 samples out of every rate samples.
     *
     * \param rate Indicates how many samples should be removed.
     * \param phase Tells the method which sample should be the first to be kept.  Valid values
     *      are 0 to "rate"-1.  Defaults to 0.
     * \return Reference to "this".
     */
    ComplexVector<T> & downsample(int rate, int phase = 0);
    
    /**
     * \brief Replaces \ref vec with the cumulative sum of the samples in \ref vec.
     *
     * \param initialVal Initializing value for the cumulative sum.  Defaults to zero.
     * \return Reference to "this".
     */
	ComplexVector<T> & cumsum(T initialVal = 0);
    
    /**
     * \brief Replaces \ref vec with the difference between successive samples in vec.
     *
     * The resulting \ref vec is one element shorter than it was previously.
     * \return Reference to "this".
     */
	ComplexVector<T> & diff();
    
    /**
     * \brief Replaces \ref vec with the difference between successive samples in vec.
     *
     * \param previousVal The last value in the sample stream before the current contents
     *      of \ref vec.  previousVal allows the resulting vec to be the same size as the
     *      previous vec.
     * \return Reference to "this".
     */
    ComplexVector<T> & diff(std::complex<T> & previousVal);
    
    /**
     * \brief Convolution method.
     *
     * \param data The vector that will be filtered.
     * \param trimTails "False" tells the method to return the entire convolution, which is
     *      the length of "data" plus the length of "this" (the filter) - 1.  "True" tells the
     *      method to retain the size of "data" by trimming the tails at both ends of
     *      the convolution.
     * \return Reference to "data", which holds the result of the convolution.
     */
    virtual ComplexVector<T> & conv(ComplexVector<T> & data, bool trimTails = false);
    
    /**
     * \brief Decimate method.
     *
     * This method is equivalent to filtering with the \ref conv method and downsampling
     * with the \ref downsample method, but is much more efficient.
     *
     * \param data The vector that will be filtered.
     * \param rate Indicates how much to downsample.
     * \param trimTails "False" tells the method to return the entire convolution.  "True"
     *      tells the method to retain the size of "data" by trimming the tails at both
     *      ends of the convolution.
     * \return Reference to "data", which holds the result of the decimation.
     */
    virtual ComplexVector<T> & decimate(ComplexVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Interpolation method.
     *
     * This method is equivalent to upsampling followed by filtering, but is much more efficient.
     *
     * \param data The vector that will be filtered.
     * \param rate Indicates how much to upsample.
     * \param trimTails "False" tells the method to return the entire convolution.  "True"
     *      tells the method to retain the size of "data" by trimming the tails at both
     *      ends of the convolution.
     * \return Reference to "data", which holds the result of the interpolation.
     */
    virtual ComplexVector<T> & interp(ComplexVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Resample method.
     *
     * This method is equivalent to upsampling by "interpRate", filtering, and downsampling
     *      by "decimateRate", but is much more efficient.
     *
     * \param data The vector that will be filtered.
     * \param interpRate Indicates how much to upsample.
     * \param decimateRate Indicates how much to downsample.
     * \param trimTails "False" tells the method to return the entire convolution.  "True"
     *      tells the method to retain the size of "data" by trimming the tails at both
     *      ends of the convolution.
     * \return Reference to "data", which holds the result of the resampling.
     */
    virtual ComplexVector<T> & resample(ComplexVector<T> & data, int interpRate, int decimateRate, bool trimTails = false);
    
    /**
     * \brief Generates a complex tone.
     *
     * \param freq The tone frequency.
     * \param sampleFreq The sample frequency.  Defaults to 1 Hz.
     * \param phase The tone's starting phase, in radians.  Defaults to 0.
     * \param numSamples The number of samples to generate.  "0" indicates to generate
     *      this->size() samples.  Defaults to 0.
     * \return Reference to "this".
     */
    T tone(T freq, T sampleFreq = 1.0, T phase = 0.0, unsigned numSamples = 0);
    
    /**
     * \brief Modulates the data with a complex sinusoid.
     *
     * \param freq The modulating tone frequency.
     * \param sampleFreq The sample frequency of the data.  Defaults to 1 Hz.
     * \param phase The modulating tone's starting phase, in radians.  Defaults to 0.
     * \return The next phase if the tone were to continue.
     */
    T modulate(T freq, T sampleFreq = 1.0, T phase = 0.0);
};


template <class T>
ComplexVector<T>& ComplexVector<T>::operator=(const ComplexVector<T>& rhs)
{
    this->vec = rhs.vec;
    domain = rhs.domain;
    return *this;
}

template <class T>
ComplexVector<T>& ComplexVector<T>::operator=(const Vector<T> & rhs)
{
    this->vec.resize(rhs.size());
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = std::complex<T>(rhs[i]);
    }
    domain = TIME_DOMAIN;
    return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::operator-()
{
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = -(this->vec[i]);
    }
    return *this;
}

template <class T>
template <class U>
ComplexVector<T> & ComplexVector<T>::operator+=(const Vector<U> &rhs)
{
    assert(this->size() == rhs.size());
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] += rhs.vec[i];
    }
    return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::operator+=(const std::complex<T> & rhs)
{
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] += rhs;
    }
    return *this;
}

/**
 * \brief Buffer addition operator.
 */
template <class T, class U>
inline ComplexVector<T> operator+(ComplexVector<T> lhs, const Vector<U>& rhs)
{
    lhs += rhs;
    return lhs;
}

/**
 * \brief Scalar addition operator.
 */
template <class T>
inline ComplexVector<T> operator+(ComplexVector<T> lhs, const std::complex<T> & rhs)
{
    lhs += rhs;
    return lhs;
}

template <class T>
template <class U>
ComplexVector<T> & ComplexVector<T>::operator-=(const Vector<U> &rhs)
{
    assert(this->size() == rhs.size());
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] -= rhs.vec[i];
    }
    return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::operator-=(const std::complex<T> &rhs)
{
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] -= rhs;
    }
    return *this;
}

/**
 * \brief Buffer subtraction operator.
 */
template <class T, class U>
inline ComplexVector<T> operator-(ComplexVector<T> lhs, const Vector<U>& rhs)
{
    lhs -= rhs;
    return lhs;
}

/**
 * \brief Scalar subtraction operator.
 */
template <class T>
inline ComplexVector<T> operator-(ComplexVector<T> lhs, const std::complex<T> & rhs)
{
    lhs -= rhs;
    return lhs;
}

template <class T>
template <class U>
ComplexVector<T> & ComplexVector<T>::operator*=(const Vector<U> &rhs)
{
    assert(this->size() == rhs.size());
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] *= rhs.vec[i];
    }
    return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::operator*=(const std::complex<T> &rhs)
{
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] *= rhs;
    }
    return *this;
}

/**
 * \brief Buffer multiplication operator.
 */
template <class T, class U>
inline ComplexVector<T> operator*(ComplexVector<T> lhs, const Vector<U>& rhs)
{
    lhs *= rhs;
    return lhs;
}

/**
 * \brief Scalar multiplication operator.
 */
template <class T>
inline ComplexVector<T> operator*(ComplexVector<T> lhs, const std::complex<T> & rhs)
{
    lhs *= rhs;
    return lhs;
}

template <class T>
template <class U>
ComplexVector<T> & ComplexVector<T>::operator/=(const Vector<U> &rhs)
{
    assert(this->size() == rhs.size());
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] /= rhs.vec[i];
    }
    return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::operator/=(const std::complex<T> &rhs)
{
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] /= rhs;
    }
    return *this;
}

/**
 * \brief Buffer division operator.
 */
template <class T, class U>
inline ComplexVector<T> operator/(ComplexVector<T> lhs, const Vector<U> & rhs)
{
    lhs /= rhs;
    return lhs;
}

/**
 * \brief Scalar division operator.
 */
template <class T>
inline ComplexVector<T> operator/(ComplexVector<T> lhs, const std::complex<T> & rhs)
{
    lhs /= rhs;
    return lhs;
}
 /*
template <class T>
Vector< std::complex<T> > & ComplexVector<T>::exp() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = std::exp(this->vec[i]);
    }
    return *this;
}
   */
template <class T>
ComplexVector<T> & ComplexVector<T>::pow(const std::complex<SLICKDSP_FLOAT_TYPE> & exponent) {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = std::pow(this->vec[i], exponent);
    }
    return *this;
}

/**
 * \brief Sets each element of "buffer" equal to its value to the power of "exponent".
 *
 * \param buffer Buffer to operate on.
 * \param exponent Exponent to use.
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & pow(ComplexVector<T> & buffer, const std::complex<SLICKDSP_FLOAT_TYPE> exponent) {
    return buffer.pow(exponent);
}

template <class T>
const std::complex<SLICKDSP_FLOAT_TYPE> ComplexVector<T>::mean() const {
    assert(this->size() > 0);
    std::complex<SLICKDSP_FLOAT_TYPE> sum = 0;
    for (unsigned i=0; i<this->size(); i++) {
        sum += this->vec[i];
    }
    return sum / ((SLICKDSP_FLOAT_TYPE) this->size());
}

template <class T>
const T ComplexVector<T>::max(unsigned *maxLoc) const {
    assert(this->size() > 0);
    T maxVal = this->vec[0].real();
    unsigned maxIndex = 0;

    for (unsigned i=1; i<this->size(); i++) {
        if (maxVal < this->vec[i].real()) {
            maxVal = this->vec[i].real();
            maxIndex = i;
        }
    }
    if (maxLoc != NULL) {
        *maxLoc = maxIndex;
    }
    return maxVal;
}

/**
 * \brief Returns the mean (average) of the data in "buffer".
 */
template <class T>
inline const std::complex<SLICKDSP_FLOAT_TYPE> mean(ComplexVector<T> & buffer) {
    return buffer.mean();
}

template <class T>
const SLICKDSP_FLOAT_TYPE ComplexVector<T>::var() const {
    assert(this->size() > 1);
    std::complex<SLICKDSP_FLOAT_TYPE> meanVal = this->mean();
    std::complex<SLICKDSP_FLOAT_TYPE> sum = 0;
    for (unsigned i=0; i<this->size(); i++) {
        std::complex<SLICKDSP_FLOAT_TYPE> varDiff = this->vec[i] - meanVal;
        sum += varDiff * std::conj(varDiff);
    }
    return sum.real() / (this->size() - 1);
}

/**
 * \brief Returns the variance of the data in "buffer".
 */
template <class T>
inline const SLICKDSP_FLOAT_TYPE var(ComplexVector<T> & buffer) {
    return buffer.var();
}

/**
 * \brief Returns the standard deviation of the data in "buffer".
 */
template <class T>
inline const SLICKDSP_FLOAT_TYPE stdDev(ComplexVector<T> & buffer) {
    return buffer.stdDev();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::saturate(const std::complex<T> & val) {
    for (unsigned i=0; i<this->size(); i++) {
        if (this->vec[i].real() > val.real())
            this->vec[i].real(val.real());
        else if (this->vec[i].real() < -val.real())
            this->vec[i].real(-val.real());
        if (this->vec[i].imag() > val.imag())
            this->vec[i].imag(val.imag());
        else if (this->vec[i].imag() < -val.imag())
            this->vec[i].imag(-val.imag());
    }
    return *this;
}

/**
 * \brief Sets the upper and lower limit of the values in "vector".
 *
 * \param vector Data to limit.
 * \param val Limiting value for the data in "vector".  Any values that
 *      are greater than "val" are made equal to "val", and
 *      any that are less than -val are made equal to -val.  This is done
 *      independently on the real and imaginary elements of "vector".
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & saturate(ComplexVector<T> & vector, const std::complex<T> & val) {
    return vector.saturate(val);
}
    
template <class T>
ComplexVector<T> & ComplexVector<T>::fft() {
    #ifdef NIMBLEDSP_DOMAIN_CHECKS
    assert(domain == TIME_DOMAIN);
    #endif
    
    kissfft<T> fftEngine = kissfft<T>(this->size(), false);
    std::vector< std::complex<T> > fftResults(this->size());
    
    fftEngine.transform((typename kissfft_utils::traits<T>::cpx_type *) VECTOR_TO_ARRAY(this->vec),
                        (typename kissfft_utils::traits<T>::cpx_type *) VECTOR_TO_ARRAY(fftResults));
    this->vec = fftResults;
    domain = FREQUENCY_DOMAIN;
    return *this;
}

/**
 * \brief Sets "buffer" equal to the FFT of the data in buffer.
 *
 * Sets \ref domain equal to NimbleDSP::FREQUENCY_DOMAIN.
 * \param buffer Buffer to operate on.
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & fft(ComplexVector<T> &buffer) {
    return buffer.fft();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::conj() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i].imag(-this->vec[i].imag());
    }
    return *this;
}

/**
 * \brief Conjugates the data in "buffer".
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & conj(ComplexVector<T> & buffer) {
    return buffer.conj();
}

/**
 * \brief Returns the squared magnitude of "val".
 */
template <class T>
inline T magSq(const std::complex<T> &val) {
    return val.real() * val.real() + val.imag() * val.imag();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::magSq() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i].real(NimbleDSP::magSq(this->vec[i]));
        this->vec[i].imag(0);
    }
    return *this;
}

/**
 * \brief Sets each element of "buffer" equal to its magnitude squared.
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & magSq(ComplexVector<T> & buffer) {
    return buffer.magSq();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::ifft() {
    #ifdef NIMBLEDSP_DOMAIN_CHECKS
    assert(domain == FREQUENCY_DOMAIN);
    #endif
    
    kissfft<T> fftEngine = kissfft<T>(this->size(), true);
    std::vector< std::complex<T> > fftResults(this->size());
    
    fftEngine.transform((typename kissfft_utils::traits<T>::cpx_type *) VECTOR_TO_ARRAY(this->vec),
                        (typename kissfft_utils::traits<T>::cpx_type *) VECTOR_TO_ARRAY(fftResults));
    this->vec = fftResults;
    domain = TIME_DOMAIN;
    return *this;
}

/**
 * \brief Sets "buffer" equal to the inverse FFT of the data in buffer.
 *
 * Sets \ref domain equal to NimbleDSP::TIME_DOMAIN.
 * \param buffer Buffer to operate on.
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & ifft(ComplexVector<T> &buffer) {
    return buffer.ifft();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::angle() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i].real(std::arg(this->vec[i]));
        this->vec[i].imag(0);
    }
    return *this;
}

/**
 * \brief Sets each element of "buffer" equal to its angle.
 *
 * The angle is held in the real portion of "buffer".
 * \return Reference to "buffer".
 */
template <class T>
inline ComplexVector<T> & angle(ComplexVector<T> & buffer) {
    return buffer.angle();
}

/**
 * \brief Returns the angle of a single std::complex value.
 *
 * \param val Value whose angle is returned.
 */
template <class T>
inline T angle(std::complex<T> &val) {
    return std::arg(val);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::abs() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = (T) std::abs(this->vec[i]);
    }
    return *this;
}

/**
 * \brief Changes the elements of \ref vec to their absolute value.
 *
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & abs(ComplexVector<T> & vector) {
    return vector.abs();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::exp() {
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] = (std::complex<T>) std::exp(this->vec[i]);
    }
    return *this;
}

/**
 * \brief Sets each element of \ref vec to e^(element).
 *
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & exp(ComplexVector<T> & vector) {
    return vector.exp();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::log() {
    for (unsigned i=0; i<this->size(); i++) {
		this->vec[i] = (std::complex<T>) std::log(this->vec[i]);
    }
    return *this;
}

/**
 * \brief Sets each element of \ref vec to the natural log of the element.
 *
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & log(ComplexVector<T> & vector) {
    return vector.log();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::log10() {
    for (unsigned i=0; i<this->size(); i++) {
		this->vec[i] = (std::complex<T>) std::log10(this->vec[i]);
    }
    return *this;
}

/**
 * \brief Sets each element of \ref vec to the base 10 log of the element.
 *
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & log10(ComplexVector<T> & vector) {
    return vector.log10();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::rotate(int numToShift) {
    while (numToShift < 0)
        numToShift += this->size();
    
    while (numToShift >= (int) this->size())
        numToShift -= this->size();
    
    if (numToShift == 0)
        return *this;

    std::rotate(this->vec.begin(), this->vec.begin()+numToShift, this->vec.end());
    return *this;
}

/**
 * \brief Circular rotation.
 *
 * \param vector Buffer to rotate.
 * \param numToShift Number of positions to shift in the circular rotation.  numToShift
 *      can be positive or negative.  If you visualize the 0 index value at the left and
 *      the end of the array at the right, positive numToShift values shift the array to
 *      the left, and negative values shift it to the right.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & rotate(ComplexVector<T> & vector, int numToShift) {
    return vector.rotate(numToShift);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::reverse() {
    std::reverse(this->vec.begin(), this->vec.end());
    return *this;
}

/**
 * \brief Reverses the order of the elements in \ref vec.
 *
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & reverse(ComplexVector<T> & vector) {
    return vector.reverse();
}

/**
 * \brief Sets the length of \ref vec to "len".
 *
 * \param vector Buffer to operate on.
 * \param len The new length for \ref vec.  If len is longer than vec's current size, the
 *      new elements will be set to "val".  If len is less than vec's current size the extra
 *      elements will be cut off and the other elements will remain the same.
 * \param val The value to set any new elements to.  Defaults to 0.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & resize(ComplexVector<T> & vector, int len, T val = 0) {
    return vector.resize(len, val);
}

/**
 * \brief Lengthens \ref vec by "len" elements.
 *
 * \param vector Buffer to operate on.
 * \param len The number of elements to add to \ref vec.
 * \param val The value to set the new elements to.  Defaults to 0.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & pad(ComplexVector<T> & vector, int len, T val = 0) {
    return vector.pad(len, val);
}
    
template <class T>
ComplexVector<T> & ComplexVector<T>::upsample(int rate, int phase) {
	assert(rate > 0);
	assert(phase >= 0 && phase < rate);
	if (rate == 1)
		return *this;

	int originalSize = this->vec.size();
	this->vec.resize(originalSize*rate);
	int from, to;
	for (from = originalSize - 1, to = this->size() - (rate - phase); to > 0; from--, to -= rate) {
		this->vec[to] = this->vec[from];
		this->vec[from] = 0;
	}
	return *this;
}

/**
 * \brief Inserts rate-1 zeros between samples.
 *
 * \param vector Buffer to operate on.
 * \param rate Indicates how many zeros should be inserted between samples.
 * \param phase Indicates how many of the zeros should be before the samples (as opposed to
 *      after).  Valid values are 0 to "rate"-1.  Defaults to 0.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & upsample(ComplexVector<T> & vector, int rate, int phase = 0) {
    return vector.upsample(rate, phase);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::downsample(int rate, int phase) {
	assert(rate > 0);
	assert(phase >= 0 && phase < rate);
	if (rate == 1)
		return *this;

	int newSize = this->size() / rate;
	int from, to;
	for (from = phase, to = 0; to < newSize; from += rate, to++) {
		this->vec[to] = this->vec[from];
	}
	this->vec.resize(newSize);
	return *this;
}

/**
 * \brief Removes rate-1 samples out of every rate samples.
 *
 * \param vector Buffer to operate on.
 * \param rate Indicates how many samples should be removed.
 * \param phase Tells the function which sample should be the first to be kept.  Valid values
 *      are 0 to "rate"-1.  Defaults to 0.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & downsample(ComplexVector<T> & vector, int rate, int phase = 0) {
    return vector.downsample(rate, phase);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::cumsum(T initialVal) {
    T sum = initialVal;
    for (unsigned i=0; i<this->size(); i++) {
        sum += this->vec[i];
        this->vec[i] = sum;
    }
    return *this;
}

/**
 * \brief Replaces "vector" with the cumulative sum of the samples in "vector".
 *
 * \param vector Data to operate on.
 * \param initialVal Initializing value for the cumulative sum.  Defaults to zero.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & cumsum(ComplexVector<T> & vector, T initialVal = 0) {
    return vector.cumsum(initialVal);
}
    
template <class T>
ComplexVector<T> & ComplexVector<T>::diff() {
	assert(this->size() > 1);
	for (unsigned i=0; i<(this->size()-1); i++) {
		this->vec[i] = this->vec[i + 1] - this->vec[i];
	}
    this->resize(this->size()-1);
    return *this;
}

/**
 * \brief Replaces \ref vec with the difference between successive samples in vec.
 *
 * The resulting \ref vec is one element shorter than it was previously.
 * \param vector Buffer to operate on.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & diff(ComplexVector<T> & vector) {
    return vector.diff();
}

template <class T>
ComplexVector<T> & ComplexVector<T>::diff(std::complex<T> & previousVal) {
	assert(this->size() > 0);
    std::complex<T> nextPreviousVal = this->vec[this->size()-1];
	for (unsigned i=this->size()-1; i>0; i--) {
		this->vec[i] = this->vec[i] - this->vec[i - 1];
	}
    this->vec[0] = this->vec[0] - previousVal;
    previousVal = nextPreviousVal;
    return *this;
}

/**
 * \brief Replaces \ref vec with the difference between successive samples in vec.
 *
 * \param vector Buffer to operate on.
 * \param previousVal The last value in the sample stream before the current contents
 *      of \ref vec.  previousVal allows the resulting vec to be the same size as the
 *      previous vec.
 * \return Reference to "vector".
 */
template <class T>
ComplexVector<T> & diff(ComplexVector<T> & vector, std::complex<T> & previousVal) {
    return vector.diff(previousVal);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::conv(ComplexVector<T> & data, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }
    *dataTmp = data.vec;
    
    if (trimTails) {
        // Initial partial overlap
        int initialTrim = (this->size() - 1) / 2;
        for (resultIndex=0; resultIndex<((int)this->size()-1) - initialTrim; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=initialTrim + resultIndex; filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        
        // Middle full overlap
        for (; resultIndex<(int)dataTmp->size() - initialTrim; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex - ((this->size()-1) - initialTrim), filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex - ((this->size()-1) - initialTrim), filterIndex=this->size()-1;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
    }
    else {
        data.resize(data.size() + this->size() - 1);
        
        // Initial partial overlap
        for (resultIndex=0; resultIndex<(int)this->size()-1; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=resultIndex; filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        
        // Middle full overlap
        for (; resultIndex<(int)dataTmp->size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex - (this->size()-1), filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex - (this->size()-1), filterIndex=this->size()-1;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
    }
    return data;
}

/**
 * \brief Convolution function.
 *
 * \param data Buffer to operate on.
 * \param filter The filter that will convolve "data".
 * \param trimTails "False" tells the function to return the entire convolution, which is
 *      the length of "data" plus the length of "filter" - 1.  "True" tells the
 *      function to retain the size of "data" by trimming the tails at both ends of
 *      the convolution.
 * \return Reference to "data", which holds the result of the convolution.
 */
template <class T>
inline ComplexVector<T> & conv(ComplexVector<T> & data, ComplexVector<T> & filter, bool trimTails = false) {
    return filter.conv(data, trimTails);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::decimate(ComplexVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }
    *dataTmp = data.vec;
    
    if (trimTails) {
        data.resize((data.size() + rate - 1) / rate);
        
        // Initial partial overlap
        int initialTrim = (this->size() - 1) / 2;
        for (resultIndex=0; resultIndex<(((int)this->size()-1) - initialTrim + rate - 1)/rate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=initialTrim + resultIndex*rate; filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        
        // Middle full overlap
        for (; resultIndex<((int)dataTmp->size() - initialTrim + rate - 1)/rate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate - ((this->size()-1) - initialTrim), filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate - ((this->size()-1) - initialTrim), filterIndex=this->size()-1;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
    }
    else {
        data.resize(((data.size() + this->size() - 1) + (rate - 1)) / rate);
        
        // Initial partial overlap
        for (resultIndex=0; resultIndex<((int)this->size()-1+rate-1)/rate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=resultIndex*rate; filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        
        // Middle full overlap
        for (; resultIndex<((int)dataTmp->size()+rate-1)/rate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate - (this->size()-1), filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate - (this->size()-1), filterIndex=this->size()-1;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
    }
    return data;
}

/**
 * \brief Decimate function.
 *
 * This function is equivalent to filtering with the \ref conv function and downsampling
 * with the \ref downsample function, but much more efficient.
 *
 * \param data Buffer to operate on.
 * \param rate Indicates how much to downsample.
 * \param filter The filter that will convolve "data".
 * \param trimTails "False" tells the function to return the entire convolution.  "True"
 *      tells the function to retain the size of "data" by trimming the tails at both
 *      ends of the convolution.
 * \return Reference to "data", which holds the result of the decimation.
 */
template <class T>
inline ComplexVector<T> & decimate(ComplexVector<T> & data, int rate, ComplexVector<T> & filter, bool trimTails = false) {
    return filter.decimate(data, rate, trimTails);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::interp(ComplexVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }
    *dataTmp = data.vec;
    
    if (trimTails) {
        data.resize(data.size() * rate);

        // Initial partial overlap
        int initialTrim = (this->size() - 1) / 2;
        for (resultIndex=0, dataStart=0; resultIndex<(int)this->size()-1 - initialTrim; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=initialTrim + resultIndex; filterIndex>=0; dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
       
        // Middle full overlap
        for (dataStart=0, filterStart=(int)this->size()-1; resultIndex<(int)dataTmp->size()*rate - initialTrim; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 filterIndex>=0; dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            ++filterStart;
            if (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= rate;
                ++dataStart;
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            ++filterStart;
            if (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= rate;
                ++dataStart;
            }
        }
    }
    else {
        data.resize(data.size() * rate + this->size() - 1 - (rate - 1));
        
        // Initial partial overlap
        for (resultIndex=0, dataStart=0; resultIndex<(int)this->size()-1; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=resultIndex; filterIndex>=0; dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        
        // Middle full overlap
        for (dataStart=0, filterStart=resultIndex; resultIndex<(int)dataTmp->size()*rate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 filterIndex>=0; dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            ++filterStart;
            if (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= rate;
                ++dataStart;
            }
        }

        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex-=rate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            ++filterStart;
            if (filterStart >= (int) this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= rate;
                ++dataStart;
            }
        }
    }
    return data;
}

/**
 * \brief Interpolation function.
 *
 * This function is equivalent to upsampling followed by filtering, but is much more efficient.
 *
 * \param data Buffer to operate on.
 * \param rate Indicates how much to upsample.
 * \param filter The filter that will convolve "data".
 * \param trimTails "False" tells the function to return the entire convolution.  "True"
 *      tells the function to retain the size of "data" by trimming the tails at both
 *      ends of the convolution.
 * \return Reference to "data", which holds the result of the interpolation.
 */
template <class T>
inline ComplexVector<T> & interp(ComplexVector<T> & data, int rate, ComplexVector<T> & filter, bool trimTails = false) {
    return filter.interp(data, rate, trimTails);
}

template <class T>
ComplexVector<T> & ComplexVector<T>::resample(ComplexVector<T> & data, int interpRate, int decimateRate,  bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }
    *dataTmp = data.vec;
    
    if (trimTails) {
        int interpLen = data.size() * interpRate;
        int resampLen = (interpLen + decimateRate - 1) / decimateRate;
        data.resize(resampLen);

        // Initial partial overlap
        int initialTrim = (this->size() - 1) / 2;
        for (resultIndex=0, dataStart=0, filterStart=initialTrim;
             resultIndex<((int)this->size()-1 - initialTrim + decimateRate-1)/decimateRate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=filterStart; filterIndex>=0; dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
        
        // Middle full overlap
        for (; resultIndex<((int)dataTmp->size()*interpRate - initialTrim + decimateRate-1)/decimateRate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 filterIndex>=0; dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
        
        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
    }
    else {
        int interpLen = data.size() * interpRate + this->size() - 1 - (interpRate - 1);
        int resampLen = (interpLen + decimateRate - 1) / decimateRate;
        data.resize(resampLen);
        
        // Initial partial overlap
        for (resultIndex=0, dataStart=0, filterStart=0; resultIndex<((int)this->size()-1+decimateRate-1)/decimateRate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=0, filterIndex=filterStart; filterIndex>=0; dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
        
        // Middle full overlap
        for (; resultIndex<((int)dataTmp->size()*interpRate + decimateRate-1)/decimateRate; resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 filterIndex>=0; dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
        
        // Final partial overlap
        for (; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=dataStart, filterIndex=filterStart;
                 dataIndex<(int)dataTmp->size(); dataIndex++, filterIndex-=interpRate) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
            filterStart += decimateRate;
            while (filterStart >= (int)this->size()) {
                // Filter no longer overlaps with this data sample, so the first overlap sample is the next one.  We thus
                // increment the data index and decrement the filter index.
                filterStart -= interpRate;
                ++dataStart;
            }
        }
    }
    return data;
}

/**
 * \brief Resample function.
 *
 * This function is equivalent to upsampling by "interpRate", filtering, and downsampling
 *      by "decimateRate", but is much more efficient.
 *
 * \param data Buffer to operate on.
 * \param interpRate Indicates how much to upsample.
 * \param decimateRate Indicates how much to downsample.
 * \param filter The filter that will convolve "data".
 * \param trimTails "False" tells the function to return the entire convolution.  "True"
 *      tells the function to retain the size of "data" by trimming the tails at both
 *      ends of the convolution.
 * \return Reference to "data", which holds the result of the resampling.
 */
template <class T>
inline ComplexVector<T> & resample(ComplexVector<T> & data, int interpRate, int decimateRate,
            ComplexVector<T> & filter, bool trimTails = false) {
    return filter.resample(data, interpRate, decimateRate, trimTails);
}

template <class T>
T ComplexVector<T>::tone(T freq, T sampleFreq, T phase, unsigned numSamples) {
    assert(sampleFreq > 0.0);
    
    if (numSamples && numSamples != this->size()) {
        this->resize(numSamples);
    }
    
    T phaseInc = (freq / sampleFreq) * 2 * M_PI;
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i].real(std::cos(phase));
        this->vec[i].imag(std::sin(phase));
        phase += phaseInc;
    }
    return phase;
}

/**
 * \brief Generates a complex tone.
 *
 * \param vec The vector to put the tone in.
 * \param freq The tone frequency.
 * \param sampleFreq The sample frequency.  Defaults to 1 Hz.
 * \param phase The tone's starting phase, in radians.  Defaults to 0.
 * \param numSamples The number of samples to generate.  "0" indicates to generate
 *      this->size() samples.  Defaults to 0.
 * \return Reference to "this".
 */
template <class T>
T tone(ComplexVector<T> & vec, T freq, T sampleFreq = 1.0, T phase = 0.0, unsigned numSamples = 0) {
    return vec.tone(freq, sampleFreq, phase, numSamples);
}

template <class T>
T ComplexVector<T>::modulate(T freq, T sampleFreq, T phase) {
    assert(sampleFreq > 0.0);
    
    T phaseInc = (freq / sampleFreq) * 2 * M_PI;
    for (unsigned i=0; i<this->size(); i++) {
        this->vec[i] *= std::complex<T>(std::cos(phase), std::sin(phase));
        phase += phaseInc;
    }
    return phase;
}

/**
 * \brief Modulates the data with a complex sinusoid.
 *
 * \param freq The modulating tone frequency.
 * \param sampleFreq The sample frequency of the data.  Defaults to 1 Hz.
 * \param phase The modulating tone's starting phase, in radians.  Defaults to 0.
 * \return The next phase if the tone were to continue.
 */
template <class T>
T modulate(ComplexVector<T> &data, T freq, T sampleFreq, T phase) {
    return data.modulate(freq, sampleFreq, phase);
}

template <class T>
void ComplexVector<T>::slice(unsigned lower, unsigned upper, ComplexVector<T> &destination) {
	assert(lower <= upper);
	assert(upper < this->size());

	destination.resize(upper - lower + 1);
	for (unsigned from=lower, to=0; from<=upper; from++, to++) {
		destination[to] = this->vec[from];
	}
}

template <class T>
ComplexVector<T> & ComplexVector<T>::ceil() {
	for (int index=0; index<this->size(); index++) {
		this->vec[index].real(std::ceil(this->vec[index].real()));
		this->vec[index].imag(std::ceil(this->vec[index].imag()));
	}
	return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::floor() {
	for (int index=0; index<this->size(); index++) {
		this->vec[index].real(std::floor(this->vec[index].real()));
		this->vec[index].imag(std::floor(this->vec[index].imag()));
	}
	return *this;
}

template <class T>
ComplexVector<T> & ComplexVector<T>::round() {
	for (int index=0; index<this->size(); index++) {
		this->vec[index].real(std::round(this->vec[index].real()));
		this->vec[index].imag(std::round(this->vec[index].imag()));
	}
	return *this;
}

};

#endif

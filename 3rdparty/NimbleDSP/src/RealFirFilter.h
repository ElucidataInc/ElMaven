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
 * @file RealFirFilter.h
 *
 * Definition of the template class RealFirFilter.
 */

#ifndef NimbleDSP_RealFirFilter_h
#define NimbleDSP_RealFirFilter_h

#include <complex>
#include <math.h>
#include "RealVector.h"
#include "ParksMcClellan.h"


namespace NimbleDSP {

/**
 * \brief Class for real FIR filters.
 */
template <class T>
class RealFirFilter : public RealVector<T> {
 protected:
    /**
     * \brief Saved data that is used for stream filtering.
     */
    std::vector<char> savedData;
    
    /**
     * \brief Indicates how many samples are in \ref savedData.  Used for stream filtering.
     */
    int numSavedSamples;
    
    /**
     * \brief Indicates the filter phase.  Used for stream filtering.
     */
    int phase;
    
    /**
     * \brief Applies a Hamming window on the current contents of "this".
     */
    void hamming(void);
    
 public:
    /**
     * \brief Determines how the filter should filter.
     *
     * NimbleDSP::ONE_SHOT_RETURN_ALL_RESULTS is equivalent to "trimTails = false" of the Vector convolution methods.
     * NimbleDSP::ONE_SHOT_TRIM_TAILS is equivalent to "trimTails = true" of the Vector convolution methods.
     * NimbleDSP::STREAMING maintains the filter state from call to call so it can produce results as if it had
     *      filtered one continuous set of data.
     */
    FilterOperationType filtOperation;

    /*****************************************************************************************
                                        Constructors
    *****************************************************************************************/
    /**
     * \brief Basic constructor.
     *
     * Just sets the size of \ref buf and the pointer to the scratch buffer, if one is provided.
     * \param size Size of \ref buf.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    RealFirFilter<T>(unsigned size = DEFAULT_BUF_LEN, FilterOperationType operation = STREAMING, std::vector<T> *scratch = NULL) : RealVector<T>(size, scratch)
            {if (size > 0) {savedData.resize((size - 1) * sizeof(std::complex<T>)); numSavedSamples = size - 1;}
             else {savedData.resize(0); numSavedSamples = 0;} phase = 0; filtOperation = operation;}
    
    /**
     * \brief Vector constructor.
     *
     * Sets buf equal to the input "data" parameter and sets the pointer to the scratch buffer,
     *      if one is provided.
     * \param data Vector that \ref buf will be set equal to.
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    template <typename U>
    RealFirFilter<T>(std::vector<U> data, FilterOperationType operation = STREAMING, std::vector<T> *scratch = NULL) : RealVector<T>(data, scratch)
            {savedData.resize((data.size() - 1) * sizeof(std::complex<T>)); numSavedSamples = data.size() - 1; phase = 0; filtOperation = operation;}
    
    /**
     * \brief Array constructor.
     *
     * Sets buf equal to the input "data" array and sets the pointer to the scratch buffer,
     *      if one is provided.
     * \param data Array that \ref buf will be set equal to.
     * \param dataLen Length of "data".
     * \param scratch Pointer to a scratch buffer.  The scratch buffer can be shared by multiple
     *      objects (in fact, I recommend it), but if there are multiple threads then it should
     *      be shared only by objects that are accessed by a single thread.  Objects in other
     *      threads should have a separate scratch buffer.  If no scratch buffer is provided
     *      then one will be created in methods that require one and destroyed when the method
     *      returns.
     */
    template <typename U>
    RealFirFilter<T>(U *data, unsigned dataLen, FilterOperationType operation = STREAMING, std::vector<T> *scratch = NULL) : RealVector<T>(data, dataLen, scratch)
            {savedData.resize((dataLen - 1) * sizeof(std::complex<T>)); numSavedSamples = dataLen - 1; phase = 0; filtOperation = operation;}
    
    /**
     * \brief Copy constructor.
     */
    RealFirFilter<T>(const RealFirFilter<T>& other) {this->vec = other.vec; savedData = other.savedData;
            numSavedSamples = other.numSavedSamples; phase = other.phase; filtOperation = other.filtOperation;}
    
    /*****************************************************************************************
                                            Operators
    *****************************************************************************************/
    /**
     * \brief Assignment operator.
     */
    RealFirFilter<T>& operator=(const Vector<T>& rhs) {this->vec = rhs.vec; savedData.resize(this->size() - 1); phase = 0; filtOperation = STREAMING; return *this;}
    
    /*****************************************************************************************
                                            Methods
    *****************************************************************************************/
    /**
     * \brief Convolution method.
     *
     * \param data The buffer that will be filtered.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the convolution.
     */
    virtual RealVector<T> & conv(RealVector<T> & data, bool trimTails = false);
    
    /**
     * \brief Convolution method for complex data.
     *
     * \param data The buffer that will be filtered.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the convolution.
     */
    virtual ComplexVector<T> & convComplex(ComplexVector<T> & data, bool trimTails = false);
    
    /**
     * \brief Decimate method.
     *
     * This method is equivalent to filtering with the \ref conv method and downsampling
     * with the \ref downsample method, but much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param rate Indicates how much to downsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the decimation.
     */
    virtual RealVector<T> & decimate(RealVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Decimate method for complex data.
     *
     * This method is equivalent to filtering with the \ref conv method and downsampling
     * with the \ref downsample method, but much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param rate Indicates how much to downsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the decimation.
     */
    virtual ComplexVector<T> & decimateComplex(ComplexVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Interpolation method.
     *
     * This method is equivalent to upsampling followed by filtering, but is much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param rate Indicates how much to upsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the interpolation.
     */
    virtual RealVector<T> & interp(RealVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Interpolation method for complex data.
     *
     * This method is equivalent to upsampling followed by filtering, but is much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param rate Indicates how much to upsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the interpolation.
     */
    virtual ComplexVector<T> & interpComplex(ComplexVector<T> & data, int rate, bool trimTails = false);
    
    /**
     * \brief Resample method.
     *
     * This method is equivalent to upsampling by "interpRate", filtering, and downsampling
     *      by "decimateRate", but is much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param interpRate Indicates how much to upsample.
     * \param decimateRate Indicates how much to downsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the resampling.
     */
    virtual RealVector<T> & resample(RealVector<T> & data, int interpRate, int decimateRate, bool trimTails = false);
    
    /**
     * \brief Resample method for complex data.
     *
     * This method is equivalent to upsampling by "interpRate", filtering, and downsampling
     *      by "decimateRate", but is much more efficient.
     *
     * \param data The buffer that will be filtered.
     * \param interpRate Indicates how much to upsample.
     * \param decimateRate Indicates how much to downsample.
     * \param trimTails This parameter is ignored.  The operation of the filter is determined by how
     *      \ref filtOperation is set.
     * \return Reference to "data", which holds the result of the resampling.
     */
    virtual ComplexVector<T> & resampleComplex(ComplexVector<T> & data, int interpRate, int decimateRate, bool trimTails = false);
    
    /**
     * \brief Parks-McClellan algorithm for generating equiripple FIR filter coefficients.
     *
     * Application of the remez algorithm to producing equiripple FIR filter coefficients.  It has issues with
     * convergence.  It can generally converge up to 128 taps- more than that and it gets iffy.
     *
     * The PM algorithm implementation is a somewhat modified version of Iowa Hills Software's port of the
     * PM algorithm from the original Fortran to C.  Much appreciation to them for their work.
     *
     * \param filterOrder Indicates that the number of taps should be filterOrder + 1.
     * \param numBands The number of pass and stop bands.  Maximum of 10 bands.
     * \param freqPoints Pairs of points specify the boundaries of the bands, thus the length of this array
     *          must be 2 * numBands.  The frequencies in between the bands are the transition bands.  "0"
     *          corresponds to 0 Hz, and "1" corresponds to the Nyquist frequency.  Example: "0.0 0.3 .5 1.0"
     *          specifies two bands, one from 0 Hz to .3 * Nyquist and the other from 0.5 Nyquist to Nyquist.
     *          .3 * Nyquist to .5 * Nyquist is the transition band.
     * \param desiredBandResponse Indicates what the desired amplitude response is in the corresponding band.
     *          This array must have "numBands" elements.
     * \param weight Indicates how much weight should be given the performance of the filter in that band.  If
     *          all of the elements are "1" then they will have equal weight which will produce a true
     *          equiripple filter (same amount of ripple in the pass and stop bands).  If the stop bands are
     *          assigned more weight than the passbands then the attenuation in the stop bands will be increased
     *          at the expense of more ripple in the pass bands.
     * \param lGrid Grid density.  Defaults to 16.  This value should generally not be set lower than 16.
     *          Setting it higher than 16 can produce a filter with a better fit to the desired response at
     *          the cost of increased computations.
     * \return Boolean that indicates whether the filter converged or not.
     */
    bool firpm(int filterOrder, int numBands, double *freqPoints, double *desiredBandResponse,
                double *weight, int lGrid = 16);
    
    /**
     * \brief Generates a filter that can delay signals by an arbitrary sub-sample time.
     *
     * \param numTaps Number of taps to use in the filter.  If the only purpose of the filter is to delay
     *          the signal then the number of taps is not crucial.  If it is doing actual stop-band filtering
     *          too then the number of taps is important.
     * \param bandwidth The approximate bandwidth of the filter, normalized to the range 0.0 to 1.0, where
     *          1.0 is the Nyquist frequency.  The bandwidth must be greater than 0 and less than 1.
     * \param delay Amount of sample time to delay.  For example, a delay value of 0.1 would indicate to delay
     *          by one tenth of a sample.  "delay" can be positive or negative.
     */
    void fractionalDelayFilter(int numTaps, double bandwidth, double delay);
    
    /**
     * \brief Correlation method.
     *
     * \param data The buffer that will be correlated.
     * \return Reference to "data", which holds the result of the convolution.
     */
    virtual RealVector<T> & corr(RealVector<T> & data);
    
    /**
     * \brief Generates a Hamming window.
     *
     * \param len The window length.
     */
     void hamming(unsigned len);
    
    /**
     * \brief Generates a Hann window.
     *
     * \param len The window length.
     */
     void hann(unsigned len);
    
    /**
     * \brief Generates a generalized Hamming window.
     *
     * \param len The window length.
     * \param alpha
     * \param beta
     */
     void generalizedHamming(unsigned len, double alpha, double beta);
    
    /**
     * \brief Generates a Blackman window.
     *
     * \param len The window length.
     */
     void blackman(unsigned len);
    
    /**
     * \brief Generates a Blackman-harris window.
     *
     * \param len The window length.
     */
     void blackmanHarris(unsigned len);
    
};


template <class T>
RealVector<T> & RealFirFilter<T>::conv(RealVector<T> & data, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector<T> scratch;
    std::vector<T> *dataTmp;
    T *savedDataArray = (T *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING:
        dataTmp->resize((this->size() - 1) + data.size());
        for (int i=0; i<this->size()-1; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + this->size() - 1] = data[i];
        }
        
        for (resultIndex=0; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex, filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        for (int i=0; i<this->size()-1; i++) {
            savedDataArray[i] = (*dataTmp)[i + data.size()];
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;

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
        break;
    }
    return data;
}

template <class T>
ComplexVector<T> & RealFirFilter<T>::convComplex(ComplexVector<T> & data, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    std::complex<T> *savedDataArray = (std::complex<T> *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING:
        dataTmp->resize((this->size() - 1) + data.size());
        for (int i=0; i<this->size()-1; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + this->size() - 1] = data[i];
        }
        
        for (resultIndex=0; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex, filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        for (int i=0; i<this->size()-1; i++) {
            savedDataArray[i] = (*dataTmp)[i + data.size()];
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;

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
        break;
    }
    return data;
}

template <class T>
RealVector<T> & RealFirFilter<T>::decimate(RealVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector<T> scratch;
    std::vector<T> *dataTmp;
    T *savedDataArray = (T *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        data.resize((data.size() + numSavedSamples - (this->size() - 1) + rate - 1)/rate);
        for (resultIndex=0; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate, filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        int nextResultDataPoint = resultIndex * rate;
        numSavedSamples = (unsigned) dataTmp->size() - nextResultDataPoint;

        for (int i=0; i<numSavedSamples; i++) {
            savedDataArray[i] = (*dataTmp)[i + nextResultDataPoint];
        }
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
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
        break;
    }
    return data;
}

template <class T>
ComplexVector<T> & RealFirFilter<T>::decimateComplex(ComplexVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    std::complex<T> *savedDataArray = (std::complex<T> *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        data.resize((data.size() + numSavedSamples - (this->size() - 1) + rate - 1)/rate);
        for (resultIndex=0; resultIndex<(int)data.size(); resultIndex++) {
            data[resultIndex] = 0;
            for (dataIndex=resultIndex*rate, filterIndex=this->size()-1;
                 filterIndex>=0; dataIndex++, filterIndex--) {
                data[resultIndex] += (*dataTmp)[dataIndex] * this->vec[filterIndex];
            }
        }
        int nextResultDataPoint = resultIndex * rate;
        numSavedSamples = ((int) dataTmp->size()) - nextResultDataPoint;

        for (int i=0; i<numSavedSamples; i++) {
            savedDataArray[i] = (*dataTmp)[i + nextResultDataPoint];
        }
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
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
        break;
    }
    return data;
}

template <class T>
RealVector<T> & RealFirFilter<T>::interp(RealVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    std::vector<T> scratch;
    std::vector<T> *dataTmp;
    T *savedDataArray = (T *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        int numTaps = (this->size() + rate - 1) / rate;
        if (numSavedSamples >= numTaps) {
            // First call to interp, have too many "saved" (really just the initial zeros) samples
            numSavedSamples = numTaps - 1;
            phase = (numTaps - 1) * rate;
        }
        
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        data.resize((unsigned) dataTmp->size() * rate);
        bool keepGoing = true;
        for (resultIndex=0, dataStart=0, filterStart=phase; keepGoing; ++resultIndex) {
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
                if (dataTmp->size() - dataStart == numSavedSamples) {
                    keepGoing = false;
                    phase = filterStart;
                }
            }
        }
        data.resize(resultIndex);

        int i;
        for (i=0; dataStart<dataTmp->size(); i++, dataStart++) {
            savedDataArray[i] = (*dataTmp)[dataStart];
        }
        numSavedSamples = i;
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
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
        break;
    }
    return data;
}

template <class T>
ComplexVector<T> & RealFirFilter<T>::interpComplex(ComplexVector<T> & data, int rate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    std::complex<T> *savedDataArray = (std::complex<T> *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        int numTaps = (this->size() + rate - 1) / rate;
        if (numSavedSamples >= numTaps) {
            // First call to interp, have too many "saved" (really just the initial zeros) samples
            numSavedSamples = numTaps - 1;
            phase = (numTaps - 1) * rate;
        }
        
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        data.resize((unsigned) dataTmp->size() * rate);
        bool keepGoing = true;
        for (resultIndex=0, dataStart=0, filterStart=phase; keepGoing; ++resultIndex) {
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
                if (dataTmp->size() - dataStart == numSavedSamples) {
                    keepGoing = false;
                    phase = filterStart;
                }
            }
        }
        data.resize(resultIndex);

        int i;
        for (i=0; dataStart<dataTmp->size(); i++, dataStart++) {
            savedDataArray[i] = (*dataTmp)[dataStart];
        }
        numSavedSamples = i;
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
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
        break;
    }
    return data;
}

template <class T>
RealVector<T> & RealFirFilter<T>::resample(RealVector<T> & data, int interpRate, int decimateRate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    int interpLen, resampLen;
    std::vector<T> scratch;
    std::vector<T> *dataTmp;
    T *savedDataArray = (T *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        int numTaps = (this->size() + interpRate - 1) / interpRate;
        if (numSavedSamples >= numTaps) {
            // First call to interp, have too many "saved" (really just the initial zeros) samples
            numSavedSamples = numTaps - 1;
            phase = (numTaps - 1) * interpRate;
        }
        
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        interpLen = (unsigned) dataTmp->size() * interpRate;
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
        data.resize(resampLen);
        bool keepGoing = true;
        for (resultIndex=0, dataStart=0, filterStart=phase; keepGoing; ++resultIndex) {
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
            if (dataTmp->size() - dataStart == numSavedSamples) {
                keepGoing = false;
                phase = filterStart;
            }
        }
        data.resize(resultIndex);
        
        int i;
        for (i=0; dataStart<dataTmp->size(); i++, dataStart++) {
            savedDataArray[i] = (*dataTmp)[dataStart];
        }
        numSavedSamples = i;
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
        interpLen = data.size() * interpRate + this->size() - 1 - (interpRate - 1);
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
        interpLen = data.size() * interpRate;
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
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
        break;
    }
    return data;
}

template <class T>
ComplexVector<T> & RealFirFilter<T>::resampleComplex(ComplexVector<T> & data, int interpRate, int decimateRate, bool trimTails) {
    int resultIndex;
    int filterIndex;
    int dataIndex;
    int dataStart, filterStart;
    int interpLen, resampLen;
    std::vector< std::complex<T> > scratch;
    std::vector< std::complex<T> > *dataTmp;
    std::complex<T> *savedDataArray = (std::complex<T> *) VECTOR_TO_ARRAY(savedData);
    
    if (data.scratchBuf == NULL) {
        dataTmp = &scratch;
    }
    else {
        dataTmp = data.scratchBuf;
    }

    switch (filtOperation) {

    case STREAMING: {
        int numTaps = (this->size() + interpRate - 1) / interpRate;
        if (numSavedSamples >= numTaps) {
            // First call to interp, have too many "saved" (really just the initial zeros) samples
            numSavedSamples = numTaps - 1;
            phase = (numTaps - 1) * interpRate;
        }
        
        dataTmp->resize(numSavedSamples + data.size());
        for (int i=0; i<numSavedSamples; i++) {
            (*dataTmp)[i] = savedDataArray[i];
        }
        for (int i=0; i<data.size(); i++) {
            (*dataTmp)[i + numSavedSamples] = data[i];
        }
        
        interpLen = (unsigned) dataTmp->size() * interpRate;
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
        data.resize(resampLen);
        bool keepGoing = true;
        for (resultIndex=0, dataStart=0, filterStart=phase; keepGoing; ++resultIndex) {
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
            if (dataTmp->size() - dataStart == numSavedSamples) {
                keepGoing = false;
                phase = filterStart;
            }
        }
        data.resize(resultIndex);
        
        int i;
        for (i=0; dataStart<dataTmp->size(); i++, dataStart++) {
            savedDataArray[i] = (*dataTmp)[dataStart];
        }
        numSavedSamples = i;
        }
        break;

    case ONE_SHOT_RETURN_ALL_RESULTS:
        *dataTmp = data.vec;
        interpLen = data.size() * interpRate + this->size() - 1 - (interpRate - 1);
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
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
        break;

    case ONE_SHOT_TRIM_TAILS:
        *dataTmp = data.vec;
        interpLen = data.size() * interpRate;
        resampLen = (interpLen + decimateRate - 1) / decimateRate;
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
        break;
    }
    return data;
}


template <class T>
bool RealFirFilter<T>::firpm(int filterOrder, int numBands, double *freqPoints, double *desiredBandResponse,
                            double *weight, int lGrid) {
    bool converged;
    
    this->resize(filterOrder + 1);
    std::vector<double> temp(filterOrder + 1);
    
    // Need to renormalize the frequency points because for us the Nyquist frequency is 1.0, but for the
    // Iowa Hills code it is 0.5.
    for (int i=0; i<numBands*2; i++) {
        freqPoints[i] /= 2;
    }
    
    // Move the pointers back 1 (i.e. subtract one) because the ParksMcClellan code was ported from Fortran,
    // which apparently uses 1-based arrays, not 0-based arrays.
    converged = ParksMcClellan2(&(temp[0]), filterOrder + 1, PASSBAND_FILTER, numBands, freqPoints-1,
                    desiredBandResponse-1, weight-1, lGrid);
    for (int i=0; i<= filterOrder; i++) {
        (*this)[i] = (T) temp[i];
    }
    return converged;
}


template <class T>
void RealFirFilter<T>::fractionalDelayFilter(int numTaps, double bandwidth, double delay) {
    assert(bandwidth > 0 && bandwidth < 1.0);
    assert(numTaps > 0);
    
    int index;
    double tapTime;
    double timeIncrement = bandwidth * M_PI;
    
    if (numTaps % 2) {
        tapTime = (numTaps / 2 + delay) * -timeIncrement;
    }
    else {
        tapTime = (((int) numTaps / 2) - 0.5 + delay) * -timeIncrement;
    }
    
    this->resize(numTaps);
    
    // Create the delayed sinc filter
    for (index = 0; index < numTaps; index++, tapTime += timeIncrement) {
        if (tapTime != 0.0) {
            (*this)[index] = (T) (sin(tapTime) / tapTime);
        }
        else {
            (*this)[index] = (T) 1.0;
        }
    }
    
    // Window the filter
    hamming();
}

template <class T>
void RealFirFilter<T>::hamming() {
    T phase = -M_PI;
    T phaseIncrement = 2 * M_PI / (this->size() - 1);
    T alpha = 0.54;
    T beta = 0.46;
    unsigned start, end;
    
    for (start=0, end=this->size()-1; start < end; start++, end--, phase += phaseIncrement) {
        double hammingVal = alpha + beta * cos(phase);
        (*this)[start] *= hammingVal;
        (*this)[end] *= hammingVal;
    }
}

template <class T>
RealVector<T> & RealFirFilter<T>::corr(RealVector<T> & data) {
    this->reverse();
    this->conv(data);
    this->reverse();
    return data;
}

/**
 * \brief Correlation function.
 *
 * \param data Buffer to operate on.
 * \param filter The filter that will correlate with "data".
 * \return Reference to "data", which holds the result of the convolution.
 */
template <class T>
inline RealVector<T> & corr(RealVector<T> & data, RealFirFilter<T> & filter) {
    return filter.corr(data);
}

template <class T>
void RealFirFilter<T>::hamming(unsigned len)
{
    generalizedHamming(len, 0.54, 0.46);
}

template <class T>
void RealFirFilter<T>::hann(unsigned len)
{
    generalizedHamming(len, 0.5, 0.5);
}

template <class T>
void RealFirFilter<T>::generalizedHamming(unsigned len, double alpha, double beta)
{
    this->resize(len);
    double N = len - 1;
    for (unsigned index=0; index<len; index++) {
        this->vec[index] = (T) (alpha - beta * cos(2 * M_PI * ((double) index) / N));
    }
}

template <class T>
void RealFirFilter<T>::blackman(unsigned len)
{
    const double alpha[] = {0.42, 0.5, 0.08};
    
    this->resize(len);
    double N = len - 1;
    for (unsigned index=0; index<len; index++) {
        this->vec[index] = (T) (alpha[0] - alpha[1] * cos(2 * M_PI * ((double) index) / N) +
                alpha[2] * cos(4 * M_PI * ((double) index) / N));
    }
}

template <class T>
void RealFirFilter<T>::blackmanHarris(unsigned len)
{
    const double alpha[] = {0.35875, 0.48829, 0.14128, 0.01168};
    
    this->resize(len);
    double N = len - 1;
    for (unsigned index=0; index<len; index++) {
        this->vec[index] = (T) (alpha[0] - alpha[1] * cos(2 * M_PI * ((double) index) / N) +
                alpha[2] * cos(4 * M_PI * ((double) index) / N) - alpha[3] * cos(6 * M_PI * ((double) index) / N));
    }
}

};


#endif

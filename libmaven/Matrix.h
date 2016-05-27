#ifndef MATRIX_H
#define MATRIX_H
/*
 Copyright (c) 2009, Dan Hagerstrand and contributors
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of Cirrus Creative nor the names of its
 contributors may be used to endorse or promote products derived from this
 software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "MersenneTwister.h"

/// The Matrix class allows you to create a two-dimensional array with any size dimensions without having to specify them at compile-time.
/// It makes use of templates so that you can use any type you desire, whether it is an int, short int, or perhaps an array of complex objects.
/// One of the advantages of using this over other dynamic array methods is that you can use a BengCArray in the same way that you would with the
/// traditional built-in C array. To access data, the class provides both a safe method (getElement() and setElement()) as well as an unsafe
/// method:
///
///
/// BengArray<int> *myArray = new Matrix<int>(32, 32);
///
/// myArrayData = myArray->getRawData();\n
/// myArrayData[2][3] = FOO_BAR;
///
/// << myArray;

template<typename T>
class mzMatrix {

protected:

	/// The number of rows the array takes up
	unsigned int nrows;

	/// The number of columns the array takes up
	unsigned int ncols;

	/// Contains the actual data used by the array, in contiguous space
	T *tileMap;
	/// Pointers to each row. This is returned on a call to getRawData().
	T **tileRows;

public:

	mzMatrix() {
		this->nrows = 0;
		this->ncols = 0;
		this->tileMap = NULL;
		this->tileRows = NULL;
	}

	/// Creates a two-dimensional array and allocates the space for it so that it can be used like a C array.
	/// Like a traditional array, all the contents will be initialized to zero.
	mzMatrix(unsigned int xsize, unsigned int ysize) //rows,columns
			{
		resize(xsize, ysize);
	}

	mzMatrix(unsigned int xsize, unsigned int ysize, T** values) //rows,columns
			{
		resize(xsize, ysize);

		for (unsigned int i = 0; i < nrows; i++) {
			for (unsigned int j = 0; j < ncols; j++) {
				this->tileRows[i][j] = values[i][j];
			}
		}
	}

	void resize(unsigned int xsize, unsigned int ysize) {
		this->nrows = xsize;
		this->ncols = ysize;
		this->tileMap = NULL;
		this->tileRows = NULL;

		this->tileMap = (T*) calloc(ysize * xsize, sizeof(T));
		if (this->tileMap == NULL)
			abort();

		this->tileRows = (T**) malloc(xsize * sizeof(T*));
		if (this->tileRows == NULL)
			abort();

		for (unsigned int i = 0; i < xsize; i++) {
			this->tileRows[i] = this->tileMap + (i * ysize);
		}

	}

	void clear() //rows,columns
	{
		//destroy Matrix
		if (this->tileRows)
			free(this->tileRows);
		if (this->tileMap)
			free(this->tileMap);
		tileMap = NULL;
		tileRows = NULL;
		this->nrows = 0;
		this->ncols = 0;
	}

	void copyMatrix(mzMatrix& o) //rows,columns
			{
		clear();
		this->nrows = o.nRows();
		this->ncols = o.nCols();
		resize(nrows, ncols);

		for (unsigned int i = 0; i < nrows; i++) {
			for (unsigned int j = 0; j < ncols; j++) {
				this->tileRows[i][j] = o.tileRows[i][j];
			}
		}
	}

	//max element
	T maxValue() {
		if (nrows == 0 || ncols == 0)
			return 0;
		T maxValue = tileRows[0][0];
		for (unsigned int i = 0; i < nrows; i++) {
			for (unsigned int j = 0; j < ncols; j++) {
				if (tileRows[i][j] > maxValue)
					maxValue = tileRows[i][j];
			}
		}
		return maxValue;
	}

	//min element
	T minValue() {
		if (nrows == 0 || ncols == 0)
			return 0;
		T minValue = tileRows[0][0];
		for (unsigned int i = 0; i < nrows; i++) {
			for (unsigned int j = 0; j < ncols; j++) {
				if (tileRows[i][j] < minValue)
					minValue = tileRows[i][j];
			}
		}
		return minValue;
	}

	/// Destroys the 2D array, freeing the allocated space. It does not, however, free up any objects referenced by pointers
	/// in the array if the array is typed as a pointer. You will need to do this manually.
	~mzMatrix(void) {
		if (this->tileRows)
			free(this->tileRows);
		if (this->tileMap)
			free(this->tileMap);
	}

	//get rows colms sizes
	inline unsigned int nRows() {
		return this->nrows;
	}
	inline unsigned int nCols() {
		return this->ncols;
	}

	/// Returns the dimensions of the array in xOut and yOut.
	void getSize(unsigned int &xOut, unsigned int &yOut) //xOut rows , yOut columns
			{
		xOut = this->nrows;
		yOut = this->ncols;
	}

	/// Changes the value of one cell in the array, given coordinates xIn and yIn, and the data to replace the cell with.
	/// This function will perform bounds checking. If the given coordinates are not in the array, the function does nothing.
	void setElement(unsigned int xIn, unsigned int yIn, T data) {
		if (xIn < this->nrows && yIn < this->ncols)
			this->tileRows[xIn][yIn] = data;
	}

	/// Retrieves the value of one cell in the array, given coordinates xIn and yIn.
	/// This function will perform bounds checking. If the given coordinates are not in the array, the function does nothing.
	T getElement(unsigned int xIn, unsigned int yIn) {
		if (xIn < this->nrows && yIn < this->ncols)
			return this->tileRows[xIn][yIn];
		else
			return 0;
	}

	/// Returns a T** pointer to the raw data of the array so that you can use it unchecked just like a C array:
	///
	/// myArrayData = myArray->getRawData();\n
	/// myArrayData[2][3] = FOO_BAR;
	T **getRawData(void) {
		return this->tileRows;
	}
	inline T& operator()(int col, int row) {
		assert(col >= 0 && col < ncols);
		assert(row >= 0 && row < nrows);
		return tileRows[row][col];
	}

	inline const T& operator()(int col, int row) const {
		assert(col >= 0 && col < ncols);
		assert(row >= 0 && row < nrows);
		return tileRows[row][col];
	}

	float rowcorrelation(unsigned int row1, unsigned int row2) {
		if (row1 >= nrows || row2 >= nrows)
			return 0;
		if (ncols == 0)
			return 0;

		int n = ncols;
		float sumx = 0;
		float sumy = 0;
		float sumxy = 0;
		float x2 = 0;
		float y2 = 0;
		float *x = tileRows[row1];
		float *y = tileRows[row2];

		for (int i = 0; i < n; i++) {
			sumx += x[i];
			sumy += y[i];
			sumxy += x[i] * y[i];
			x2 += x[i] * x[i];
			y2 += y[i] * y[i];
		}
		float var1 = x2 - (sumx * sumx) / n;
		float var2 = y2 - (sumy * sumy) / n;
		if (var1 == 0 || var2 == 0)
			return 0;
		return (sumxy - (sumx * sumy) / n)
				/ sqrt((x2 - (sumx * sumx) / n) * (y2 - (sumy * sumy) / n));
	}

	void shuffle() {
		MTRand mtrand;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				int ii = mtrand.randInt(nrows - 1);
				int jj = mtrand.randInt(ncols - 1);
				T tmp = this->tileRows[ii][jj];
				tileRows[ii][jj] = tileRows[i][j];
				tileRows[i][j] = tmp;
			}
		}
	}

	/// Print matrix
	void print(void) {
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				std::cerr << this->tileRows[i][j] << "\t";
			}
			std::cerr << std::endl;
		}
	}

};

#endif

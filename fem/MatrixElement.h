/**----------------------------------------------------------------------------
 Copyright (c) 2015-, UT-Battelle, LLC
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of fern nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author(s): Jay Jay Billings (jayjaybillings <at> gmail <dot> com)
 -----------------------------------------------------------------------------*/
#ifndef SOLVERS_MATRIXELEMENT_H_
#define SOLVERS_MATRIXELEMENT_H_

#include <utility>

namespace fire {

/**
 * This is a simple class that represents an individual element in a matrix. It
 * is templated so that it can be used in an appropriately typed scenario.
 *
 * This class is not meant to be used for dense matrices or in situations where
 * array serialization will suffice. However, it may be particularly useful to
 * classes that need to work with sparse matrices or do reductions across
 * matrix elements without sticky pointer arithmetic.
 *
 * The i index is the major index and the j index is the minor index. So in a
 * row-major storage system i is the row index and j is the column index. In a
 * column-major storage system i is the column index and j is the row index.
 *
 * Clients must be especially careful to make sure that they use this class
 * correctly since calling the initializer constructors will attempt to move
 * the variable T into the matrix element instead of copying it.
 */
template<typename T>
struct MatrixElement {

	/**
	 * The major index of the matrix.
	 */
	int i;

	/**
	 * The minor index of the matrix.
	 */
	int j;

	/**
	 * The value stored in the matrix at element (i,j).
	 */
	T value;

	/**
	 * The default constructor. i = j = 0 and the value is default constructed.
	 */
	MatrixElement() : i(0), j(0), value() {}

	/**
	 * A constructor. i and j are initialized to 0 and value is
	 * _moved_ from the original storage location into the matrix element.
	 */
	MatrixElement(T val) : i(0), j(0), value(std::move(val)) {};

	/**
	 * An initalizer constructor that will set the values of i,j, and the element.
	 * The value of T is _moved_ into the MatrixElement.
	 * @param the value of the major order index
	 * @param the value of the minor order index
	 * @param the value of the matrix element
	 */
	MatrixElement(const int & i_in, const int & j_in, T val) : i(i_in), j(j_in), value(std::move(val)) {}

};

/**
 * This operation computes the row-major index of a matrix element based on its
 * indices. This is useful for computing which index in a 1D array would
 * represent this matrix element if the array was ordered in a row-major
 * fashion. It assumes i is the row index.
 * @param the i index of the matrix element
 * @param the j index of the matrix element
 * @param the number of elements in a row of the matrix
 * @return the index in the row-major ordered matrix
 */
inline int rowMajorIndex(int i, int j, int rowLength) {
	return i*rowLength + j;
}

/**
 * This operation computes the column-major index of a matrix element based on
 * its indices. This is useful for computing which index in a 1D array would
 * represent this matrix element if the array was ordered in a column-major
 * fashion. It assumes the starting index is 0.
 * @param the i index of the matrix element
 * @param the j index of the matrix element
 * @param the number of elements in a column of the matrix
 * @return the index in the column-major ordered matrix
 */
inline int colMajorIndex(int i, int j, int colLength) {
    return i + colLength*j;
}

/**
 * This operation computes the row-major index of a matrix element based on its
 * indices. This is useful for computing which index in a 1D array would
 * represent this matrix element if the array was ordered in a row-major
 * fashion. It assumes the starting index is 0.
 * @param the matrix element
 * @param the number of elements in a row of the matrix
 * @return the index in the row-major ordered matrix
 */
template<typename T>
inline int rowMajorIndex(MatrixElement<T> e, int rowLength) {
	return rowMajorIndex(e.i,e.j,rowLength);
}

/**
 * This operation computes the column-major index of a matrix element based on
 * its indices. This is useful for computing which index in a 1D array would
 * represent this matrix element if the array was ordered in a column-major
 * fashion. It assumes the starting index is 0.
 * @param the matrix element
 * @param the number of elements in a column of the matrix
 * @return the index in the column-major ordered matrix
 */
template<typename T>
inline int colMajorIndex(MatrixElement<T> e, int colLength) {
    return colMajorIndex(e.i,e.j,colLength);
}

} /* end namespace fire */

#endif /* SOLVERS_MATRIXELEMENT_H_ */

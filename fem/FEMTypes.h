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
 * This is a basic pair of two types. It is modeled after std::pair in that it
 * uses "first" and "second" elements, but it does not provide as many utility
 * functions.
 */
template<typename T, typename K>
struct BasicPair {

	/**
	 * The first member of the pair, often considered the key or x-coordinate
	 * value.
	 */
	T first;

	/**
	 * The second member of the pair, often considered the value or
	 * y-coordinate value.
	 */
	K second;

	/**
	 * Basic constructor
	 */
	BasicPair() : first(), second() {};

	/**
	 * Initialization by value-copy constructor
	 * @param the value of the first element
	 * @param the value of the second element
	 */
	BasicPair(const T & _first, const K & _second) : first(_first), second(_second) {};

};

/**
 * This class provides a generic template for working with key-value pairs that
 * are identifiable by some means or associated with a third value. It can be
 * used, for example, for numbered coordinate pairs,
 * @code
 * int id = 1;
 * double x = 1.0, y = 2.0;
 * IdentifiablePair<double,double,int> myPair(x,y,id);
 * @endcode
 * or for labeled data in machine learning,
 * @code
 * string label = "ufo-sighting-alpha2";
 * int mon = 10, year = 2017;
 * IdentifiablePair<int,double,double> myPair(mon,year,label);
 * @endcode
 * It can also be used to store function values at grid points
 * @code
 * int i = 1, j = 1;
 * double value = 5.0;
 * IdentifiablePair<int,int,double> myPair(i,j,value);
 * @endcode
 */
template<typename T, typename K, typename V>
struct IdentifiablePair : public BasicPair<T,K> {

	/**
	 * The value associated with the pair
	 */
	V value;

	/**
	 * The constructor
	 */
	IdentifiablePair() : BasicPair<T,K>::BasicPair(), value() {};

	/**
	 * Initialization by value-copy constructor
	 * @param the value of the first element
	 * @param the value of the second element
	 * @param the value of the "value" attribute
	 */
	IdentifiablePair(const T & _first, const K & _second, const V & _value) :
		BasicPair<T,K>::BasicPair(_first,_second), value(_value) {};

};

/**
 * This class extends IdentifiablePair to add a third dimension.
 */
template <typename T, typename K, typename V, typename Z>
struct IdentifiableTriplet : public IdentifiablePair<T,K,Z> {

	/**
	 * The third member of the pair, often considered the secondary value or
	 * z-coordinate value.
	 */
	V third;

    /**
     * The constructor
     */
    IdentifiableTriplet() : IdentifiablePair<T,K,Z>::IdentifiablePair(), third() {};

    /**
     * Initialization by value-copy constructor
	 * @param the value of the first element
	 * @param the value of the second element
	 * @param the value of the third element
	 * @param the value of the "value" attribute
     */
    IdentifiableTriplet(const T & _first, const K & _second, const V & _third,
    		const Z _value) : IdentifiablePair<T,K,Z>::IdentifiablePair(_first,_second, _value),
    				third(_third) {};

};


/**
 * The following structure is a simple 2D node with "first" and "second"
 * coordinates that can represent (x,y), (r,theta), or any other two-value, 2D
 * coordinate set. The value member can represent a node id or function value
 * at the x,y point.
 *
 * It can be initialized with default values of (0,0.0,0.0) or set to a
 * specific set of coordinates as follows.
 * @code
 * TwoDNode node1, node2(1,5.0,6.0);
 * @endcode
 */
using TwoDNode = IdentifiablePair<double,double,int>;

/**
 * This structure is a collection of the coordinates (L_1, L_2, L_3) in a
 * Constant Strain Triangle Element. It is used internally by the class
 * of the same name, but may be useful elsewhere.
 */
using CSTLocalPoint = IdentifiableTriplet<double,double,double,int>;

/**
 * This is a simple structure that represents an individual element in a
 * vector. It is templated so that it can be used in an appropriately typed
 * scenario. Its actual implementation is provided by std::pair through an
 * alias declaration. The first value of the pair stores the index in the
 * vector and the second value stores the associated value. It works as such
 * @code
 * VectorElement<double> vecElem;
 * vecElem.first = 1;
 * vecElem.second = 867.5309;
 * @endcode
 */
template<typename T>
using VectorElement = BasicPair<int,T>;

/**
 * This is a simple structure that represents an individual element in a
 * matrix. It is templated so that it can be used in an appropriately typed
 * scenario. Its actual implementation is provided by std::tuple through an
 * alias declaration. The indicies i and j are stored in the first and second
 * positions respectively. The "value" of the matrix element is stored in the
 * third position of the tuple. Here is an example that shows getting the
 * value of a matrix element positioned at (1,0):
 * @code
 * MatrixElement<double> element{1, 0, 0.5};
 * int i = std::get<0>(element);
 * int j = std::get<1>(element);
 * double value = std::get<2>(element);
 * @endcode
 *
 * This structure is not meant to be used for dense matrices or in situations
 * where array serialization will suffice. However, it may be particularly
 * useful to classes that need to work with sparse matrices or do reductions
 * across matrix elements without sticky pointer arithmetic.
 *
 * The first int is the major index and the second int is the minor index. So
 * in a row-major storage system the first int is the row index and the second
 *  is the column index. In a column-major storage system i is the column
 *  index and j is the row index.
 */
template<typename T>
using MatrixElement = IdentifiablePair<int,int,T>;

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
	return rowMajorIndex(e.first,e.second,rowLength);
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
    return colMajorIndex(e.first,e.second,colLength);
}

} /* end namespace fire */

#endif /* SOLVERS_MATRIXELEMENT_H_ */

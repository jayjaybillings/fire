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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FEM

#include <boost/test/included/unit_test.hpp>
#include <FEMTypes.h>

using namespace std;
using namespace fire;

/**
 * This operation checks the BasicPair structure, and its equality operator in
 * particular.
 */
BOOST_AUTO_TEST_CASE(checkBasicPair) {
	BasicPair<int,int> pair1, pair2(5,5);

	// Check the first and second pairs
	BOOST_REQUIRE_EQUAL(0,pair1.first);
	BOOST_REQUIRE_EQUAL(0,pair1.second);
	BOOST_REQUIRE_EQUAL(5,pair2.first);
	BOOST_REQUIRE_EQUAL(5,pair2.second);
	// Manually assign values to the first pair.
	pair1.first = 5;
	pair1.second = 6;
	// They should not be equal!
	BOOST_REQUIRE(pair1 != pair2);
	// Make them equal
	pair1.second = 5;
	BOOST_REQUIRE(pair1 == pair2);

	return;
}

/**
 * This operation checks the 2D Node struct.
 */
BOOST_AUTO_TEST_CASE(checkTwoDNode) {

	TwoDNode node1, node2(2.0,3.0,1);

	BOOST_REQUIRE_CLOSE(0.0,node1.first,0.0);
	BOOST_REQUIRE_CLOSE(0.0,node1.second,0.0);
	BOOST_REQUIRE_EQUAL(0,node1.value);
	BOOST_REQUIRE_CLOSE(2.0,node2.first,1.0e-15);
	BOOST_REQUIRE_CLOSE(3.0,node2.second,1.0e-15);
	BOOST_REQUIRE_EQUAL(1,node2.value);

	// Check setting
	node1.first = 1.5;
	node1.second = 5.0;
	BOOST_REQUIRE_CLOSE(1.5,node1.first,1.0e-15);
	BOOST_REQUIRE_CLOSE(5.0,node1.second,1.0e-15);

	// Check equality to make sure that IdentifiablePair::operator== works.
	BOOST_REQUIRE(node1 != node2);
	// Set them equal
	node1.first = node2.first;
	node1.second = node2.second;
	node1.value = node2.value;
	BOOST_REQUIRE(node1 == node2);

	return;
}

/**
 * This operation checks the CSTLocalPoint structure.
 */
BOOST_AUTO_TEST_CASE(checkCSTLocalPoint) {
	CSTLocalPoint p1, p2(1.2,2.3,3.4,9);

	// Check default construction
	BOOST_REQUIRE_CLOSE(0.0,p1.first,1.0e-15);
	BOOST_REQUIRE_CLOSE(0.0,p1.second,1.0e-15);
	BOOST_REQUIRE_CLOSE(0.0,p1.third,1.0e-15);
	BOOST_REQUIRE_EQUAL(0,p1.value);

	// Check value-copy construction
	BOOST_REQUIRE_CLOSE(1.2,p2.first,1.0e-15);
	BOOST_REQUIRE_CLOSE(2.3,p2.second,1.0e-15);
	BOOST_REQUIRE_CLOSE(3.4,p2.third,1.0e-15);
	BOOST_REQUIRE_EQUAL(9,p2.value);

	// Check setting
	p1.first = 1.8;
	p1.second = 2.9;
	p1.third = 4.0;
	p1.value = 5;
	BOOST_REQUIRE_CLOSE(1.8,p1.first,1.0e-15);
	BOOST_REQUIRE_CLOSE(2.9,p1.second,1.0e-15);
	BOOST_REQUIRE_CLOSE(4.0,p1.third,1.0e-15);
	BOOST_REQUIRE_EQUAL(5,p1.value);

	// Check equality, which also insures IdentifiableTriplet::operator==
	// works.
	BOOST_REQUIRE(p1 != p2); // Not equal yet
	// Make them equal
	p1.first = p2.first;
	p1.second = p2.second;
	p1.third = p2.third;
	p1.value = p2.value;
	BOOST_REQUIRE(p1 == p2);

	return;
}

/**
 * This operation checks the Vector Element structure.
 */
BOOST_AUTO_TEST_CASE(checkVectorElement) {
	VectorElement<int> e1, e2(2,8);

	BOOST_REQUIRE_EQUAL(e1.first,0);
	BOOST_REQUIRE_EQUAL(e1.second,0);
	BOOST_REQUIRE_EQUAL(e2.first,2);
	BOOST_REQUIRE_EQUAL(e2.second,8);

	// Check setting
	e1.first = 2;
	e1.second = 4;
	BOOST_REQUIRE_EQUAL(2,e1.first);
	BOOST_REQUIRE_EQUAL(4,e1.second);
}

/**
 * This operation checks the Matrix Element.
 */
BOOST_AUTO_TEST_CASE(checkMatrixElement) {

	MatrixElement<int> e1, e2{0,0,5}, e3{1,8,7};

	BOOST_REQUIRE_EQUAL(e1.first,0);
	BOOST_REQUIRE_EQUAL(e1.second,0);
	BOOST_REQUIRE_EQUAL(e1.value,0);
	BOOST_REQUIRE_EQUAL(e2.first,0);
	BOOST_REQUIRE_EQUAL(e2.second,0);
	BOOST_REQUIRE_EQUAL(e2.value,5);
	BOOST_REQUIRE_EQUAL(e3.first,1);
	BOOST_REQUIRE_EQUAL(e3.second,8);
	BOOST_REQUIRE_EQUAL(e3.value,7);

	// Check setting
	e1.value = 6;
	BOOST_REQUIRE_EQUAL(6,e1.value);

	return;
}

/**
 * This operation checks the row and column index computation functions.
 */
BOOST_AUTO_TEST_CASE(checkMatrixElementIndicesComputation) {
	int i = 1, j = 2, lRow = 4, lCol = 5, rowIndex = i*lRow + j, colIndex = i + lCol*j;
    MatrixElement<int> e(1,2,3);

    BOOST_REQUIRE_EQUAL(rowIndex,rowMajorIndex(i, j, lRow));
    BOOST_REQUIRE_EQUAL(colIndex,colMajorIndex(i, j, lCol));
    BOOST_REQUIRE_EQUAL(rowIndex,rowMajorIndex(e, lRow));
    BOOST_REQUIRE_EQUAL(colIndex,colMajorIndex(e, lCol));
}

BOOST_AUTO_TEST_CASE(checkTwoDRobinBoundaryCondition) {
	std::function<double(const double &)> f = [](const double & foo) {
		return -1.0;
	};
	std::function<double(const double &)> g = [](const double & foo) {
		return -5.0;
	};
	TwoDNode node1, node2(2.0,3.0,1);
	TwoDRobinBoundaryCondition cond1(node1,node2,f,f), cond2(node1,node2,g,g),
			cond3(node1,node2,f,f);

	// Make sure they are not equal
	BOOST_REQUIRE(cond1 != cond2);
	// Make sure they are equal
	BOOST_REQUIRE(cond1 == cond3);

	return;
}

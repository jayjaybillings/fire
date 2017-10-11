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
#include <MatrixElement.h>
#include <vector>

using namespace std;
using namespace fire;

/**
 * This operation checks the Matrix Element.
 */
BOOST_AUTO_TEST_CASE(checkMatrixElement) {

	MatrixElement<int> e1, e2(5), e3(1,8,7);

	BOOST_REQUIRE_EQUAL(e1.i,0);
	BOOST_REQUIRE_EQUAL(e1.j,0);
	BOOST_REQUIRE_EQUAL(e2.i,0);
	BOOST_REQUIRE_EQUAL(e2.j,0);
	BOOST_REQUIRE_EQUAL(e2.value,5);
	BOOST_REQUIRE_EQUAL(e3.i,1);
	BOOST_REQUIRE_EQUAL(e3.j,8);
	BOOST_REQUIRE_EQUAL(e3.value,7);

	return;
}

/**
 * This operation checks the row and column index computation functions.
 */
BOOST_AUTO_TEST_CASE(checkIndicesComputation) {
	int i = 1, j = 2, lRow = 4, lCol = 5, rowIndex = i*lRow + j, colIndex = i + lCol*j;
    MatrixElement<int> e(1,2,3);

    BOOST_REQUIRE_EQUAL(rowIndex,rowMajorIndex(i, j, lRow));
    BOOST_REQUIRE_EQUAL(colIndex,colMajorIndex(i, j, lCol));
    BOOST_REQUIRE_EQUAL(rowIndex,rowMajorIndex(e, lRow));
    BOOST_REQUIRE_EQUAL(colIndex,colMajorIndex(e, lCol));
}

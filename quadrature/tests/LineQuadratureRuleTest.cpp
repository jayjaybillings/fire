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
#define BOOST_TEST_MODULE QUADRATURE

#include <boost/test/included/unit_test.hpp>
#include <LineQuadratureRule.h>
#include <functional>
#include <limits>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace fire;


/**
 * This operation checks the Line quadrature routine. It computes the
 * area of a square on x = [-1,1] with y = 1. In both cases, if the index is
 * non-zero it returns the value of the index, which just scales the area
 * estimate by the same amount. The area should be 2.0.
 *
 * This test checks the accuracy to a relative error less than twice machine
 * precision because this routine should be exact for polynomials up to degree
 * 7. Most of the results have a relative error exactly equal to zero, although
 * for some values of the indices this will produce an error that is close to
 * epsilon.
 */
BOOST_AUTO_TEST_CASE(checkOneIndexQuadrature) {

	LineQuadratureRule rule;
	int index = 0;
	double area = 2.0, relErr = 0.0, result = 0.0,
			epsilon = 2.0*numeric_limits<double>::epsilon();

	// Create a test function
	std::function<double(const double &,
				const int &)> function = [](const double & point,
			const int & i) {

		// If the index is set to a non-zero value, return it. This makes it
		// possible to test forwarding indices.
		if (i != 0) {
			return (double) i;
		}

		return 1.0;
	};

	// Just integrate over the unit square and make sure the area is
	// correct. Note that index is unused for this function, so the area
	// should just be 1.0;
	result = rule.integrate(function, index);
	relErr = abs((result - area))/area;
	BOOST_TEST_MESSAGE("Result = " << result << ", relErr = " << relErr);
	BOOST_REQUIRE(relErr < epsilon);

	// Compute the integral, which should be equal to index*area, for
	// 1<=index<=100.
	for (index = 1; index < 101; index++) {
		result = rule.integrate(function, index);
		relErr = abs((result - ((double) index)*area))/(((double) index)*area);
		BOOST_TEST_MESSAGE("Result = " << result << ", relErr = " << relErr);
		BOOST_REQUIRE(relErr < epsilon);
	}

	// Now try it with a parabola over the same bounds. Area = 2/3.
	std::function<double(const double &,
				const int &)> parabola = [](const double & point,
			const int & i) {
		return point*point;
	};
	result = rule.integrate(parabola, 0);
	area = 2.0/3.0;
	relErr = abs((result - area))/area;
	BOOST_TEST_MESSAGE("Result = " << result << ", relErr = " << relErr);
	BOOST_REQUIRE(relErr < epsilon);

	return;
}

/**
 * Same as above, but checks for forwarding two indices.
 */
BOOST_AUTO_TEST_CASE(checkTwoIndexQuadrature) {

	LineQuadratureRule rule;
	int i = 0, j = 0;
	double area = 2.0, relErr = 0.0, result = 0.0,
			epsilon = 2.0*numeric_limits<double>::epsilon();

	// Create a test function
	std::function<double(const double &,
				const int &, const int &)> function = [](const double & point,
			const int & i, const int & j) {

		// If the indices are set to non-zero values, return their product
		// instead. This makes it possible to test forwarding indices.
		if (i != 0 && j != 0) {
			return (double) (i*j);
		}

		return 1.0;
	};

	// Check the unit square.
	result = rule.integrate(function, i, j);
	relErr = abs((result - area))/area;
	BOOST_TEST_MESSAGE("Result = " << result << ", i = " << i << ", j = "
			<< j << ", relErr = " << relErr);
	BOOST_REQUIRE(relErr < epsilon);

	// Compute the integral, which should be equal to index*area, for
	// 1<={i,j}x<=10. In this case, directly compute the relative error
	// and compare it to 1.0e-15.
	for (i = 1; i < 11; i++) {
		for (j = 1; j < 11; j++) {
		    result = rule.integrate(function, i, j);
		    relErr = abs((result - ((double) (i*j))*area))/(((double) (i*j))*area);
		BOOST_TEST_MESSAGE("Result = " << result << ", i = " << i << ", j = "
				<< j << ", relErr = " << relErr);
		BOOST_REQUIRE(relErr < epsilon);
		}
	}

	// Now try it with a parabola over the same bounds. Area = 2/3.
	std::function<double(const double &,
				const int &, const int &)> parabola = [](const double & point,
			const int & i, const int & j) {
		return point*point;
	};
	result = rule.integrate(parabola, 0, 0);
	area = 2.0/3.0;
	relErr = abs((result - area))/area;
	printf("%lf\n",result);
	BOOST_TEST_MESSAGE("Result = " << result << ", relErr = " << relErr);
	BOOST_REQUIRE(relErr < epsilon);

	return;
}

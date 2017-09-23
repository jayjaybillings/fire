/**----------------------------------------------------------------------------
 Copyright (c) 2017-, UT-Battelle, LLC
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
#define BOOST_TEST_MODULE Solvers

#include <boost/test/included/unit_test.hpp>
#include <State.h>
#include <memory>
#include <algorithm>
#include <IVPSolver.h>
#include <iostream>

using namespace std;
using namespace fire;

/**
 * A simple test struct for the tests.
 */
struct TestStruct {
	vector<double> y;
	vector<double> dydt;
	TestStruct(const int & size) :
			y(size), dydt(size) {
	};
};

/**
 * Explicit member function instantiations for the test structure to return the
 * unknowns and derivatives.
 */
namespace fire {

// Getter for test struct data from a State<TestStruct>
template<>
double * State<TestStruct>::u() const {
	return state->y.data();
};

// Getter for test struct derivative data from a State<TestStruct>
template<>
double * State<TestStruct>::dudt(const double & t) const {

	// k constant for this problem
	const double k = 0.85;

	// Multiply the y vector by the constant
	transform(state->y.begin(), state->y.end(), state->dydt.begin(),
			bind1st(multiplies<double>(), k));

	return state->dydt.data();
};

} // end namespace fire


BOOST_AUTO_TEST_CASE(checkAccessors) {

	IVPSolver<TestStruct> solver;

	double testTime = 5.9989;

	// Make sure everything is zero to start
	BOOST_REQUIRE_CLOSE(0.0,solver.t(),1.0e-8);
	BOOST_REQUIRE_CLOSE(0.0,solver.tInit(),1.0e-8);
	BOOST_REQUIRE_CLOSE(0.0,solver.tFinal(),1.0e-8);
	solver.t(testTime);
	BOOST_REQUIRE_CLOSE(testTime,solver.t(),1.0e-8);
	solver.tInit(testTime);
	BOOST_REQUIRE_CLOSE(testTime,solver.tInit(),1.0e-8);
	solver.tFinal(testTime);
	BOOST_REQUIRE_CLOSE(testTime,solver.tFinal(),1.0e-8);

	return;
}

/**
 * This operation checks the IVPSolver to make sure that it can solve a simple
 * ODE: y' = 0.85y. This is an example on Wikipedia with the solution
 * y = 19e^(0.85t).
 */
BOOST_AUTO_TEST_CASE(checkSingleVariableSolve) {
	int size = 1;
	// Create the state and forward the size of TestStruct. This is a good
	// example because not only does it show forwarding, but it also
	// illustrates that the proper types must be used during the forward. If
	// proper types are not used (i.e. - int instead of const int &) then the
	// compiler will fail.
	State<TestStruct> state = buildState<TestStruct,const int &>(size, size);

	// Set the initial t value on the state
	double tInit = 0.0, t = 0.0, tFinal = 1.0;
	state.t(t);
	// Configure the solver
	IVPSolver<TestStruct> solver;
	solver.t(t);
	solver.tInit(tInit);
	solver.tFinal(tFinal);

	solver.solve(state);

	std::cout << "value = " << state.get().y[0] << std::endl;

	return;
}

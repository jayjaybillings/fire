
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

using namespace std;
using namespace fire;

/**
 * A simple test struct for the tests.
 */
struct TestStruct {
	vector<double> A;
	vector<double> dAdt;
	TestStruct() : A{5.0,2.0} , dAdt{2.0,5.0} {};
};

/**
 * Explicit member function instantiations for the test structure to return the
 * unknowns and derivatives.
 */
namespace fire {

// Getter for test struct data from a State<TestStruct>
template<>
double * State<TestStruct>::u(const double & time) const {return userState->A.data();};

// Setter for test struct data from a State<TestStruct>
template<>
void State<TestStruct>::u(double * data, const double & time) {};

// Getter for test struct derivative data from a State<TestStruct>
template<>
double * State<TestStruct>::dudt(const double & time) const {return userState->dAdt.data();};

} // end namespace fire



/**
 * This operation checks all of the various accessors on State<T>.
 */
BOOST_AUTO_TEST_CASE(checkAccessors) {

	// Create the state
	shared_ptr<TestStruct> testStruct = make_shared<TestStruct>();
	State<TestStruct> state;

	// Test the size setters
	state.size(2);
	BOOST_REQUIRE_EQUAL(2,state.size());

	return;
}

/**
 * This operation checks build<TestStruct>().
 */
BOOST_AUTO_TEST_CASE(checkStateAccessors) {

	// Create the state
	State<TestStruct> state;
	state.size(2);

	// Set the state
	auto testStruct = state.add(0.0);
	// Get it at the most recent time
	auto retStruct = state.get();

	// Check the state
	BOOST_REQUIRE_CLOSE(testStruct.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[1],retStruct.dAdt[1],1.0e-8);

	// Set the state at t = 1
	auto ts1 = state.add(1.0);
	ts1.A[0] = 15.0;
	ts1.A[1] = 12.0;
	ts1.dAdt[0] = 3.0;
	ts1.dAdt[1] = 21.0;

	// Check the newly added state
	retStruct = state.get(1.0);
	BOOST_REQUIRE_CLOSE(ts1.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.dAdt[1],retStruct.dAdt[1],1.0e-8);

	// Make sure the old state is still available too
	retStruct = state.get(0.0);
	BOOST_REQUIRE_CLOSE(testStruct.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[1],retStruct.dAdt[1],1.0e-8);

	// Test the final setter with a time step size
	// Set the state at t = 1
	auto ts2 = state.add(2.5, 1.5);
	ts1.A[0] = 51.0;
	ts1.A[1] = 21.0;
	ts1.dAdt[0] = 30.0;
	ts1.dAdt[1] = 210.0;

	// Check the newly added state
	retStruct = state.get(2.5);
	BOOST_REQUIRE_CLOSE(ts2.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts2.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts2.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts2.dAdt[1],retStruct.dAdt[1],1.0e-8);

	// Make sure the old states are still available too
	retStruct = state.get(1.0);
	BOOST_REQUIRE_CLOSE(ts1.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(ts1.dAdt[1],retStruct.dAdt[1],1.0e-8);
	retStruct = state.get(0.0);
	BOOST_REQUIRE_CLOSE(testStruct.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[1],retStruct.dAdt[1],1.0e-8);

	return;
}

BOOST_AUTO_TEST_CASE(checkStateUAndDUAccessors) {
	BOOST_FAIL("Test not yet implemented.");
	return;
}


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
	const int testK;
	TestStruct() : A{5.0,2.0} , dAdt{2.0,5.0} , testK(0) {};
	TestStruct(const int & k) : testK{k} {};
};

/**
 * Explicit member function instantiations for the test structure to return the
 * unknowns and derivatives.
 */
namespace fire {

// Getter for test struct data from a State<TestStruct>
template<>
double * State<TestStruct>::u() const {return state->A.data();};

// Getter for test struct derivative data from a State<TestStruct>
template<>
double * State<TestStruct>::dudt(const double & t) const {return state->dAdt.data();};

} // end namespace fire



/**
 * This operation checks all of the various simple accessors on State<T>.
 */
BOOST_AUTO_TEST_CASE(checkAccessors) {

	// Create the state
	State<TestStruct> state;

	// Test the size setters
	BOOST_REQUIRE_EQUAL(0,state.size());
	state.size(2);
	BOOST_REQUIRE_EQUAL(2,state.size());

	// Test the t setters
	BOOST_REQUIRE_CLOSE(0.0,state.t(),1.0e-8);
	state.t(1.5998);
	BOOST_REQUIRE_CLOSE(1.5998,state.t(),1.0e-8);

	// Check defaulting the size.
	State<TestStruct> secondState(99);
	BOOST_REQUIRE_EQUAL(99,secondState.size());

	return;
}

/**
 * This operation checks the accessors for state data on State<T>.
 */
BOOST_AUTO_TEST_CASE(checkStateAccessors) {

	// Create the state
	State<TestStruct> state;
	int size = 2;
	state.size(size);
	TestStruct testStruct;

	// Get the initial state
	auto & retStruct = state.get();

	// Check the state
	BOOST_REQUIRE_CLOSE(testStruct.A[0],retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.A[1],retStruct.A[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[0],retStruct.dAdt[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(testStruct.dAdt[1],retStruct.dAdt[1],1.0e-8);

	// Update the state with data from a second vector.
	double * myU = new double[size];
	myU[0] = 8.0;
	myU[1] = 9.99;
	state.u(myU);

	// Check that the state updated.
	BOOST_REQUIRE_CLOSE(8.0,retStruct.A[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(9.99,retStruct.A[1],1.0e-8);

	// Note that u() the getter and dudt are not tested because they are
	// explicitly instantiated above.

	return;
}

/**
 * This test checks the buildState<>() template and the ability to forward
 * arguments to the constructor of the contained class.
 */
BOOST_AUTO_TEST_CASE(checkBuildState) {
	// Create the state
	int size = 2;
	State<TestStruct> state = buildState<TestStruct,const int &>(8, size);
	BOOST_REQUIRE_EQUAL(size,state.size());
	BOOST_REQUIRE_EQUAL(state.get().testK,8);
}

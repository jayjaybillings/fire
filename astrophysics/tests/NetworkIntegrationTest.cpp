
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
#define BOOST_TEST_MODULE Astro

#if defined __GNUC__ && __GNUC__>=6
  #pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <INIPropertyParser.h>
#include <ReactionNetwork.h>
#include <IVPSolver.h>
#include <StringCaster.h>
#include <State.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace fire;
using namespace fire::astrophysics;

static const std::string & propertyFileName = "alpha_gold.ini";

/**
 * This operation checks the ability of the Network to load itself properly
 * from an input parameter file.
 */
BOOST_AUTO_TEST_CASE(checkLoading) {

	// Load the properties
	INIPropertyParser parser = build<INIPropertyParser,const string &>(propertyFileName);

	// Create the network
	State<ReactionNetwork> state;
	ReactionNetwork & network = state.get();

	// Set the properties from the property block and load the network
	auto props = parser.getPropertyBlock("network");
	network.setProperties(props);

	// Load the remaining data
	network.load();

	// Compute the prefactors and rates
	props = parser.getPropertyBlock("initialConditions");
	double temperature = StringCaster<double>::cast(props.at("T9"));
	double density = StringCaster<double>::cast(props.at("density"));
	network.computePrefactors(density);
	network.computeRates(temperature);

	// Set the state size;
	state.size(network.species->size());

	// Set the initial time
	state.t(0.0);

	// Do the solve
	IVPSolver<ReactionNetwork> solver;
    solver.solve(state);

    // FIXME! - Actually add some tests!

	// Good enough for government work
	return;
}

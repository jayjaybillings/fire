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

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <INIPropertyParser.h>
#include <ReactionNetwork.h>

using namespace std;
using namespace fire;
using namespace fire::astrophysics;

static const string & propertyFileName = "alpha_gold.ini";

/**
 * This operation checks the ability of the Network to load itself properly
 * from an input parameter file.
 */
BOOST_AUTO_TEST_CASE(checkLoading) {

	// Load the properties
	INIPropertyParser parser = build<INIPropertyParser,const string &>(propertyFileName);

	// Create the network
	ReactionNetwork network;
	// Set the properties from the property block and load the network
	auto props = parser.getPropertyBlock("network");
	network.setProperties(props);

	// Check the properties
	BOOST_REQUIRE_EQUAL(16,network.numSpecies);
	BOOST_REQUIRE_EQUAL(48,network.numReactions);
	BOOST_REQUIRE_EQUAL(19,network.numReactionGroups);
	BOOST_REQUIRE_CLOSE(1.0e-7,network.massTol,1.0e-8);
	BOOST_REQUIRE_CLOSE(0.01,network.fluxFrac,1.0e3);
	BOOST_REQUIRE_EQUAL("CUDAnet_alpha.inp",network.networkFileName);
	BOOST_REQUIRE_EQUAL("rateLibrary_alpha.data",network.rateFileName);

	// Load the remaining data
	network.load();

	// Checking the species and reactions is done thoroughly elsewhere,
	// so just check total sizes now.
	auto species = network.species;
	BOOST_REQUIRE_EQUAL(16,network.species->size());
	BOOST_REQUIRE_EQUAL(48,network.reactions->size());

	// The proper course of action at this point would be to check all of the
	// flux maps, but that code is really nasty and I don't plan to use it as
	// originally written in FERN. I mostly just copied it over to get the
	// functionality. Everything looks OK in the debugger, so I will enhance
	// this test once I determine how to rewrite the routine.

	// Check the prefactor and rate computations
	network.computePrefactors(1.0);
	network.computeRates(1.0);
	for (Reaction & reaction : *(network.reactions)) {
		// Check the prefactor computations. If rho=1, then
		// p_s = s*rho^(numReactants-1) = s for all reactions.
	    BOOST_REQUIRE_CLOSE(reaction.statisticalFactor,reaction.prefactor,1.0e-8);

		// Make sure the reaction rate can be computed correctly. If T = 1, then
		// the rate is equal to prefactor*e^(sum of reaclibRateCoeffs[0:5]) for all
		// reactions. Note log(1) = 0, so reaclibRateCoeff[6] is omitted.
		double sumCoeffs = reaction.reaclibRateCoeff[0] + reaction.reaclibRateCoeff[1]
				+ reaction.reaclibRateCoeff[2] + reaction.reaclibRateCoeff[3]
	            + reaction.reaclibRateCoeff[4] + reaction.reaclibRateCoeff[5];
		double rate = reaction.prefactor*exp(sumCoeffs);
		BOOST_REQUIRE_CLOSE(rate,reaction.rate,1.0e-8);
	}
	// Reset the network to use the appropriate temperature and density for this test.
	network.computePrefactors(1.0e8);
	network.computeRates(7.0);
	// Make sure the rates are realistic under these conditions. It is sufficient to
	// just check a couple.
	BOOST_REQUIRE_CLOSE(103926.77067175004,network.reactions->at(0).rate,1.0e-8);
	BOOST_REQUIRE_CLOSE(0.0,network.reactions->at(1).rate,1.0e-8);

	// Check the flux values
	vector<double> referenceFluxes =
			{ 3692943778.241045, -7210391517.142347,
			-155893155.846619, 3536467448.167465, 143648559.705471,
			2510685.357474, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			0.000000, 0.000000, 0.000000, 0.000000, 0.000000 };
	network.computeFluxes();
	for (int i = 0; i < network.numSpecies; i++) {
		BOOST_REQUIRE_CLOSE(referenceFluxes[i],network.species->at(i).flux,1.0e-10);
	}

	// Good enough for government work
	return;
}







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
#include <Reaction.h>
#include <ReactionLocalParser.h>
#include <math.h>

using namespace std;
using namespace fire;
using namespace fire::astrophysics;

// Test file names
static std::string networkFileName = "rateLibrary_alpha.data";

/**
 * This operation checks the ability of the ReactionLocalParser to parse the
 * reactions in the network.
 */
BOOST_AUTO_TEST_CASE(checkParsing) {

	// Create the parser
	LocalParser<vector<Reaction>> parser = build
			< LocalParser<vector<Reaction>>,const string &>(networkFileName);
	parser.parse();
	auto reactionListPtr = parser.getData();
	vector<Reaction> reactionList = *reactionListPtr;

	// The test file is an alpha network with fourty eight reactions.
	BOOST_REQUIRE(parser.isFile());
	BOOST_REQUIRE_EQUAL(48,reactionList.size());

	reactionList.size();
	reactionList[0];

	// Check the first reaction
	Reaction reaction = reactionList[0];
	BOOST_REQUIRE_EQUAL("he4+he4+he4-->c12",reaction.name);
	BOOST_REQUIRE_EQUAL(3,reaction.reactionGroupClass);
	BOOST_REQUIRE_EQUAL(0,reaction.reactionGroupMemberIndex);
	BOOST_REQUIRE_EQUAL(8,reaction.reaclibClass);
	BOOST_REQUIRE_EQUAL(3,reaction.numReactants);
	BOOST_REQUIRE_EQUAL(1,reaction.numProducts);
	BOOST_REQUIRE_EQUAL(false,reaction.isElectronCapture);
	BOOST_REQUIRE_EQUAL(false,reaction.isReverse);
	BOOST_REQUIRE_CLOSE(0.16666667, reaction.statisticalFactor,1.0e-8);
	BOOST_REQUIRE_CLOSE(7.27500, reaction.energyRelease,1.0e-8);
	BOOST_REQUIRE_CLOSE(-24.99350000, reaction.reaclibRateCoeff[0],1.0e-8);
	BOOST_REQUIRE_CLOSE(-4.29702000, reaction.reaclibRateCoeff[1],1.0e-8);
	BOOST_REQUIRE_CLOSE(-6.69304000, reaction.reaclibRateCoeff[2],1.0e-8);
	BOOST_REQUIRE_CLOSE(15.59030000, reaction.reaclibRateCoeff[3],1.0e-8);
	BOOST_REQUIRE_CLOSE(-1.57387000, reaction.reaclibRateCoeff[4],1.0e-8);
	BOOST_REQUIRE_CLOSE(0.17058800, reaction.reaclibRateCoeff[5],1.0e-8);
	BOOST_REQUIRE_CLOSE(-9.02800000, reaction.reaclibRateCoeff[6],1.0e-8);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantZ[0]);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantZ[1]);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantZ[2]);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantN[0]);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantN[1]);
	BOOST_REQUIRE_EQUAL(2,reaction.reactantN[2]);
	BOOST_REQUIRE_EQUAL(6,reaction.productZ[0]);
	BOOST_REQUIRE_EQUAL(6,reaction.productN[0]);
	BOOST_REQUIRE_EQUAL(0,reaction.reactants[0]);
	BOOST_REQUIRE_EQUAL(0,reaction.reactants[1]);
	BOOST_REQUIRE_EQUAL(0,reaction.reactants[2]);
	BOOST_REQUIRE_EQUAL(1,reaction.products[0]);

	// Make sure the statistical factor was read correctly. If rho=1, then
	// p_s = s*rho^(numReactants-1) = s.
	reaction.setPrefactor(1.0);
	BOOST_REQUIRE_CLOSE(0.16666667,reaction.prefactor ,1.0e-8);

	// Make sure the reaction rate can be computed correctly. If T = 1, then
	// the rate is equal to prefactor*e^(sum of reaclibRateCoeffs[0:5]). Note
	// log(1) = 0, so reaclibRateCoeff[6] is omitted.
	reaction.setRate(1.0);
	double sumCoeffs = reaction.reaclibRateCoeff[0] + reaction.reaclibRateCoeff[1]
			+ reaction.reaclibRateCoeff[2] + reaction.reaclibRateCoeff[3]
            + reaction.reaclibRateCoeff[4] + reaction.reaclibRateCoeff[5];
	double rate = reaction.prefactor*exp(sumCoeffs);
	BOOST_REQUIRE_CLOSE(rate,reaction.rate,1.0e-8);

	// Good enough for government work
	return;
}







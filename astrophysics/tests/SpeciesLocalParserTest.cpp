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
#include <Species.h>
#include <LocalParser.h>
#include <SpeciesLocalParser.h>

using namespace std;
using namespace fire;
using namespace fire::astrophysics;

// Test file names
static std::string networkFileName = "CUDAnet_alpha.inp";

/**
 * This operation checks the ability of the SpeciesLocalParser to parse the
 * network.
 */
BOOST_AUTO_TEST_CASE(checkParsing) {

	// Create the parser
	LocalParser<vector<Species>> parser =
			buildParser<vector,Species>(networkFileName);
	parser.parse();
	auto speciesListPtr = parser.getData();
	auto speciesList = *speciesListPtr;

	// The test file is an alpha network with sixteen species.
	BOOST_REQUIRE(parser.isFile());
	BOOST_REQUIRE_EQUAL(16,speciesList.size());

	// Check the first element
	Species helium = speciesList[0];
    BOOST_REQUIRE_EQUAL(helium.name,"4He");
    BOOST_REQUIRE_EQUAL(helium.massNumber,4);
    BOOST_REQUIRE_EQUAL(helium.atomicNumber,2);
    BOOST_REQUIRE_EQUAL(helium.neutronNumber,2);
    BOOST_REQUIRE_CLOSE(helium.massFraction,0.0,0.0);
    BOOST_REQUIRE_CLOSE(helium.massExcess,2.4250,1.0e-4);

	// Check the last element
	Species selenium = speciesList[15];
    BOOST_REQUIRE_EQUAL(selenium.name,"68Se");
    BOOST_REQUIRE_EQUAL(selenium.massNumber,68);
    BOOST_REQUIRE_EQUAL(selenium.atomicNumber,34);
    BOOST_REQUIRE_EQUAL(selenium.neutronNumber,34);
    BOOST_REQUIRE_CLOSE(selenium.massFraction,0.0,0.0);
    BOOST_REQUIRE_CLOSE(selenium.massExcess,-53.5530,1.0e-4);

	// Good enough for government work
	return;
}







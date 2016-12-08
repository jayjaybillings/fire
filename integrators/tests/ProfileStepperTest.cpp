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
#define BOOST_TEST_MODULE Parsers

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <stdio.h>
#include "ProfileStepper.h"

using namespace std;

/**
 * This operation checks the stepping with a profile.
 */
BOOST_AUTO_TEST_CASE(checkProfile) {

	std::vector<double> steps{1.0, 1.5, 1.9, 2.0};
	std::vector<double> sizes{0.5, 0.4, 0.1, 0.0};
	fire::ProfileStepper profile(steps,sizes);

	// Step 1
	BOOST_REQUIRE_EQUAL(1.0,profile.getStep());
	BOOST_REQUIRE_EQUAL(0.5,profile.getStepSizeAtStage(1));
	profile.updateStep();
	// Step 2
	BOOST_REQUIRE_EQUAL(1.5,profile.getStep());
	BOOST_REQUIRE_EQUAL(0.4,profile.getStepSizeAtStage(1));
	profile.updateStep();
	// Step 3
	BOOST_REQUIRE_EQUAL(1.9,profile.getStep());
	BOOST_REQUIRE_EQUAL(0.1,profile.getStepSizeAtStage(1));
	profile.updateStep();
	// Step 4
	BOOST_REQUIRE_EQUAL(2.0,profile.getStep());
	BOOST_REQUIRE_EQUAL(0.0,profile.getStepSizeAtStage(1));
	profile.updateStep();

	return;
}

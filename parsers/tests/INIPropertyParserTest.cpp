/**----------------------------------------------------------------------------
 Copyright (c) 2015-, Jay Jay Billings
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
#define BOOST_TEST_MODULE Regression

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include "INIPropertyParser.h"

using namespace std;

// Test file name
static std::string testFileName = "INIPropertyParserTestFile.txt";

// This is a class that creates and later deletes the test input file.
class BlockGenerator {
public:

	// Constructor - Setup
	BlockGenerator() {
		// Open the file and write some blocks to it
		std::fstream testFile;
		testFile.open(testFileName.c_str());
		testFile << "[block1]\n";
		testFile << "prop1=value1\n";
		testFile << "prop2=value2\n";
		testFile << "\n";
		testFile << "[block2]\n";
		testFile << "prop3=value4\n";
		testFile << "prop4=value4\n";
		testFile << "prop5=value5\n";
	}

	// Destructor - Teardown
	~BlockGenerator() {
		// Delete the test file
		remove(testFileName.c_str());
	}

};

BOOST_FIXTURE_TEST_SUITE(BasicPropertyBlock_testSuite, BlockGenerator)

/**This operation checks default parsing setup of the INIPropertyParser.*/
BOOST_AUTO_TEST_CASE(checkBlocks) {

	// Configure the parser
	fire::INIPropertyParser parser;
	parser.setSource(testFileName);
	parser.parse();

	// The third line should skipped because it is a comment so, get the fourth
	// line and check it
//	auto dLine = 1;
//	auto aValue = 1.05;
//	BOOST_REQUIRE(dLine > 0);
//	BOOST_REQUIRE_EQUAL(1, dLine);
//	BOOST_REQUIRE_CLOSE_FRACTION(1.05, aValue, 0.001);

	return;
}

BOOST_AUTO_TEST_SUITE_END()

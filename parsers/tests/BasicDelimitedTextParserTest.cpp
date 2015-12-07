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
#define BOOST_TEST_MODULE Parsers

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include "INIPropertyParser.h"
#include "BasicDelimitedTextParser.h"

using namespace std;

// Test file name
static std::string testFileName = "INIPropertyParserTestFile.txt";

// This is a class that creates and later deletes the test input file.
struct BlockGenerator {

	// Constructor - Setup
	BlockGenerator() {
		BOOST_TEST_MESSAGE( "Configuring fixture." );

		// Open the file and write some blocks to it
		std::fstream testFile;
		testFile.open(testFileName.c_str(), std::fstream::out | std::fstream::app);
		testFile << "[block1]\n";
		testFile << "prop1=value1\n";
		testFile << "prop2=value2\n";
		testFile << "\n";
		testFile << "[block2]\n";
		testFile << "prop3=value3\n";
		testFile << "prop4=value4\n";
		testFile << "prop5=value5\n";
		testFile.close();

		BOOST_TEST_MESSAGE( "Fixture configured." );

		return;
	}

	// Destructor - Teardown
	~BlockGenerator() {
		// Delete the test file
		remove(testFileName.c_str());
	}

};

/**This operation checks default parsing setup of the INIPropertyParser.*/
BOOST_FIXTURE_TEST_CASE(checkBlocks, BlockGenerator) {

	// Configure the parser
	fire::INIPropertyParser parser;
	parser.setSource(testFileName);
	parser.parse();

	// Get the block names and check them
	auto names = parser.getPropertyBlockNames();
	BOOST_REQUIRE_EQUAL(2,names.size());
	BOOST_REQUIRE_EQUAL(names[0],"block1");
	BOOST_REQUIRE_EQUAL(names[1],"block2");

	// Get the blocks and check them
	auto block = parser.getPropertyBlock("block1");
	BOOST_TEST_MESSAGE(block["prop1"]);
	BOOST_REQUIRE_EQUAL("value1",block.at("prop1"));
	BOOST_REQUIRE_EQUAL("value2",block.at("prop2"));
	block = parser.getPropertyBlock("block2");
	BOOST_REQUIRE_EQUAL("value3",block.at("prop3"));
	BOOST_REQUIRE_EQUAL("value4",block.at("prop4"));
	BOOST_REQUIRE_EQUAL("value5",block.at("prop5"));

	return;
}

BOOST_AUTO_TEST_CASE(checkSource) {

	// Configure the parser
	fire::INIPropertyParser parser;
	parser.setSource(testFileName);
	BOOST_REQUIRE_EQUAL(testFileName,parser.getSource());

	return;
}

BOOST_AUTO_TEST_CASE(checkBools) {

	// Configure the parser
	fire::INIPropertyParser parser;
	BOOST_REQUIRE_EQUAL(true,parser.isFile());
	BOOST_REQUIRE_EQUAL(true,parser.isLocal());
	BOOST_REQUIRE_EQUAL(false,parser.isParallel());

	return;
}

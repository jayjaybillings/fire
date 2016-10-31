
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
#include <memory>
#include <DelimitedTextParser.h>

using namespace std;

// Test file names
static string csvFileName = "testFile.csv";
static string spaceFileName = "testFile.dat";

// This is a class that creates and later deletes the test input file.
struct FileGenerator {

	// Constructor - Setup
	FileGenerator() {
		BOOST_TEST_MESSAGE( "Configuring fixture." );

		// Open the file and write some CSV blocks to it
		fstream testFile;
		testFile.open(csvFileName.c_str(), fstream::out | fstream::app);
		testFile << "#Comment\n";
		testFile << "1.0, 3.1, 99.99\n";
		testFile << "8,6,7,5,3,0,9\n";
		testFile << "\n";
		testFile << "38,7, 74.12,8729.9 \n";
		testFile.close();

		// Open the file and write some space blocks to it
		testFile.open(spaceFileName.c_str(), fstream::out | fstream::app);
		testFile << "#Comment\n";
		testFile << "1.0 3.1 99.99\n";
		testFile << "8 6 7 5 3 0 9\n";
		testFile << "\n";
		testFile << "38 7 74.12 8729.9 \n";
		testFile.close();

		BOOST_TEST_MESSAGE( "Fixture configured." );

		return;
	}

	// Destructor - Teardown
	~FileGenerator() {
		// Delete the test files
		remove(csvFileName.c_str());
		remove(spaceFileName.c_str());
	}

};

/**
 * This operation checks the data in the array.
 * @param data the array of data that should be checked
 */
void checkData(vector<vector<double>> data) {
	// Check the data
	BOOST_REQUIRE_EQUAL(3,data.size());
	vector<double> & dataEntry = data[0];
	BOOST_REQUIRE_EQUAL(3,dataEntry.size());
	BOOST_REQUIRE_EQUAL(1.0,dataEntry[0]);
	BOOST_REQUIRE_EQUAL(3.1,dataEntry[1]);
	BOOST_REQUIRE_EQUAL(99.99,dataEntry[2]);
	dataEntry = data[1];
	BOOST_REQUIRE_EQUAL(7,dataEntry.size());
	BOOST_REQUIRE_EQUAL(8.0,dataEntry[0]);
	BOOST_REQUIRE_EQUAL(6.0,dataEntry[1]);
	BOOST_REQUIRE_EQUAL(7.0,dataEntry[2]);
	BOOST_REQUIRE_EQUAL(5.0,dataEntry[3]);
	BOOST_REQUIRE_EQUAL(3.0,dataEntry[4]);
	BOOST_REQUIRE_EQUAL(0.0,dataEntry[5]);
	BOOST_REQUIRE_EQUAL(9.0,dataEntry[6]);
	dataEntry = data[2];
	BOOST_REQUIRE_EQUAL(4,dataEntry.size());
	BOOST_REQUIRE_EQUAL(38.0,dataEntry[0]);
	BOOST_REQUIRE_EQUAL(7.0,dataEntry[1]);
	BOOST_REQUIRE_EQUAL(74.12,dataEntry[2]);
	BOOST_REQUIRE_EQUAL(8729.9,dataEntry[3]);

	return;
}

/**
 * This operation checks CSV parsing.
 */
BOOST_FIXTURE_TEST_CASE(checkCSV, FileGenerator) {

	// Configure the parser and grab the data
	fire::DelimitedTextParser<vector<vector<double>>,double> parser(string(","),string("#"));
	parser.setSource(csvFileName);
	parser.parse();
	shared_ptr<vector<vector<double>>> data = parser.getData();

	// Check the data
	checkData(*data);

	return;
}

/**
 * This operation checks default parsing setup of the INIPropertyParser.
 */
BOOST_FIXTURE_TEST_CASE(checkSpace, FileGenerator) {

	// Configure the parser and grab the data
	fire::DelimitedTextParser<vector<vector<double>>,double> parser(string(" "),string("#"));
	parser.setSource(spaceFileName);
	parser.parse();
	shared_ptr<vector<vector<double>>> data = parser.getData();

	// Check the data
	checkData(*data);

	return;
}

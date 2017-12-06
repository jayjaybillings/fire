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

#ifndef ASTROPHYSICS_SPECIESLOCALPARSER_H_
#define ASTROPHYSICS_SPECIESLOCALPARSER_H_

#include <vector>
#include <memory>
#include "Species.h"
#include "LocalParser.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "StringCaster.h"

using namespace std;
using namespace fire::astrophysics;

namespace fire {

/**
 * This operation parses a file that holds the basic species
 * information for a thermonuclear network.
 */
template<>
void LocalParser<std::vector<Species>>::parse() {

	// Note: "data" has already been initialized by the base class.
	// Load the contents of the file
	string value, line;
	ifstream fileStream;
	fileStream.open(sourceFile.c_str(), ifstream::in);
	string delimiter = " ";
	// Pull each line and push it into the list
	if (fileStream.is_open()) {
		// Pull each line from the file
		while (getline(fileStream, line)) {
			if (!line.empty() && !line.find("#") == 0) {
				istringstream ss(line);
				vector < string > lineVec;
				// Split the line and push each element into the line list.
				while (getline(ss, value, *delimiter.c_str())) {
					lineVec.push_back(value);
				}
				// Load the main species data from the lines with six elements,
				// which is every fifth line. Ignore all the other lines for now
				// because they contain unused partition function data.
				if (lineVec.size() == 6) {
					// Create the species from the line, which has the format of
					// "name massNumber atomicNumber neutronNumber massFraction
					// massExcess". Use the builder to put this right into the
					// data list. Note there is a *real* copy from the return
					// value of the build function.
					data->push_back(build<Species,const vector<string> &>(lineVec));
				}
			}
		}
		fileStream.close();
	} else {
		throw "Species file stream not open! Check directory?";
	}

	return;
};


} /* namespace fire */

#endif /* ASTROPHYSICS_SPECIESLOCALPARSER_H_ */

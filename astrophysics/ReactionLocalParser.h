/**----------------------------------------------------------------------------
 Copyright (c) 2016-, UT-Battelle, LLC
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

#ifndef ASTROPHYSICS_ReactionLOCALPARSER_H_
#define ASTROPHYSICS_ReactionLOCALPARSER_H_

#include <vector>
#include <memory>
#include <Reaction.h>
#include <LocalParser.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <build.h>

using namespace std;
using namespace fire::astrophysics;

namespace fire {

/**
 * This operation parses a file that holds the basic Reaction
 * information for a thermonuclear network.
 */
template<>
void LocalParser<vector<Reaction>>::parse() {

	// Note: "data" has already been initialized by the base class.
	// Load the contents of the file
	string value, line;
	ifstream fileStream;
	// Source file is from the base class
	fileStream.open(sourceFile.c_str(), ifstream::in);
	string delimiter = " ";
	vector<string> lines, lineVec;

	// The method parses the file by first reading it entirely into memory.
	// This is not the optimal way to implement it because it requires twice
	// amount of memory as pure streaming would, but since the current layout
	// of the Reaction class requires that arrays be allocated during parsing
	// and that the exact number of Reactions be known in advance, this is
	// currently the best approach.

	// Pull each line and push it into the list
	if (fileStream.is_open()) {
		// Pull each line from the file
		while (getline(fileStream, line)) {
			if (!line.empty() && !line.find("#") == 0) {
				lines.push_back(line);
			}
		}
		fileStream.close();
	} else {
		throw "Reaction file stream not open! Check directory?";
	}

	// Parse the line list into individual reactions. Check first to make
	// sure that the total number of lines is divisible by eight, which
	// means that it contains a complete set of reactions since each reaction
	// requires eight lines of data.
	if (!(lines.size() % 8)) {
		// Loop in eight line blocks
		for (int i = 0; i < lines.size(); i=i+8) {
			// Load the eight lines that represent a reaction into a vector
			vector<string> reactionLines;
			reactionLines.push_back(lines[i]);
			reactionLines.push_back(lines[i+1]);
			reactionLines.push_back(lines[i+2]);
			reactionLines.push_back(lines[i+3]);
			reactionLines.push_back(lines[i+4]);
			reactionLines.push_back(lines[i+5]);
			reactionLines.push_back(lines[i+6]);
			reactionLines.push_back(lines[i+7]);
			// Push it to the builder
			Reaction reaction = build<Reaction,const vector<string> &>(reactionLines);

			// N.B. - The FERN routine for parsing this file includes
			// a significantly large amount of code beyond parsing to
			// construct arrays necessary for solving this problem in
			// a partial equilibrium approximation. That code would
			// not be appropriate for this routine and is therefore
			// ignored.

			// Copy it into the data vector. Note this is a *real*
			// copy.
			data->push_back(reaction);
		}
	} else {
		throw "Reaction file is incomplete. Check entries?";
	}

	return;
};

} /* namespace fire */

#endif /* ASTROPHYSICS_ReactionLOCALPARSER_H_ */

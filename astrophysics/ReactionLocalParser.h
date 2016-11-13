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

#ifndef ASTROPHYSICS_ReactionLOCALPARSER_H_
#define ASTROPHYSICS_ReactionLOCALPARSER_H_

#include <vector>
#include <memory>
#include "Reaction.h"
#include "LocalParser.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "StringCaster.h"

using namespace std;
using namespace fire::astrophysics;

/**
 * This is a utility function used for splitting lines.
 * @param the line to be split
 * @return a vector with a string for each entry in the line.
 */
static inline vector<string> splitLine(string line) {

	string value;
	istringstream ss(line);
	vector < string > lineVec;
	string delimiter = " ";
	//Split the line and push each element into the line list.
	while (getline(ss, value, *delimiter.c_str())) {
	      lineVec.push_back(value);
	}
	return lineVec;
}

namespace fire {

/**
 * This operation parses a file that holds the basic Reaction
 * information for a thermonuclear network.
 */
template<>
void LocalParser<std::vector<Reaction>>::parse() {

	// Note: "data" has already been initialized by the base class.
	// Load the contents of the file
	string value, line;
	ifstream fileStream;
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
			Reaction reaction;
			// Line 1 - Basic Reaction Metadata
			lineVec = splitLine(lines[i]);
			if (lineVec.size() == 10) {
				reaction.name = lineVec[0];
				reaction.reactionGroupClass = StringCaster<int>::cast(
						lineVec[1]);
				reaction.reactionGroupMemberIndex = StringCaster<int>::cast(
						lineVec[2]);
				reaction.reaclibClass = StringCaster<int>::cast(lineVec[3]);
				reaction.numReactants = StringCaster<int>::cast(
						lineVec[4]);
				reaction.numProducts = StringCaster<int>::cast(lineVec[5]);
				reaction.isElectronCapture = StringCaster<bool>::cast(
						lineVec[6]);
				reaction.isReverse = StringCaster<bool>::cast(lineVec[7]);
				reaction.statisticalFactor = StringCaster<double>::cast(
						lineVec[8]);
				reaction.energyRelease = StringCaster<double>::cast(lineVec[9]);
			} else {
				ostringstream error;
				error << "Invalid first line for reaction in file! Missing an element? ";
				error << "Check around reaction " << (i*8+1) << ".";
				throw runtime_error(error.str());
			}
			// Line 2 - Reaclib Coefficients
			lineVec = splitLine(lines[i+1]);
			if (lineVec.size() == 7) {
				reaction.reaclibRateCoeff[0] = StringCaster<double>::cast(lineVec[0]);
				reaction.reaclibRateCoeff[1] = StringCaster<double>::cast(lineVec[1]);
				reaction.reaclibRateCoeff[2] = StringCaster<double>::cast(lineVec[2]);
				reaction.reaclibRateCoeff[3] = StringCaster<double>::cast(lineVec[3]);
				reaction.reaclibRateCoeff[4] = StringCaster<double>::cast(lineVec[4]);
				reaction.reaclibRateCoeff[5] = StringCaster<double>::cast(lineVec[5]);
				reaction.reaclibRateCoeff[6] = StringCaster<double>::cast(lineVec[6]);
			} else {
				ostringstream error;
				error << "Invalid number of reaclib rate coefficients in file! ";
				error << "Check coefficients for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 3 - Reactant Z values
			lineVec = splitLine(lines[i+2]);
			if (lineVec.size() > 0 && lineVec.size() < 4) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.reactantZ[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of reactant atomic numbers in file! ";
				error << "Check values for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 4 - Reactant N values
			lineVec = splitLine(lines[i+3]);
			if (lineVec.size() > 0 && lineVec.size() <= 4) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.reactantN[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of reactant neutron numbers in file! ";
				error << "Check values for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 5 - Product Z values
			lineVec = splitLine(lines[i+4]);
			if (lineVec.size() > 0 && lineVec.size() <= 4) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.productZ[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of product atomic numbers in file! ";
				error << "Check values for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 6 - Product N Values
			lineVec = splitLine(lines[i+5]);
			if (lineVec.size() > 0 && lineVec.size() <= 4) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.productN[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of product neutronn numbers in file! ";
				error << "Check values for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 7 - Partial Equilibrium data
			lineVec = splitLine(lines[i+6]);
			if (lineVec.size() > 0 && lineVec.size() <= 3) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.reactants[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of PE reactants in file! ";
				error << "Check values in line 7 for " << reaction.name << ".";
				throw runtime_error(error.str());
			}
			// Line 8 - Partial Equilibrium data
			lineVec = splitLine(lines[i+7]);
			if (lineVec.size() > 0 && lineVec.size() <= 3) {
				// We can't unroll this because we don't know how many values are
				// available.
				for(int j = 0; j < lineVec.size(); j++) {
					reaction.products[j] = StringCaster<int>::cast(lineVec[j]);
				}
			} else {
				ostringstream error;
				error << "Invalid number of PE products in file! ";
				error << "Check values in line 8 for " << reaction.name << ".";
				throw runtime_error(error.str());
			}

			// N.B. - The FERN routine for parsing this file includes
			// a significantly large amount of code beyond parsing to
			// construct arrays necessary for solving this problem in
			// a partial equilibrium approximation. That code would
			// not be appropriate for this routine and is therefore
			// ignored.

			// PUSH REACTION ONTO LIST!
			// Copy it into the data vector. Note this is a *real*
			// copy.
			data->push_back(reaction);
		}
	} else {
		throw "Reaction file is incomplete. Check entries?";
	}



	return;
}
;

} /* namespace fire */

#endif /* ASTROPHYSICS_ReactionLOCALPARSER_H_ */

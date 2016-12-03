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
#ifndef ASTROPHYSICS_REACTIONNETWORK_H_
#define ASTROPHYSICS_REACTIONNETWORK_H_

#include <build.h>
#include <Reaction.h>
#include <ReactionLocalParser.h>
#include <Species.h>
#include <SpeciesLocalParser.h>
#include <map>
#include <memory>
#include <StringCaster.h>
#include <parse.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <array>

using namespace std;

namespace fire {
namespace astrophysics {

/**
 * This class collects all of the information about a thermonuclear reaction
 * network for astrophysical systems. It includes all species and reaction
 * information and other stateful variable.
 *
 * The design of this class is not ideal because it exposes public members
 * variables. This design is still an advancement over the original design
 * in FERN and is good enough for now. We'll improve it later.
 */
class ReactionNetwork {

public:

	/**
	 * The number of species in the network
	 */
	int numSpecies;

	/**
	 * The number of reactions between the species in the
	 * network
	 */
	int numReactions;

	/**
	 * The number of reaction groups in the network
	 */
	int numReactionGroups;

	/**
	 * The mass tolerance for integration in the network
	 */
	double massTol;

	/**
	 * A tunable parameter to limit the integration step size based
	 * solely on the flux
	 */
	double fluxFrac;

	/**
	 * The name of file that contains the species present in the network.
	 */
	string networkFileName;

	/**
	 * The name of the file that contains the reaction data for the network.
	 */
	string rateFileName;

	/**
	 * This is the list of species in the network.
	 */
	shared_ptr<vector<Species>> species;

	/**
	 * This is the list of reactions in the network.
	 */
	shared_ptr<vector<Reaction>> reactions;

	/**
	 * The array of size numSpecies*numReactions that contains the serialized
	 * map of contributing fluxes for each reaction for each species.
	 */
	vector<double> fPlusMap;

	/**
	 * The array of size numSpecies*numReactions that contains the serialized
	 * map of detracting fluxes for each reaction for each species.
	 */
	vector<double> fMinusMap;

	/**
	 * The array of size numSpecies*numReactions that contains the serialized
	 * set of numerical factors due to contributing fluxes.
	 */
	vector<double> fPlusFactors;

	/**
	 * The array of size numSpecies*numReactions that contains the serialized
	 * set of numerical factors due to detracting fluxes.
	 */
	vector<double> fMinusFactors;

	/**
	 * The array of maximum contributing flux values for each species in the
	 * network. (Size = numSpecies)
	 */
	vector<unsigned short> fPlusMaximums;

	/**
	 * The array of maximum detracting flux values for each species in the
	 * network. (Size = numSpecies)
	 */
	vector<unsigned short> fMinusMaximums;

	/**
	 * This operation sets the properties of the network from a map. It is
	 * designed to work with property blocks pulled from INI files. It expects
	 * the following keys to have values in the map and accepts the default if
	 * they do not:
	 * numSpecies
	 * numReactions
	 * numReactionGroups
	 * massTol
	 * fluxFrac
	 * networkFile
	 * rateFile
	 * @param the property map
	 */
	void setProperties(const map<string,string> & props) {
		// Set the properties
		numSpecies = StringCaster<int>::cast(props.at("numSpecies"));
		numReactions = StringCaster<int>::cast(props.at("numReactions"));
		numReactionGroups = StringCaster<int>::cast(props.at("numReactionGroups"));
		massTol = StringCaster<double>::cast(props.at("massTol"));
		fluxFrac = StringCaster<double>::cast(props.at("fluxFrac"));
		networkFileName = props.at("networkFile");
		rateFileName = props.at("rateFile");
	}

	/**
	 * This operations directs the Network to load the species and reactions
	 * from the files provided in the properties list.
	 */
	void load() {
		// Load the species
		species = parse<Species>(networkFileName);
		// Load the reactions
		reactions = parse<Reaction>(rateFileName);
		if (numSpecies == species->size() && numReactions == reactions->size()) {
		   // Build the flux maps based on the network configuration
		   buildFluxMaps();
		} else {
			ostringstream error;
			error << "Error in species or reaction sizes while loading network!"
					<< endl;
			error << "# Species in parameter file = " << numSpecies << endl;
			error << "# Species in species file = " << species->size() << endl;
			error << "# Reactions in parameter file = " << numReactions << endl;
			error << "# Reactions in species file = " << reactions->size() << endl;
			throw runtime_error(error.str());
		}

		return;
	}

	/**
	 * This operation builds the "flux maps" that map the contributions of each
	 * reaction for each species in the network.
	 *
	 * This function was originally written as part of the FERN code and
	 * desperately needs to be refactored. It is has been adapted to use the
	 * Species and Reaction classes in Fire, but still contains a number of old
	 * constructs that have poor performance implications. Likewise, it lacks a
	 * sophisticated view of data structures that could greatly increase its
	 * speed. It is fine for now because ultimately the formation of the
	 * flux maps and numerical prefactors on the RHS take up an extremely small
	 * amount of computational time compared to the rest of the integration
	 * since they are only computed once on initialization of the network.
	 */
	void buildFluxMaps() {
		// These tempInt blocks will become fPlusMap and fMinusMap eventually.
		const size_t tempIntSize = numSpecies * numReactions / 2;
		unsigned short tempInt1 [tempIntSize];
		unsigned short tempInt2 [tempIntSize];

		// Access elements by reacMask[speciesIndex + species * reactionIndex].
		int reacMask[numSpecies * numReactions]; // [species][reactions]
		int numFluxPlus [numSpecies];
		int numFluxMinus[numSpecies];

		// Start of Guidry's original parseF() code
		int incrementPlus = 0;
		int incrementMinus = 0;

		int totalFplus = 0;
		int totalFminus = 0;

		// Loop over all isotopes in the network

		for (int i = 0; i < numSpecies; i++) {
			int total = 0;
			int numFplus = 0;
			int numFminus = 0;

			// Loop over all possible reactions for this isotope, finding those that
			// change its population up (contributing to F+) or down (contributing
			// to F-).
			Species currentSpecies = species->at(i);
			for (int j = 0; j < numReactions; j++) {
				int totalL = 0;
				int totalR = 0;

				// Loop over reactants for this reaction
				Reaction & reaction = reactions->at(j);
				for (int k = 0; k < reaction.numReactants; k++) {
					if (currentSpecies.atomicNumber == reaction.reactantZ[k]
							&& currentSpecies.neutronNumber
									== reaction.reactantN[k])
						totalL++;
				}

				// Loop over products for this reaction
				for (int k = 0; k < reaction.numProducts; k++) {
					if (currentSpecies.atomicNumber == reaction.productZ[k]
							&& currentSpecies.neutronNumber
									== reaction.productN[k])
						totalR++;
				}

				total = totalL - totalR;

				if (total > 0)       // Contributes to F- for this isotope
						{
					numFminus++;
					reacMask[i + numSpecies * j] = -total;
					tempInt2[incrementMinus + numFminus - 1] = j;
				} else if (total < 0)  // Contributes to F+ for this isotope
						{
					numFplus++;
					reacMask[i + numSpecies * j] = -total;
					tempInt1[incrementPlus + numFplus - 1] = j;
				} else               // Does not contribute to flux for this isotope
				{
					reacMask[i + numSpecies * j] = 0;
				}
			}

			// Keep track of the total number of F+ and F- terms in the network for all isotopes
			totalFplus += numFplus;
			totalFminus += numFminus;

			numFluxPlus[i] = numFplus;
			numFluxMinus[i] = numFminus;

			incrementPlus += numFplus;
			incrementMinus += numFminus;

			// if (showParsing == 1)
			// 	printf("%d %s numF+ = %d numF- = %d\n", i, isoLabel[i], numFplus, numFminus);
		}

		// Create 1D arrays that will be used to map finite F+ and F- to the Flux array.
		int FplusIsotopeCut[numSpecies];
		int FminusIsotopeCut[numSpecies];
		int FplusIsotopeIndex[totalFplus];
		int FminusIsotopeIndex[totalFminus];

		FplusIsotopeCut[0] = numFluxPlus[0];
		FminusIsotopeCut[0] = numFluxMinus[0];
		for (int i = 1; i < numSpecies; i++) {
			FplusIsotopeCut[i] = numFluxPlus[i] + FplusIsotopeCut[i - 1];
			FminusIsotopeCut[i] = numFluxMinus[i] + FminusIsotopeCut[i - 1];
		}

		int currentIso = 0;
		for (int i = 0; i < totalFplus; i++) {
			FplusIsotopeIndex[i] = currentIso;
			if (i == (FplusIsotopeCut[currentIso] - 1))
				currentIso++;
		}

		currentIso = 0;
		for (int i = 0; i < totalFminus; i++) {
			FminusIsotopeIndex[i] = currentIso;
			if (i == (FminusIsotopeCut[currentIso] - 1))
				currentIso++;
		}

		fPlusMap.resize(totalFplus);
		for (int i = 0; i < totalFplus; i++) {
			fPlusMap[i] = tempInt1[i];
		}
		fMinusMap.resize(totalFminus);
		for (int i = 0; i < totalFminus; i++) {
			fMinusMap[i] = tempInt2[i];
		}

		// Populate the FplusMin and FplusMax arrays
		unsigned short FplusMin [numSpecies];
		unsigned short FminusMin [numSpecies];

		FplusMin[0] = 0;
		fPlusMaximums.resize(numSpecies);
		fPlusMaximums[0] = numFluxPlus[0] - 1;
		for (int i = 1; i < numSpecies; i++) {
			FplusMin[i] = fPlusMaximums[i - 1] + 1;
			fPlusMaximums[i] = FplusMin[i] + numFluxPlus[i] - 1;
		}
		// Populate the FminusMin and FminusMax arrays
		FminusMin[0] = 0;
		fMinusMaximums.resize(numSpecies);
		fMinusMaximums[0] = numFluxMinus[0] - 1;
		for (int i = 1; i < numSpecies; i++) {
			FminusMin[i] = fMinusMaximums[i - 1] + 1;
			fMinusMaximums[i] = FminusMin[i] + numFluxMinus[i] - 1;
		}

		// Allocate the flux vectors
		fPlusFactors.resize(numSpecies*numReactions);
		fMinusFactors.resize(numSpecies*numReactions);

		// Populate the fPlusFac and FminusFac arrays that hold the factors counting the
		// number of occurences of the species in the reaction.  Note that this can only
		// be done after parseF() has been run to give reacMask[i][j].
		int tempCountPlus = 0;
		int tempCountMinus = 0;
		for (int i = 0; i < numSpecies; i++) {
			for (int j = 0; j < numReactions; j++) {
				if (reacMask[i + numSpecies * j] > 0) {
					fPlusFactors[tempCountPlus] = reacMask[i + numSpecies * j];
					tempCountPlus++;
				} else if (reacMask[i + numSpecies * j] < 0) {
					fMinusFactors[tempCountMinus] = -1.0*reacMask[i
							+ numSpecies * j];
					tempCountMinus++;
				}
			}
		} // If you run this through the debugger, it is a lot of work to calculate 0!

		// Notice that fPlusFactors == fMinusFactors! ~ Question for Mike!

		return;
	}

	/**
	 * This function computes the prefactors for the reaction rates. It is primarily a
	 * convenience function for configuring the reactions.
	 */
	void computePrefactors(const double & rho) {
		// Compute the factors.
		for(Reaction & reaction : *reactions) {
	        reaction.setPrefactor(rho);
		}
	}

	/**
	 * This function computers the reaction rates. It is primarily a convenience function
	 * for configuring the reactions.
	 */
	void computeRates(const double & temp) {
		// Compute the temperatures
		array<double,6> tempValues;
		double cbrtT = cbrt(temp); // Cube root of T
		tempValues[0] = 1 / temp; // 1/T
		tempValues[1] = 1 / cbrtT;
		tempValues[2] = cbrtT;
		tempValues[3] = temp;
		tempValues[4] = cbrtT * cbrtT * cbrtT * cbrtT * cbrtT; // T^(5/3)
		tempValues[5] = log(temp);

		// Compute the rates
		for (Reaction & reaction : *reactions) {
			reaction.setRate(tempValues);
		}
	}

};

} /** namespace astrophysics **/
} /** namespace fire **/



#endif /* ASTROPHYSICS_REACTIONNETWORK_H_ */

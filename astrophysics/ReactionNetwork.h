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

	/*
	 * The total number of reactions that result in increases in flux. It is
	 * also the number of entries in the fPlusMap member.
	 */
	int numFPlus;

	/*
	 * The total number of reactions that result in decreases in flux. It is
	 * also the number of entries in the fMinusMap member.
	 */
	int numFMinus;

	double & operator()(int i) { return species->at(i).massFraction; }


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
	 * @param props the property map
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
		vector<int> tempInt1(tempIntSize,0);
	    vector<int> tempInt2(tempIntSize,0);

		// Access elements by reacMask[speciesIndex + species * reactionIndex].
		vector<int> reacMask(numSpecies * numReactions,0); // [species][reactions]
		vector<int> numFluxPlus(numSpecies,0), numFluxMinus(numSpecies,0);

		// Start of Guidry's original parseF() code
		int incrementPlus = 0;
		int incrementMinus = 0;
		int localNumFPlus = 0, localNumFMinus = 0;
		numFPlus = 0; numFMinus = 0;

		// Loop over all isotopes in the network

		for (int i = 0; i < numSpecies; i++) {
			int total = 0;
			localNumFPlus = 0;
			localNumFMinus = 0;

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
					localNumFMinus++;
					reacMask[i + numSpecies * j] = -total;
					tempInt2[incrementMinus + localNumFMinus - 1] = j;
				} else if (total < 0)  // Contributes to F+ for this isotope
						{
					localNumFPlus++;
					reacMask[i + numSpecies * j] = -total;
					tempInt1[incrementPlus + localNumFPlus - 1] = j;
				} else               // Does not contribute to flux for this isotope
				{
					reacMask[i + numSpecies * j] = 0;
				}
			}

			// Keep track of the total number of F+ and F- terms in the network for all isotopes
			numFPlus += localNumFPlus;
			numFMinus += localNumFMinus;

			numFluxPlus[i] = localNumFPlus;
			numFluxMinus[i] = localNumFMinus;

			incrementPlus += localNumFPlus;
			incrementMinus += localNumFMinus;

			// if (showParsing == 1)
			// 	printf("%d %s numF+ = %d numF- = %d\n", i, isoLabel[i], numFplus, numFminus);
		}

		// Create 1D arrays that will be used to map finite F+ and F- to the Flux array.
		int FplusIsotopeCut[numSpecies];
		int FminusIsotopeCut[numSpecies];
		int FplusIsotopeIndex[numFPlus];
		int FminusIsotopeIndex[numFMinus];

		FplusIsotopeCut[0] = numFluxPlus[0];
		FminusIsotopeCut[0] = numFluxMinus[0];
		for (int i = 1; i < numSpecies; i++) {
			FplusIsotopeCut[i] = numFluxPlus[i] + FplusIsotopeCut[i - 1];
			FminusIsotopeCut[i] = numFluxMinus[i] + FminusIsotopeCut[i - 1];
		}

		int currentIso = 0;
		for (int i = 0; i < numFPlus; i++) {
			FplusIsotopeIndex[i] = currentIso;
			if (i == (FplusIsotopeCut[currentIso] - 1))
				currentIso++;
		}

		currentIso = 0;
		for (int i = 0; i < numFMinus; i++) {
			FminusIsotopeIndex[i] = currentIso;
			if (i == (FminusIsotopeCut[currentIso] - 1))
				currentIso++;
		}

		fPlusMap.resize(numFPlus);
		for (int i = 0; i < numFPlus; i++) {
			fPlusMap[i] = tempInt1[i];
		}
		fMinusMap.resize(numFMinus);
		for (int i = 0; i < numFMinus; i++) {
			fMinusMap[i] = tempInt2[i];
		}

		// Populate the fPlusMin and FplusMax arrays
		unsigned short fPlusMin [numSpecies];
		unsigned short fMinusMin [numSpecies];

		fPlusMin[0] = 0;
		fPlusMaximums.resize(numSpecies);
		fPlusMaximums[0] = numFluxPlus[0] - 1;
		for (int i = 1; i < numSpecies; i++) {
			fPlusMin[i] = fPlusMaximums[i - 1] + 1;
			fPlusMaximums[i] = fPlusMin[i] + numFluxPlus[i] - 1;
		}
		// Populate the fMinusMin and FminusMax arrays
		fMinusMin[0] = 0;
		fMinusMaximums.resize(numSpecies);
		fMinusMaximums[0] = numFluxMinus[0] - 1;
		for (int i = 1; i < numSpecies; i++) {
			fMinusMin[i] = fMinusMaximums[i - 1] + 1;
			fMinusMaximums[i] = fMinusMin[i] + numFluxMinus[i] - 1;
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
	 * This function computes the prefactors for the reaction rates. It is
	 * primarily a convenience function for configuring the reactions. The
	 * prefactors are stored in the reactions themselves.
	 * @param rho the current density in units of g/m^3.
	 */
	void computePrefactors(const double & rho) {
		// Compute the factors.
		for(Reaction & reaction : *reactions) {
	        reaction.setPrefactor(rho);
		}
	}

	/**
	 * This function computers the reaction rates. It is primarily a
	 * convenience function for configuring the reactions. The rates are
	 * stored in the rate member variable on the reaction itself.
	 * @param temp the current temperature in units of 10^9 Kelvin.
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

	/**
	 * This operation computes the fluxes for the species in the network under
	 * the given conditions. The fluxes are stored in the flux member variable
	 * on the species itself.
	 */
	void computeFluxes() {

		vector<double> reactionFlux(numReactions, 0);

		// Compute the flux due to each reaction
		for (int i = 0; i < numReactions; i++) {
			Reaction & reaction = reactions->at(i);
			// This computes the flux as the product of the rate times some
			// power of the current abundance. It is written as presented below
			// to (hopefully) streamline the performance. There is a branchless
			// implementation too, but it isn't clear to me that it is faster.
			// 1 Body
			reactionFlux[i] = reaction.rate
					* (species->at(reaction.reactants[0]).massFraction);
			// 2 Body
			if (reaction.numReactants > 1)
				reactionFlux[i] *=
						(species->at(reaction.reactants[1]).massFraction);
			// 3 Body
			else if (reaction.numReactants > 2)
				reactionFlux[i] *=
						(species->at(reaction.reactants[1]).massFraction)
								* (species->at(reaction.reactants[2]).massFraction);
		}

		// Populate the incoming (fPlus) and outgoing flux (fMinus).
		vector<double> fPlus(numFPlus, 0);
		for (int i = 0; i < numFPlus; i++) {
			fPlus[i] = fPlusFactors[i] * reactionFlux[fPlusMap[i]];
		}
		vector<double> fMinus(numFMinus, 0);
		for (int i = 0; i < numFMinus; i++) {
			fMinus[i] = fMinusFactors[i] * reactionFlux[fMinusMap[i]];
		}

		// Sum the F+ and F- for each isotope. These are "sub-arrays"
		// of Fplus and Fminus at (F[+ or -] + minny) of size fPlusMaximums[i].
		int minny;
		vector<double> fPlusSum(numSpecies, 0);
		vector<double> fMinusSum(numSpecies, 0);
		for (int i = 0; i < numSpecies; i++) {
			minny = (i > 0) ? fPlusMaximums[i - 1] + 1 : 0;
			// Sum F+.
			fPlusSum[i] = 0.0;
			for (int j = minny; j <= fPlusMaximums[i]; j++) {
				fPlusSum[i] += fPlus[j];
			}
			// Sum F-.
			minny = (i > 0) ? fMinusMaximums[i - 1] + 1 : 0;
			fMinusSum[i] = 0.0;
			for (int j = minny; j <= fMinusMaximums[i]; j++) {
				fMinusSum[i] += fMinus[j];
			}
			species->at(i).flux = fPlusSum[i] - fMinusSum[i];
		}

		return;
	}

};

} /** namespace astrophysics **/
} /** namespace fire **/



#endif /* ASTROPHYSICS_REACTIONNETWORK_H_ */

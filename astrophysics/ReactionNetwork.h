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

		return;
	}

};


} /** namespace astrophysics **/
} /** namespace fire **/



#endif /* ASTROPHYSICS_REACTIONNETWORK_H_ */

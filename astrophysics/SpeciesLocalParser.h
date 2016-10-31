/*
 * SpeciesLocalParser.h
 *
 *  Created on: Oct 30, 2016
 *      Author: bkj
 */

#ifndef ASTROPHYSICS_SPECIESLOCALPARSER_H_
#define ASTROPHYSICS_SPECIESLOCALPARSER_H_

#include <vector>
#include <memory>
#include "Species.h"
#include "LocalParser.h"

using namespace std;
using namespace fire::astrophysics;

namespace fire {

/**
 * This operation parses the a network file with the species information
 * for the network.
 * @return a shared pointer
 */
template<>
void LocalParser<std::vector<Species>>::parse() {

	return;
};

} /* namespace fire */

#endif /* ASTROPHYSICS_SPECIESLOCALPARSER_H_ */

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

#ifndef ASTROPHYSICS_SPECIES_H_
#define ASTROPHYSICS_SPECIES_H_

#include <string>
#include <vector>
#include <StringCaster.h>
#include <build.h>

namespace fire {
namespace astrophysics {

/**
 * This class represents a standard nuclear species within astrophysics such as
 * Helium or Carbon. The mass fraction is the only value on this struct which
 * can be changed.
 *
 * This class is composed of const-qualified members because atomic parameters
 * are immutable. The amount of a species present in a given context varies
 * though.
 */
struct Species {

	/**
	 * The name of this species.
	 */
	const std::string name;

	/**
	 * The total number of nucleons, equal to the sum of the atomic and neutron
	 * numbers, in the nucleus of this species. Commonly denoted "A."
	 */
	const int massNumber;

	/**
	 * The total number of protons in the nucleus of this species. Also known
	 * as the proton number. Commonly denoted "Z."
	 */
	const int atomicNumber;

	/**
	 * The total number of neutrons in the nucleus of this species. Commonly
	 * denoted "N."
	 */
	const int neutronNumber;

	/**
	 * The fraction of the total mass of the system that is composed of this
	 * species. The quantity is normalized to 1.0. It is sometimes called the
	 * "abundance" as well.
	 */
	double massFraction;

	/**
	 * The difference between the actual mass of this species and the mass
	 * number.
	 */
	const double massExcess;

	/**
	 * The total flux in the species under current conditions.
	 */
	double flux;

	/**
	 * The constructor. This class is meant to be initialized from a vector of
	 * data created by reading a line from a network file. The vector should
	 * have the following entries:
	 * v[0] - The name
	 * v[1] - The mass number
	 * v[2] - The atomic number
	 * v[3] - The neutron number
	 * v[4] - The mass fraction
	 * v[5] - The mass excess
	 * @param values a vector with entries for each of the data members in this class
	 * as described above.
	 */
	Species(const std::vector<std::string> & values) :
			name(values[0]),
			massNumber(StringCaster<int>::cast (values[1])),
			atomicNumber(StringCaster<int>::cast (values[2])),
			neutronNumber(StringCaster<int>::cast (values[3])),
			massFraction(StringCaster<double>::cast (values[4])),
			massExcess(StringCaster<double>::cast (values[5])) {};

};

} /* namespace astrophysics */

} /* namespace fire */

#endif /* ASTROPHYSICS_SPECIES_H_ */

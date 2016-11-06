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
#ifndef ASTROPHYSICS_REACTION_H_
#define ASTROPHYSICS_REACTION_H_

#include <string>

namespace fire {
namespace astrophysics {

/**
 * This class represents a reaction, and specifically a nuclear reaction in the
 * astrophysical case. This includes both forward reactions and backward (decay)
 * reactions with one to four reacting bodies.
 */
class Reaction {
public:

	/**
	 * The name, or label, for the reaction. This should be of the form:
	 * "he4+he4+he4-->c12"
	 */
    std::string name;

    /**
     * The class of this reaction within its reaction group.
     */
    int reactionGroupClass;

    /**
     * The index of this reaction within the reaction group.
     */
    int reactionGroupMemberIndex;

    /**
     * The class of this reaction in the REACLIB rate library.
     */
    int reaclibClass;

    /**
     * The number of species in this reaction.
     */
    int numReactingSpecies;

    /**
     *
     */
    int numProducts;
    bool isEC;
    bool isReverse;
    double statisticalFactor;
    double energyRelease;

};


} /* namespace astrophysics */
} /* namespace fire */


#endif /* ASTROPHYSICS_REACTION_H_ */

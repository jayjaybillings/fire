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
#include <array>

namespace fire {
namespace astrophysics {

/**
 * This class represents a reaction, and specifically a nuclear reaction in the
 * astrophysical case. This includes both forward reactions and backward (decay)
 * reactions with one to four reacting bodies.
 *
 * At the moment, this struct has an extremely bad design. However, it is worth
 * noting that this is significantly better than what is currently used. There
 * are several useful optimizations that will be added in time, such as using
 * the Species class and creating a new reaction group class.
 *
 */
struct Reaction {

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
    int numReactants;

    /**
     * The number of products produced as a result of the reaction.
     */
    int numProducts;

    /**
     * This is flag that designates whether or not the reaction captures an
     * electron.
     */
    bool isElectronCapture;

    /**
     * True if this reaction is a reverse reaction.
     *
     * ASK MIKE! Why is this set to false by in FERN's loadReactions operation?
     */
    bool isReverse;

    /**
     * A statistical factor associated with this reaction that avoids double
     * counting. It also accounts for the sign needed to designate whether or
     * not the population is depleting or increasing.
     */
    double statisticalFactor;

    /**
     * The energy released by this reaction in electron volts.
     */
    double energyRelease;

    /**
     * The array of REACLIB p-coefficients used in the parameterized
     * computation of the rate. The rate is computed by
     * \f[
     * R = \sum_k R_k
     * \f]
     * where
     * \f[
     * R_k = \exp(p_1 + \frac{p_2}{T_9} + \frac{p_3}{T_9^{1/3}} + p_{4}T_9^{1/3}
     * + p_{5}T_9 + p_{6}T_9^{5/3} + p_{7}\ln T_9).
     * \f]
     *
     * \f$T_9\f$ is the the temperature in units of \f$10^9\f$ Kelvin. Note
     * that p1 = reaclibRateCoeff[0] since C++ is a zero-indexed language.
     *
     * See: "Stars and Stellar Processes", Mike Guidry, to be published Cambridge University Press.
     */
    std::array<double,7> reaclibRateCoeff;

    /**
     * The array of atomic numbers for the reactants in this reaction.
     */
    std::array<int, 4> reactantZ;

    /**
     * The array of neutron numbers for the reactants in this reaction.
     */
    std::array<int, 4> reactantN;

    /**
     * The array of atomic numbers for the products in this reaction.
     */
    std::array<int, 4> productZ;

    /**
     * The array of neutron numbers for the products in this reaction.
     */
    std::array<int, 4> productN;

    /**
     * The array of reactants to subtract from reacVector.
     *
     * No idea. ASK MIKE! This is used by the partial equilibrium code and may be useless for now.
     */
    std::array<int, 3> reactants;

    /**
     * The array of products to add to reacVector.
     *
     * No idea. ASK MIKE! This is used by the partial equilibrium code and may be useless for now.
     */
    std::array<int, 3> products;

};


} /* namespace astrophysics */
} /* namespace fire */


#endif /* ASTROPHYSICS_REACTION_H_ */

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
#include <build.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <StringCaster.h>

using namespace std;

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
    array<double,7> reaclibRateCoeff;

    /**
     * The array of atomic numbers for the reactants in this reaction.
     */
    array<int, 4> reactantZ;

    /**
     * The array of neutron numbers for the reactants in this reaction.
     */
    array<int, 4> reactantN;

    /**
     * The array of atomic numbers for the products in this reaction.
     */
    array<int, 4> productZ;

    /**
     * The array of neutron numbers for the products in this reaction.
     */
    array<int, 4> productN;

    /**
     * The array of reactants to subtract from reacVector.
     *
     * No idea. ASK MIKE! This is used by the partial equilibrium code and may be useless for now.
     */
    array<int, 3> reactants;

    /**
     * The array of products to add to reacVector.
     *
     * No idea. ASK MIKE! This is used by the partial equilibrium code and may be useless for now.
     */
    array<int, 3> products;

	/**
	 * The statistical prefactor that act as constant multipliers on this reaction.
     * \f[
     * p_s = s^(\rho (n_R -1)).
     * \f]
	 * @param the current density
	 */
    double prefactor;

    /**
     * The reaction rate as described in setRate().
     */
    double rate;

	/**
	 * The statistical prefactor that act as constant multipliers on this reaction.
     * \f[
     * p_s = s^(\rho (n_R -1)).
     * \f]
	 * @param the current density
	 */
	void setPrefactor(const double & rho) {
		prefactor = statisticalFactor*pow(rho,(numReactants-1));
	}

	/**
	 * This operation computes and sets the reaction rate. This version is optimized
	 * to use pre-computed temperature values so that the costly exponentiation does
	 * not need to be repeated for each reaction if the temperature doesn't change.
	 * @param An array of all six temperature values used to compute the rate.
	 *
	 * \f[
     * R_k = \exp(p_1 + \frac{p_2}{T_9} + \frac{p_3}{T_9^{1/3}} + p_{4}T_9^{1/3}
     * + p_{5}T_9 + p_{6}T_9^{5/3} + p_{7}\ln T_9).
     * \f]
     *
     * \f$T_9\f$ is the the temperature in units of \f$10^9\f$ Kelvin. Note
     * that p1 = reaclibRateCoeff[0] since C++ is a zero-indexed language.
	 */
	void setRate(array<double,6> tempValues) {
		// Compute the exponent
		double x = reaclibRateCoeff[0] + tempValues[0] * reaclibRateCoeff[1]
				+ tempValues[1] * reaclibRateCoeff[2] + tempValues[2] * reaclibRateCoeff[3]
				+ tempValues[3] * reaclibRateCoeff[4] + tempValues[4] * reaclibRateCoeff[5]
				+ tempValues[5] * reaclibRateCoeff[6];
		// Compute the rate
		rate = prefactor * exp(x);
	}

	/**
	 * This operation computes and sets the reaction rate. This version will use
	 * the provided temperature to compute all of the temperature coefficients.
	 * See the other version of this function for a more efficient version (which
	 * this function actually calls).
	 * @param the temperature
	 *
	 * \f[
     * R_k = \exp(p_1 + \frac{p_2}{T_9} + \frac{p_3}{T_9^{1/3}} + p_{4}T_9^{1/3}
     * + p_{5}T_9 + p_{6}T_9^{5/3} + p_{7}\ln T_9).
     * \f]
     *
     * \f$T_9\f$ is the the temperature in units of \f$10^9\f$ Kelvin. Note
     * that p1 = reaclibRateCoeff[0] since C++ is a zero-indexed language.
	 */
	void setRate(const double & temp) {
		// Compute the temperatures
		array<double,6> tempValues;
		double cbrtT = cbrt(temp); // Cube root of T
		tempValues[0] = 1 / temp; // 1/T
		tempValues[1] = 1 / cbrtT;
		tempValues[2] = cbrtT;
		tempValues[3] = temp;
		tempValues[4] = cbrtT * cbrtT * cbrtT * cbrtT * cbrtT; // T^(5/3)
		tempValues[5] = log(temp);

		// Delegate the actual computation
		setRate(tempValues);
	}

};

} /* namespace astrophysics */

/**
 * This is a builder for constructing reactions from a vector of strings. This is meant
 * to work with data parsed from the legacy ASCII format.
 * @param The input vector with size 8.
 * @return the fully initialized Reaction.
 */
template<>
astrophysics::Reaction build(const vector<string> & lines) {
	ostringstream error;
	astrophysics::Reaction reaction;

	if (lines.size() == 8) {
		// Line 1 - Basic Reaction Metadata
		vector<string> lineVec = splitString(lines[0]);
		if (lineVec.size() == 10) {
			reaction.name = lineVec[0];
			reaction.reactionGroupClass = StringCaster<int>::cast(lineVec[1]);
			reaction.reactionGroupMemberIndex = StringCaster<int>::cast(
					lineVec[2]);
			reaction.reaclibClass = StringCaster<int>::cast(lineVec[3]);
			reaction.numReactants = StringCaster<int>::cast(lineVec[4]);
			reaction.numProducts = StringCaster<int>::cast(lineVec[5]);
			reaction.isElectronCapture = StringCaster<bool>::cast(lineVec[6]);
			reaction.isReverse = StringCaster<bool>::cast(lineVec[7]);
			reaction.statisticalFactor = StringCaster<double>::cast(lineVec[8]);
			reaction.energyRelease = StringCaster<double>::cast(lineVec[9]);
		} else {
			error
					<< "Invalid first line for reaction in file! Missing an element? ";
			error << "Check line beginning with ' " << lines[0] << "'.";
			throw runtime_error(error.str());
		}
		// Line 2 - Reaclib Coefficients
		lineVec = splitString(lines[1]);
		if (lineVec.size() == 7) {
			reaction.reaclibRateCoeff[0] = StringCaster<double>::cast(
					lineVec[0]);
			reaction.reaclibRateCoeff[1] = StringCaster<double>::cast(
					lineVec[1]);
			reaction.reaclibRateCoeff[2] = StringCaster<double>::cast(
					lineVec[2]);
			reaction.reaclibRateCoeff[3] = StringCaster<double>::cast(
					lineVec[3]);
			reaction.reaclibRateCoeff[4] = StringCaster<double>::cast(
					lineVec[4]);
			reaction.reaclibRateCoeff[5] = StringCaster<double>::cast(
					lineVec[5]);
			reaction.reaclibRateCoeff[6] = StringCaster<double>::cast(
					lineVec[6]);
		} else {
			error << "Invalid number of reaclib rate coefficients in file! ";
			error << "Check coefficients for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 3 - Reactant Z values
		lineVec = splitString(lines[2]);
		if (lineVec.size() > 0 && lineVec.size() < 4) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.reactantZ[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of reactant atomic numbers in file! ";
			error << "Check values for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 4 - Reactant N values
		lineVec = splitString(lines[3]);
		if (lineVec.size() > 0 && lineVec.size() <= 4) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.reactantN[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of reactant neutron numbers in file! ";
			error << "Check values for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 5 - Product Z values
		lineVec = splitString(lines[4]);
		if (lineVec.size() > 0 && lineVec.size() <= 4) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.productZ[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of product atomic numbers in file! ";
			error << "Check values for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 6 - Product N Values
		lineVec = splitString(lines[5]);
		if (lineVec.size() > 0 && lineVec.size() <= 4) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.productN[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of product neutron numbers in file! ";
			error << "Check values for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 7 - Partial Equilibrium data
		lineVec = splitString(lines[6]);
		if (lineVec.size() > 0 && lineVec.size() <= 3) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.reactants[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of PE reactants in file! ";
			error << "Check values in line 7 for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
		// Line 8 - Partial Equilibrium data
		lineVec = splitString(lines[7]);
		if (lineVec.size() > 0 && lineVec.size() <= 3) {
			// We can't unroll this because we don't know how many values are
			// available.
			for (int j = 0; j < lineVec.size(); j++) {
				reaction.products[j] = StringCaster<int>::cast(lineVec[j]);
			}
		} else {
			error << "Invalid number of PE products in file! ";
			error << "Check values in line 8 for " << reaction.name << ".";
			throw runtime_error(error.str());
		}
	} else {
		ostringstream error;
		error << "This function cannot build from a vector of size != 8. ";
		error << "Did you read your reaction data incorrectly?";
		throw runtime_error(error.str());
	}

	return reaction;
}

} /* namespace fire */


#endif /* ASTROPHYSICS_REACTION_H_ */

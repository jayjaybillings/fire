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
#ifndef FEM_LAPLACECSTELEMENT_H_
#define FEM_LAPLACECSTELEMENT_H_

#include <functional>
#include <array>
#include <ConstantStrainTriangleElement.h>

namespace fire {

/**
 * This is a ConstantStrainTriangleElement for Laplace's Equation
 * \f[
 * \kappa(\bar{x})\nabla^{2}u(\bar{x}) = 0
 * \f]
 * where \f$\kappa(\bar{x})\f$ is the spatially-dependent transfer coefficient.
 * (See transferCoefficient() for details on how to specify it.)
 *
 * The contributions to the stiffness matrix for for Laplace's equation are
 * given by
 * \f[
 * k_{ij} = \iint_{A} \frac{\kappa(\bar{x})}{4A^{2}} (b_{i}b_{j} + c_{i}c_{j}) dx dy
 * \f]
 * If k is constant, then this can be simplified further:
 * \f[
 * k_{ij} = \frac{\kappa}{4A} (b_{i}b_{j} + c_{i}c_{j})
 * \f]
 */
class LaplaceCSTElement: public ConstantStrainTriangleElement {
protected:

	/**
	 * A function to compute the value of the Laplace constant k. By default,
	 * this function will always return 1.0. It can be configured by clients
	 * with the transferCoefficient() function.
	 */
	std::function<double(const std::array<double,3> &,
			const int &, const int &)> kFunction;

public:

	/**
	 * The constructor
	 * @param node1 the first node in the element
	 * @param node2 the second node in the element
	 * @param node3 the third node in the element
	 */
	LaplaceCSTElement(const TwoDNode & node1,
			const TwoDNode & node2, const TwoDNode & node3);

	/**
	 * This operations sets the function that should be used for computing the
	 * transfer coefficient.
	 * @param function the transfer coefficient function which should depend on
	 * three local coordinates (area coordinates) and the (i,j) element
	 * indices.
	 */
	void transferCoefficient(const std::function<double(const std::array<double,3> &,
			const int &, const int &)> & function);

	/**
	 * This operations returns a reference to the function that is used for
	 * computing the transfer coefficient.
	 * @param function the transfer coefficient function which should depend on
	 * three local coordinates (area coordinates) and the (i,j) element
	 * indices.
	 */
	const std::function<double(const std::array<double,3> &,
				const int &, const int &)> & transferCoefficient() const;

};

} /* namespace fire */

#endif /* FEM_LaplaceCSTElement_H_ */

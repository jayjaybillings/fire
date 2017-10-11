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
#ifndef FEM_CONSTANTSTRAINTRIANGLEELEMENT_H_
#define FEM_CONSTANTSTRAINTRIANGLEELEMENT_H_

#include <array>
#include <vector>
#include <Node.h>

namespace fire {

/**
 * This class represents a triangular element where the strain or similar
 * quantities across the element is constant, a "Constant Strain Triangle"
 * (CST). Full mathematical documentation of this class is available in the
 * Fire Documentation article "Finite Elements" and can be found in the source
 * tree under
 * docs/_posts/2017-10-05-finite-elements.markdown.
 *
 * Instead of storing the a_i, b_i_, and c_i constants with the nodes, this
 * class computes the constants once on construction and passes the values
 * using lambdas. This greatly decreases the book keeping required and
 * increases the compute performance.
 */
class ConstantStrainTriangleElement {

private:
	/**
	 * The area of the element.
	 */
	double area;

	/**
	 * The a_1 constant for the triangle
	 */
	double a1;

	/**
	 * The a_2 constant for the triangle
	 */
	double a2;

	/**
	 * The a_3 constant for the triangle
	 */
	double a3;

	/**
	 * The b_1 constant for the triangle
	 */
	double b1;

	/**
	 * The b_2 constant for the triangle
	 */
	double b2;

	/**
	 * The b_3 constant for the triangle
	 */
	double b3;

	/**
	 * The c_1 constant for the triangle
	 */
	double c1;

	/**
	 * The c_2 constant for the triangle
	 */
	double c2;

	/**
	 * The c_3 constant for the triangle
	 */
	double c3;

	/**
	 * The nodes in the element
	 */
	std::array<TwoDNode,3> nodes;

public:

	/**
	 * This operation returns the contributions, k_ij, made by this element to
	 * the global stiffness matrix.
	 */
	std::vector<double> stiffnessMatrix() {

	}

};

} // end namespace fire


#endif /* FEM_CONSTANTSTRAINTRIANGLEELEMENT_H_ */

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
#include <functional>
#include <FEMTypes.h>
#include <TriangularQuadratureRule.h>

namespace fire {

/**
 * This class represents a triangular element where the strain or similar
 * quantities across the element is constant, a "Constant Strain Triangle"
 * (CST). Full mathematical documentation of this class is available in the
 * Fire Documentation article "Finite Elements" and can be found in the source
 * tree under docs/_posts/2017-10-05-finite-elements.markdown.
 *
 * Instead of storing the a_i, b_i_, and c_i constants with the nodes, this
 * class computes the constants once on construction and passes the values
 * using lambdas. This greatly decreases the book keeping required and
 * increases the compute performance.
 *
 * The API for this class is incredibly specific with all bounds specified
 * exactly because dynamic memory is not required since the element has such
 * a well-defined mathematical description.
 *
 * Clients are expected to use this class by subclassing and configuring their
 * kernel and force functions in the subclass' constructor. Stiffness kernels
 * have the following signature:
 * @code
 * double f(const std::array<double,3> & coords, const int & i,
 * const int & j);
 * @endcode
 * where the array of doubles is the area coordinates (L_1, L_2, L_3) of the
 * quadrature point and the following integers are used to identify the matrix
 * indices i and j for the current element. The body force kernels have the
 * following signature:
 * @code
 * double f(const std::array<double,3> & coords, const int & i);
 * @endcode
 * In this case, only one identifier is required to specify the vector element
 * index i of the kernel. PoissonCST provides a good example of an
 * appropriately configured subclass. The default implementations of
 * stiffnessKernel and bodyForceKernel evaluate to -1.0 for all values of (x,y)
 * input and will throw an exception if called.
 *
 * Node ordering is not determined by the element. Instead, it assumes that the
 * nodes are passed to it in the correct order and locally identifies the first
 * node in the constructor as node 1, the second as node 2, and third as node 3.
 * Boundaries can then be set on the element by calling addBoundary() with
 * local side ids. A triangle cannot have more than three boundaries.
 *
 */
class ConstantStrainTriangleElement {

protected:
	/**
	 * The area of the element.
	 */
	double eArea = 0.0;

	/**
	 * The array of constants a_1, a_2, a_3 for the area coordinates.
	 */
	std::array<double,3> a;

	/**
	 * The array of constants b_1, b_2, b_3 for the area coordinates.
	 */
	std::array<double,3> b;

	/**
	 * The array of constants c_1, c_2, c_3 for the area coordinates.
	 */
	std::array<double,3> c;

	/**
	 * The nodes in the element.
	 *
	 * Note that this is initialized on construction because the array elements
	 * are references.
	 */
	std::array<std::reference_wrapper<const TwoDNode>,3> nodes;

	/**
	 * The matrix elements k_ij computed for this element.
	 */
	std::array<MatrixElement<double>,9> kIJElements;

	/**
	 * The body force vector elements f_i computed for this element.
	 */
	std::array<VectorElement<double>,3> bodyElements;

	/**
	 * A function handle to the kernel that will be evaluated in the
	 * computation of the stiffness matrix for this element.
	 */
	std::function<double(const std::array<double,3> &,
			const int &, const int &)> stiffnessKernel;

	/**
	 * A function handle to the kernel that will be evaluated in the
	 * computation of the body forces for this element.
	 */
	std::function<double(const std::array<double,3> &,
			const int &)> bodyForceKernel;

	/**
	 * This is a four point Gaussian Quadrature rule over triangular
	 * coordinates that is used to integrate the kernels. It should be more
	 * efficient to drop to a one point rule since this element has a constant
	 * value across it, but we can test that later.
	 */
	TriangularQuadratureRule triQuadRule;

	/**
	 * This is the number of active boundaries on the triangle.
	 */
	short int numBoundaries = 0;

	/**
	 * This array stores the active boundaries on the triangle.
	 */
	std::array<BasicPair<short int, short int>, 3> boundaries;

public:

	/**
	 * The identification number of the element. Defaults to -1 since there
	 * should be no negatively numbered elements.
	 */
	long elementID = -1;

	/**
	 * The constructor
	 */
	ConstantStrainTriangleElement(const TwoDNode & node1,
			const TwoDNode & node2, const TwoDNode & node3);

	/**
	 * This operation returns the contributions, k_ij, made by this element to
	 * the global stiffness matrix.
	 * @return An array of 9 matrix elements, each containing a single
	 * contribution k_ij to the global stiffness matrix.
	 */
	const std::array<MatrixElement<double>,9> & stiffnessMatrix();

	/**
	 * This operation returns the contributions, f_i, made by this element to
	 * the global body force vector.
	 * @return An array of 3 vector elements, each containing a single
	 * contribution f_i to the global body force vector..
	 */
	std::array<VectorElement<double>,3> & bodyForceVector();

	/**
	 * This operation directs the element to recompute its constants. That is,
	 * it notifies the element that nodal information has changed and the a,
	 * b, c, and area constants should be recomputed.
	 */
	void recomputeConstants();

	/**
	 * This operation returns the area of the element.
	 * @return the area of the element as computed by recomputeConstants().
	 */
	double area();

	/**
	 * This operation computes the local coordinates of a global point (x,y).
	 * Note that the point (x,y) should be in the element!
	 * @param the global coordinate x that lands within the element.
	 * @param the global coordinate y that lands within the element.
	 * @return the set of local (area) coordinates (L_1,L_2,L_3). The value of
	 * the identifiable/tag element (CSTLocalPoint.value) is not used.
	 */
	CSTLocalPoint computeLocalPoint(const double & x, const double & y) const;

	/**
	 * This operation will designate the edge between two nodes with the given
	 * local ids as a boundary for which boundary value contributions to the
	 * stiffness and force matrices should be computed. A triangle cannot have
	 * more than three boundaries and this function will throw an exception if
	 * it is called to set a fourth boundary. Likewise, this function will also
	 * throw an exception if the two boundary node ids are the same (no self-
	 * boundary loops).
	 * @param the local node id (1, 2, or 3) of the first node on the edge
	 * @param the local node id (1, 2, or 3, but not the same as firstNodeId),
	 * of the second node on the edge.
	 */
	void addBoundary(const int & firstNodeId, const int & secondNodeId);

};

} // end namespace fire


#endif /* FEM_CONSTANTSTRAINTRIANGLEELEMENT_H_ */

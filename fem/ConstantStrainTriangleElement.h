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
#include <LineQuadratureRule.h>
#include <TwoDRobinBoundaryCondition.h>
#include <TwoDDirichletBoundaryCondition.h>

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
 * index i of the kernel. LaplaceCST provides a good example of an
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
 * Adding Dirichlet Boundary Conditions will cause the element to deflate its
 * stiffness matrix and force vector by deleting unneeded matrix and vector
 * elements, which greatly increases performance. Since this is a destructive
 * process, Robin Boundary Conditions should always be added before Dirichlet
 * Boundary Conditions. Cross-coupled elements of the stiffness matrix are
 * subtracted from both sides (zeroed on the left, subtracted on the right)
 * and scaled by the value of unknown due to the Dirichlet condition.
 */
class ConstantStrainTriangleElement {

protected:

	/**
	 * The number of nodes in the element.
	 */
	constexpr static const int numNodes = 3;

	/**
	 * The square of the number of nodes in the element.
	 */
	constexpr static int numElements = numNodes*numNodes;

	/**
	 * The maximum number of Dirichlet Boundary Conditions allowed for the
	 * element. This is a triangle, so if you need 3 Dirichlet Boundary
	 * Conditions, then you don't need this code!
	 */
	constexpr static int maxNumDBCs = 2;

	/**
	 * The area of the element.
	 */
	double eArea = 0.0;

	/**
	 * The array of constants a_1, a_2, a_3 for the area coordinates.
	 */
	std::array<double,numNodes> a;

	/**
	 * The array of constants b_1, b_2, b_3 for the area coordinates.
	 */
	std::array<double,numNodes> b;

	/**
	 * The array of constants c_1, c_2, c_3 for the area coordinates.
	 */
	std::array<double,numNodes> c;

	/**
	 * The nodes in the element.
	 *
	 * Note that this wraps references because the element does not own the
	 * nodes, only uses them.
	 */
	std::array<std::reference_wrapper<const TwoDNode>,numNodes> nodes;

	/**
	 * The matrix elements k_ij computed for this element.
	 */
	std::vector<MatrixElement<double>> kIJElements;

	/**
	 * The body force vector elements f_i computed for this element.
	 */
	std::vector<VectorElement<double>> bodyElements;

	/**
	 * This vector stores stiffness matrix elements that are removed due to
	 * deflation from Dirichlet Boundary Conditions that must be added to the
	 * right hand side.
	 */
	std::vector<IdentifiableTriplet<int,int,int,int>> forceVectorUpdateElements;

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
	 * This is a four point Gaussian Quadrature rule over a line that is used
	 * to integrate the boundary condition kernels. It should be more
	 * efficient to drop to a two point rule since this element's boundaries
	 * transform into quadratic functions in the bounds change, but we can test
	 * that later.
	 */
	LineQuadratureRule lineQuadRule;

	/**
	 * This is the number of active Robin boundary conditions on the triangle.
	 */
	short int numRobinBoundaries = 0;

	/**
	 * This array stores the active Robin boundary conditions on the triangle.
	 */
	std::vector<std::reference_wrapper<const TwoDRobinBoundaryCondition>> robinBoundaries;

	/**
	 * This stores a small amount of information about the boundary conditions
	 * so that sparsity can be maximally leveraged (at the expense of some
	 * memory, yes) for the stiffness matrix.
	 */
	std::vector<RobinBoundaryMatrixContribution> robinBoundaryContributions;

	/**
	 * This stores boundary condition contributions that have been removed from
	 * the stiffness matrix and need to be added to force vector because of
	 * deflation from a Dirichlet Boundary Condition.
	 */
	std::vector<RobinBoundaryMatrixContribution> robinBoundaryForceContributions;

	/**
	 * This is a pair that stores a small amount of information about the
	 * boundary conditions so that sparsity can maximally leverage (at the
	 * expense of some memory, yes) for the force vector.
	 */
	std::vector<IdentifiableTriplet<int,int,int,std::function<double(const double &,
			const int &)>>> robinBoundaryForces;


	/**
	 * This array stores the active Diriclet boundary conditions on the
	 * triangle.
	 */
	std::vector<std::reference_wrapper<const TwoDDirichletBoundaryCondition>> dirichletBoundaries;

	/**
	 * This function computes the contribution to the stiffness matrix at (j,j)
	 * element due to a boundary between nodes (i,j) that has a Robin Boundary
	 * Condition. It computes
	 * \f[
	 * \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}L_{j}
	 * \f]
	 * which is integrated as
	 * \f[
	 * \int_{0}^{1} \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}L_{j} dL_{j}
	 * \f]
	 * where \f$b_{k}\f$ and \f$c_{k}^2\f$ are the b and c constants for node
	 * k, which is not on the boundary.
	 *
	 * Note that the bounds of integration are shifted in the code to cover
	 * [-1,1] and better fit standard 1D Gaussian Quadrature routines such as
	 * the one implemented by LineQuadratureRule. The bound shift is
	 * accomplished using u-substitution of
	 * \f[
	 * x = 2L_{i} - 1
	 * \f]
	 */
	std::function<double(const double &, const int &)> jjNodeRobinBoundaryStiffnessFunction;

	/**
	 * This function computes the contribution to the stiffness matrix at the
	 * (i,j) and (j,i) elements due to a boundary between nodes (i,j) that has
	 * a Robin Boundary Condition. It computes
	 * \f[
	 * \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}(L_{j}-L_{j}^{2})
	 * \f]
	 * which is integrated as
	 * \f[
	 * \int_{0}^{1} \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}(L_{j}-L_{j}^{2}) dL_{j}
	 * \f]
	 * where \f$b_{k}\f$ and \f$c_{k}^2\f$ are the b and c constants for node
	 * k, which is not on the boundary.
	 *
	 * Note that the bounds of integration are shifted in the code to cover
	 * [-1,1] and better fit standard 1D Gaussian Quadrature routines such as
	 * the one implemented by LineQuadratureRule. The bound shift is
	 * accomplished using u-substitution of
	 * \f[
	 * x = 2L_{i} - 1
	 * \f]
	 */
	std::function<double(const double &, const int &)> jiNodeRobinBoundaryStiffnessFunction;

	/**
	 * This function computes the contribution to the stiffness matrix at (j,j)
	 * element due to a boundary between nodes (i,j) that has a Robin Boundary
	 * Condition. It computes
	 * \f[
	 * \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}(1-L_{j})^{2}
	 * \f]
	 * which is integrated as
	 * \f[
	 * \int_{0}^{1} \sigma(L_{j})(b_{k}^2 + c_{k}^2)^{1/2}(1-L_{j})^{2} dL_{j}
	 * \f]
	 * where \f$b_{k}\f$ and \f$c_{k}^2\f$ are the b and c constants for node
	 * k, which is not on the boundary.
	 *
	 * Note that the bounds of integration are shifted in the code to cover
	 * [-1,1] and better fit standard 1D Gaussian Quadrature routines such as
	 * the one implemented by LineQuadratureRule. The bound shift is
	 * accomplished using u-substitution of
	 * \f[
	 * x = 2L_{i} - 1
	 * \f]
	 */
	std::function<double(const double &, const int &)> iiNodeRobinBoundaryStiffnessFunction;

	/**
	 * This function computes the contribution to the force vector at the first
	 * node of a boundary due to a Robin Boundary Condition. It computes
	 * \f[
	 * h(L_{i})(1-L_{i})
	 * \f]
	 * which is integrated as
	 * \f[
	 * \int_{0}^{1} h(L_{i})(1-L_{i}) dL_{i}
	 * \f]
	 *
	 * Note that the bounds of integration are shifted in the code to cover
	 * [-1,1] and better fit standard 1D Gaussian Quadrature routines such as
	 * the one implemented by LineQuadratureRule. The bound shift is
	 * accomplished using u-substitution of
	 * \f[
	 * x = 2L_{i} - 1
	 * \f]
	 */
	std::function<double(const double &, const int &)> firstNodeRobinBoundaryForceFunction;

	/**
	 * This function computes the contribution to the force vector at the second
	 * node of a boundary due to a Robin Boundary Condition. It computes
	 * \f[
	 *  h(L_{i})L_{i}
	 * \f]
	 * which is integrated as
	 * \f[
	 * \int_{0}^{1} h(L_{i})L_{i}dL_{i}
	 * \f]
	 *
	 * Note that the bounds of integration are shifted in the code to cover
	 * [-1,1] and better fit standard 1D Gaussian Quadrature routines such as
	 * the one implemented by LineQuadratureRule. The bound shift is
	 * accomplished using u-substitution of
	 * \f[
	 * x = 2L_{i} - 1
	 * \f]
	 */
	std::function<double(const double &, const int &)> secondNodeRobinBoundaryForceFunction;

	/**
	 * This function checks a 2D boundary to see if it was previously modified
	 * or configured in this element. It specifically checks that the boundary:
	 * 1) is not a loop where the same node is the start and end of the
	 * boundary.
	 * 2) has nodes that are in the element.
	 * @param boundaryCond the boundary condition to check
	 * @return true if the boundary is already configured on the element, false
	 * if not
	 */
    bool checkBoundary(const NodePair<TwoDNode> & boundary) const;

    /**
     * FIXME! Add docs when finished with it
     */
    void computeIds();

    /**
     * This operation computes the value of the stiffness matrix without
     * consideration of Robin Boundary Conditions at the given indices.
     * @param i the first index
     * @param j the second index
     * @return the result
     */
    double getStiffnessElement(const int & i, const int & j);

public:

	/**
	 * The identification number of the element. Defaults to -1 since there
	 * should be no negatively numbered elements.
	 */
	long elementID = -1;

	/**
	 * The constructor
	 * @param node1 the first node in the element
	 * @param node2 the second node in the element
	 * @param node3 the third node in the element
	 */
	ConstantStrainTriangleElement(const TwoDNode & node1,
			const TwoDNode & node2, const TwoDNode & node3);

	/**
	 * This operation returns the contributions, k_ij, made by this element to
	 * the global stiffness matrix.
	 * @return An array of 9 matrix elements, each containing a single
	 * contribution k_ij to the global stiffness matrix.
	 */
	const std::vector<MatrixElement<double>> & stiffnessMatrix();

	/**
	 * This operation returns the contributions, f_i, made by this element to
	 * the global body force vector.
	 * @return An array of 3 vector elements, each containing a single
	 * contribution f_i to the global body force vector..
	 */
	const std::vector	<VectorElement<double>> & bodyForceVector();

	/**
	 * This operation directs the element to recompute its constants. That is,
	 * it notifies the element that nodal information has changed and the a,
	 * b, c, and area constants should be recomputed.
	 */
	void recomputeConstants();

	/**
	 * This operation returns the area of the element. Note that this operation
	 * does not compute the area of the element. The area should be constant
	 * unless the coordinates of the nodes change, so the area is computed once
	 * during construction and must be recomputed by calling
	 * recomputeConstants() if the nodes change.
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
	 * This operation checks to see if the element contains the given node.
	 * @param node the node to check for containment within the element
	 * @return true if the node is in the element, false otherwise.
	 */
	bool hasNode(const TwoDNode & node) const;

	/**
	 * This operation returns the local id for the node in question if and
	 * only if it is a member of the element.
	 * @param node the node to check for inclusion
	 * @return 0, 1, or 2 if the element contains the node, -1 otherwise.
	 */
	int getLocalNodeId(const TwoDNode & node) const;

	/**
	 * This operation returns the local id for the node in question if and
	 * only if it is a member of the element.
	 * @param nodeId the nodeId to check for inclusion
	 * @return 0, 1, or 2 if the element contains the node, -1 otherwise.
	 */
	int getLocalNodeId(const int & nodeId) const;

	/**
	 * This operation returns the id of the node in this element that IS NOT on
	 * the boundary or -1 if the boundary is not on this element.
	 */
	int getOffBoundaryLocalNodeId(const TwoDRobinBoundaryCondition & boundary) const;

	/**
	 * This operation will assign the supplied TwoDRobinBoundaryCondition to
	 * the element. A triangle cannot have more than three boundaries and this
	 * function will throw an exception if it is called to set a fourth
	 * boundary. Likewise, this function will also throw an exception if the
	 * two boundary node ids are the same (no self- boundary loops) or if the
	 * boundary condition has been added previously.
	 * @param boundary the 2D Robin Boundary Condition
	 */
	void addRobinBoundary(const TwoDRobinBoundaryCondition & boundary);

	/**
	 * This operation will assign the supplied TwoDDirichletBoundaryCondition to
	 * the element. A triangle cannot have more than three boundaries and this
	 * function will throw an exception if it is called to set a fourth
	 * boundary. Likewise, this function will also throw an exception if the
	 * two boundary node ids are the same (no self-boundary loops) or if the
	 * boundary condition has been added previously.
	 * @param boundary the 2D Dirichlet Boundary Condition
	 */
	void addDirichletBoundary(const TwoDDirichletBoundaryCondition & boundary);

};

} // end namespace fire


#endif /* FEM_CONSTANTSTRAINTRIANGLEELEMENT_H_ */

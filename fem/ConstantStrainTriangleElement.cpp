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

// See the header file for API documentation

#include <ConstantStrainTriangleElement.h>
#include <stdexcept>
#include <iostream>

using namespace std;

namespace fire {

ConstantStrainTriangleElement::ConstantStrainTriangleElement(const TwoDNode & node1,
		const TwoDNode & node2, const TwoDNode & node3) :
		nodes { { std::cref(node1), std::cref(node2), std::cref(node3) } } {

			// Initialize the kernel functions to something obviously wrong.
	stiffnessKernel = [](const std::array<double,3> & coords, const int & i,
			const int & j) {
		throw runtime_error("Stiffness kernel not implemented for element!");
		return -1.0;};
	bodyForceKernel = [](const std::array<double,3> & coords, const int & i) {
		throw runtime_error("Body force kernel not implemented for element!");
		return -1.0;};

	// Get node ids
	auto & node1Id = node1.value, & node2Id = node2.value,
			& node3Id = node3.value;

	// Set the element ids. This array is zero-indexed, btw! Start at k_00.
	kIJElements[0].first = node1Id;
	kIJElements[0].second = node1Id;
	// k_01
	kIJElements[1].first = node1Id;
	kIJElements[1].second = node2Id;
	// k_02
	kIJElements[2].first = node1Id;
	kIJElements[2].second = node3Id;
	// k_10
	kIJElements[3].first = node2Id;
	kIJElements[3].second = node1Id;
	// k_11
	kIJElements[4].first = node2Id;
	kIJElements[4].second = node2Id;
	// k_12
	kIJElements[5].first = node2Id;
	kIJElements[5].second = node3Id;
	// k_20
	kIJElements[6].first = node3Id;
	kIJElements[6].second = node1Id;
	// k_21
	kIJElements[7].first = node3Id;
	kIJElements[7].second = node2Id;
	// k_22
	kIJElements[8].first = node3Id;
	kIJElements[8].second = node3Id;

	// Set the force vector element ids
	bodyElements[0].first = node1Id;
	bodyElements[1].first = node2Id;
	bodyElements[2].first = node3Id;

	// Compute the a, b, c and area constants for the element
	recomputeConstants();

};

void ConstantStrainTriangleElement::recomputeConstants() {

	// Grab the references - makes the calculations so much prettier!
	TwoDNode node1 = nodes[0].get(), node2 = nodes[1].get(),
			node3 = nodes[2].get();

	// Compute a coefficients: x_2*y_3 - x_3*y_2
	a[0] = node2.first*node3.second - node3.first*node2.second;
	// x_3*y_1 - x_1*y_3
	a[1] = node3.first*node1.second - node1.first*node3.second;
	// x_1*y_2 - x_2*y_1
	a[2] = node1.first*node2.second - node2.first*node1.second;

	// Compute b coefficients: y_2 - y_3
	b[0] = node2.second - node3.second;
	// y_3 - y_1
	b[1] = node3.second - node1.second;
    // y_1 - y_2
	b[2] = node1.second - node2.second;

	// Compute c coefficients: x_3 - x_2
	c[0] = node3.first - node2.first;
	// x_1 - x_3
	c[1] = node1.first - node3.first;
	// x_2 - x_1
	c[2] = node2.first - node1.first;

	// Compute area
	eArea = a[0] + node1.first*b[0] + node1.second*c[0];

	return;
}

const std::array<MatrixElement<double>,9> & ConstantStrainTriangleElement::stiffnessMatrix() {

	// For each node paired with all other nodes, compute k_ij.

	// Node 1 with itself
	kIJElements[0].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[0].first, kIJElements[0].second);
	// Node 1 with node 2
	kIJElements[1].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[1].first, kIJElements[1].second);
	// Node 1 with node 3
	kIJElements[2].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[2].first, kIJElements[2].second);
	// Node 2 with node 1 - may be symmetric with 1,2, but no way to know if the kernel is
	// symmetric a priori.
	kIJElements[3].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[3].first, kIJElements[3].second);
	// Node 2 with itself
	kIJElements[4].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[4].first, kIJElements[4].second);
	// Node 2 with node3
	kIJElements[5].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[5].first, kIJElements[5].second);
	// Node 3 with node 1 - may be the same as 1,3
	kIJElements[6].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[6].first, kIJElements[6].second);
	// Node 3 with node 2 - may be the same as 2,3
	kIJElements[7].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[7].first, kIJElements[7].second);
	// Node 3 with itself
	kIJElements[8].value = triQuadRule.integrate(stiffnessKernel,
			kIJElements[8].first, kIJElements[8].second);

	// Boundary conditions? - FIXME!

	return kIJElements;
}

std::array<VectorElement<double>,3> & ConstantStrainTriangleElement::bodyForceVector() {

	// FIXME! - Needs to use quadrature

	std::array<double,3> qp = {0.0,0.0,0.0};

	// For each node, compute body forces f_i.
	bodyElements[0].second = triQuadRule.integrate(bodyForceKernel,
			bodyElements[0].first);
	bodyElements[1].second = triQuadRule.integrate(bodyForceKernel,
			bodyElements[1].first);
	bodyElements[2].second = triQuadRule.integrate(bodyForceKernel,
			bodyElements[2].first);

	// Boundary conditions? - FIXME!

    return bodyElements;
}

double ConstantStrainTriangleElement::area() {
	return eArea;
}

CSTLocalPoint ConstantStrainTriangleElement::computeLocalPoint(const double & x,
		const double & y) const {
	CSTLocalPoint localPoint;

	// L_1
	localPoint.first = (a[0] + b[0]*x + c[0]*y)/(2.0*eArea);
	// L_2
	localPoint.second = (a[1] + b[1]*x + c[1]*y)/(2.0*eArea);
	// L_3
	localPoint.third = (a[2] + b[2]*x + c[2]*y)/(2.0*eArea);

	return localPoint;
}


void ConstantStrainTriangleElement::addBoundary(const int & firstNodeId, const int & secondNodeId) {

	bool hasBoundary = false;

	// The node ids must be different and only three boundaries can be added.
	if (firstNodeId != secondNodeId && numBoundaries < 3) {
		// Side 3,1
		if ((firstNodeId > 0 && firstNodeId <= 3) &&
				(secondNodeId > 0 && secondNodeId <= 3)) {
			// Check to see if the boundary is already configured
			hasBoundary = (boundaries[0].first == firstNodeId
					&& boundaries[0].second == secondNodeId)
					|| (boundaries[0].first == secondNodeId
					&& boundaries[0].second == firstNodeId) // End side 1 check
					|| (boundaries[1].first == firstNodeId
					&& boundaries[1].second == secondNodeId)
					|| (boundaries[1].first == secondNodeId
					&& boundaries[1].second == firstNodeId) // End side 2 check
					|| (boundaries[2].first == firstNodeId
					&& boundaries[2].second == secondNodeId)
					|| (boundaries[2].first == secondNodeId
					&& boundaries[2].second == firstNodeId); // End side 3 check
			// If is genuinely new, then add it
			if (!hasBoundary) {
			   boundaries[numBoundaries].first = firstNodeId;
			   boundaries[numBoundaries].second = secondNodeId;
			   numBoundaries++;
			} else {
				// Otherwise complain
				throw runtime_error("Boundary value already configured.");
			}
		} else {
			// Some other side that cannot exist.
			throw runtime_error("Invalid boundary! Valid ids are {1,2,3}.");
		}
	} else {
		// Either self-boundary reference or too many boundaries.
		throw runtime_error("Invalid boundary configuration or two many boundaries.");
	}


	return;
}

} /* end namespace fire */

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
	int numElements = numNodes*numNodes;

	// There are exactly numNodes*numNodes matrix elements, each one being
	// compared to itself and its neighbors. In this case, it progresses
	// through the list as {node1,node1},{node1,node2}, {node1,node3}, etc.
	for (int i = 0; i < numElements; i++) {
		kIJElements[i].value = triQuadRule.integrate(stiffnessKernel,
				kIJElements[i].first, kIJElements[i].second);
	}

	// Boundary conditions? - FIXME!

	return kIJElements;
}

std::array<VectorElement<double>,3> & ConstantStrainTriangleElement::bodyForceVector() {

	// For each node, compute body forces f_i.
	for (int i = 0; i < numNodes; i++) {
		bodyElements[i].second = triQuadRule.integrate(bodyForceKernel,
				bodyElements[i].first);
	}

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

bool ConstantStrainTriangleElement::hasNode(const TwoDNode & node) const {
	for(int i = 0; i < numNodes; i++) {
		if(node == nodes[i]) return true;
	}
	return false;
}

int ConstantStrainTriangleElement::getLocalNodeId(const TwoDNode & node) const {
	int id = -1;

	// The node ids are ordered according to their arrangement in the array.
	if (node == nodes[0]) {
		id = 0;
	} else if (node == nodes[1]) {
		id = 1;
	} else if (node == nodes[2]) {
		id = 2;
	}

	return id;
}

int ConstantStrainTriangleElement::getOffBoundaryNodeId(
		const TwoDRobinBoundaryCondition & boundary) const {
	int id = -1, nodeId1, nodeId2 = 0;

	// Get the local node ids
	nodeId1 = getLocalNodeId(boundary.firstNode);
	nodeId2 = getLocalNodeId(boundary.secondNode);
    // Cycle through the id pairs by adding up their values, but only if the
	// ids are not the same.
	if (nodeId1 != nodeId2) {
	    // This works by the fact that the sums of pairs of numbers 0, 1, and 2
		// are 1, 2, and 3 if pairs cannot have members that are equal to each
		// other. That is, (0,0), (1,1), and (2,2) are not allowed. A matrix of
		// ids sums is as follows, where the diagonals must be 0 because they
		// are not allowed:
		// 0 1 2
		// 1 0 3
		// 2 3 0
		int sum = nodeId1 + nodeId2;
		if (sum == 1) {
			// sum == 1 means nodes with local id 0 and 1 form the boundary,
			// so node 2 is not on the boundary.
			id = 2;
		} else if (sum == 2) {
			// sum == 2 means nodes 0 and 2 form the boundary, node 1 is off.
			id = 1;
		} else if (sum == 3) {
			// sum == 3 means nodes 1 and 2 form the boundary, node 0 is off.
			id = 0;
		}
	}

	return id;
}

void ConstantStrainTriangleElement::addRobinBoundary(
		const TwoDRobinBoundaryCondition & boundary) {

	bool hasBoundary = false;

	// The node ids must be different and only three boundaries can be added.
	if (boundary.firstNode != boundary.secondNode && numBoundaries < 3) {
		// Check to see if the element contains the nodes
		if (hasNode(boundary.firstNode) && hasNode(boundary.secondNode)) {
			// Check to see if the boundary is already configured
			hasBoundary = (boundaries[0] == boundary)
					|| (boundaries[1] == boundary)
					|| (boundaries[2] == boundary);
			// If is genuinely new, then add it and set the off-boundary node
			// id for the boundary conditions calculation.
			if (!hasBoundary) {
			   boundaries[numBoundaries] = boundary;
			   offBoundaryNodeIds[numBoundaries] =
					   getOffBoundaryNodeId(boundary);
			   numBoundaries++;
			} else {
				// Otherwise complain
				throw runtime_error("Boundary value already configured.");
			}
		} else {
			// Some other side that cannot exist.
			throw runtime_error("Invalid boundary! One or both nodes not in element.");
		}
	} else {
		// Either self-boundary reference or too many boundaries.
		throw runtime_error("Invalid boundary configuration or two many boundaries.");
	}

	return;
}

} /* end namespace fire */

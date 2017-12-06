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
#include <math.h>
#include <algorithm>
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
	    return -1.0;
	};
	bodyForceKernel = [](const std::array<double,3> & coords, const int & i) {
		throw runtime_error("Body force kernel not implemented for element!");
	    return -1.0;
	};

	// Initialize body force boundary condition kernels
	firstNodeRobinBoundaryForceFunction = [&](const double & x, const int & i) {
		double result = 0.0;
		auto & cond = robinBoundaries[i].get();
		// Be kind on the end of the next line - multiply by 0.5 instead of
		// dividing by 2.0. Your runtime thanks you.
		result = cond.h(x)*(1.0 - x)*0.5;
		// Note 1/2 is from bounds change
		return 0.5*result;
	};
	secondNodeRobinBoundaryForceFunction = [&](const double & x, const int & i) {
		double result = 0.0;
		auto & cond = robinBoundaries[i].get();
		// Be kind on the end of the next line - multiply by 0.5 instead of
		// dividing by 2.0. Your runtime thanks you.
		result = cond.h(x)*(x + 1.0)*0.5;
		// Note 1/2 is from bounds change
		return 0.5*result;
	};

	// Initialize stiffness matrix boundary condition kernels
	iiNodeRobinBoundaryStiffnessFunction = [&](const double & x, const int & i) {
		auto & cond = robinBoundaries[i].get();
		int kId = getOffBoundaryLocalNodeId(cond);
		double result = 0.0, dist = b[kId]*b[kId] + c[kId]*c[kId];
		result = cond.sigma(x)*sqrt(dist)*((1.0-2.0*x+x*x)*0.25);
		// Be kind on the end of the last line - multiply by 0.25 instead of
		// dividing by 4.0. Your runtime thanks you.
		// Note 1/2 below is from bounds change
		return 0.5*result;
	};
	jiNodeRobinBoundaryStiffnessFunction = [&](const double & x, const int & i) {
		auto & cond = robinBoundaries[i].get();
		int kId = getOffBoundaryLocalNodeId(cond);
		double result = 0.0, dist = b[kId]*b[kId] + c[kId]*c[kId];
		result = cond.sigma(x)*sqrt(dist)*((1.0-x*x)*0.25);
		// Be kind on the end of the last line - multiply by 0.25 instead of
		// dividing by 4.0. Your runtime thanks you.
		// Note 1/2 below is from bounds change
		return 0.5*result;
	};
	jjNodeRobinBoundaryStiffnessFunction = [&](const double & x, const int & i) {
		auto & cond = robinBoundaries[i].get();
		int kId = getOffBoundaryLocalNodeId(cond);
		double result = 0.0, dist = b[kId]*b[kId] + c[kId]*c[kId];
		result = cond.sigma(x)*sqrt(dist)*((x*x+2.0*x+1)*0.25);
		// Be kind on the end of the last line - multiply by 0.5 instead of
		// dividing by 2.0. Your runtime thanks you.
		// Note 1/2 below is from bounds change
		return 0.5*result;
	};

	// Get node ids
	auto & node1Id = node1.value, & node2Id = node2.value,
			& node3Id = node3.value;

	// Reserve the memory for the stiffness matrix and body force vector
	MatrixElement<double> element;
	kIJElements.reserve(numElements);
	kIJElements.resize(numElements,element);
	VectorElement<double> vElement;
	bodyElements.reserve(numNodes);
	bodyElements.resize(numNodes,vElement);

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
	eArea = 0.5*(a[0] + node1.first*b[0]	+ node1.second*c[0]);

	return;
}

double ConstantStrainTriangleElement::getStiffnessElement(const int & i,
		const int & j) {
	double result = triQuadRule.integrate(stiffnessKernel,i,j);
	return result;
}

const std::vector<MatrixElement<double>> & ConstantStrainTriangleElement::stiffnessMatrix() {

	// For each node paired with all other nodes, compute k_ij.

	// There are exactly numNodes*numNodes matrix elements, each one being
	// compared to itself and its neighbors. In this case, it progresses
	// through the list as {node1,node1},{node1,node2}, {node1,node3}, etc.
	for (int i = 0; i < kIJElements.size(); i++) {
		kIJElements[i].value = getStiffnessElement(
				getLocalNodeId(kIJElements[i].first),
				getLocalNodeId(kIJElements[i].second));
	}

	// Enforce Robin Boundary Conditions
	double result = 0.0;
	int firstNodeId, secondNodeId = 0, index = 0;
	for (int i = 0; i < robinBoundaryContributions.size(); i++) {
		// Grab the matrix contribution
		auto & cond = robinBoundaryContributions[i];
		result = lineQuadRule.integrate(cond.func,cond.robinCondID);
		kIJElements[cond.kIndex].value += result;
	}

	return kIJElements;
}

const std::vector<VectorElement<double>> & ConstantStrainTriangleElement::bodyForceVector() {

	// For each node, compute body forces f_i.
	for (int i = 0; i < bodyElements.size(); i++) {
		double result = triQuadRule.integrate(bodyForceKernel,
				bodyElements[i].first);
		bodyElements[i].second = result;
	}

	// Enforce Robin Boundary Conditions
	for (int i = 0; i < robinBoundaryForces.size(); i++) {
		// Grab the currobinBoundariesary
		auto & cond = robinBoundaryForces[i];
		// Compute the contributions due to that boundary and add them.
		double result = lineQuadRule.integrate(cond.value, cond.second);
		bodyElements[cond.third].second += result;
	}

	// Subtract contributions from matrix elements that were removed from both
	// sides because of Dirichlet Boundary Conditions.

	//  Need to get the DBC index for the regular kIJ elements that have been
	//  moved over.
	//  T element
	//  K int - dbc index
	for (int i = 0; i < forceVectorUpdateElements.size(); i++) {
		auto & element = forceVectorUpdateElements[i];
		double result = getStiffnessElement(
				getLocalNodeId(element.first),
				getLocalNodeId(element.second))*
				dirichletBoundaries[element.third].get().f(0.0,0.0);
		bodyElements[element.value].second -= result;
	}

	// Subtract contributions from matrix elements for Robin Boundary
	// Conditions that were subtracted from both sides because of
	// Dirichlet Boundary Conditions. Scale by the Dirichlet Condition.
	for (int i = 0; i < robinBoundaryForceContributions.size(); i++) {
		auto & contribution = robinBoundaryForceContributions[i];
		auto & dCond = dirichletBoundaries[contribution.dirichletCondID].get();
		double result = lineQuadRule.integrate(
	    		contribution.func, contribution.robinCondID)*dCond.f(0.0,0.0);
	    bodyElements[contribution.fIndex].second -= result;
	}

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

	// Just forward the request to the integer version.
	int id = getLocalNodeId(node.value);

	return id;
}

int ConstantStrainTriangleElement::getLocalNodeId(const int & nodeId) const {
	int id = 0;

	// The node ids are ordered according to their arrangement in the array.
	id = (1*(nodeId == nodes[0].get().value)
		+ 2*(nodeId == nodes[1].get().value)
		+ 3*(nodeId == nodes[2].get().value)) - 1;

	return id;
}

int ConstantStrainTriangleElement::getOffBoundaryLocalNodeId(
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

bool ConstantStrainTriangleElement::checkBoundary(
		const NodePair<TwoDNode> & boundary) const {

	bool isGood = false;

	// The node ids must be different and only three boundaries can be added.
	if (boundary.firstNode != boundary.secondNode) {
		// Check to see if the element contains the nodes
		if (hasNode(boundary.firstNode) && hasNode(boundary.secondNode)) {
			isGood = true;
		} else {
			// Some other side that cannot exist.
			throw runtime_error(
					"Invalid boundary! One or both nodes not in element.");
		}
	} else {
		// Either self-boundary reference or too many boundaries.
		throw runtime_error(
				"Invalid boundary configuration or two many conditions.");
	}

	return isGood;
}

void ConstantStrainTriangleElement::addRobinBoundary(
		const TwoDRobinBoundaryCondition & boundary) {

	bool hasBoundary = false;
	// This is a comparison function used to find the boundary if it already
	// exists.
	auto comparisonFunc = [&](
			const std::reference_wrapper<const TwoDRobinBoundaryCondition> &i) {
		return boundary == i.get();
	};

	// If is genuinely new, then add it and set the off-boundary node
	// id for the boundary conrobinBoundarieslculation.
	if (checkBoundary(boundary) && numRobinBoundaries < 3) {
		// Check to see if the boundary is already stored
		hasBoundary = find_if(robinBoundaries.begin(), robinBoundaries.end(),
				comparisonFunc) != end(robinBoundaries);
		if (!hasBoundary) {
			robinBoundaries.push_back(ref(boundary));
			numRobinBoundaries++;
			// Get its local node ids
			int firstNodeId = getLocalNodeId(boundary.firstNode);
			int secondNodeId = getLocalNodeId(boundary.secondNode);
			// Store the function pointers for the elements that will be
			// updated in the stiffness matrix.
			int index = rowMajorIndex(firstNodeId,firstNodeId, numNodes);
			robinBoundaryContributions.emplace_back(index,-1,kIJElements[index],
					numRobinBoundaries-1,0,jjNodeRobinBoundaryStiffnessFunction);
			index = rowMajorIndex(firstNodeId,secondNodeId, numNodes);
			robinBoundaryContributions.emplace_back(index,-1,kIJElements[index],
					numRobinBoundaries-1,0,jiNodeRobinBoundaryStiffnessFunction);
			index = rowMajorIndex(secondNodeId,firstNodeId, numNodes);
			robinBoundaryContributions.emplace_back(index,-1,kIJElements[index],
					numRobinBoundaries-1,0,jiNodeRobinBoundaryStiffnessFunction);
			index = rowMajorIndex(secondNodeId,secondNodeId, numNodes);
			robinBoundaryContributions.emplace_back(index,-1,kIJElements[index],
					numRobinBoundaries-1,0,iiNodeRobinBoundaryStiffnessFunction);
			// Store the function pointers for the parts of the force vector
			// that will be updated due to this condition
			robinBoundaryForces.emplace_back(firstNodeId,numRobinBoundaries-1,
					firstNodeId,
					firstNodeRobinBoundaryForceFunction);
			robinBoundaryForces.emplace_back(secondNodeId,numRobinBoundaries-1,
					secondNodeId,
					secondNodeRobinBoundaryForceFunction);
		} else {
			throw runtime_error("Boundary condition already set.");
		}
	} else {
		// Otherwise complain
		throw runtime_error("Boundary value erroneously configured.");
	}

	return;
}

void ConstantStrainTriangleElement::computeIds() {

	// For each Dirichlet boundary condition, remove the correct elements
	// from the list.
	for (int j = 0; j < dirichletBoundaries.size(); j++) {
		int id = dirichletBoundaries[j].get().value;
		// Deflate the stiffness matrix
		for (int i = 0; i < kIJElements.size(); i++) {
			if (kIJElements[i].first == id) {
				// Erase the element from the row
				kIJElements.erase(kIJElements.begin() + i);
				// Decrement the counters to role back to the last remaining
				// element
				i--;
			} else if (kIJElements[i].second == id) {
				// If the second id is equal to the Dirichlet condition id,
				// then this is a cross term that needs to be subtracted from
				// the body force vector in addition to being deleted from the
				// stiffness matrix. Store the id so it can be computed later.
				// This only applies for the two elements other than (id,id).
				forceVectorUpdateElements.emplace_back(kIJElements[i].first,
						kIJElements[i].second,j,0);
				// Erase the element from the column
				kIJElements.erase(kIJElements.begin() + i);
				// Decrement the counters to role back to the last remaining
				// element
				i--;
			}
		}
		// Deflate the "second" stiffness matrix of Robin Boundary Condition
		// contributions to the main matrix.
		for (int i = 0; i < robinBoundaryContributions.size(); i++) {
			auto & cond = robinBoundaries[robinBoundaryContributions[i].
										  robinCondID];
			if (cond.get().firstNode.value == id) {
				// Erase the element from the row
				robinBoundaryContributions.erase(
						robinBoundaryContributions.begin() + i);
				// Decrement the counters to role back to the last remaining
				// element
				i--;
			} else if (cond.get().secondNode.value == id) {
				// If the second id is equal to the Dirichlet condition id,
				// then this is a cross term that needs to be subtracted from
				// the body force vector in addition to being deleted from the
				// stiffness matrix. Store the id so it can be computed later.
				// This only applies for the two elements other than (id,id).
				robinBoundaryForceContributions.push_back(
						robinBoundaryContributions[i]);
				// Set the Dirichlet Boundary Condition ID so the scaling can
				// be properly handled.
				robinBoundaryForceContributions.back().dirichletCondID = id;
				// Erase the element from the set of elements that would have
				// otherwise been added into the stiffness matrix.
				robinBoundaryContributions.erase(
						robinBoundaryContributions.begin() + i);
				// Decrement the counters to role back to the last remaining
				// element
				i--;
			}
		}
		// Deflate the force vector
		for (int i = 0; i < bodyElements.size(); i++) {
			if (bodyElements[i].first == id) {
				bodyElements.erase(bodyElements.begin() + i);
				i--;
			}
		}
		// Deflate the Robin Boundary Conditions contributions to the forces.
		for (int i = 0; i < robinBoundaryForces.size(); i++) {
			if (robinBoundaryForces[i].first == id) {
				robinBoundaryForces.erase(robinBoundaryForces.begin() + i);
				i--;
			}
		}
	}

	// Remove any redundant entries in the force update vector. This works
	// by recognizing the fact that the redundant elements will have the global
	// id of the last Dirichlet node in their first element index.
	// Alternatively, this could be done by deleting the elements that do not
	// have the same row index as the remaining nodes.
	for (int i = 0; i < forceVectorUpdateElements.size(); i++) {
		if (forceVectorUpdateElements[i].first
				== dirichletBoundaries.back().get().value) {
			forceVectorUpdateElements.erase(
					forceVectorUpdateElements.begin() + i);
			i--;
		}
	}

	// Re-index the force vector updates array & the robin boundary conditions
	for (int i = 0; i < bodyElements.size(); i++) {
		// Updates
		for (int j = 0; j < forceVectorUpdateElements.size(); j++) {
			if (bodyElements[i].first == forceVectorUpdateElements[j].first) {
				forceVectorUpdateElements[j].value = i;
			}
		}
		// Robin Boundary Conditions - regular RHS contributions
		for (int j = 0; j < robinBoundaryForces.size(); j++) {
			if (bodyElements[i].first == robinBoundaryForces[j].first) {
				robinBoundaryForces[j].third = i;
			}
		}
	}

	// Re-index the Robin Boundary Condition contributons to the stiffness
	// matrix.
	for (int i = 0; i < robinBoundaryContributions.size(); i++) {
		auto & contribution = robinBoundaryContributions[i];
		// Just do a linear search over the matrix elements. i & j are small.
		for (int j = 0; j < kIJElements.size(); j++) {
			if (contribution.matrixElement.first == kIJElements[j].first &&
					contribution.matrixElement.second == kIJElements[j].second) {
				// Set the index in the kIJElements array that this
				// contribution modifies.
				contribution.kIndex = j;
			}
		}
	}

	// Re-index the Robin Boundary Condition contributions to the forces since
	// the stiffness matrices bounds have changed and their local indices are
	// off.
	for (int i = 0; i < robinBoundaryForceContributions.size(); i++) {
		auto & contribution = robinBoundaryForceContributions[i];
		auto & cond = robinBoundaries[contribution.robinCondID];
		// Just do a linear search over the matrix elements. i & j are small.
		for (int j = 0; j < kIJElements.size(); j++) {
			if (cond.get().firstNode.value == kIJElements[j].first &&
					cond.get().secondNode.value == kIJElements[j].second) {
				// Set the index in the kIJElements array that this
				// contribution modifies.
				contribution.kIndex = j;
			}
		}
		// Likewise do a linear search over the forces to set the updates
		// to the force vector since it has changed too.
		for (int j = 0; j < bodyElements.size(); j++) {
			// In this case, it is just the row id that matters.
			if (cond.get().firstNode.value == bodyElements[j].first) {
				// Set the index in the kIJElements array that this
				// contribution modifies.
				contribution.fIndex = j;
			}
		}
	}

	return;
}

void ConstantStrainTriangleElement::addDirichletBoundary(
		const TwoDDirichletBoundaryCondition & boundary) {

	bool hasBoundary = false;
	// This is a comparison function used to find the boundary if it already
	// exists.
	auto comparisonFunc = [&](
			const std::reference_wrapper<const TwoDDirichletBoundaryCondition> &i) {
		return boundary == i.get();
	};

	// If is genuinely new, then add it
	if (dirichletBoundaries.size() < 2) {
		// Check to see if the boundary is already stored
		hasBoundary = find_if(dirichletBoundaries.begin(), dirichletBoundaries.end(),
				comparisonFunc) != end(dirichletBoundaries);
		if (!hasBoundary) {
			dirichletBoundaries.push_back(ref(boundary));
		} else {
			throw runtime_error("Boundary condition already set.");
		}
	} else {
		// Otherwise complain
		throw runtime_error("Boundary value erroneously configured.");
	}

	computeIds();

	return;
}

} /* end namespace fire */

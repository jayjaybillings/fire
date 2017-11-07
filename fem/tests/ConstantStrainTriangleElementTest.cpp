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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FEM

#include <boost/test/included/unit_test.hpp>
#include <ConstantStrainTriangleElement.h>

using namespace std;
using namespace fire;

/**
 * This operation checks the area of the CST.
 */
BOOST_AUTO_TEST_CASE(checkCSTArea) {

	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3);
	ConstantStrainTriangleElement element(node1,node2,node3);

	double area = (node2.first*node3.second - node3.first*node2.second)
			- node1.first*(node3.second-node2.second)
			+ node1.second*(node3.first-node2.first);

	// Check the area
	BOOST_REQUIRE_CLOSE(area,element.area(),1.0e-15);

	return;
}

/**
 * This operation checks the computation of local node ids and the ids
 * of nodes not on the boundary.
 */
BOOST_AUTO_TEST_CASE(checkLocalNodeIdsAndOffIds) {
	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3),
			node4(9.0,9.0,9.0);
	ConstantStrainTriangleElement element(node1,node2,node3);
	std::function<double(const double &)> f = [](const double & foo) {
		return 1.0;
	};
	std::function<double(const double &)> g = [](const double & foo) {
		return 2.0;
	};
	std::function<double(const double &)> h = [](const double & foo) {
		return 3.0;
	};
	TwoDRobinBoundaryCondition cond1(node3,node1,f,g),
			cond2(node1,node2,g,h), cond3(node2,node3,h,f),
			cond4(node2,node2,h,f);

	// Check local node ids. Should be 0, 1 and 2 accordingly.
	BOOST_REQUIRE_EQUAL(0,element.getLocalNodeId(node1));
	BOOST_REQUIRE_EQUAL(1,element.getLocalNodeId(node2));
	BOOST_REQUIRE_EQUAL(2,element.getLocalNodeId(node3));
	// Node 4 is not on the element, so it should return -1.
	BOOST_REQUIRE_EQUAL(-1,element.getLocalNodeId(node4));

	// Check off boundary ids
	BOOST_REQUIRE_EQUAL(1,element.getOffBoundaryNodeId(cond1));
	BOOST_REQUIRE_EQUAL(2,element.getOffBoundaryNodeId(cond2));
	BOOST_REQUIRE_EQUAL(0,element.getOffBoundaryNodeId(cond3));
	BOOST_REQUIRE_EQUAL(-1,element.getOffBoundaryNodeId(cond4));

	return;
}

/**
 * This operation insures that CST element correct throws exceptions if the
 * default kernel and body force functions are used since these should always
 * be overridden by users.
 */
BOOST_AUTO_TEST_CASE(checkKernelExceptions) {

	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3);
	ConstantStrainTriangleElement element(node1,node2,node3);

	// Stiffness matrix
	try {
       element.stiffnessMatrix();
       BOOST_FAIL("Stiffness matrix exception not caught!");
	} catch (...) {/* Nothing to do - it passed */	}

	// Body forces
	try {
	   element.bodyForceVector();
	   BOOST_FAIL("Body force vector exception not caught!");
	} catch (...) {/* Nothing to do - it passed */	}

}

/**
 * This operation checks that the CST will accurately recompute its
 * constants after an update.
 */
BOOST_AUTO_TEST_CASE(checkRecomputingConstants) {

	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3);
	ConstantStrainTriangleElement element(node1,node2,node3);

	double area = (node2.first*node3.second - node3.first*node2.second)
			- node1.first*(node3.second-node2.second)
			+ node1.second*(node3.first-node2.first);

	// Check the area
	BOOST_REQUIRE_CLOSE(area,element.area(),1.0e-15);

	// Double

	return;
}

/**
 * This operation checks the ability of the CST to translate a global x,y
 * coordinate into local area coordinates (L_1,L_2,L_3).
 */
BOOST_AUTO_TEST_CASE(checkCSTLocalPoints) {

	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3);
	ConstantStrainTriangleElement element(node1,node2,node3);

	// Compute constants
	double a_1 = node2.first*node3.second - node3.first*node2.second;
	double a_2 = node3.first*node1.second - node1.first*node3.second;
	double a_3 = node1.first*node2.second - node2.first*node1.second;
	double b_1 = node2.second - node3.second;
	double b_2 = node3.second - node1.second;
	double b_3 = node1.second - node2.second;
	double c_1 = node3.first - node2.first;
	double c_2 = node1.first - node3.first;
	double c_3 = node2.first - node1.first;
	double twoA = 2.0*element.area();

	// Check the computation of the local points at the nodes. Start with node
	// 1 -  Should be L_i = a_i/twoA since x and y are zero for node 1.
	auto localPoint = element.computeLocalPoint(node1.first, node1.second);
	BOOST_REQUIRE_CLOSE(localPoint.first,a_1/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.second,a_2/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.third,a_3/twoA,1.0e-15);
	// Node 2 - Should be L_i = (a_i + b_i)/twoA since x = 1, y = 0.
	localPoint = element.computeLocalPoint(node2.first, node2.second);
	BOOST_REQUIRE_CLOSE(localPoint.first,(a_1+b_1)/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.second,(a_2+b_2)/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.third,(a_3+b_3)/twoA,1.0e-15);
	// Node 3 - Should be L_i = (a_i + b_i + c_i)/twoA since x = 1, y = 1.
	localPoint = element.computeLocalPoint(node3.first, node3.second);
	BOOST_REQUIRE_CLOSE(localPoint.first,(a_1+b_1+c_1)/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.second,(a_2+b_2+c_2)/twoA,1.0e-15);
	BOOST_REQUIRE_CLOSE(localPoint.third,(a_3+b_3+c_3)/twoA,1.0e-15);

	return;
}

/**
 * This operations makes sure that the addBoundary() operation works
 * appropriately.
 */
BOOST_AUTO_TEST_CASE(checkBoundaries) {

	TwoDNode node1(0.0,0.0,1), node2(1.0,0.0,2), node3(1.0,1.0,3),
			node4(9.0,9.0,9.0);
	ConstantStrainTriangleElement element(node1,node2,node3);
	std::function<double(const double &)> f = [](const double & foo) {
		return 1.0;
	};
	std::function<double(const double &)> g = [](const double & foo) {
		return 2.0;
	};
	std::function<double(const double &)> h = [](const double & foo) {
		return 3.0;
	};
	TwoDRobinBoundaryCondition cond1(node3,node1,f,g),
			cond2(node1,node2,g,h), cond3(node2,node2,h,f),
			cond4(node2,node3,h,f), cond5(node4,node1,f,g);

	// NEED to create TwoDRobinBoundaryCondition

	// Add a couple of boundaries
	element.addRobinBoundary(cond1);
	element.addRobinBoundary(cond2);
	// Try to add a boundary with the same node ids
	try {
		element.addRobinBoundary(cond3);
		BOOST_FAIL("Adding boundary with same node ids not caught!");
	} catch (...) { /** Do nothing, it passed **/ }
	// Add a third boundary
	element.addRobinBoundary(cond4);
	// Try to add a fourth, invalid boundary
	try {
		element.addRobinBoundary(cond5);
		BOOST_FAIL("Adding fourth boundary on triangle not caught!");
	} catch (...) { /** Do nothing, it passed **/ }
	// Try to add a boundary that is already configured
	try {
		element.addRobinBoundary(cond1);
		BOOST_FAIL("Adding pre-existing boundary on triangle not caught!");
	} catch (...) { /** Do nothing, it passed **/ }

	return;
}

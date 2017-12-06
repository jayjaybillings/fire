

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
#include <LaplaceCSTElement.h>
#include <FEMTypes.h>

using namespace std;
using namespace fire;

/**
 * This operation checks the LaplaceCSTElement for known values of k and given
 * nodal coordinates on the unit square. It is based on example 3.5 from
 * Davies' Finite Element Method text.
 */
BOOST_AUTO_TEST_CASE(checkLaplaceCSTElement) {

	TwoDNode node1(0.0,0.0,0), node2(1.0,0.0,1), node3(1.0,1.0,2), node4(0.0,1.0,3);
	LaplaceCSTElement e1(node1,node2,node4), e2(node4,node2,node3);
	array<double,16> e1Ref = {
			1.0,-0.5,0.0,-0.5,
		   -0.5,0.5,0.0,0.0,
		    0.0,0.0,0.0,0.0,
			-0.5,0.0,0.0,0.5};
	array<double,16> e2Ref = {
			0.0,0.0,0.0,0.0,
		    0.0,0.5,-0.5,0.0,
		    0.0,-0.5,1.0,-0.5,
			0.0,0.0,-0.5,0.5};
	array<double,16> e2BCRef = {
			0.0,0.0,0.0,0.0,
		    0.0,0.0,0.0,0.0,
		    0.0,0.0,1.0/3.0,1.0/6.0,
			0.0,0.0,1.0/6.0,1.0/3.0};
	array<double,4> e2FinalFRef = {0.0,0.0,1.0,1.0};

	// Check the stiffness matrix for element 1
	auto stiffnessMatrixElements = e1.stiffnessMatrix();
	for (auto & element : stiffnessMatrixElements) {
		int index = rowMajorIndex(element,4);
		BOOST_REQUIRE_CLOSE(element.value,e1Ref[index],1.0e-9);
	}
	// Check the force vector for element 1. f is zero.
	auto forceVector = e1.bodyForceVector();
	for (auto & element : forceVector) {
		BOOST_REQUIRE_CLOSE(element.second,0.0,0.0);
	}

	// Check the stiffness matrix for element 2
	stiffnessMatrixElements = e2.stiffnessMatrix();
	for (auto & element : stiffnessMatrixElements) {
		int index = rowMajorIndex(element,4);
		BOOST_REQUIRE_CLOSE(element.value,e2Ref[index],1.0e-9);
	}
	// Check the force vector for element 2. f is zero.
	forceVector = e2.bodyForceVector();
	for (auto & element : forceVector) {
		BOOST_REQUIRE_CLOSE(element.second,0.0,0.0);
	}

	// Now add a boundary condition at y = 1 on the second element.
	std::function<double(const double &)> sigma = [](const double & foo) {
		return 1.0;
	};
	std::function<double(const double &)> h = [](const double & foo) {
		return 2.0;
	};
	TwoDRobinBoundaryCondition cond(node4,node3,sigma,h);
	e2.addRobinBoundary(cond);

	// Check the stiffness matrix now. It should be equal to e2Ref+e2BCRef.
	stiffnessMatrixElements = e2.stiffnessMatrix();
	for (auto & element : stiffnessMatrixElements) {
		int index = rowMajorIndex(element,4);
		BOOST_TEST_MESSAGE(element.first << " " << element.second << " "
				<< element.value << " " << index << " " << e2Ref[index]
				<< " " << e2BCRef[index]);
		BOOST_REQUIRE_CLOSE(element.value,(e2Ref[index]+e2BCRef[index]),1.0e-9);
	}
	// Check the force vector for element 2. f is zero.
	forceVector = e2.bodyForceVector();
	for (int i = 0; i < 3; i++) {
		int index = forceVector[i].first;
		BOOST_TEST_MESSAGE(forceVector[i].first << " " << forceVector[i].second
				<< " " << e2FinalFRef[index]);
		BOOST_REQUIRE_CLOSE(forceVector[i].second,e2FinalFRef[index],1.0e-9);
	}

	// Create functions to be used in the Dirichlet conditions
	std::function<double(const double &, const double &)> dFunc = [](const double & x,
			const double & y) {
		return 1.0;
	};

	// Add a dirichlet condition
	TwoDDirichletBoundaryCondition dCond1(node1,dFunc), dCond2(node2,dFunc);
	e1.addDirichletBoundary(dCond1);
	e1.addDirichletBoundary(dCond2); // Node 2 is in both e1 and e2!
	e2.addDirichletBoundary(dCond2);

	// Get the stiffness matrices and assemble the full matrix.
	auto & e1SM = e1.stiffnessMatrix();
	auto & e2SM = e2.stiffnessMatrix();
	array<MatrixElement<double>,16> fullMatrix;

	cout << "----- e1 full matrix updates" << endl;

	// e1 first
	for (auto & elem : e1SM) {
		int index = rowMajorIndex(elem, 4);
		fullMatrix[index].first = elem.first;
		fullMatrix[index].second = elem.second;
		fullMatrix[index].value += elem.value;
		cout << fullMatrix[index].first << " " << fullMatrix[index].second
				<< ", (addition) " << elem.value << ", (new value), "
				<< fullMatrix[index].value << endl;

	}

	cout << "----- e2 full matrix contributions" << endl;

	// e2
	for (auto & elem : e2SM) {
		int index = rowMajorIndex(elem, 4);
		// Just overwrite the element ids since they don't change or are unset.
		fullMatrix[index].first = elem.first;
		fullMatrix[index].second = elem.second;
		fullMatrix[index].value += elem.value;
		cout << fullMatrix[index].first << " " << fullMatrix[index].second
				<< ", (addition) " << elem.value << ", (new value), "
				<< fullMatrix[index].value << endl;
	}

	// Construct the full matrix and make sure the elements are correct. In a
	// real solve, all the non-zero elements would be ignored, but the only way
	// we can check correctness here and insure the sparsity is correctly
	// represented is to check everything.
	cout << "----- Full matrix" << endl;
	for (int i = 0; i < 16; i++) {
		auto & elem = fullMatrix[i];
		if (i == 10 || i == 15) {
			BOOST_REQUIRE_CLOSE(1.3333333333333333,elem.value,1.0e-8);
		} else if (i == 11 || i == 14) {
			BOOST_REQUIRE_CLOSE(-0.3333333333333333,elem.value,1.0e-8);
		} else {
			BOOST_REQUIRE_CLOSE(0.0,elem.value,0.0);
		}
 		cout << elem.first << " " << elem.second << " " << elem.value << endl;
	}

	// Now get the force vectors and check those too.
	auto & e1FV = e1.bodyForceVector();
	auto & e2FV = e2.bodyForceVector();
	array<VectorElement<double>,4> fullRHS;

	cout << "----- e1 full force vector contributions" << endl;

	// e1 first
	for (auto & elem : e1FV) {
		int index = elem.first;
		fullRHS[index].first = elem.first;
		fullRHS[index].second += elem.second;
		cout << fullRHS[index].first << ", (addition) "
				<< elem.second << ", (new value), "
				<< fullRHS[index].second << endl;
	}

	cout << "----- e2 full force vector contributions" << endl;

	cout << "e2FV.size() = " << e2FV.size() << endl;

	// e2
	for (auto & elem : e2FV) {
		int index = elem.first;
		fullRHS[index].first = elem.first;
		fullRHS[index].second += elem.second;
		cout << fullRHS[index].first << ", (addition) "
				<< elem.second << ", (new value), "
				<< fullRHS[index].second << endl;
	}

	// Construct the full RHS vector. Same reasons as for the matrix.
	cout << "----- Full RHS" << endl;
	for (int i = 0; i < 4; i++) {
		auto & elem = fullRHS[i];
//		if (i == 10 || i == 15) {
//			BOOST_REQUIRE_CLOSE(1.3333333333333333,elem.value,1.0e-8);
//		} else if (i == 11 || i == 14) {
//			BOOST_REQUIRE_CLOSE(-0.3333333333333333,elem.value,1.0e-8);
//		} else {
//			BOOST_REQUIRE_CLOSE(0.0,elem.value,0.0);
//		}
 		cout << elem.first << " " << elem.second << endl;
	}

	cout << "Element test complete!" << endl;

	return;
}

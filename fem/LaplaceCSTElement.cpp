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
#include <LaplaceCSTElement.h>
#include <iostream>

using namespace std;

namespace fire {

LaplaceCSTElement::LaplaceCSTElement(const TwoDNode & node1,
		const TwoDNode & node2, const TwoDNode & node3) :
				ConstantStrainTriangleElement(node1,node2,node3) {

	kFunction = [&](const std::array<double,3> & coords, const int & i,
			const int & j) {
	    return 1.0;
	};

	// The stiffness kernel is very simple across a CST for Laplace's equation.
	// This is taken from Davies, p. 109.
	stiffnessKernel = [&](const std::array<double,3> & coords, const int & i,
			const int & j) {
		double result = (kFunction(coords,i,j)/(4.0*eArea*eArea))*(b[i]*b[j]+c[i]*c[j]);
	    return result;
	};
	// f = 0 for Laplace's equation
	bodyForceKernel = [&](const std::array<double,3> & coords, const int & i) {
	    return 0.0;
	};

}

void LaplaceCSTElement::transferCoefficient(const std::function<double(const std::array<double,3> &,
		const int &, const int &)> & function) {
	kFunction = function;
}

const std::function<double(const std::array<double,3> &,
		const int &, const int &)> & LaplaceCSTElement::transferCoefficient() const {
	return kFunction;
}


} /* namespace fire */

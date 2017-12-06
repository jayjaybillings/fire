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
#include <LineQuadratureRule.h>

using namespace std;

namespace fire {

// Define the constants from the class. Definition is different than
// declaration, so this needs to be here or the compiler will not be able to
// link.
constexpr std::array<double,4> const LineQuadratureRule::weights;
constexpr std::array<double,4> const LineQuadratureRule::points;

LineQuadratureRule::LineQuadratureRule() {
}

double LineQuadratureRule::integrate(const std::function<double(const double &,
		const int &, const int &)> & f,
		int i, int j) const {

	// Compute the integral.
	double result = weights[0]*f(points[0],i,j) + weights[1]*f(points[2],i,j)
			+ weights[2]*f(points[2],i,j) + weights[3]*f(points[3],i,j);

	return result;
}

double LineQuadratureRule::integrate(const std::function<double(const double &,
		const int &)> & f,
		int i) const {

	// Compute the integral.
	double result = weights[0]*f(points[0],i) + weights[1]*f(points[1],i)
			+ weights[2]*f(points[2],i) + weights[3]*f(points[3],i);

	return result;
}


} /* end namespace fire */

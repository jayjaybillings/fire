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
#ifndef QUADRATURE_TRIANGULARQUADRATURERULE_H_
#define QUADRATURE_TRIANGULARQUADRATURERULE_H_

#include <functional>
#include <array>

namespace fire {

/**
 * This class performs an integration over the area of a triangle using 2D
 * Gaussian Quadrature. It integrates a function defined across the triangle
 * with the following signature:
 * @code
 * double f(const std::array<double,3> & coords, const int & i, const int & j);
 * @endcode
 * or, in the case where only one optional index is required
 * @code
 * double f(const std::array<double,3> & coords, const int & i);
 * @endcode
 * For the latter case, the following code sample illustrates using the class:
 * @code
 *  TriangularQuadratureRule rule;
 *  std::function<double(const std::array<double,3> &,
 *		const int &)> areaFunction = [](const std::array<double,3> & point,
 *		const int & i) {
 *		return 1.0;
 *	};
 *	result = rule.integrate(areaFunction, index);
 * @endcode
 * where the array of doubles contains the area coordinates (L_1, L_2, L_3) of the
 * quadrature point and the following integer is the optional identifier of
 * the integration. Including integer identifiers makes it possible to, for
 * example, facilitate the integration of matrix elements, derivatives, and
 * otherwise indexed functions. For example, a function might differ
 * depending on whether it is the matrix element (i,j) or (j+1,i).
 *
 * This class provides an alternative version of the integrate function that
 * will forward a single index instead of two, which is useful for dealing
 * with vector indices.
 *
 * This class uses a four point Gaussian Quadrature rule in area coordinates
 * that exactly integrates all polynomials of degree 7 or less. It requires
 * four evaluations of the function f. Specifically,
 * \f[
 * \iint_{A} f(L_{1},L_{2},L_{3}) dA \approx \sum_{i=0}^{i=4}
 * \omega_{i}f(L_{1,i},L_{2,i},L_{3,i})
 * \f]
 *
 * Note that the weights and area coordinates are defined statically because
 * they are used unmodified across all instances of this class. Failing to
 * declare them as such could lead to an explosion in memory use if a new
 * instance of the quadrature rule is used in many places.
 */
class TriangularQuadratureRule {
private:

	/**
	 * The 4-point quadrature weights: -9.0/32.0,25.0/96.0,25.0/96.0,25.0/96.0
	 */
	static constexpr std::array<double,4> const weights = {-9.0/32.0,25.0/96.0,25.0/96.0,25.0/96.0};

	/**
	 * Quadrature Point 1 - (1.0/3.0,1.0/3.0,1.0/3.0)
	 */
	static constexpr std::array<double,3> const point1 = {1.0/3.0,1.0/3.0,1.0/3.0};

	/**
	 * Quadrature Point 2 - (3.0/5.0,1.0/5.0,1.0/5.0)
	 */
	static constexpr std::array<double,3> const point2 = {3.0/5.0,1.0/5.0,1.0/5.0};

	/**
	 * Quadrature Point 3 - (1.0/5.0,3.0/5.0,1.0/5.0)
	 */
	static constexpr std::array<double,3> const point3 = {1.0/5.0,3.0/5.0,1.0/5.0};

	/**
	 * Quadrature Point 4 - (1.0/5.0,1.0/5.0,3.0/5.0)
	 */
	static constexpr std::array<double,3> const point4 = {1.0/5.0,1.0/5.0,3.0/5.0};

public:

	/**
	 * Constructor
	 */
	TriangularQuadratureRule();

	/**
	 * This operation integrates the function across the area of the triangle.
	 * @param the function to integrate with the signature as described above
	 * @param optional first identifier
	 * @param optional second identifier
	 */
	double integrate(const std::function<double(const std::array<double,3> &,
			const int &, const int &)> & f,
			int i = 0, int j = 0) const;

	/**
	 * This operation integrates the function across the area of the triangle,
	 * but only forwards one index to the integrand.
	 * @param the function to integrate with the signature as described above
	 * @param optional identifier
	 */
	double integrate(const std::function<double(const std::array<double,3> &,
			const int &)> & f,
			int i = 0) const;
};

} /* namespace fire */

#endif /* QUADRATURE_TRIANGULARQUADRATURERULE_H_ */

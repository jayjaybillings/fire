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
#ifndef QUADRATURE_LINEQUADRATURERULE_H_
#define QUADRATURE_LINEQUADRATURERULE_H_

#include <functional>
#include <array>
#include <math.h>

namespace fire {

/**
 * This class performs an line integral using Gaussian Quadrature. It
 * integrates a function along the line with the following signature:
 * @code
 * double f(const double & x, const int & i, const int & j);
 * @endcode
 * or, in the case where only one optional index is required
 * @code
 * double f(const double & x, const int & i);
 * @endcode
 * For the latter case, the following code sample illustrates using the class:
 * @code
 *  LineQuadratureRule rule;
 *  std::function<double(const double &,
 *		const int &)> lineFunction = [](const double & point,
 *		const int & i) {
 *		return 1.0;
 *	};
 *	result = rule.integrate(lineFunction, index);
 * @endcode
 * where the point is the quadrature point along the line at which the function
 * will be evaluated and the following integer is the optional identifier of
 * the integration. Including integer identifiers makes it possible to, for
 * example, facilitate the integration of matrix elements, derivatives, and
 * otherwise indexed functions. For example, a function might differ
 * depending on whether it is the matrix element (i,j) or (j+1,i).
 *
 * This class provides an alternative version of the integrate function that
 * will forward a single index instead of two, which is useful for dealing
 * with vector indices.
 *
 * This class uses a four point Gaussian Quadrature rule in regular coordinates
 * that exactly integrates all polynomials of degree 7 or less, although in
 * practice this is limited to about seven decimal places for most polynomials
 * since the weights are not provided to that high of a precision. It requires
 * four evaluations of the function f. Specifically,
 * \f[
 * \int_{-1}^{1} f(x) dx \approx \sum_{i=0}^{i=4}
 * \omega_{i}f(x_{i})
 * \f]
 *
 * The bounds of the integration are from [-1,1] and functions not defined over
 * this region will need to be converted. This is usually done by
 * u-substitution. For example, to go from x = [-1,1] to t = [0,1], let
 * \f{eqnarray*}{
 * t &=& \frac{x-1}{2}\\
 * \frac{dt}{dx} &=& \frac{1}{2}\\
 * dt &=& \frac{1}{2} dx\\
 * dx &=& 2 dt\\
 * \f}
 * such that
 * \f[
 * \int_{-1}^{1} f(x) dx = \int_{0}^{1} 2f(t) dt
 * \f]
 *
 * Note that the weights and area coordinates are defined statically because
 * they are used unmodified across all instances of this class. Failing to
 * declare them as such could lead to an explosion in memory use if a new
 * instance of the quadrature rule is used in many places.
 */
class LineQuadratureRule {
private:

	/**
	 * The 4-point quadrature weights:
	 * \f[
	 * \omega = [\frac{18-\sqrt{30}}{36},
	 *	\frac{18+\sqrt{30}}{36},
	 *	\frac{18+\sqrt{30}}{36},
	 *	\frac{18-\sqrt{30}}{36}]
	 * \f]
	 */
	static constexpr std::array<double,4> const weights = {(18.0-sqrt(30.0))/36.0,
			(18.0+sqrt(30.0))/36.0,
			(18.0+sqrt(30.0))/36.0,
			(18.0-sqrt(30.0))/36.0};

	/**
	 * The 4 quadrature points:
	 * \f[
     * x = [-\sqrt{\frac{3}{7}+\frac{2}{7}\sqrt{\frac{6}{5}}},
	 *	-\sqrt{\frac{3}{7}-\frac{2}{7}\sqrt{\frac{6}{5}}},
	 *	\sqrt{\frac{3}{7}+\frac{2}{7}\sqrt{\frac{6}{5}}},
	 *	\sqrt{\frac{3}{7}-\frac{2}{7}\sqrt{\frac{6}{5}}}]
	 * \f]
	 */
	static constexpr std::array<double,4> const points = {
			-1.0*sqrt((3.0/7.0)+(2.0/7.0)*sqrt(6.0/5.0)),
			-1.0*sqrt((3.0/7.0)-(2.0/7.0)*sqrt(6.0/5.0)),
			sqrt((3.0/7.0)-(2.0/7.0)*sqrt(6.0/5.0)),
			sqrt((3.0/7.0)+(2.0/7.0)*sqrt(6.0/5.0))};

public:

	/**
	 * Constructor
	 */
	LineQuadratureRule();

	/**
	 * This operation integrates the function along the line.
	 * @param the function to integrate with the signature as described above
	 * @param optional first identifier
	 * @param optional second identifier
	 */
	double integrate(const std::function<double(const double &,
			const int &, const int &)> & f,
			int i = 0, int j = 0) const;

	/**
	 * This operation integrates the function along the line, but only forwards
	 * one index to the integrand.
	 * @param the function to integrate with the signature as described above
	 * @param optional identifier
	 */
	double integrate(const std::function<double(const double &,
			const int &)> & f,
			int i = 0) const;
};

} /* namespace fire */

#endif /* QUADRATURE_LINEQUADRATURERULE_H_ */

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
#ifndef FEM_TWODDIRICHLETBoundaryCONDITION_H_
#define FEM_TWODDIRICHLETBoundaryCONDITION_H_

#include <FEMTypes.h>
#include <NodePair.h>
#include <functional>

namespace fire {

/**
 * This struct represents a two dimensional Dirichlet Boundary condition in which
 * the condition is defined across an edge between two nodes. The Dirichlet
 * condition is defined as
 * \f[
 * u(i,j) = f(i,j)
 * \f]
 * All members of this struct must be initialized on construction. The idea is
 * that the references should not need to change during execution because the
 * boundaries do not change.
 *
 * Dirichlet Boundary Conditions are defined at nodes such that this class is
 * itself represented as a node, but with a functional value assigned to it.
 */
struct TwoDDirichletBoundaryCondition : public TwoDNode {
	/**
	 * As defined in the definition of the condition.
	 * @param i first axis/coordinate value
	 * @param j second axis/coordinate value
	 * @return the value of the function f(i,j)
	 */
    std::function<double(const double &, const double &)> f;

    /**
     * Constructor
     * @param first the node that should be used as the first node on the path
     * @param second the node that should be used as the second or terminal
     * node on the path
     * @param _f the function that provides the value of the condition
     */
    TwoDDirichletBoundaryCondition(const TwoDNode & first,
			const std::function<double(const double &, const double &)> & _f);

    /**
     * This is an operator override to compare two dimensional Dirichlet
     * conditions. Note that this operation implements equality checks for the
     * functions by generating pointer targets for the functions and checking
     * target types. This may or may not be the best way to check for equality
     * of these functions, depending on your application.
     * @param otherCond the other condition
     * @return true if the conditions are equal, false otherwise.
     */
    bool operator == (const TwoDDirichletBoundaryCondition & otherCond) const;
    /**
     * The inequality operator override to invert ==.
     */
    bool operator != (const TwoDDirichletBoundaryCondition & otherCond) const;

};

} /* namespace fire */

#endif /* FEM_TWODDirichletBoundaryCONDITION_H_ */

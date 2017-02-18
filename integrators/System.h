/**----------------------------------------------------------------------------
 Copyright (c) 2017-, UT-Battelle, LLC
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

#ifndef INTEGRATORS_SYSTEM_H_
#define INTEGRATORS_SYSTEM_H_

#include <memory>

namespace fire {

/**
 * This class represents a system of equations in Fire, where a system of
 * equations is a collection of equations with the same unknowns.
 *
 * The system of equations is represented by a collection of operators that
 * operate on a problem state provided by the user. The state defines all of
 * the system's unknown quantities. However, since the type of the state is
 * generic, the system does not explicitly known the identity of those
 * unknowns. The user-defined state class is expected to provide the unknowns
 * contiguously when queried.
 *
 * The system of equations can be integrated by calling the integrate()
 * operation. The system will be solved using a default integrator if a user-
 * defined integrator is not provided.
 */
template<class T>
class System {

protected:

	/**
	 * The total number of equations in the system.
	 */
	long numEqs;

	/**
	 * The total number of physical dimensions of the system, not including
	 * time.
	 */
	short nDim;

	/**
	 * The state of the system, including unknowns, materials information, etc.
	 */
	T * state;

public:

	/**
	 * This operation sets the size of the system of equations by defining the
	 * total number of equations and the number of dimensions.
	 * @param n The total number of equations in the system.
	 * @param d The total number of physical dimensions of the system, not
	 * including time. 1 for 1D, 2 for 2D, 3 for 3D.
	 */
	void setSize(const long & n, const short & d) {
		numEqs = n;
		nDim = d;
	}

	/**
	 * This operation specifies the state of the system of equations.
	 *
	 * The state is provided using a raw pointer because the system of
	 * equations does not participate in the state's memory lifecycle. This
	 * class explicitly guarantees that it will never deallocate the state
	 * data! Classes utilized by this class, such as integrators, may overwrite
	 * or manipulate the state data in ways described in their API docs,
	 * including attempting to deallocate it (frowned upon). Clients should
	 * generally expect that the state variables provided by the
	 * state type T will be modified and that in some instances those
	 * modifications may be done in place.
	 */
	void setState(T * systemState) {
		state = systemState;
	}

};


} /* end namespace fire */



#endif /* INTEGRATORS_SYSTEM_H_ */

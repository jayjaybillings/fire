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

#ifndef SOLVERS_STATE_H_
#define SOLVERS_STATE_H_

#include <memory>

namespace fire {

/**
 * This is a container class for user-provided data structures that can be
 * passed to solvers. It is designed such that container operations are
 * handled by default by the template and clients override the u() and dudt()
 * operations to tailor the behavior for the templated type.
 *
 * The variable t, when discussed below, is meant to represent a free parameter
 * on which u and du depend. It commonly represents time, but might might also
 * represent some other parameter. It is not explicitly called "time" to keep
 * the interface clean of any concepts that might limit the scope of the class
 * to only data that is used in time integrations, when in fact it can be used
 * in the solution of any initial value problem.
 *
 * The layout of the state is, in effect, a matrix with size n_e * n_t, where
 * n_e is the number of unique elements (or unknowns) considered in the system
 * and n_t is the number of values of t at which those values are considered.
 * Thus, in the case where a problem has six unknowns (n_e = 6) and fifty time
 * steps (t = time and n_t = 50), the state class could represent a 6 x 50
 * matrix with a total of 300 elements. Note that whether or not "the matrix"
 * is stored in column or row major format is not important.
 *
 * Using the operations on this class that work directly with doubles is the
 * most efficient way to interact with instances. With respect to setting data
 * using the templated functions that work with shared pointers, these can be
 * expensive because of atomic increments and decrements required for reference
 * counting in the shared pointer. The author's own experience has showed that
 * in tight loops this can kill performance by as much as 60% for a single
 * shared pointer. However, as the state class is responsible for maintaining
 * state and must subscribe to the memory life cycle of its own data, using
 * std::shared_ptrs is preferable to raw pointers because of the easy of
 * managing the data. FIXME! - Rewrite and finish this paragraph.
 */
template<typename T>
class State {
protected:

	std::shared_ptr<T> userState;

	std::unique_ptr<T> refTest;

	double tLast = 0.0;

	/**
	 * Does this class need to use set(shared_ptr<T>) or can it use set(T*)?
	 * Does this class need to return shared_ptr<T> from get or can it return T*?
	 * How should this class store data internally? unique_ptr? shared_ptr? Packed in a map with t values?
	 *
	 * Return reference to all states, including initial. Don't accept _any_ shared pointers. That is:
	 * _if you going to be greedy, then be greedy!_ Don't share ownership!
	 */

public:

	/**
	 * This operation sets the state to the contents of the shared pointer at
	 * the given value of t. This is the main operation for setting the state
	 * at a given value of t and other operations are covenience
	 * implementations of this one.
	 *
	 * This operation may be operationally advantageous when deltaT has already
	 * been computed by the client code, even though calling the other set()
	 * operations will only result in one extra subtraction.
	 *
	 * @param data the data for the given value of t
	 * @param t the value of the independent variable, typically time
	 * @param deltaT the difference between the current t and the last t
	 */
	void set(std::shared_ptr<T> data, const double & t,
			const double & deltaT) {}

	/**
	 * This operation sets the state to the contents of the shared pointer at
	 * the given value of t. This is a convenience method for set(data,t,dt)
	 * where dt = tlast - t.
	 *
	 * @param data the data for the given value of t
	 * @param t the value of the independent variable, typically time
	 */
	void set(std::shared_ptr<T> data, const double & t) {
		double dt = t-tLast;
		set(data,t,dt);
	}

	/**
	 * This operation will set the state to the contents of the shared pointer
	 * at t = 0. That is, it sets the initial conditions.
	 *
	 * It is a convenience operation equivalent to State.set(data,0.0,0.0).
	 *
	 * @param data the initial conditions at t = 0
	 */
	void set(std::shared_ptr<T> data) {
		set(data,0.0);
	}

	T & add(const double & t, const double & deltaT) {
		return *(refTest.get());
	}

	T & add(const double & t) {
		return add(t,0.0); // FIXME! - Does this work? Needs to be |t - tLast| not 0!
	}

	T & add() {
		return add(0.0);
	}

	/**
	 * This operation returns the state at the most recent value of t.
	 * @return state the state
	 */
	T & get() const {
		return get(tLast);
	}

	T & get(const double t) const {
		*(refTest.get());
	}

	/**
	 * This operation returns the state at the specified value of t.
	 * @param t the value of t at which the state should be retrieved
	 * @return state the state
	 */
	std::shared_ptr<T> get(double t) const {
		return userState;
	}

	/**
	 * @param t
	 * @return uValues
	 */
	double * u(const double & t) const {return 0;}

	/**
	 * @param data
	 * @param t
	 */
	void u(double * data, const double & t) {}

	/**
	 * @param t
	 * @return dudtValues
	 */
	double * dudt(const double & t) const {return 0;}

	/**
	 * This operation explicitly sets the number of unique data elements in the
	 * state. At present this value is constant for all values of t.
	 * present in the state.
	 * @param int the number of unique data elements in the state
	 */
	void size(const int & dataSize) {};

	/**
	 * This operation returns the number of unique elements in the state. At
	 * present this value is constant for all values of t.
	 * This is the size of the arrays u and dudt.
	 * @return int the size of the state and the state arrays
	 */
	int size() const {return 0;}
};

} /* namespace fire */

#endif /* SOLVERS_STATE_H_ */

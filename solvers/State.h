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
#include <utility>

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
 * This class is designed to be greedy and narcissistic. In lieu of allowing
 * users to create and store their own instances of T, it creates (add()) those
 * instances and forces users to interact with references retrieved using the
 * getters. Being so greedy makes it possible to this class to efficiently
 * manage memory with lower implementation overhead and no need to rely on
 * clients to correctly initialize data. The original design of this class
 * called for extensive use of smart pointers (using std::share), but there
 * are few computational advantages - if any - compared to simple references.
 * This is because simple references do not incur atomic increment and
 * decrement costs when crossing function boundaries, but smart pointers do.
 * In the author's personal experience, the cost of working with
 * smart pointers can be as much as a factor of two in tight loops with
 * function boundaries. References are, by comparison, free in tight loops.
 * Alternatively, passing a reference to the smart pointer is also free, but it
 * is hardly necessary to share a smart pointer at all if the client does not
 * need to participate in the memory lifecycle of the class, and indeed the
 * practice is not recommended (c.f. - Sutter's 2014 talk) at all in that
 * case.
 *
 * Three operations are provided for working with "fundamental state
 * vectors" as arrays of doubles, but casual users are warned that these
 * operations are for those implementing fast solvers based on this class and,
 * in general, casual users should stick to add() and get().
 */
template<typename T, typename... Args>
class State {
protected:

	/**
	 * The managed instance of the State of type T.
	 */
	std::unique_ptr<T> state;

	/**
	 * The most recent value of t provided by State.t().
	 */
	double tVal;

	/**
	 * The system size of the state.
	 */
	int systemSize;

public:

	/**
	 * Constructor with optional arguments for T.
	 * @param Optional argument list for constructing T.
	 */
	State(Args&& ... args) {
		state = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
		tVal = 0.0;
		systemSize = 0;
	}

	/**
	 * This operation returns the state at the most recent value of t.
	 * @return state the state at the most recent value of t
	 */
	T & get() const {
		return *(state.get());
	}

	/**
	 * This operation updates the value of t.
	 * @param t the new value of t
	 */
	void t(double t) {
		tVal = t;
	}

	/**
	 * This operation retrieves the most recent value of t.
	 * @return the most recent value of t
	 */
	const double & t() const {
		return tVal;
	}

	/**
	 * This function returns a simple array of size State.size() which contains
	 * the values of the fundamental state vector for this type. This vector is
	 * normally used by independent solvers looking to solve systems of
	 * equations or analysis routines (regression, etc.) in which case it
	 * represents the most recent values of the "unknowns" in the (thus the
	 * name "u"). So, for example, when solving the heat equation this could
	 * include a vector of temperatures at a given t, but it wouldn't, in
	 * general, contain the heat transfer coefficients. The distinction is
	 * that the temperature is the unknown in the system and coefficients are
	 * already well known.
	 *
	 * In general, this function should only be used for coupling to Solvers
	 * and other systems by developers and most clients should work with their
	 * classes retrieved via the get() operation.
	 *
	 * @return uValues the state
	 */
	double * u() const {
		throw "Operation not implemented for this type.";
	}

	/**
	 * This function returns a simple array of size State.size() which contains
	 * the derivatives of the primary State variables with respect to t. Thus
	 * it behaves identically to u(t), but provides derivatives instead.
	 *
	 * Note that calling this function with the value t will not reset the
	 * stored value of t! So calling t() before and after this function will
	 * always return the same value.
	 *
	 * @param t the value of the free variable, normally time, at which the
	 * derivatives of state should be retrieved
	 * @return dudtValues the derivatives at the given value of t
	 */
	double * dudt(const double & t) const {
		throw "Operation not implemented for this type.";
	}

	/**
	 * This operation explicitly sets the number of unique data elements in the
	 * state. At present this value is constant for all values of t.
	 * present in the state.
	 * @param int the number of unique data elements in the state
	 */
	void size(const int & numElements) {
		systemSize = numElements;
	};

	/**
	 * This operation returns the number of unique elements in the state. At
	 * present this value is constant for all values of t.
	 * This is the size of the arrays u and dudt.
	 * @return int the size of the state and the state arrays
	 */
	int size() const {return systemSize;}
};

} /* namespace fire */

#endif /* SOLVERS_STATE_H_ */

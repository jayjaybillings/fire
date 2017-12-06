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
#include <functional>
#include <vector>
#include <build.h>

namespace fire {

/**
 * This is a container class for user-provided data structures that can be
 * passed to solvers. It is designed such that container operations are
 * handled by default by the template and clients override the u() and dudt()
 * operations to tailor the behavior for the templated type.
 *
 * Instances should be constructed using the build<>() templates as follows:
 * @code
 * State<T> myState = build<T>(); // If T() takes no arguments
 * State<T> myState2 = build<T,int>(); // If T takes one argument
 * State<T> myState3 = build<T,int, const double &>(); // If T takes 2 args
 * //...etc.
 * @endcode
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
 * getters. Being so greedy makes it possible for this class to efficiently
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
 *
 * State objects can be monitored by registering a functor with the
 * addMonitor() operation. The simplest example is to use a Lambda as follows:
 * @code
 * 	// Register an observer to write a message when the state changes.
 *	state.add([](State<ReactionNetwork> & state) {
 *		std::cout << "Lambda Test " << state.t() << std::endl;
 *		return;
 *	});
 * @endcode
 * Monitors are only called when u(double *) is called. Classes that create
 * explicit specializations of that function should call notifyMonitors(), a
 * protected notification function, to notify the monitors if they require that
 * functionality. Simply calling the notifyMonitors() function immediately
 * before return is sufficient.
 *
 * This class does not yet consider thread safety issues when calling the
 * monitors or updating the state, so be warned.
 *
 * Common Errors:
 * Declaring T instead of T& - The C++ compiler will not throw an error in the
 * following scenario and will create a copy instead:
 * @code
 * T myT1 = state.get(); // WRONG - Creates a copy of the state!
 * T & myT2 = state.get(); // Correct - Uses the state by reference.
 * auto myT3 = state.get(); // WRONG - Same as the first case.
 * auto & myT4 = state.get(); // Correct - Same as the second case.
 * @endcode
 *
 * Road Map:
 * - Right now the auxilliary storage buffers uArr and udtArr are
 * always allocated. It should be possible to enable or disable this.
 * - Should this be thread safe?
 * - Would be interesting to name monitors.
 */
template<typename T>
class State {
protected:

	/**
	 * The managed instance of the State of type T.
	 */
	T state;

	/**
	 * The most recent value of t provided by State.t().
	 */
	double tVal;

	/**
	 * The system size of the state.
	 */
	long systemSize;

	/**
	 * This is a utility array that can be used by clients a buffer for holding
	 * state values if they cannot be directly mapped to a member on T.
	 */
	std::unique_ptr<double> uArr;

	/**
	 * This is a utility array that can be used by clients a buffer for holding
	 * state derivative values if they cannot be directly mapped to a member on
	 * T.
	 */
	std::unique_ptr<double> dudtArr;

	/**
	 * The list of monitors that should be notified when the state changes.
	 */
	std::vector<std::function<void(State<T>&)>> monitors;

	/**
	 * This function notifies the monitors.
	 */
    void notifyMonitors() {
    	// Nothing special - just call them.
    	for(auto const &monitor : monitors) {
           monitor(*this);
    	}
    }

public:

	/**
	 * Constructor
	 */
	State() : tVal(0.0), systemSize(0), state() {}

	/**
	 * Alternative constructor that also sets the system size.
	 * @param the number of unique data elements in the state
	 */
    State(const long & numElements) : State() {
    	size(numElements);
    }

    /**
     * An alternative default constructor that will accept a pre-configured
     * unique_ptr<T>. In general this constructor should not be called directly
     * by clients. It is used by the build<>() builder for State<T> to safely
     * and easily enable two-phase construction where the arguments can be
     * passed to T's constructor without requiring State<T> to know about those
     * arguments.
     *
     * This is important because requiring State<T> to know about the arguments
     * of T's constructor would require declaring those arguments in all client
     * classes of State<T> that used that version of T, which is an undue
     * burden on the user compared to safely enabling two-phase construction.
     */
    State(T domainState) : tVal(0.0), systemSize(0), state(std::move(domainState)) {}

    /**
     * An alternative constructor that also sets the system size. Analog of
     * State(const long &).
     */
    State(T domainState, const long & numElements) : State(domainState) {
    	systemSize = numElements;
    }

	/**
	 * This operation registers a function with the state that will be called
	 * when the state changes. Multiple functions may be registered with the
	 * State and each will be notified when the state is modified. The monitor
	 * may be implemented in many ways, but should only take a single
	 * State<T>& as an input an return void.
	 *
	 * When the monitoring function is called, it will be passed a reference
	 * to the instance of the State class that changed, not merely to the
	 * maintained type.
	 * @param monitor the functor that should be notified when the state
	 * changes.
	 */
	void addMonitor(std::function<void(State<T>&)> monitor) {
		// Just put the monitor on the list and try to call it later.
		monitors.push_back(monitor);
	}

	/**
	 * This operation returns the state at the most recent value of t.
	 * Please note the following nuances related to the use of "auto:"
	 * @code
	 * State<T> state;
	 * T & myT1 = state.get(); // Work
	 * auto & myT2 = state.get(); // Works - myT2 has type T &
	 * auto myT3 = state.get(); // Fails - myT3 has type T and is a copy
	 * @endcode
	 * @return state a reference to the state.
	 */
	T & get() {
		return state;
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
	double * u() {
		throw "Operation not implemented for this type.";
	}

	/**
	 * This function sets the values of the unknown quantities. It is the
	 * inverse of double * u() above and is meant to set the values, at the
	 * given value of t, for the same fundamental state vector for t_n >
	 * t_(n-1). So, for example, this operation could take an input array for
	 * t > t_(n-1) that represents the values of temperature just solved for in
	 * a coupled thermomechanics system. However, it would not take diffusion
	 * coefficient or other quantities.
	 *
	 * If the result of the getter State.u():double* (see above) is a pointer
	 * that is mapped to an array in the stored instance of T, then it is not
	 * necessary to call this operation because editing u* directly will update
	 * the state. However, in cases where it is desirable to either not map
	 * directly to the managed data structure or to use "test values" of u,
	 * then this function is a handy convenience function. For example,
	 * @code
	 * double * u = state.u()  // Get the current state
	 * double t = state.t()    // and t
	 * int size = state.size() // and size
	 *
	 * // Make some test updates to u
	 * double * myU = new double[size];
	 * for (int i = 0; i < size; i++) {
	 *     myU[i] = u[i] + delta();
	 *     ... bunch of other stuff...
	 * }
	 *
	 * // Submit the new values
	 * state.u(myU);
	 * double * myDudt = state.dudt();
	 *
	 * ... other stuff ...
	 *
	 * delete myU;
	 * @endcode
	 *
	 * The default implementation assumes that u is direct pointer to an array
	 * in the underlying type T, so implementations may need to override this
	 * operation if they do not map directly to the state instance.
	 * Furthermore, this operation assumes that the incoming array should be
	 * copied instead of referred to directly.
	 *
	 * @param data the values of the unknown state variables to be set. The
	 * size of this array is expected to be equal to State.size().
	 */
	void u(double * data) {
		// Update the state - not the fastest way to handle this, but good
		// enough for now.
		auto uLoc = u();
		for (int i = 0; i < systemSize; i++) {
			uLoc[i] = data[i];
		}
		// Notify the monitors that the state changed.
		notifyMonitors();
		return;
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
	double * dudt(const double & t) {
		throw "Operation not implemented for this type.";
	}

	/**
	 * This operation explicitly sets the number of unique data elements in the
	 * state. At present this value is constant for all values of t.
	 * present in the state.
	 * @param int the number of unique data elements in the state
	 */
	void size(const long & numElements) {
		systemSize = numElements;
		// Allocate the storage arrays for u and dudt
		uArr = std::unique_ptr<double>(new double[systemSize]);
		dudtArr = std::unique_ptr<double>(new double[systemSize]);
	};

	/**
	 * This operation returns the number of unique elements in the state. At
	 * present this value is constant for all values of t.
	 * This is the size of the arrays u and dudt.
	 * @return int the size of the state and the state arrays
	 */
	int size() const {return systemSize;}
};

/**
 * This is a builder for building instances of the State class.
 *
 * This builder can be used to initalize a State<T> as follows:
 * @code
 * State<MyClass> state = build<T>();
 * State<MyClass> state2 = build<T,int>(4); // Forwards integer value of 4 to
 * // the constructor of T.
 * State<MyClass> state2 = build<T,int>(4,5); // Forwards both an integer value of
 * // 4 to the constructor of T and a size of 5 to constructor of State.
 * @endcode
 *
 * @param Optional list of arguments that are forwarded to constructor of the
 * domain class T.
 * @param The default size of the State - the total number of elements in the
 * instance of T. This is an optional para
 * @return A State<T> class initialized with the arguments, if provided.
 */
template<typename T, typename... Args>
State<T> buildState(Args&& ...args, const long & size = 0) {
	State<T> stateT(T(std::forward<Args>(args)...),size);
	return stateT;
}

} /* namespace fire */

#endif /* SOLVERS_STATE_H_ */

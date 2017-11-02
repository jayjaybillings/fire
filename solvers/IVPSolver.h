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
#ifndef SOLVERS_IVPSOLVER_H_
#define SOLVERS_IVPSOLVER_H_

#include <State.h>
#include <fire_cvode_functions.h>

namespace fire {

/**
 * This class numerically integrates a set of Ordinary Differential Equations
 * \f[
 * \frac{d\vec{u}}{dt} = \vec{f}(t,\vec{u})
 * \f]
 * given
 * \f[
 * u(t=t_{0}), t_{0} \ge t \le t_{f}
 * \f]
 *
 * User state (read: values of u), including initial conditions, are provided
 * to the solver via the State class. The solver can be configured roughly as
 * follows (which is adapted from IVPSolverTest's checkSingleVariableSolve()):
 * @code
 *  // Configure the solver - it is templated on the same type as the State
 *  IVPSolver<T> solver;
 *  // Set current, initial and final times
 *  solver.t(t);
 *  solver.tInit(tInit);
 *  solver.tFinal(tFinal);
 *  // Execute the solver using the fully-configured State.
 *	solver.solve(state);
 * @endcode
 *
 * The maximum number of output steps that will be displayed at the end of the
 * solve can be configured by calling maxOutputSteps() with the desired number
 * of steps.
 *
 * The original and present implementation is based on CVODE's example_v2.c
 * with user-defined Jacobians disabled and other adaptations for fitness.
 *
 */
template<typename T>
class IVPSolver {

protected:

	/**
	 * The initial t value from which the solve should start.
	 */
	double initialT = 0.0;

	/**
	 * The final t value at which the solve should stop.
	 */
	double finalT = 0.0;

	/**
	 * The current value of t in the integration.
	 */
	double currentT = 0.0;

	/**
	 * The maximum number of "output" steps the solver should take. An
	 * output step is a step where output is produced by the solver at
	 * a regular user-specified interval. The solver may take more
	 * computational steps than output steps but will always report
	 * for a number of output steps required to meet the final time or
	 * the maximum number of output steps, whichever is less. The default
	 * value is 10.
	 */
	int maxNumOutputSteps = 10;

public:

	/**
	 * This operation sets the current value of t.
	 * @param the current value of t in the solver
	 */
	void t(const double & tVal) { currentT = tVal;};

	/**
	 * This operation sets the initial value of t
	 * @param the initial value of t in the solver
	 */
	void tInit(const double & tVal) { initialT = tVal;};

	/**
	 * This operation sets the final value of t
	 * @param the final value of t in the solver
	 */
	void tFinal(const double & tVal) { finalT = tVal;};

	/**
	 * This operation sets the maximum number of output steps in the solver.
	 * @param the number of maximum output steps the solver should produce.
	 */
	void maxOutputSteps(const int & steps) { maxNumOutputSteps = steps;};

	/**
	 * This operation returns the current value of t in the system.
	 * @param the current value of t in the solver
	 */
	double t() const {return currentT;};

	/**
	 * This operation returns the initial value of t configured for the solver.
	 * @param the initial value of t in the solver
	 */
	double tInit() const {return initialT;};

	/**
	 * This operation returns the final value of t configured for the solver.
	 * @param the final value of t in the solver
	 */
	double tFinal() const {return finalT;};

	/**
	 * This operation returns the maximum number of output steps that the
	 * solver will produce.
	 * @param the number of maximum output steps the solver will produce.
	 */
	int maxOutputSteps() { return maxNumOutputSteps;};

	/**
	 * This operation solves the system of equations specified in the State
	 * @param the State describing the system to be solved.
	 */
	void solve(State<T> & state) {

		// Begin CVODE code. Adapted from their examples.
		realtype dx, dy, reltol, abstol, tout, umax;
		N_Vector u;
		void *cvode_mem;
		int iout, flag;
		long int nst;

		u = NULL;
		cvode_mem = NULL;
		int size = state.size();

		/* Create a serial vector */
		u = N_VNew_Serial(size);
		/* Allocate the U vector */
		if (fire::cvode::check_flag((void*) u, "N_VNew_Serial", 0))
			return;

		reltol = ZERO; /* Set the tolerances */
		abstol = ATOL;

		// Set the initial conditions in u from the states
		fire::cvode::setICs<T>(u, state);

		/* Call CVodeCreate to create the solver memory and specify the
		 * Backward Differentiation Formula and the use of a Newton iteration */
		cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
		if (fire::cvode::check_flag((void *) cvode_mem, "CVodeCreate", 0))
			return;

		/* Call CVodeInit to initialize the integrator memory and specify the
		 * user's right hand side function in u'=f(t,u), the inital time T0, and
		 * the initial dependent variable vector u. */
		flag = CVodeInit(cvode_mem, fire::cvode::f<T>, initialT, u);
		if (fire::cvode::check_flag(&flag, "CVodeInit", 1))
			return;

		/* Call CVodeSStolerances to specify the scalar relative tolerance
		 * and scalar absolute tolerance */
		flag = CVodeSStolerances(cvode_mem, reltol, abstol);
		if (fire::cvode::check_flag(&flag, "CVodeSStolerances", 1))
			return;

		/* Set the pointer to user-defined data */
		flag = CVodeSetUserData(cvode_mem, &state);
		if (fire::cvode::check_flag(&flag, "CVodeSetUserData", 1))
			return;

		/* Call CVBand to specify the CVBAND band linear solver */
		flag = CVDense(cvode_mem, size);
		if (fire::cvode::check_flag(&flag, "CVDense", 1))
			return;

		/* Set the user-supplied Jacobian routine Jac */
//        flag = CVDlsSetBandJacFn(cvode_mem, Jac);
//        if(fire::cvode::check_flag(&flag, "CVDlsSetBandJacFn", 1)) return;

		/* In loop over output points: call CVode, print results, test for errors */
		umax = N_VMaxNorm(u);
		fire::cvode::PrintHeader(reltol, abstol, umax, initialT);
		for (iout = 1, tout = DTOUT; iout <= maxNumOutputSteps && currentT < finalT; iout++, tout += DTOUT) {
			flag = CVode(cvode_mem, tout, u, &currentT, CV_NORMAL);
			if (fire::cvode::check_flag(&flag, "CVode", 1))
				break;
			umax = N_VMaxNorm(u);
			flag = CVodeGetNumSteps(cvode_mem, &nst);
			fire::cvode::check_flag(&flag, "CVodeGetNumSteps", 1);
			// Update the state
			realtype * udata = NV_DATA_S(u);
			state.t(currentT);
			state.u(udata);
			// Replace! - Notify observers that the state has changed
			fire::cvode::PrintOutput(currentT, umax, nst);
		}

		fire::cvode::PrintFinalStats(cvode_mem); /* Print some final statistics   */

		N_VDestroy_Serial(u); /* Free the u vector */
		CVodeFree(&cvode_mem); /* Free the integrator memory */

		return;
	}

};

} /* namespace fire */

#endif /* SOLVERS_IVPSOLVER_H_ */

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
#include <cvode/cvode.h>             /* prototypes for CVODE fcts., consts. */
#include <cvode/cvode_dense.h>        /* prototype for CVBand */
#include <nvector/nvector_serial.h>  /* serial N_Vector types, fcts., macros */
#include <sundials/sundials_band.h>  /* definitions of type DlsMat and macros */
#include <sundials/sundials_types.h> /* definition of type realtype */
#include <sundials/sundials_math.h>  /* definition of ABS and EXP */

namespace fire {

namespace cvode {

/* Problem Constants */
#define XMAX  RCONST(2.0)    /* domain boundaries         */
#define YMAX  RCONST(1.0)
#define MX    10             /* mesh dimensions           */
#define MY    5
#define NEQ   MX*MY          /* number of equations       */
#define ATOL  RCONST(1.0e-5) /* scalar absolute tolerance */
#define DTOUT RCONST(0.1)    /* output time increment     */

#define ZERO RCONST(0.0)
#define HALF RCONST(0.5)
#define ONE  RCONST(1.0)
#define TWO  RCONST(2.0)
#define FIVE RCONST(5.0)

/* Private Helper Functions */
static void PrintHeader(realtype reltol, realtype abstol, realtype umax, double t);
static void PrintOutput(const realtype & t, const realtype & umax, const long int & nst);
static void PrintFinalStats(void *cvode_mem);

/* Private function to check function return values */
static int check_flag(void *flagvalue, char *funcname, int opt);

/*
 *-------------------------------
 * Functions called by the solver
 *-------------------------------
 */

/**
 * This operation pulls the initial conditions from the state and provides them
 * to the solver.
 * @param the vector u that should be filled with initial conditions for the solver.
 * @param the user-provided state that will be queried for initial conditions.
 */
template<typename T>
void setICs(N_Vector u, State<T> & state) {
	  int i, j;
	  realtype *udata;

	  /* Set pointer to data array in vector u. */
	  udata = NV_DATA_S(u);

	  // Get the user's state data.
	  auto * stateU = state.u();
	  int size = state.size();
	  // Copy the initial state from the state structure to the u vector.
	  for (int i = 0; i < size; i++) {
		  udata[i] = stateU[i];
	  }

	  return;
}

/**
 * This is the basic function f(t,u) in du/dt = f(t,u). It provides values
 * for the derivatives as required by CVODE.
 * @param the time
 * @param the vector u of current state values in the system
 * @param the vector udot also known as du/dt. This array is updated in place
 * with the values of f(t,u).
 * @param the user data, which is reinterpreted as State<T>.
 */
template<typename T>
int f(realtype t, N_Vector u,N_Vector udot, void *user_data) {
  realtype *udata, *dudata;
  int i, j;
  State<T> * state = reinterpret_cast<State<T> *>(user_data);

  udata = NV_DATA_S(u);
  dudata = NV_DATA_S(udot);

  // Update the state with the test values of u.
  state->u(udata);

  // Compute the time derivatives
  auto * rhs = state->dudt(t);
  int size = state->size(); // FIXME!
  for (j=0; j < size; j++) {
      dudata[j] = rhs[j];
  }

  return(0);
}

/*
 *-------------------------------
 * Private helper functions
 *-------------------------------
 */

// ---------------- Added to system or state, remove from here ----------- //
/* Print first lines of output (problem description) */
static void PrintHeader(realtype reltol, realtype abstol, realtype umax, double t)
{
	// Would be nice for the header to be configurable by users.
	printf("\nInitial Value Problem Solver Initializing\n");
#if defined(SUNDIALS_EXTENDED_PRECISION)
	printf("Tolerance parameters: reltol = %Lg   abstol = %Lg\n\n",
			reltol, abstol);
	printf("At t = %Lg      max.norm(u) =%14.6Le \n", T0, umax);
#elif defined(SUNDIALS_DOUBLE_PRECISION)
	printf("Tolerance parameters: reltol = %g   abstol = %g\n\n", reltol,
			abstol);
	printf("At t = %g      max.norm(u) =%14.6e \n", t, umax);
#else
	printf("Tolerance parameters: reltol = %g   abstol = %g\n\n", reltol, abstol);
#endif

	return;
}

// -------------- FIXME! Replace with observer --------------- //
/* Print current value */
static void PrintOutput(const realtype & t, const realtype & umax, const long int & nst)
{
#if defined(SUNDIALS_EXTENDED_PRECISION)
  printf("At t = %4.2Lf   max.norm(u) =%14.6Le   nst = %4ld\n", t, umax, nst);
#elif defined(SUNDIALS_DOUBLE_PRECISION)
  printf("At t = %4.2f   max.norm(u) =%14.6e   nst = %4ld\n", t, umax, nst);
#else
  printf("At t = %4.2f   max.norm(u) =%14.6e   nst = %4ld\n", t, umax, nst);
#endif

  return;
}

//----------- Replace with metrics tracking! ---------------///
/* Get and print some final statistics */
static void PrintFinalStats(void *cvode_mem)
{
  int flag;
  long int nst, nfe, nsetups, netf, nni, ncfn, nje, nfeLS;

  flag = CVodeGetNumSteps(cvode_mem, &nst);
  check_flag(&flag, "CVodeGetNumSteps", 1);
  flag = CVodeGetNumRhsEvals(cvode_mem, &nfe);
  check_flag(&flag, "CVodeGetNumRhsEvals", 1);
  flag = CVodeGetNumLinSolvSetups(cvode_mem, &nsetups);
  check_flag(&flag, "CVodeGetNumLinSolvSetups", 1);
  flag = CVodeGetNumErrTestFails(cvode_mem, &netf);
  check_flag(&flag, "CVodeGetNumErrTestFails", 1);
  flag = CVodeGetNumNonlinSolvIters(cvode_mem, &nni);
  check_flag(&flag, "CVodeGetNumNonlinSolvIters", 1);
  flag = CVodeGetNumNonlinSolvConvFails(cvode_mem, &ncfn);
  check_flag(&flag, "CVodeGetNumNonlinSolvConvFails", 1);

  flag = CVDlsGetNumJacEvals(cvode_mem, &nje);
  check_flag(&flag, "CVDlsGetNumJacEvals", 1);
  flag = CVDlsGetNumRhsEvals(cvode_mem, &nfeLS);
  check_flag(&flag, "CVDlsGetNumRhsEvals", 1);

  printf("\nFinal Statistics:\n");
  printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld nfeLS = %-6ld nje = %ld\n",
	 nst, nfe, nsetups, nfeLS, nje);
  printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n",
	 nni, ncfn, netf);

  return;
}

// ------------------- KEEP! ---------------- //
// Eventually replace with error handling.
/* Check function return value...
     opt == 0 means SUNDIALS function allocates memory so check if
              returned NULL pointer
     opt == 1 means SUNDIALS function returns a flag so check if
              flag >= 0
     opt == 2 means function allocates memory so check if returned
              NULL pointer */
static int check_flag(void *flagvalue, char *funcname, int opt)
{
  int *errflag;

  /* Check if SUNDIALS function returned NULL pointer - no memory allocated */

  if (opt == 0 && flagvalue == NULL) {
    fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return(1); }

  /* Check if flag < 0 */

  else if (opt == 1) {
    errflag = (int *) flagvalue;
    if (*errflag < 0) {
      fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with flag = %d\n\n",
              funcname, *errflag);
      return(1); }}

  /* Check if function returned NULL pointer - no memory allocated */

  else if (opt == 2 && flagvalue == NULL) {
    fprintf(stderr, "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return(1); }

  return(0);
}

} /* namespace cvode */

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

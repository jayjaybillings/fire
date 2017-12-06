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
#ifndef SOLVERS_FIRE_CVODE_FUNCTIONS_H_
#define SOLVERS_FIRE_CVODE_FUNCTIONS_H_

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

/* Helper Functions */
void PrintHeader(realtype reltol, realtype abstol, realtype umax, double t);
void PrintOutput(const realtype & t, const realtype & umax, const long int & nst);
void PrintFinalStats(void *cvode_mem);
/* Function to check function return values */
int check_flag(void *flagvalue, char *funcname, int opt);

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

} /* end namespace cvode */
} /* end namespace fire */

#endif /* SOLVERS_FIRE_CVODE_FUNCTIONS_H_ */

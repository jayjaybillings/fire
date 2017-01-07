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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Astro

#if defined __GNUC__ && __GNUC__>=6
  #pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <INIPropertyParser.h>
#include <ReactionNetwork.h>
#include <ODESolver.h>
#include <StringCaster.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Header files with a description of contents used in cvbanx.c */

#include <cvode/cvode.h>             /* prototypes for CVODE fcts., consts. */
#include <cvode/cvode_dense.h>        /* prototype for CVBand */
#include <nvector/nvector_serial.h>  /* serial N_Vector types, fcts., macros */
#include <sundials/sundials_band.h>  /* definitions of type DlsMat and macros */
#include <sundials/sundials_types.h> /* definition of type realtype */
#include <sundials/sundials_math.h>  /* definition of ABS and EXP */

/* Problem Constants */

#define XMAX  RCONST(2.0)    /* domain boundaries         */
#define YMAX  RCONST(1.0)
#define MX    10             /* mesh dimensions           */
#define MY    5
#define NEQ   MX*MY          /* number of equations       */
#define ATOL  RCONST(1.0e-5) /* scalar absolute tolerance */
#define T0    RCONST(0.0)    /* initial time              */
#define T1    RCONST(0.1)    /* first output time         */
#define DTOUT RCONST(0.1)    /* output time increment     */
#define NOUT  10             /* number of output times    */

#define ZERO RCONST(0.0)
#define HALF RCONST(0.5)
#define ONE  RCONST(1.0)
#define TWO  RCONST(2.0)
#define FIVE RCONST(5.0)

using namespace std;
using namespace fire;
using namespace fire::astrophysics;

static const string & propertyFileName = "alpha_gold.ini";

/* Private Helper Functions */

static void SetIC(N_Vector u, ReactionNetwork & data);
static void PrintHeader(realtype reltol, realtype abstol, realtype umax);
static void PrintOutput(realtype t, realtype umax, long int nst);
static void PrintFinalStats(void *cvode_mem);

/* Private function to check function return values */

static int check_flag(void *flagvalue, char *funcname, int opt);

/* Functions Called by the Solver */

static int f(realtype t, N_Vector u, N_Vector udot, void *user_data);

/*
 *-------------------------------
 * Functions called by the solver
 *-------------------------------
 */

/* f routine. Compute f(t,u). */

static int f(realtype t, N_Vector u,N_Vector udot, void *user_data) {
  realtype *udata, *dudata;
  int i, j;
  ReactionNetwork * network;

  udata = NV_DATA_S(u);
  dudata = NV_DATA_S(udot);

  /* Extract needed constants from data */

  network = (ReactionNetwork *) user_data;

  // Update the solution values from the array
  for (j=0; j < network->species->size(); j++) {
      network->species->at(j).massFraction = udata[j];
  }

  // Compute the fluxes
  network->computeFluxes();

  // Update the derivative values in the array
  cout << t;
  for (j=0; j < network->species->size(); j++) {
      dudata[j] = network->species->at(j).flux;
      cout << ", " << udata[j];
  }
  cout << endl;

  return(0);
}

/*
 *-------------------------------
 * Private helper functions
 *-------------------------------
 */

/* Set initial conditions in u vector */

static void SetIC(N_Vector u, ReactionNetwork & network)
{
  int i, j;
  realtype *udata;

  /* Set pointer to data array in vector u. */

  udata = NV_DATA_S(u);

  /* Load initial profile into u vector */

  for (j=0; j < network.species->size(); j++) {
    udata[j] = network.species->at(j).massFraction;
    cout << network.species->at(j).massFraction << endl;
  }
}

/* Print first lines of output (problem description) */

static void PrintHeader(realtype reltol, realtype abstol, realtype umax)
{
  printf("\n2-D Advection-Diffusion Equation\n");
  printf("Mesh dimensions = %d X %d\n", MX, MY);
  printf("Total system size = %d\n", NEQ);
#if defined(SUNDIALS_EXTENDED_PRECISION)
  printf("Tolerance parameters: reltol = %Lg   abstol = %Lg\n\n",
         reltol, abstol);
  printf("At t = %Lg      max.norm(u) =%14.6Le \n", T0, umax);
#elif defined(SUNDIALS_DOUBLE_PRECISION)
  printf("Tolerance parameters: reltol = %g   abstol = %g\n\n",
         reltol, abstol);
  printf("At t = %g      max.norm(u) =%14.6e \n", T0, umax);
#else
  printf("Tolerance parameters: reltol = %g   abstol = %g\n\n", reltol, abstol);
  printf("At t = %g      max.norm(u) =%14.6e \n", T0, umax);
#endif

  return;
}

/* Print current value */

static void PrintOutput(realtype t, realtype umax, long int nst)
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

/**
 * This operation checks the ability of the Network to load itself properly
 * from an input parameter file.
 */
BOOST_AUTO_TEST_CASE(checkLoading) {

	// Load the properties
	INIPropertyParser parser = build<INIPropertyParser,const string &>(propertyFileName);

	// Create the network
	ReactionNetwork network;
	// Set the properties from the property block and load the network
	auto props = parser.getPropertyBlock("network");
	network.setProperties(props);
	// Load the remaining data
	network.load();
	// Compute the prefactors and rates
	props = parser.getPropertyBlock("initialConditions");
	double temperature = StringCaster<double>::cast(props.at("T9"));
	double density = StringCaster<double>::cast(props.at("density"));
	network.computePrefactors(density);
	network.computeRates(temperature);

	// Delete later
    foo myFoo;

    realtype dx, dy, reltol, abstol, t, tout, umax;
    N_Vector u;
    void *cvode_mem;
    int iout, flag;
    long int nst;

    u = NULL;
    cvode_mem = NULL;

    /* Create a serial vector */

    u = N_VNew_Serial(network.species->size());  /* Allocate u vector */
    if(check_flag((void*)u, "N_VNew_Serial", 0)) return;

    reltol = ZERO;  /* Set the tolerances */
    abstol = ATOL;

    // Set the initial conditions
    SetIC(u, network);  /* Initialize u vector */

    /* Call CVodeCreate to create the solver memory and specify the
     * Backward Differentiation Formula and the use of a Newton iteration */
    cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
    if(check_flag((void *)cvode_mem, "CVodeCreate", 0)) return;

    /* Call CVodeInit to initialize the integrator memory and specify the
     * user's right hand side function in u'=f(t,u), the inital time T0, and
     * the initial dependent variable vector u. */
    flag = CVodeInit(cvode_mem, f, T0, u);
    if(check_flag(&flag, "CVodeInit", 1)) return;

    /* Call CVodeSStolerances to specify the scalar relative tolerance
     * and scalar absolute tolerance */
    flag = CVodeSStolerances(cvode_mem, reltol, abstol);
    if (check_flag(&flag, "CVodeSStolerances", 1)) return;

    /* Set the pointer to user-defined data */
    flag = CVodeSetUserData(cvode_mem, &network);
    if(check_flag(&flag, "CVodeSetUserData", 1)) return;

    /* Call CVBand to specify the CVBAND band linear solver */
    flag = CVDense(cvode_mem, network.species->size());
    if(check_flag(&flag, "CVDense", 1)) return;

    /* Set the user-supplied Jacobian routine Jac */
//    flag = CVDlsSetBandJacFn(cvode_mem, Jac);
//    if(check_flag(&flag, "CVDlsSetBandJacFn", 1)) return;

    /* In loop over output points: call CVode, print results, test for errors */

    umax = N_VMaxNorm(u);
    PrintHeader(reltol, abstol, umax);
    for(iout=1, tout=T1; iout <= NOUT; iout++, tout += DTOUT) {
      flag = CVode(cvode_mem, tout, u, &t, CV_NORMAL);
      if(check_flag(&flag, "CVode", 1)) break;
      umax = N_VMaxNorm(u);
      flag = CVodeGetNumSteps(cvode_mem, &nst);
      check_flag(&flag, "CVodeGetNumSteps", 1);
      PrintOutput(t, umax, nst);
    }

    PrintFinalStats(cvode_mem);  /* Print some final statistics   */

    N_VDestroy_Serial(u);   /* Free the u vector */
    CVodeFree(&cvode_mem);  /* Free the integrator memory */

	// Good enough for government work
	return;
}

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
#ifndef INTEGRATORS_ISTEPPER_H_
#define INTEGRATORS_ISTEPPER_H_

namespace fire {

/**
 * This is an interface for managing discrete steps as required by Initial
 * Value Problem Integrators. IStepper implementations track both the step and
 * the stepsize. In the case of time-dependent initial value problems,
 * implementations would be called "time steppers" and would be responsible for
 * computing both the present time (the step) and the step between the present
 * size and the next (the step size). In purely spatial or otherwise
 * on-temporal problems, the step represents the current value of the
 * independent variable and the step size represents the distance between the
 * present and next steps.
 *
 * This interface is designed for single- and multi-stage solvers, such as
 * Runge-Kutta solvers.
 */
class IStepper {
public:

	/**
	 * This operation returns the step value for the current step.
	 * @return the step value
	 */
	virtual double getStep() = 0;

	/**
	 * This operation returns the step size for the given stage.
	 * @param the stage of the solver for which the stepsize should be computed
	 * @return the step size
	 */
	virtual double getStepSizeAtStage(int i) = 0;

	/**
	 * This operation replaces the current step and stepsize with the next step
	 * and stepsize values.
	 */
	virtual void updateStep() = 0;

    /**
     * This operation sets the initial step for the stepper.
     * @param initialStep the initial step
     */
    virtual void setInitialStep(double initialStep) = 0;

	/**
	 * This operation returns the initial step for the stepper.
	 * @return the initial step
	 */
	virtual double getInitialStep() = 0;

	/**
	 * This operation sets the final step for the stepper.
	 * @param finalStep the final step
	 */
	virtual void setFinalStep(double finalStep) = 0;

	/**
	 * This operation returns the final step for the stepper.
	 * @return the final step
	 */
	virtual double getFinalStep() = 0;

	/**
	 * This operation sets the initial step size for the stepper
	 * @param the initial step size
	 */
	virtual void setInitialStepsize(double stepSize) = 0;

	/**
	 * This operation gets the initial step size for the stepper
	 * @return the initial step size
	 */
	virtual double getInitialStepsize() = 0;

	/**
	 * Virtual Destructor
	 */
	virtual ~IStepper() {};

};

} /* namespace fire */

#endif /* INTEGRATORS_ISTEPPER_H_ */

/**----------------------------------------------------------------------------
 Copyright (c) 2015-, Jay Jay Billings
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
#ifndef INTEGRATORS_PROFILESTEPPER_H_
#define INTEGRATORS_PROFILESTEPPER_H_

#include "IStepper.h"

namespace fire {

/**
 * This class is a Stepper that pulls its steps and step sizes from two
 * matching lists. It can be used for restricting the stepping to a
 * pre-determined profile, which is useful for coupling and testing.
 */
class ProfileStepper: public IStepper {

protected:

	/**
	 * The lists of steps
	 */
	const std::vector<double> & steps;

	/**
	 * The list of step sizes - the distances between the steps
	 */
	const std::vector<double> & stepSizes;

	/**
	 * The id number of the current staff
	 */
	int stepID = 0;

public:

	/**
	 * Constructor
	 */
	ProfileStepper(const std::vector<double> & stepsList,
			const std::vector<double> & stepSizeList) :
			steps(stepsList), stepSizes(stepSizeList) {};

	/**
	 * Destructor
	 */
	virtual ~ProfileStepper() {};

	virtual double getStep() {
		return steps[stepID];
	};

	virtual double getStepSizeAtStage(int i) {
		return stepSizes[stepID];
	};

	virtual void updateStep() {
		++stepID;
	};

};

} /* namespace fire */

#endif /* INTEGRATORS_PROFILESTEPPER_H_ */

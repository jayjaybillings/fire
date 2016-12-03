/**----------------------------------------------------------------------------
 Copyright (c) 2016-, UT-Battelle LLC
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

 Author(s): Alex McCaskey (mccaskeyaj <at> ornl <dot> gov)
 -----------------------------------------------------------------------------*/

#ifndef TENSORS_TENSOR_HPP_
#define TENSORS_TENSOR_HPP_

namespace fire {

/**
 * This class provides a customizable abstraction for tensor-structured
 * data and associated operations. It is template parameterized by a
 * TensorProvider implementation - an abstraction that is itself a Tensor
 * (and thus provides implementations for all Tensor methods) but provides
 * a concrete C++ Tensor library to achieve desired Tensor functionality.
 */
template<typename DerivedTensorProvider = void>
class Tensor {

public:

	Tensor() {

		// If we have been provided with a Parameter that is not
		// TensorProvider, while at the same time it is not void
		// (here void indicates that this Tensor is a TensorProvider),
		// then throw a compiler error
		static_assert(!std::is_void<DerivedTensorProvider>::value &&
				!std::is_base_of<TensorProvider, DerivedTensorProvider>::value,
				"The provided Tensor template parameter is not derived "
				"from TensorProvider. Compilation Error.");

		// If the provider is not void the instatiate the provider
		if (!std::is_void<DerivedTensorProvider>::value) provider = std::make_shared<DerivedTensorProvider>();
	}

	virtual Tensor& contract(Tensor& other) {
		return provider->contract(other);
	}

	virtual ~Tensor() {}

private:

	std::shared_ptr<DerivedTensorProvider> provider;
};

}

#endif

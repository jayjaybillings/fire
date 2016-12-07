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

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <boost/variant.hpp>
#include <numeric>
#include "EigenTensorProvider.hpp"

namespace fire {


/**
 * The Tensor class provides an extensible data structure describing
 * tensors. It realizes the ITensor interface but delegates all
 * actual tensor-work to a user-specified TensorProvider. This class enables
 * the expression of Tensors and operations on Tensors in a familiar way that is
 * extensible to any number of 3rd party tensor algebra libraries.
 */
template<typename Scalar, int Rank, typename Builder = EigenTensor>
class Tensor : public virtual ITensor {

private:

	// This is some cool voodoo...
	// Setting the type of the Provider based on the provided Builder.
	using Provider = decltype(Builder().template build<Scalar, Rank>());

	/**
	 * Reference to the TensorProvider backing this Tensor.
	 */
	std::shared_ptr<Provider> provider;

public:

	/**
	 * The constructor for creating Tensors of any shape. The number
	 * of provided dimensions in this variadic constructor must
	 * be equal to the Tensor's Rank template parameter.
	 *
	 * @param firstDim The dimension of the first rank
	 * @param otherDims The dimension of the remaining ranks
	 */
	template<typename... Dimension>
	Tensor(int firstDim, Dimension ... otherDims) {
		// Assert that our input parameters are consistent
		static_assert( sizeof...(otherDims) + 1 == Rank, "Incorrect number of dimension integers");
		static_assert( std::is_fundamental<Scalar>::value, "Fire Tensors can only contain C++ fundamental types (double, int, etc)." );
		static_assert( std::is_base_of<ProviderBuilder, Builder>::value, "Third Tensor Template Parameter must be of type fire::ProviderBuilder.");

		// Create the requested TensorProvider
		provider = std::make_shared<Provider>(firstDim, otherDims...);
	}

	template<typename... Indices>
	Scalar operator() (Indices... indices) const {
	}

	/**
	 * See ITensor::contract for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @param other The other Tensor to contract with
	 * @param dimensions The indices to contract
	 * @return result The result of the contraction.
	 */
	virtual ITensor& contract(ITensor& other, std::vector<std::pair<int,int>>& dimensions) {
		return provider->contract(other, dimensions);
	}

	/**
	 * See ITensor::add for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @param other Tensor to add to this one.
	 * @param scale Scale factor to multiply other tensor by before addition.
	 */
	virtual void add(ITensor& other, double scale = 1.0) {
		 provider->add(other);
	}

	/**
	 * See ITensor::norm1 for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @return norm The 1-norm value
	 */
	virtual double norm1() {
		return provider->norm1();
	}

	/**
	 * See ITensor::norm2 for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @return norm The 2-norm value
	 */
	virtual double norm2() {
		return provider->norm1();
	}

	/**
	 * The destructor
	 */
	virtual ~Tensor() {}


};

}

#endif

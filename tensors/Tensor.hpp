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

#include "TensorProvider.hpp"

namespace fire {

/**
 *
 */
template<const int Rank, typename Scalar = double,
		typename DerivedTensorBackendBuilder = fire::EigenBuilder>
class Tensor {

protected:

	using DerivedTensorBackend = decltype(DerivedTensorBackendBuilder().template build<Rank, Scalar>());

	/**
	 */
	std::shared_ptr<DerivedTensorBackend> provider;

	/**
	 *
	 */
	std::shared_ptr<TensorShape> shape;

	/**
	 *
	 * @param data
	 * @param newShape
	 */
	Tensor(TensorReference& reference) {

		auto newShape = reference.second;
		auto data = reference.first.data();

		shape = std::make_shared<TensorShape>(newShape.dimensions());

		// Create the requested TensorProvider
		provider = std::make_shared<DerivedTensorBackend>();
		if (!provider)
			throw "Could not find provider";

		provider->initializeFromData(data, newShape);

	}

public:

	/**
	 * The constructor for creating Tensors of any shape. The number
	 * of provided dimensions in this variadic constructor must
	 * be equal to the Tensor's Rank template parameter.
	 *
	 * @param firstDim The dimension of the first rank
	 * @param otherDims The dimension of the remaining ranks
	 */
	template<typename ... Dimension>
	Tensor(int firstDim, Dimension ... otherDims) {

		// assert rank is right number

		shape = std::make_shared<TensorShape>(firstDim, otherDims...);

		// Create the requested TensorProvider
		provider = std::make_shared<DerivedTensorBackend>();
		if (!provider)
			throw "Could not find provider";

		provider->initialize(firstDim, otherDims...);
	}

	/**
	 *
	 * @param indices
	 * @return
	 */
	template<typename ... Indices>
	double& operator()(Indices ... indices) const {
		return provider->template coeff<Scalar>(indices...);
	}

	/**
	 *
	 * @param other
	 * @return
	 */
	Tensor<Rank, Scalar, DerivedTensorBackendBuilder> operator+(
			Tensor<Rank, Scalar, DerivedTensorBackendBuilder>& other) {
		// Create a reference for other
		auto otherReference = other.createReference();
		auto ref = provider->addTensors(otherReference);
		Tensor<Rank, Scalar, DerivedTensorBackendBuilder> result(ref);
		return result;
	}

	bool operator==(Tensor<Rank, Scalar, DerivedTensorBackendBuilder>& other) {
		auto ref = other.createReference();
		return provider->equalTensors(ref);
	}

	bool operator!=(Tensor<Rank, Scalar, DerivedTensorBackendBuilder>& other) {
		return !operator==(other);
	}

	template<typename OtherDerived, typename ContractionDims>
	Tensor<
			DerivedTensorBackend::getRank() + OtherDerived::getRank()
					- 2 * array_size<ContractionDims>::value, Scalar,
			DerivedTensorBackendBuilder> contract(OtherDerived& other,
			ContractionDims& indices) {

		// Compute new Tensor rank
		static constexpr int newRank = DerivedTensorBackend::getRank()
				+ OtherDerived::getRank()
				- 2 * array_size<ContractionDims>::value;

		// Compute the contraction, get reference data on new Tensor
		auto ref = provider->contract(other, indices);

		Tensor<newRank, Scalar, DerivedTensorBackendBuilder> result(ref);

		return result;
	}

	template<typename OtherDerived>
	void operator*(OtherDerived& other) {
	}

	void setRandom() {
		provider->setRandomValues();
	}

	/**
	 *
	 * @param index
	 * @return
	 */
	int dimension(int index) {
		return shape->dimension(index);
	}

	/**
	 *
	 * @return
	 */
	static constexpr int getRank() {
		return DerivedTensorBackend::getRank();
	}

	TensorReference createReference() {
		auto ref = fire::make_tensor_reference(
				provider->template getTensorData<Scalar>(), *shape.get());
		return ref;
	}

	/**
	 * The destructor
	 */
	virtual ~Tensor() {
	}

};

}

#endif


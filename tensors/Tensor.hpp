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
 * The Tensor class provides an abstraction for data and operations on
 * general multi-dimensional arrays, otherwise known as tensors. It provides
 * a high-level API for tensor data composition and operations but delegates all work to
 * a provided 3rd party tensor algebra backend (eigen, TAL-SH, etc). By default,
 * Tensor uses Eigen's unsupported Tensor module as its backend.
 *
 * Tensor relies on three template parameters - the rank of the tensor, the
 * type of data this tensor contains (double by default), and the 3rd party
 * backend tensor provider (Eigen by default).
 *
 * Furthermore, Tensors also keep track of their TensorShape - a data structure that
 * encapsulates knowledge of the number of dimensions along each rank.
 *
 * Tensors can be used by clients in the following manner:
 *
 * @code
 * Tensor<2> matrix(2,2); // A 2x2 matrix of doubles backed by Eigen
 * Tensor<3, MyTensorBackend> tensor(1,2,3); // A rank-3 tensor with shape (1,2,3) backed by MyTensorBackend
 * Tensor<4, Eigen, float> tensor(2,2,3,3); // A rank-4 tensor with shape (2,2,3,3) backed by Eigen
 * @endcode
 *
 * Note, due to C++ template parameter syntax, if you want to specify the tensor data type
 * (something other than double) you must also specify the tensor provider backend as the
 * second tensor parameter.
 */
template<const int Rank,
		typename DerivedTensorBackendBuilder = fire::EigenProvider,
		typename Scalar = double>
class Tensor {

protected:

	// Get reference to the type the provided builder builds.
	using DerivedTensorBackend = decltype(DerivedTensorBackendBuilder().template build<Rank, Scalar>());

	// This makes typing easier...
	using ThisTensorType = Tensor<Rank, DerivedTensorBackendBuilder, Scalar>;

	/**
	 * Reference to the backend tensor algebra provider.
	 */
	std::shared_ptr<DerivedTensorBackend> provider;

	/**
	 * Reference to this Tensors shape
	 */
	std::shared_ptr<TensorShape> shape;

public:

	/**
	 * The constructor, takes a TensorReference
	 * which encapsulates the 1-D array of tensor
	 * data and the tensors shape.
	 * @param data
	 * @param newShape
	 */
	Tensor(TensorReference& reference) {

		// Create a new TensorShape and set it as this Tensor's shape
		shape = std::make_shared<TensorShape>(reference.second.dimensions());

		// Create the requested TensorProvider
		provider = std::make_shared<DerivedTensorBackend>();
		if (!provider)
			throw "Could not find provider";

		// Initialize the backend tensor provider with
		// the provided tensor data and shape
		provider->initializeFromReference(reference);

	}

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

		// Assert at compile time that this will be a valid Tensor
		static_assert( sizeof...(otherDims) + 1 == Rank, "Incorrect number of dimension integers");
		static_assert( std::is_fundamental<Scalar>::value, "Fire Tensors can only contain C++ fundamental types (double, int, etc)." );
		static_assert( std::is_base_of<ProviderBuilder, DerivedTensorBackendBuilder>::value, "Third Tensor Template Parameter must be of type fire::ProviderBuilder.");

		// Create the TensorShape from the given dimensions
		shape = std::make_shared<TensorShape>(firstDim, otherDims...);

		// Create the requested TensorProvider
		provider = std::make_shared<DerivedTensorBackend>();
		if (!provider)
			throw "Could not find provider";

		// Initialize the backend tensor provider
		provider->initialize(firstDim, otherDims...);
	}

	/**
	 * Return the value at the given set of
	 * tensor indices.
	 *
	 * @param indices The indices for the desired value
	 * @return val The value at the indices.
	 */
	template<typename ... Indices>
	Scalar& operator()(Indices ... indices) const {
		static_assert( sizeof...(indices) == Rank, "Incorrect number of indices");
		return provider->template coeff<Scalar>(indices...);
	}

	/**
	 * Add the given Tensor to this Tensor and return a new Tensor
	 *
	 * @param other The tensor to add to this one
	 * @return result A new tensor representing the sum of this and other.
	 */
	ThisTensorType operator+(ThisTensorType& other) {
		// Create a reference for other
		auto otherReference = other.createReference();
		auto ref = provider->addTensors(otherReference);
		Tensor<Rank, DerivedTensorBackendBuilder, Scalar> result(ref);
		return result;
	}

	/**
	 * Return true if this Tensor is equal to the provided other Tensor.
	 * Here equality means same rank, same dimension, and all values
	 * at corresponding indices equal.
	 *
	 * @param other The tensor to check equality against.
	 * @return equal A boolean indicating if these Tensors are equal
	 */
	bool operator==(ThisTensorType& other) {
		auto ref = other.createReference();
		return provider->equalTensors(ref);
	}

	/**
	 * Return true if this Tensor is not equal to the provided other Tensor.
	 * See operator==() for definition of equal.
	 *
	 * @param other The other Tensor to check not equal against
	 * @return notEqual A boolean indicating if these Tensors are not equal
	 *
	 */
	bool operator!=(ThisTensorType& other) {
		return !operator==(other);
	}

	/**
	 * This operation performs Tensor contraction between this Tensor
	 * and the provided other Tensor resulting in a new Tensor of
	 * appropriate rank. It requires a std::array of integer pairs indicating
	 * which indices between the two tensor are to be contracted. Passing
	 * an empty set of contraction indices here will result in the
	 * computation of the tensor product.
	 *
	 * @param other The other Tensor to contract with.
	 * @param indices The contraction indices.
	 * @return result The result Tensor of this contraction
	 */
	template<typename OtherDerived, typename ContractionDims>
	Tensor<
			DerivedTensorBackend::getRank() + OtherDerived::getRank()
					- 2 * array_size<ContractionDims>::value,
			DerivedTensorBackendBuilder, Scalar> contract(OtherDerived& other,
			ContractionDims& indices) {

		// Compute new Tensor rank
		static constexpr int newRank = DerivedTensorBackend::getRank()
				+ OtherDerived::getRank()
				- 2 * array_size<ContractionDims>::value;

		// Compute the contraction, get reference data on new Tensor
		auto ref = provider->contract(other, indices);

		// Create the result from the TensorReference
		Tensor<newRank, DerivedTensorBackendBuilder, Scalar> result(ref);

		return result;
	}

	/**
	 * This operator performs the tensor product operation on thi s
	 * Tensor and the provided other tensor.
	 * @param other
	 */
	template<typename OtherDerived>
	Tensor<DerivedTensorBackend::getRank() * OtherDerived::getRank(),
			DerivedTensorBackendBuilder, Scalar> operator*(
			OtherDerived& other) {
		auto emptyIndices = std::array<std::pair<int, int>, 0> { { } };
		return contract(other, emptyIndices);
	}

	/**
	 * This method sets all tensor values to a random number.
	 */
	void setRandom() {
		provider->setRandomValues();
	}

	/**
	 * Return the dimension of the provided rank index.
	 *
	 * @param index The index of the tensor rank
	 * @return dim The dimension of the rank
	 */
	int dimension(int index) {
		return shape->dimension(index);
	}

	/**
	 * Return the rank of this Tensor
	 *
	 * @return
	 */
	static constexpr int getRank() {
		return DerivedTensorBackend::getRank();
	}

	/**
	 * Return a TensorReference view for this Tensor.
	 *
	 * @return
	 */
	TensorReference createReference() {
		auto ref = fire::make_tensor_reference(
				provider->template getTensorData<Scalar>(), *shape.get());
		return ref;
	}

	/**
	 * Set the tensor values using nested initializer_list. For a Tensor of
	 * rank N, this method takes an std::initializer_list with N nested
	 * std::initializer_lists. The deepest nested list should contain M scalars
	 * where M is the size of the LAST dimension of the Tensor. An example:
	 *
	 * @code
	 * Tensor<2> tensor(2,3);
	 * tensor.setValues({{1, 2, 3},{4, 5, 6}});
	 * @endcode
	 *
	 * @param vals The values as a nest std::initializer_lists
	 */
	void setValues(
			const typename fire::Initializer<ThisTensorType, Scalar, Rank>::InitList& vals) {
		provider->setValues(vals);
	}

	/**
	 * Multiply all elements of this Tensor by the provided Scalar.
	 *
	 * @param val Scalar to multiply this tensor by.
	 * @return result A TensorReference representing the result
	 */
	ThisTensorType operator*(Scalar val) {
		auto ref = provider->template multiplyByScalar<Scalar>(val);
		ThisTensorType result(ref);
		return result;
	}

	/**
	 * Output this Tensor to the provided output stream.
	 *
	 * @param outputStream The output stream to write the tensor to.
	 */
	void print(std::ostream& outputStream) {
		provider->print(outputStream);
	}

	/**
	 * Return the total number of elements in this tensor.
	 * @return
	 */
	const int size() {
		return shape->size();
	}

	/**
	 * Reshape the tensor with a new array of dimensions.
	 * Note that the new dimensions must correspond to a Tensor
	 * that has the same number of elements as before.
	 *
	 * @param array Array of new dimensions for each rank index
	 * @return reshapedTensor A new reshaped tensor.
	 */
	template<typename DimArray>
	Tensor<array_size<DimArray>::value, DerivedTensorBackendBuilder, Scalar> reshape(
			DimArray& array) {

		// First make sure that the new shape will retain the
		// same total number of elements.

		// Count the elements for the new shape
		int nNewElements = 1;
		for (int i = 0; i < array_size<DimArray>::value; i++) {
			nNewElements *= array[i];
		}

		// Assert it won't change
		assert(nNewElements == size());

		// Reshape the tensor
		auto ref = provider->reshape(array);

		// Create the result from the TensorReference
		Tensor<array_size<DimArray>::value, DerivedTensorBackendBuilder, Scalar> result(ref);

		return result;
	}
	/**
	 * The destructor
	 */
	virtual ~Tensor() {
	}

};

}

#endif


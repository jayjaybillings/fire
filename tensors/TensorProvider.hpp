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
#ifndef TENSORS_TENSOROPERATIONPROVIDER_HPP_
#define TENSORS_TENSOROPERATIONPROVIDER_HPP_

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "TensorShape.hpp"
#include "TensorUtils.hpp"

namespace fire {

/**
 * The TensorProvider is the base class for injecting 3rd-party
 * tensor algebra libraries into the Fire framework. It utilizes the
 * curiously recurring template pattern to enable static polymorphism of
 * TensorProvider subclasses. The Tensor class delegates all tensor-related data
 * storage and work to the methods provided by TensorProvider. TensorProvider takes
 * as a template parameter a derived TensorProvider and delegates all work to that
 * subclass. It is up to subclasses of TensorProvider (subclassed with themselves as the
 * TensorProvider template parameter) to perform the actual work with their
 * representative tensor algebra library.
 *
 * Subclasses of TensorProvider must provide a subclass of the ProviderBuilder
 * struct with the following public method (see EigenTensorProvider.hpp for example):
 *
 * @code
 * 	template<const int Rank, typename Scalar>
 *  MyTensorProvider<Rank, Scalar> build() {
 *		MyTensorProvider<Rank, Scalar> prov;
 *		return prov;
 *	}
 * @endcode
 *
 * @author Alex McCaskey
 */
template<typename Derived>
class TensorProvider {

private:

	/**
	 * Return a reference of this TensorProvider as its
	 * derived subclass. Used to delegate work to subclasses.
	 *
	 * @return
	 */
	Derived& getAsDerived() {
		return *static_cast<Derived*>(this);
	}

public:

	/**
	 * Initialize this TensorProvider with the provided
	 * set of tensor dimensions.
	 *
	 * @param firstDim The first tensor dimension
	 * @param otherDims The parameter pack of other tensor dimensions
	 */
	template<typename ... Dimensions>
	void initialize(int firstDim, Dimensions ... otherDims) {
		getAsDerived().initializeTensorBackend(firstDim, otherDims...);
	}

	/**
	 * Initialize this TensorProvider from an existing TensorReference.
	 *
	 * @param reference The set of data and dimensions as a TensorReference
	 */
	void initializeFromReference(TensorReference& reference) {
		getAsDerived().initializeTensorBackendWithReference(reference);
	}

	/**
	 * Return the coefficient value at the provided set
	 * of indices.
	 *
	 * @param indices The indices for the desired value
	 * @return val The value at the indices.
	 */
	template<typename Scalar, typename ... Indices>
	Scalar& coeff(Indices ... indices) {
		return getAsDerived().tensorCoefficient(indices...);
	}

	/**
	 * Return if the tensor wrapped by this TensorProvider is
	 * equal to the tensor represented by the provided TensorReference.
	 *
	 * @param other TensorReference view of the other Tensor
	 * @return equal A boolean indicating if these Tensors are equal
	 */
	bool equalTensors(TensorReference& other) {
		return getAsDerived().checkEquality(other);
	}

	/**
	 * Return the data wrapped by the tensor in this TensorProvider.
	 *
	 * @return data 1-D array of data representing the tensor in this TensorProvider
	 */
	template<typename Scalar>
	Scalar * getTensorData() {
		return getAsDerived().data();
	}

	/**
	 * Return a TensorReference representing the sum of this TensorProvider's tensor
	 * and the one represented by the other TensorReference.
	 *
	 * @param other TensorReference view of the other Tensor
	 * @return result A new TensorReference representing the sum of this and other.
	 */
	TensorReference addTensors(TensorReference& other) {
		auto r = getAsDerived().add(other);
		return r;
	}

	/**
	 * Return the rank of this TensorProvider's tensor
	 *
	 * @return rank The rank of this Tensor
	 */
	static constexpr int getRank() {
		return Derived::getRank();
	}

	/**
	 * Perform tensor contraction, returning the result as a TensorReference.
	 *
	 * @param t2 The other Tensor
	 * @param indices The contraction indices.
	 * @return result The contraction result as a TensorReference
	 */
	template<typename OtherDerived, typename ContractionDims>
	TensorReference contract(OtherDerived& t2, ContractionDims& indices) {
		return getAsDerived().executeContraction(t2, indices);
	}

	/**
	 * Set the tensor values using nested initializer_list
	 *
	 * @param vals The values as a nest std::initializer_lists
	 */
	template<typename InitList>
	void setValues(InitList& vals) {
		getAsDerived().setTensorValues(vals);
	}

	/**
	 * Output this Tensor to the provided output stream.
	 *
	 * @param outputStream The output stream to write the tensor to.
	 */
	void print(std::ostream& stream) {
		getAsDerived().printTensor(stream);
	}

	/**
	 * Set the tensor values wrapped by this TensorProvider to random values.
	 */
	void setRandomValues() {
		getAsDerived().fillWithRandomValues();
	}

	/**
	 * Multiply all elements of this tensor by the provided Scalar.
	 *
	 * @param val Scalar to multiply this tensor by.
	 * @return result A TensorReference representing the result
	 */
	template<typename Scalar>
	TensorReference multiplyByScalar(Scalar& val) {
		return getAsDerived().scalarProduct(val);
	}

	/**
	 * Reshape the Tensor with a new array of dimensions
	 *
	 * @param array Array of new dimensions for each rank index
	 * @return reshapedTensor A TensorReference representing new reshaped tensor.
	 */
	template<typename DimArray>
	TensorReference reshape(DimArray& array) {
		return getAsDerived().reshapeTensor(array);
	}

};

/**
 * A placeholder to ensure that clients give the Tensor an
 * appropriate TensorProvider Builder.
 */
struct ProviderBuilder {
};

}

// Fire defaults to Eigen so go ahead and include it...
// This lets users just #include "Tensor.hpp" and get all
// Eigen support.
#include "EigenTensorProvider.hpp"

#endif

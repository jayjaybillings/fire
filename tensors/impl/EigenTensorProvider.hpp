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
#ifndef TENSORS_EIGENTENSORPROVIDER_HPP_
#define TENSORS_EIGENTENSORPROVIDER_HPP_

// Turn on ability to use Eigen ThreadPool
#define EIGEN_USE_THREADS 1

#include "TensorProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <unsupported/Eigen/KroneckerProduct>

namespace fire {

/**
 * The EigenTensorProvider is a TensorProvider that provides
 * tensor data and operations using the Eigen C++ tensor module.
 */
template<const int Rank, typename Scalar = double>
class EigenTensorProvider: public TensorProvider<
		EigenTensorProvider<Rank, Scalar>> {

private:

	// Declare a Eigen Tensor member
	using EigenTensor = Eigen::Tensor<Scalar, Rank>;
	std::shared_ptr<EigenTensor> tensor;

	/**
	 * Keep track of the number of available OMP threads
	 */
	int nThreads = 0;

	/**
	 * Reference to the ThreadPool
	 */
	std::shared_ptr<Eigen::ThreadPool> pool;

	/**
	 * Reference to the thread pool device used for
	 * performing computational intensive operaitons
	 * with multiple threads.
	 */
	std::shared_ptr<Eigen::ThreadPoolDevice> device;

	// Return a new Eigen::Tensor (really a TensorMap) from the
	// provided TensorReference.
	Eigen::TensorMap<EigenTensor> getEigenTensorFromReference(
			TensorReference<Scalar>& ref) {

		// Get the data and shape
		auto shape = ref.second;
		auto data = ref.first.data();

		// Create an Eigen dimension array
		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}

		// Create the Tensor and return it
		Eigen::TensorMap<Eigen::Tensor<Scalar, Rank>> t(data, dsizes);
		return t;
	}

	template<const int NewRank>
	Eigen::TensorMap<Eigen::Tensor<Scalar, NewRank>> getEigenTensorFromReferenceWithRank(
			TensorReference<Scalar>& ref) {

		// Get the data and shape
		auto shape = ref.second;
		auto data = ref.first.data();

		// Create an Eigen dimension array
		Eigen::DSizes<Eigen::DenseIndex, NewRank> dsizes;
		for (int d = 0; d < NewRank; d++) {
			dsizes[d] = shape.dimension(d);
		}

		// Create the Tensor and return it
		Eigen::TensorMap<Eigen::Tensor<Scalar, NewRank>> t(data, dsizes);
		return t;
	}
public:

	/**
	 * Static reference to the rank of the tensor
	 * wrapped by this provider
	 */
	static const int rank = Rank;

	/**
	 * The Constructor
	 */
	EigenTensorProvider() {
		int n = 0;
#pragma omp parallel reduction(+:n)
		n += 1;
		nThreads = n;
		pool = std::make_shared<Eigen::ThreadPool>(nThreads);
		device = std::make_shared<Eigen::ThreadPoolDevice>(pool.get(),
				nThreads);
	}

	/**
	 * Initialize the Eigen Tensor with all zeros.
	 * @param firstDim
	 * @param otherDims
	 */
	template<typename ... Dimensions>
	void initializeTensorBackend(int firstDim, Dimensions ... otherDims) {
		// static assert here on rank and dims
		tensor = std::make_shared<EigenTensor>(firstDim, otherDims...);
		tensor->setZero();
	}

	/**
	 * Initialize the Eigen Tensor from an existing TensorReference
	 * @param reference
	 */
	void initializeTensorBackendWithReference(TensorReference<Scalar>& reference) {
		Eigen::TensorMap<Eigen::Tensor<Scalar, Rank>> t =
				getEigenTensorFromReference(reference);
		tensor = std::make_shared<EigenTensor>(t);
	}

	/**
	 * Return the coefficient at the given tensor indices.
	 *
	 * @param indices The indices for the desired value
	 * @return val The value at the indices.
	 */
	template<typename ... Indices>
	Scalar& tensorCoefficient(Indices ... indices) {
		return tensor->operator()(indices...);
	}

	/**
	 * Return true if the provided TensorReference as a tensor is
	 * equal to this tensor.
	 *
	 * @param other TensorReference view of the other Tensor
	 * @return equal A boolean indicating if these Tensors are equal
	 */
	bool checkEquality(TensorReference<Scalar>& other) {
		auto t = getEigenTensorFromReference(other);
		Eigen::Tensor<bool, 0> eq = tensor->operator==(t).all();
		return eq(0);
	}

	/**
	 * Compute the tensor contraction of this Eigen Tensor with the provided
	 * Other Tensor.
	 *
	 * @param t2 The other Tensor
	 * @param indices The contraction indices.
	 * @return result The contraction result as a TensorReference
	 */
	template<typename OtherDerived, typename ContractionDims>
	TensorReference<Scalar> executeContraction(OtherDerived& t2,
			ContractionDims& cIndices) {

		// Compute new Tensor rank
		static constexpr int newRank = Rank + OtherDerived::getRank()
				- 2 * array_size<ContractionDims>::value;

		// Create an Eigen Tensor from OtherDerived...
		auto otherRef = t2.createReference();
		Eigen::TensorMap<Eigen::Tensor<Scalar, OtherDerived::getRank()>> otherTensor =
				getEigenTensorFromReferenceWithRank<OtherDerived::getRank()>(
						otherRef);

		// ---------------------------------------------------------
		// Need to compute dimensions of new tensor
		Eigen::DSizes<Eigen::DenseIndex, newRank> newDimensions;
		if (newRank != 0) {
			int counter = 0;
			if (array_size<ContractionDims>::value != 0) {
				for (int i = 0; i < tensor->dimensions().size(); i++) {
					for (auto c : cIndices) {
						if (i != c.first) {
							newDimensions[counter] = tensor->dimension(i);
							counter++;
						}
					}
				}
				for (int i = 0; i < otherTensor.dimensions().size(); i++) {
					for (auto c : cIndices) {
						if (i != c.second) {
							newDimensions[counter] = otherTensor.dimension(i);
							counter++;
						}
					}
				}
			} else {
				// If there are no contraction dims, then keep all
				// dimensions
				for (int i = 0; i < tensor->dimensions().size(); i++) {
					newDimensions[counter] = tensor->dimension(i);
					counter++;
				}
				for (int i = 0; i < otherTensor.dimensions().size(); i++) {
					newDimensions[counter] = otherTensor.dimension(i);
					counter++;
				}
			}
		}

		// ---------------------------------------------------------

		// Perform the Tensor Contraction multi-threaded
		Eigen::Tensor<Scalar, newRank> result(newDimensions);
		result.device(*device.get()) = tensor->contract(otherTensor, cIndices);

		// Create TensorShape
		std::vector<int> dimensions(newRank);
		for (int i = 0; i < newRank; i++) {
			dimensions[i] = result.dimension(i);
		}
		TensorShape newShape(dimensions);

		// Create and return a reference to the new Tensor.
		TensorReference<Scalar> newReference = fire::make_tensor_reference(
				result.data(), newShape);

		return newReference;
	}

	/**
	 * Return the 1-D array wrapped by this Eigen Tensor
	 *
	 * @return data 1-D array of data representing the tensor in this TensorProvider
	 */
	Scalar * data() {
		return tensor->data();
	}

	/**
	 * Return the rank of this Eigen Tensor
	 *
	 * @return rank The rank of this Tensor
	 */
	static constexpr int getRank() {
		return Rank;
	}

	/**
	 * Return a TensorReference representing the sum of this Eigen Tensor
	 * and an Eigen Tensor represented by the other TensorReference.
	 *
	 * @param other TensorReference view of the other Tensor
	 * @return result A new TensorReference representing the sum of this and other.
	 */
	TensorReference<Scalar> add(TensorReference<Scalar>& other) {

		auto t = getEigenTensorFromReference(other);

		EigenTensor result(tensor->dimensions());

		result.device(*device.get()) = *tensor.get() + t;

		// Shape should be the same...
		auto newRef = fire::make_tensor_reference(result.data(), other.second);

		return newRef;
	}

	/**
	 * Set the Eigen Tensor values using nested initializer_list
	 *
	 * @param vals The values as a nest std::initializer_lists
	 */
	template<typename InitList>
	void setTensorValues(InitList& vals) {
		tensor->setValues(vals);
	}

	/**
	 * Output this Eigen Tensor to the provided output stream.
	 *
	 * @param outputStream The output stream to write the tensor to.
	 */
	void printTensor(std::ostream& stream) {
		stream << *tensor.get();
	}

	/**
	 * Set the Eigen Tensor values to random values.
	 */
	void fillWithRandomValues() {
		tensor->setRandom();
	}

	/**
	 * Multiply all elements of this Eigen Tensor by the provided Scalar.
	 *
	 * @param val Scalar to multiply this tensor by.
	 * @return result A TensorReference representing the result
	 */
	TensorReference<Scalar> scalarProduct(Scalar& val) {
		Eigen::Tensor<Scalar, Rank> result(tensor->dimensions());
		result.device(*device.get()) = tensor->operator*(val);

		// Create TensorShape
		std::vector<int> dimensions(Rank);
		for (int i = 0; i < Rank; i++) {
			dimensions[i] = result.dimension(i);
		}
		TensorShape newShape(dimensions);

		// Create and return a reference to the new Tensor.
		auto newReference = fire::make_tensor_reference(
				result.data(), newShape);

		return newReference;
	}

	/**
	 * Reshape the Eigen Tensor with a new array of dimensions
	 *
	 * @param array Array of new dimensions for each rank index
	 * @return reshapedTensor A TensorReference representing new reshaped tensor.
	 */
	template<typename DimArray>
	TensorReference<Scalar> reshapeTensor(DimArray& array) {

		static constexpr int newRank = array_size<DimArray>::value;

		Eigen::Tensor<Scalar, newRank> result = tensor->reshape(array);

		// Create TensorShape
		std::vector<int> dimensions(newRank);
		for (int i = 0; i < newRank; i++) {
			dimensions[i] = result.dimension(i);
		}
		TensorShape newShape(dimensions);

		// Create and return a reference to the new Tensor.
		auto newReference = fire::make_tensor_reference(
				result.data(), newShape);

		return newReference;
	}

	template<typename DimArray>
	TensorReference<Scalar> shuffleTensor(DimArray& array) {
		Eigen::Tensor<Scalar, Rank> result = tensor->shuffle(array);

		// Create TensorShape
		std::vector<int> dimensions(Rank);
		for (int i = 0; i < Rank; i++) {
			dimensions[i] = result.dimension(i);
		}
		TensorShape newShape(dimensions);

		// Create and return a reference to the new Tensor.
		auto newReference = fire::make_tensor_reference(
				result.data(), newShape);

		return newReference;
	}

	std::tuple<TensorReference<Scalar>, TensorReference<Scalar>, TensorReference<Scalar>> computeSvd(
			TensorReference<Scalar>& ref, double cutoff) {

		// Get the data and shape
		auto shape = ref.second;
		auto data = ref.first.data();

		// Express the Tensor as an Eigen Matrix. We know
		// at this point the Tensor is Rank 2. Double check anyway
		assert(shape.dimensions().size() == 2);

		Eigen::Map<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> matrix(data, shape.dimension(0),
				shape.dimension(1));

		Eigen::BDCSVD<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> svd(matrix,
				Eigen::ComputeThinU | Eigen::ComputeThinV);

		auto singularValues = svd.singularValues();
		auto u = svd.matrixU();
		auto v = svd.matrixV();

		// These values are in decreasing order,
		// so we just need to get the first one less than
		// the cutoff, the rest will be too
		int truncIndex = singularValues.rows() - 1;
		for (int i = 0; i < singularValues.rows(); i++) {
			if (singularValues(i) < cutoff) {
				truncIndex = i;
				break;
			}
		}

		Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> S(truncIndex + 1, truncIndex + 1);
		S.setZero();
		for (int i = 0; i < truncIndex + 1; i++)
			S(i, i) = singularValues(i);

		Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> truncatedU = u.block(0, 0, truncIndex + 1,
				truncIndex + 1);
		Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> truncatedV = v.block(0, 0, truncIndex + 1,
				truncIndex + 1);

		assert ( (u*S*v.transpose() - matrix).array().abs().sum() < 1e-6 );

		std::vector<int> uDims(2), sDims(2), vDims(2);
		uDims[0] = truncatedU.rows();
		uDims[1] = truncatedU.cols();
		sDims[0] = S.rows();
		sDims[1] = S.cols();
		vDims[0] = truncatedV.rows();
		vDims[1] = truncatedV.cols();

		TensorShape uShape(uDims), sShape(sDims), vShape(vDims);

		// Shape should be the same as the given tensor ref
		// for the computed U and V tensors
		return std::make_tuple(
				fire::make_tensor_reference(truncatedU.data(), uShape),
				fire::make_tensor_reference(S.data(), sShape),
				fire::make_tensor_reference(truncatedV.data(), vShape));

	}

	TensorReference<Scalar> kronProd(TensorReference<Scalar>& other) {
		// Get the data and shape
		auto shape = other.second;
		auto d = other.first.data();

		// Express the Tensor as an Eigen Matrix. We know
		// at this point the Tensor is Rank 2. Double check anyway
		assert(shape.dimensions().size() == 2);

		Eigen::Map<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> matrix(d, shape.dimension(0),
				shape.dimension(1));

		Eigen::Map<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> thisAsMat(data(), tensor->dimension(0),
				tensor->dimension(1));

		auto result = kroneckerProduct(thisAsMat, matrix).eval();
		std::vector<int> dims(2);
		dims[0] = result.rows();
		dims[1] = result.cols();
		TensorShape newShape(dims);
		return fire::make_tensor_reference(result.data(), newShape);
	}
};

/**
 * This class provides a mechanism for building
 * EigenTensorProviders. It is used by the Tensor
 * class to appropriately construct a TensorProvider
 * backed by Eigen Tensors.
 */
class EigenProvider: ProviderBuilder {
public:
	template<const int Rank, typename Scalar>
	EigenTensorProvider<Rank, Scalar> build() {
		EigenTensorProvider<Rank, Scalar> prov;
		return prov;
	}
};

}

#endif

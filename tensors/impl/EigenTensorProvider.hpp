#ifndef TENSORS_EIGENTENSORPROVIDER_HPP_
#define TENSORS_EIGENTENSORPROVIDER_HPP_

#include "TensorProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

namespace fire {

/**
 *
 */
template<const int Rank, typename Scalar = double>
class EigenTensorProvider: public TensorProvider<
		EigenTensorProvider<Rank, Scalar>> {

private:

	// Declare a Eigen Tensor member
	using EigenTensor = Eigen::Tensor<Scalar, Rank>;
	std::shared_ptr<EigenTensor> tensor;

	Eigen::TensorMap<EigenTensor> getEigenTensorFromReference(
			TensorReference& ref) {
		auto shape = ref.second;
		auto data = ref.first.data();

		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}
		Eigen::TensorMap<Eigen::Tensor<Scalar, Rank>> t(data, dsizes);
		return t;
	}

public:

	static const int rank = Rank;

	/**
	 *
	 */
	EigenTensorProvider() {/*std::cout << "Using Eigen\n";*/
	}

	template<typename ... Dimensions>
	void initializeTensorBackend(int firstDim, Dimensions ... otherDims) {
		// static assert here on rank and dims
		tensor = std::make_shared<EigenTensor>(firstDim, otherDims...);
		tensor->setZero();
	}

	void initializeTensorBackendWithData(Scalar * data, TensorShape& shape) {
		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}

		Eigen::TensorMap<Eigen::Tensor<Scalar, Rank>> t(data, dsizes);
		tensor = std::make_shared<EigenTensor>(t);
	}

	template<typename ... Indices>
	double& tensorCoefficient(Indices ... indices) {
		return tensor->operator()(indices...);
	}

	bool checkEquality(TensorReference& other) {
		auto t = getEigenTensorFromReference(other);
		Eigen::Tensor<bool, 0> eq = tensor->operator==(t).all();
		return eq(0);
	}

	template<const int r>
	static EigenTensorProvider<r> makeTensorProvider() {
		EigenTensorProvider<r> tp;
		return tp;
	}

	template<typename OtherDerived,
			typename ContractionDims>
	TensorReference executeContraction(OtherDerived& t2,
			ContractionDims& cIndices) {

		// Compute new Tensor rank
		static constexpr int newRank = Rank + OtherDerived::getRank()
				- 2 * array_size<ContractionDims>::value;

		// Create an Eigen Tensor from OtherDerived...
		auto otherRef = t2.createReference();
		Eigen::TensorMap<Eigen::Tensor<Scalar, OtherDerived::getRank()>> otherTensor =
				getEigenTensorFromReference(otherRef);

		// Perform the Tensor Contraction
		Eigen::Tensor<Scalar, newRank> result = tensor->contract(otherTensor,
				cIndices);

		// Create TensorShape
		std::vector<int> dimensions(newRank);
		for (int i = 0; i < newRank; i++) {
			dimensions[i] = result.dimension(i);
		}
		TensorShape newShape(dimensions);

		// Create and return a reference to the new Tensor.
		TensorReference newReference = fire::make_tensor_reference(
				result.data(), newShape);

		return newReference;
	}

	Scalar * data() {
		return tensor->data();
	}

	static constexpr int getRank() {
		return Rank;
	}

	TensorReference add(TensorReference& other) {

		auto t = getEigenTensorFromReference(other);

		EigenTensor result = *tensor.get() + t;

		// Shape should be the same...
		auto newRef = fire::make_tensor_reference(result.data(), other.second);

		return newRef;
	}

	void fillWithRandomValues() {
		tensor->setRandom();
	}

};

class EigenBuilder : ProviderBuilder {
public:
	template<const int Rank, typename Scalar>
    EigenTensorProvider<Rank, Scalar> build() {
		EigenTensorProvider<Rank, Scalar> prov;
		return prov;
	}
};

}

#endif

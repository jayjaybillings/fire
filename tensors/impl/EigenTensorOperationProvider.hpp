#ifndef TENSORS_EIGENTENSOR_HPP_
#define TENSORS_EIGENTENSOR_HPP_

#include "TensorOperationProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

namespace fire {

/**
 *
 */
template<const int Rank>
class EigenTensorOperationProvider : public TensorOperationProvider<
		EigenTensorOperationProvider<Rank>> {

private:

	// Declare a Eigen Tensor member
	using EigenTensor = Eigen::Tensor<double, Rank>;
	std::shared_ptr<EigenTensor> tensor;

public:

	/**
	 *
	 */
	EigenTensorOperationProvider() {/*std::cout << "Using Eigen\n";*/ }

	template<typename ... Dimensions>
	void initializeTensorBackend(int firstDim, Dimensions ... otherDims) {
		// static assert here on rank and dims
		tensor = std::make_shared<EigenTensor>(firstDim, otherDims...);
		tensor->setZero();
	}

	void initializeTensorBackendWithData(double * data, TensorShape& shape) {
		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}

		Eigen::TensorMap<Eigen::Tensor<double, Rank>> t(data, dsizes);
		tensor = std::make_shared<EigenTensor>(t);
	}

	template<typename... Indices>
	double& tensorCoefficient(Indices... indices) {
		return tensor->operator()(indices...);
	}

	double * data() {
		return tensor->data();
	}

	const int getRank() {
		return Rank;
	}

	TensorReference add(TensorReference& other) {

		auto shape = other.second;
		auto data = other.first.data();

		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}
		Eigen::TensorMap<Eigen::Tensor<double, Rank>> t(data, dsizes);

		EigenTensor result = *tensor.get() + t;

		// Shape should be the same...
		auto newRef = fire::make_tensor_reference(result.data(), shape);

		return newRef;
	}

};

}


#endif

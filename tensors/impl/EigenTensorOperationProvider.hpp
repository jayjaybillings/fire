#ifndef TENSORS_EIGENTENSOR_HPP_
#define TENSORS_EIGENTENSOR_HPP_

#include "TensorOperationProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

namespace fire {

/**
 *
 */
template<const int Rank>
class EigenTensorOperationProvider: public TensorOperationProvider<
		EigenTensorOperationProvider<Rank>> {

private:

	// Declare a Eigen Tensor member
	using EigenTensor = Eigen::Tensor<double, Rank>;
	std::shared_ptr<EigenTensor> tensor;

	Eigen::TensorMap<EigenTensor> getEigenTensorFromReference(
			TensorReference& ref) {
		auto shape = ref.second;
		auto data = ref.first.data();

		Eigen::DSizes<Eigen::DenseIndex, Rank> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = shape.dimension(d);
		}
		Eigen::TensorMap<Eigen::Tensor<double, Rank>> t(data, dsizes);
		return t;
	}

public:

	static const int rank = Rank;

	/**
	 *
	 */
	EigenTensorOperationProvider() {/*std::cout << "Using Eigen\n";*/
	}

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
	static EigenTensorOperationProvider<r> makeTensorProvider() {
		EigenTensorOperationProvider<r> tp;
		return tp;
	}

	template<typename OtherDerived,
			typename ContractionDims>
	void executeContraction(OtherDerived& t2,
			ContractionDims cIndices) {

		// Compute new Tensor rank
		static constexpr int newRank = Rank
				+ OtherDerived::getRank()
				- 2 * array_size<ContractionDims>::value;

		auto otherRef = t2.createReference();
		Eigen::DSizes<Eigen::DenseIndex, OtherDerived::getRank()> dsizes;
		for (int d = 0; d < Rank; d++) {
			dsizes[d] = otherRef.second.dimension(d);
		}
		Eigen::TensorMap<Eigen::Tensor<double, OtherDerived::getRank()>> otherTensor(otherRef.first.data(), dsizes);

		Eigen::Tensor<double, newRank> result = tensor->contract(otherTensor, cIndices);

//		Eigen::array<DimPair, 1> dims3 = { { DimPair(0, 0) } };
//		typedef TensorEvaluator<decltype(tensor->contract(otherTensor, cIndices)),
//				DefaultDevice> Evaluator;
//		Evaluator eval(tensor->contract(otherTensor, cIndices), DefaultDevice());
//		eval.evalTo(result.data());

	}

	double * data() {
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
};

}

#endif

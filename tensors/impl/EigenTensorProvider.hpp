#ifndef TENSORS_EIGENTENSORPROVIDER_HPP_
#define TENSORS_EIGENTENSORPROVIDER_HPP_

#include "TensorProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

using DimPair = Eigen::Tensor<double, 1>::DimensionPair;


namespace fire {

template<typename Scalar, int Rank>
class EigenTensorProvider: public TensorProvider<Scalar, Rank> {

protected:

	using EigenTensor = Eigen::Tensor<Scalar, Rank>;

	std::shared_ptr<EigenTensor> _tensor;

public:

	EigenTensorProvider() {
	}

	template<typename ... Args>
	EigenTensorProvider(int firstDim, Args ... dims) :
			TensorProvider<Scalar, Rank>(firstDim, dims...) {
		_tensor = std::make_shared<Eigen::Tensor<Scalar, Rank>>(firstDim,
				dims...);
		_tensor->setZero();
	}

	std::shared_ptr<EigenTensor>& getEigenTensor() {return _tensor;}

	virtual int dimension(int index) {
		return _tensor->dimension(index);
	}

	template<typename ... Indices>
	Scalar& operator()(Indices ... indices) const {
		return _tensor->operator()(indices...);
	}


	virtual Tensor<Scalar, Rank - 2>& contract(ITensor& other,
			std::vector<std::pair<int, int>>& dimensions) {

//		EigenTensorProvider<Scalar, Rank - 1> result;
//		auto otherEigenTensor = dynamic_cast<EigenTensorProvider&>(other).getEigenTensor();
//
//		int i = 0;
//		Eigen::array<DimPair, Rank> dims;
//		for (auto d : dimensions) {
//			dims[i] = DimPair{d->first, d->second};
//			i++;
//		}
//
//		typedef TensorEvaluator<decltype(_tensor->contract(*otherEigenTensor.get(), dims)),
//				DefaultDevice> Evaluator;
//		Evaluator eval(_tensor->contract(*otherEigenTensor.get(), dims), DefaultDevice());
//		eval.evalTo(mat4.data());

	}

	virtual void add(ITensor& other, double scale = 1.0) {

	}

	virtual double norm1() {

	}

	virtual double norm2() {

	}

	virtual ~EigenTensorProvider() {
	}
};

class EigenBuilder: ProviderBuilder {
public:
	template<typename Scalar, int Rank>
	EigenTensorProvider<Scalar, Rank> build() {
	}
};
using EigenTensor = EigenBuilder;

}

#endif

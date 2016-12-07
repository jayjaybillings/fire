#ifndef TENSORS_EIGENTENSORPROVIDER_HPP_
#define TENSORS_EIGENTENSORPROVIDER_HPP_

#include "TensorProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

using Eigen::Tensor;

namespace fire {

template<typename Scalar, int Rank>
class EigenTensorProvider: public TensorProvider<Scalar, Rank> {

protected:

	std::shared_ptr<Tensor<Scalar, Rank>> _tensor;

public:

	EigenTensorProvider() {}

	template<typename... Args>
	EigenTensorProvider(int firstDim, Args ... dims) :
			TensorProvider<Scalar, Rank>(firstDim, dims...) {
		_tensor = std::make_shared<Tensor<Scalar, Rank>>(firstDim, dims...);
	}

	virtual ITensor& contract(ITensor& other,
			std::vector<std::pair<int, int>>& dimensions) {



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

class EigenBuilder : ProviderBuilder {
public:
	template<typename Scalar, int Rank>
    EigenTensorProvider<Scalar, Rank> build() {
	}
};
using EigenTensor = EigenBuilder;

}

#endif

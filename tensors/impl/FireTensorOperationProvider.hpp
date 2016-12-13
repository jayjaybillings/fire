#if 0
#ifndef TENSORS_EIGENTENSOR_HPP_
#define TENSORS_EIGENTENSOR_HPP_

#include "TensorOperationProvider.hpp"

namespace fire {

class FireTensorOperationProvider : public TensorOperationProvider {

public:

	FireTensorOperationProvider() {std::cout << "Using Fire\n"; }

	virtual TensorReference& executeBinaryOperation(
				TensorOperation op, TensorReference& first, TensorReference& second);

	virtual TensorReference& executeUnaryOperation(
				TensorOperation op, TensorReference& tensor);

	virtual double& coefficient(TensorReference& ref, std::vector<int>& indices) {
	}

	virtual void allocateTensor(const int& rank) {
		}
};

}


#endif
#endif

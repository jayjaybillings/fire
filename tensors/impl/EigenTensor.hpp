#ifndef TENSORS_EIGENTENSOR_HPP_
#define TENSORS_EIGENTENSOR_HPP_

#include "Tensor.hpp"

namespace fire {

class EigenTensor : public TensorProvider {

public:

	EigenTensor(const int& r, std::vector<int>& dims);

	virtual ITensor& contract(ITensor& other, std::vector<std::pair<int, int>>& dimensions);

	virtual void add(ITensor& other, double scale = 1.0);

	virtual double norm1();

	virtual double norm2();

};

}


#endif

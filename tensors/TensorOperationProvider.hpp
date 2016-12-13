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
#include <boost/variant.hpp>
#include <numeric>
#include "TensorShape.hpp"
#include "build.h"

namespace fire {

template <typename T> struct array_size;
template<class T, std::size_t N> struct array_size<std::array<T,N> > {
  static const size_t value = N;
};

using TensorReference = std::pair<std::vector<double>, TensorShape>;

TensorReference make_tensor_reference(double* data, TensorShape& shape) {
	std::vector<double> v;
	std::copy(data, data + shape.size(), std::back_inserter(v));
	auto pair = std::make_pair(v, shape);
	return pair;
}


/**
 *
 * @author Alex McCaskey
 */
template<typename Derived>
class TensorOperationProvider  {

private:

	Derived& getAsDerived() {
		return *static_cast<Derived*>(this);
	}

public:

	TensorOperationProvider() {
	}

	template<typename ... Dimensions>
	void initialize(int firstDim, Dimensions... otherDims) {
		getAsDerived().initializeTensorBackend(firstDim, otherDims...);
	}

	void initializeFromData(double * data, TensorShape& shape) {
		getAsDerived().initializeTensorBackendWithData(data, shape);
	}

	template<typename... Indices>
	double& operator()(Indices... indices) {
		return getAsDerived().tensorCoefficient(indices...);
	}

	bool equalTensors(TensorReference& other) {
		return getAsDerived().checkEquality(other);
	}

	double* getTensorData() {
		return getAsDerived().data();
	}

	TensorReference addTensors(TensorReference& other) {
		auto r = getAsDerived().add(other);
		return r;
	}

	static constexpr int getRank() {
		return getAsDerived().getRank();
	}

	template<typename OtherDerived, typename ContractionDims>
	TensorReference contract(OtherDerived& t2, ContractionDims indices) {
		getAsDerived().executeContraction(t2, indices);
	}

//	template<const int r>
//	createNewBackend() {
//
//	}
};

}

#endif

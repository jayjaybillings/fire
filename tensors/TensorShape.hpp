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
#ifndef TENSORS_TENSORSHAPE_HPP_
#define TENSORS_TENSORSHAPE_HPP_

#include <vector>
#include <iostream>
#include <numeric>

namespace fire {

template<int n>
struct store_int {
	static const int num = n;
	static const int den = 1;
};

class TensorShape {

protected:

	const int tensorRank = 0;

	std::vector<int> shape;

	int nElements;

public:

//	static store_int<Rank> rankStorage;

	template<typename ... Dims>
	TensorShape(int firstDim, Dims ... otherDims) :
			shape { static_cast<int>(otherDims)... }, tensorRank(1+shape.size()) {
		shape.insert(shape.begin(), firstDim);
		nElements = std::accumulate(shape.begin(), shape.end(),
						1, std::multiplies<double>());
	}

	TensorShape(std::vector<int> dimensions) : shape(dimensions) {
		nElements = std::accumulate(shape.begin(), shape.end(),
								1, std::multiplies<double>());
	}
    const int size() {
    	return nElements;
    }

    const int dimension(int index) {
    	assert(index < shape.size());
    	return shape[index];
    }

    std::vector<int> dimensions() {
    	return shape;
    }

    const int rank() {
    	return tensorRank;
    }

};

}

#endif


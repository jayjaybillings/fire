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

/**
 * TensorShape is a simple class that holds reference to a
 * vector of integers representing the dimensions of each
 * rank for a given Tensor. It also keeps reference to the
 * total number of values in the tensor.
 */
class TensorShape {

protected:

	/**
	 * Reference to the vector of tensor dimensions
	 */
	std::vector<int> shape;

	/**
	 * Reference to the number of tensor elements
	 */
	int nElements;

public:

	/**
	 * The constructor, takes a variadic list of
	 * tensor dimensions.
	 *
	 * @param firstDim The dimension of the first rank
	 * @param otherDims The dimension of the remaining ranks
	 */
	template<typename ... Dims>
	TensorShape(int firstDim, Dims ... otherDims) :
			shape { static_cast<int>(otherDims)... } {
		shape.insert(shape.begin(), firstDim);
		nElements = std::accumulate(shape.begin(), shape.end(),
						1, std::multiplies<double>());
	}

	/**
	 * The constructor, takes an existing vector of
	 * tensor dimensions
	 *
	 * @param dimensions The vector of tensor dimensions
	 */
	TensorShape(std::vector<int> dimensions) : shape(dimensions) {
		nElements = std::accumulate(shape.begin(), shape.end(),
								1, std::multiplies<double>());
	}

	/**
	 * Return the total number of tensor elements. This is the
	 * size of the tensors 1-D array of tensor data.
	 * @return
	 */
    const int size() {
    	return nElements;
    }

    /**
     * Return the dimension at the given rank index.
     *
 	 * @param index The index of the tensor rank
	 * @return dim The dimension of the rank
    */
    const int dimension(int index) {
    	assert(index < shape.size());
    	return shape[index];
    }

    /**
     * Return the vector of tensor dimensions
     *
     * @return dims The vector of tensor dimensions
     */
    std::vector<int> dimensions() {
    	return shape;
    }

};

}

#endif


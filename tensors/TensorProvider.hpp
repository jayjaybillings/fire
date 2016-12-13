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
#ifndef TENSORS_TENSORPROVIDER_HPP_
#define TENSORS_TENSORPROVIDER_HPP_

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <numeric>
#include "Tensor.hpp"

namespace fire {

/**
 * ITensor serves as an interface describing operations
 * that can be done with tensor data structures.
 *
 * @author Alex McCaskey
 */
class ITensor {

public:

	/**
	 * Add two tensors together with a potential
	 * scale factor for the 'other' tensor. This operation
	 * modifies the existing tensor.
	 *
	 * @param other Tensor to add to this one.
	 * @param scale Scale factor to multiply other tensor by before addition.
	 * @return
	 */
	virtual void add(ITensor& other, double scale = 1.0) = 0;

	/**
	 * Perform tensor contraction between this tensor and the provided
	 * 'other' tensor using the provided index pairs.
	 *
	 * @param other The other Tensor to contract with
	 * @param dimensions The indices to contract
	 * @return result The result of the contraction.
	 */
	virtual ITensor& contract(ITensor& other,
			std::vector<std::pair<int, int>>& dimensions) = 0;

	/**
	 * Compute the 1-norm for this tensor
	 *
	 * @return norm The 1-norm value
	 */
	virtual double norm1() = 0;

	/**
	 * Compute the 2-norm for this tensor
	 *
	 * @return norm The 2-norm value
	 */
	virtual double norm2() = 0;

	/**
	 * Return the dimension of the given rank index.
	 *
	 * @param index The rank index
	 * @return dim The dimension
	 */
	virtual int dimension(int index) = 0;

	/**
	 * The destructor
	 */
	virtual ~ITensor() {
	}
};

/**
 * Enumeration of Device types
 */
enum DEVICE {
	CPU, GPU
};

/**
 * The TensorProvider is used by the Tensor class to delegate
 * tensor data storage and associated operations to 3rd party tensor
 * implementations (Eigen, TAL-SH, cuTorch, etc.).
 *
 * Subclasses of TensorProvider must implement the ITensor interface methods.
 * Switching TensorProvider backends can be done through the third template
 * parameter of the Tensor class. By default, Fire provides a TensorProvider
 * implementation based on Eigen.
 *
 * @author Alex McCaskey
 */
template<typename Scalar, int Rank>
class TensorProvider: public Tensor<Scalar, Rank> {

protected:

	/**
	 * The shape of the tensor controlled by this TensorProvider
	 */
	std::vector<int> tensorShape;

	/**
	 * Reference to the Device this tensor lives on.
	 */
	DEVICE device = CPU;

	/**
	 * Reference to the RowMajor tensor data.
	 */
	Scalar * m_data;

public:

	template<typename... Args>
	TensorProvider(int firstDim, Args ... restOfDims) {
		// Create a vector from the variadic template dimensions
		std::vector<int> dims { static_cast<int>(restOfDims)... };
		dims.insert(dims.begin(), firstDim);
		// Get the number of elements in the tensor
		auto nElements = std::accumulate(tensorShape.begin(), tensorShape.end(),
				0, std::plus<Scalar>());
		if (device == CPU)
			m_data = new Scalar[nElements];
		else
			allocateTensorGPU();

	}

	/**
	 * This method can be implemented by subclasses who provide
	 * GPU support to allocate the tensor on the attached GPU.
	 */
	virtual void allocateTensorGPU() {
		throw "Base TensorProvider does not have GPU support";
	}

	template<typename ... Indices>
	Scalar& operator()(Indices ... indices) const {
		throw "Base TensorProvider does not implement this method.";
	}
};

// Create a struct name that derived types can inherit
// to indicate to Fire that they are TensorProvider builders
struct ProviderBuilder {};
}

#endif

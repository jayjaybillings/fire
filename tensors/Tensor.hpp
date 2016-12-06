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
#ifndef TENSORS_TENSOR_HPP_
#define TENSORS_TENSOR_HPP_

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <boost/variant.hpp>
#include <numeric>

namespace fire {

std::string TensorBackend = "default";
bool TensorDebug = false;

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
	 * The destructor
	 */
	virtual ~ITensor() {}
};

enum DEVICE {
	CPU, GPU
};

/**
 * The TensorProvider is used by the Tensor class to delegate
 * tensor data and operations to 3rd party tensor data structure
 * implementations (Eigen, TAL-SH, cuTorch, etc.). The TensorProvider
 * implements a runtime registration factory to enable subclasses
 * (that do actual tensor work with real 3rd party tensor data structures)
 * to register themselves as available for use as the Tensor backend.
 *
 * Subclasses of TensorProvider must implement the ITensor interface methods
 * and invoke the REGISTER_TENSORPROVIDER macro to make themselves available to
 * the Fire Tensor framework.
 *
 * @author Alex McCaskey
 */
class TensorProvider: public virtual ITensor {

protected:

	/**
	 * The rank of this TensorProviders internal tensor.
	 */
	const int rank = 0;

	/**
	 * The shape of the tensor controlled by this TensorProvider
	 */
	std::vector<int> dimensions;

	/**
	 * Reference to the actual tensor data, stored as a
	 * variant to handle different Tensor Scalar types.
	 * FIXME IS THERE A BETTER WAY???
	 */
	boost::variant<int*, double*, float*, long*> data;

	/**
	 * Reference to the Device this tensor lives on.
	 */
	DEVICE device = CPU;

public:

	/**
	 * The constructor, takes the tensor rank and shape (each rank dimension)
	 *
	 * @param r
	 * @param dims
	 */
	TensorProvider(const int r, std::vector<int> dims) :
			rank(r), dimensions(dims) {
	}

	/**
	 * This method must be invoked before use of this TensorProvider
	 * to allocate the correct amount of memory for the tensor.
	 */
	template<typename Scalar>
	void allocate() {
		if (device == CPU) {
			// Allocate Scalar data...
			auto nElements = std::accumulate(dimensions.begin(),
					dimensions.end(), 0, std::plus<Scalar>());
			data = new Scalar[nElements];
			memset(boost::get<Scalar*>(data), 0.0, sizeof(Scalar) * nElements);
		} else {
			allocateTensorGPU();
		}
	}

	/**
	 * This method can be implemented by subclasses who provide
	 * GPU support to allocate the tensor on the attached GPU.
	 */
	virtual void allocateTensorGPU() {
		throw "Base TensorProvider does not have GPU support";
	}

	/**
	 * Static method that enables the construction of runtime-injected
	 * derived TensorProviders.
	 *
	 * @param id The name of the desired TensorProvider
	 * @param rank The rank of the tensor controlled by the created TensorProvidder
	 * @param dims The tensor dimensions for the tensor controlled by the created TensorProvider
	 * @return tensorProvider The desired TensorProvider.
	 */
	static TensorProvider * create(const std::string& id, const int rank,
			std::vector<int>& dims) {
		auto iter = getConstructors().find(id);
		return iter == getConstructors().end() ?
				nullptr : (*iter->second)(rank, dims);
	}

private:

	// Create an alias for TensorProvider Constructors
	using TPCtor = TensorProvider*(const int rank, std::vector<int> dims);

	// Create and alias for a mapping between ids and TensorProvider constructors
	using CtorMap = std::map<std::string, TPCtor*>;

	/**
	 * Return the current mapping of ids and constructors
	 * @return
	 */
	static CtorMap& getConstructors() {
		static CtorMap constructors;
		return constructors;
	}

	/**
	 * This struct provides a convenient way to register TensorProviders
	 * dynamically.
	 */
	template<class T = int>
	struct DynamicRegister {

		/** Create a template provider with the given rank
		 * and dimension, and of template type T
		 *
		 * @param rank
		 * @param dims
		 * @return
		 */
		static TensorProvider* create(const int rank, std::vector<int> dims) {
			static_assert(std::is_base_of<TensorProvider, T>::value, "Cannot register a class if it is not a derived TensorProvider.");
			return new T(rank, dims);
		}

		/**
		 * Initialize a new TensorProvider constructor and place it
		 * in the map with the given id.
		 * @param id
		 * @return
		 */
		static TPCtor * initialize(const std::string& id) {
			return getConstructors()[id] = create;
		}

		/**
		 * The constructor function pointer for type T
		 */
		static TPCtor * creator;

	};

};

// This Macro can be used by derived classes to automatically
// inject themselves into the TensorProvider factory at runtimme
#define REGISTER_TENSORPROVIDER(T, STR) template<> fire::TensorProvider::TPCtor* \
	fire::TensorProvider::DynamicRegister<T>::creator = \
	fire::TensorProvider::DynamicRegister<T>::initialize(STR)

/**
 * The Tensor class provides an extensible data structure describing
 * tensors. It realizes the ITensor interface but delegates all
 * actual tensor-work to a user-specified TensorProvider. This class enables
 * the expression of Tensors and operations on Tensors in a familiar way that is
 * extensible to any number of 3rd party tensor algebra libraries.
 */
template<typename Scalar, int Rank>
class Tensor : public virtual ITensor {

private:

	/**
	 * Reference to the TensorProvider backing this Tensor.
	 */
	std::shared_ptr<TensorProvider> provider;

public:

	/**
	 * The constructor for creating Tensors of
	 * arbitrary rank with each rank having equal dimension
	 * (ex. a square matrix, rank-2 tensor with equal dimensions)
	 *
	 * @param allRankDimension The dimension of all tensor ranks
	 */
	Tensor(int allRankDimension) {
		static_assert( std::is_fundamental<Scalar>::value, "Fire Tensors can only contain C++ fundamental types (double, int, etc)." );
		std::vector<int> dims;
		for (int i = 0; i < Rank; i++) {
			dims.push_back(allRankDimension);
		}

		if (TensorDebug)
			std::cout << "[TensorMessage] Using " << TensorBackend
					<< " Backend\n";

		// Create the requested TensorProvider
		provider = std::shared_ptr<TensorProvider>(
				TensorProvider::create(TensorBackend, Rank, dims));
		if (!provider)
			throw "Could not find provider";

		// Allocate the Tensor data
		provider->allocate<Scalar>();

	}

	/**
	 * The constructor for creating Tensors of any shape. The number
	 * of provided dimensions in this variadic constructor must
	 * be equal to the Tensor's Rank template parameter.
	 *
	 * @param firstDim The dimension of the first rank
	 * @param otherDims The dimension of the remaining ranks
	 */
	template<typename... Dimension>
	Tensor(int firstDim, Dimension ... otherDims) {
		// Assert that our input parameters are consistent
		static_assert( sizeof...(otherDims) + 1 == Rank, "Incorrect number of dimension integers");
		static_assert( std::is_fundamental<Scalar>::value, "Fire Tensors can only contain C++ fundamental types (double, int, etc)." );

		// Create a vector from the variadic template dimensions
		std::vector<int> dims { static_cast<int>(otherDims)... };
		dims.insert(dims.begin(), firstDim);

		if (TensorDebug) std::cout << "[TensorMessage] Using " << TensorBackend << " Backend\n";

		// Create the requested TensorProvider
		provider = std::shared_ptr<TensorProvider>(
				TensorProvider::create(TensorBackend, Rank, dims));
		if (!provider) throw "Could not find provider";

		// Allocate the Tensor data
		provider->allocate<Scalar>();
	}

	template<typename... Indices>
	Scalar operator() (Indices... indices) const {

	}

	/**
	 * See ITensor::contract for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @param other The other Tensor to contract with
	 * @param dimensions The indices to contract
	 * @return result The result of the contraction.
	 */
	virtual ITensor& contract(ITensor& other, std::vector<std::pair<int,int>>& dimensions) {
		return provider->contract(other, dimensions);
	}

	/**
	 * See ITensor::add for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @param other Tensor to add to this one.
	 * @param scale Scale factor to multiply other tensor by before addition.
	 */
	virtual void add(ITensor& other, double scale = 1.0) {
		return provider->add(other);
	}

	/**
	 * See ITensor::norm1 for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @return norm The 1-norm value
	 */
	virtual double norm1() {
		return provider->norm1();
	}

	/**
	 * See ITensor::norm2 for details on this method. Tensor delegates this
	 * work to the TensorProvider.
	 *
	 * @return norm The 2-norm value
	 */
	virtual double norm2() {
		return provider->norm1();
	}

	/**
	 * The destructor
	 */
	virtual ~Tensor() {}

	/**
	 *
	 * @return
	 */
	template<typename ValueType, int IdentityRank>
	static Tensor<ValueType, IdentityRank> identity() {}

};

}

#endif

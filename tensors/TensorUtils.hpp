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
#ifndef TENSORS_TENSORUTILS_HPP_
#define TENSORS_TENSORUTILS_HPP_

#if defined __GNUC__ && __GNUC__>=6
  #pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

namespace fire {
/**
 * This utility template enables the use of std::array size integer.
 */
template <typename T> struct array_size;
template<class T, std::size_t N> struct array_size<std::array<T,N> > {
  static const size_t value = N;
};


/**
 * A TensorReference is just a pair of the Tensors data array and its TensorShape
 */
template<typename Scalar>
using TensorReference = std::pair<std::vector<Scalar>, TensorShape>;

/**
 * Utility method for creating TensorReferences.
 *
 * @param data
 * @param shape
 * @return
 */
template<typename Scalar>
auto make_tensor_reference(Scalar* data, TensorShape& shape) -> TensorReference<Scalar> {
	std::vector<Scalar> v;
	std::copy(data, data + shape.size(), std::back_inserter(v));
	auto pair = std::make_pair(v, shape);
	return pair;
}

/**
 * The following three templates set up the recursion necessary
 * to express nexted std::initializer_lists. We use this to accept
 * tensor values and set them on the internal TensorProvider tensor instance.
 */
template <typename FireTensor, typename Scalar, int N>
struct Initializer {
  typedef std::initializer_list<
    typename Initializer<FireTensor, Scalar, N - 1>::InitList> InitList;
};

template <typename FireTensor, typename Scalar>
struct Initializer<FireTensor, Scalar, 1> {
  typedef std::initializer_list<Scalar> InitList;
};

template <typename FireTensor, typename Scalar>
struct Initializer<FireTensor, Scalar, 0> {
  typedef Scalar InitList;
};

}

#endif


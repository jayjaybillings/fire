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

#include "ITensor.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace fire {

/**
 *
 */
class TensorProvider : public virtual ITensor {

protected:

	const int rank = 0;

public:

	TensorProvider(const int r) : rank(r) {}

	static TensorProvider * create(const std::string& id, const int rank) {
		auto iter = getConstructors().find(id);
		return iter == getConstructors().end() ? nullptr : (*iter->second)(rank);
	}

private:

	typedef TensorProvider * TPCtor(const int rank);
	typedef std::map<std::string, TPCtor*> CtorMap;

	static CtorMap& getConstructors() {
		static CtorMap constructors;
		return constructors;
	}

	template<class T = int>
	struct DynamicRegister {
		static TensorProvider* create(const int rank) {
			return new T(rank);
		}

		static TPCtor * initialize (const std::string& id) {
			return getConstructors()[id] = create;
		}

		static TPCtor * creator;

	};

};

#define REGISTER_TENSORPROVIDER(T, STR) template<> fire::TensorProvider::TPCtor* \
	fire::TensorProvider::DynamicRegister<T>::creator = \
	fire::TensorProvider::DynamicRegister<T>::initialize(STR)

}



#endif

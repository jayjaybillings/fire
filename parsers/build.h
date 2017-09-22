/**----------------------------------------------------------------------------
 Copyright (c) 2016-, UT-Battelle, LLC
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

 Author(s): Jay Jay Billings (jayjaybillings <at> gmail <dot> com)
 -----------------------------------------------------------------------------*/
#ifndef PARSERS_BUILD_H_
#define PARSERS_BUILD_H_

#include <iostream>
#include <vector>

namespace fire {

/**
 * The build<>() template function defines the basic construction mechanism for
 * objects in Fire. It is an implementation of a dynamic builder pattern that
 * does not require a singleton for construction or registration with a
 * singleton to provide such a construction service.
 *
 * Build function specializations should always be provided for domain classes
 * used in Fire to insure clean construction.
 *
 * Those concerned about the performance of calling build<>() should know that
 * it performs very well because of C++11's move semantics and return value
 * optimizations provided by the compiler. It may perform slowly when used in
 * debug mode with gcc since these optimizations may be disabled.
 *
 * @return an instance of class T created from the default constructor.
 */
template<typename T>
T build() {
	T object;
	return object;
}

/**
 * An implementation of the build template for classes that do not use
 * nullary/default constructors.
 * @param values of type K that should be passed to the appropriate constructor.
 * @return an instance of class T created from the alternative constructor.
 */
//template<typename T, typename K>
//T build(K values) {
//	T object(values);
//	return object;
//}

/**
 * An implementation of the build template for classes that do not use
 * nullary/default constructors and have either more than one or a variadic
 * number of arguments
 * @param full list argument values for the constructor of T
 */
template<typename T, typename... Args>
T build(Args&& ... args) {
	T object(std::forward<Args>(args)...);
	return object;
}

/**
 * This is a utility function used for splitting strings.
 * @param the strings to be split
 * @param a delimiter that marks where tokens should be split.
 * @return a vector with a string for each entry in the original.
 */
inline std::vector<std::string> splitString(const std::string & line,
		const std::string & delimiter) {

	std::string value;
	std::istringstream ss(line);
	std::vector < std::string > lineVec;
	//Split the line and push each element into the line list.
	while (std::getline(ss, value, *delimiter.c_str())) {
	      lineVec.push_back(value);
	}
	return lineVec;
}

/**
 * This is a utility function used for splitting strings. It is a
 * convenience function for calling splitLine(line,delimiter) with
 * delimiter equal to a single whitespace character.
 * @param the strings to be split
 * @return a vector with a string for each entry in the original.
 */
inline std::vector<std::string> splitString(const std::string & line) {
	std::string delimiter = " ";
	return splitString(line,delimiter);
}

} /* namespace fire */

#endif /* PARSERS_BUILD_H_ */

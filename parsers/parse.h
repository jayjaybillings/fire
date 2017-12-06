/**----------------------------------------------------------------------------
 Copyright (c) 2015-, UT-Battelle, LLC
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
#ifndef PARSERS_PARSE_H_
#define PARSERS_PARSE_H_

#include <LocalParser.h>
#include <build.h>
#include <vector>
#include <memory>

using namespace std;

namespace fire {

/**
 * The parse<>() template function defines a basic mechanism for parsing data
 * objects in Fire. This function was added to clean up parsing with
 * LocalParsers in Fire since the code below is common to almost all local
 * parser types. Instead of copying the four line implementation below all over
 * the code base, client code now looks like the following
 * @code
 * auto data = parse<T>(filename);
 * @endcode
 *
 * Those concerned about the performance of calling parse<>() should know that
 * it performs very well because of C++11's move semantics and return value
 * optimizations provided by the compiler. It may perform slowly when used in
 * debug mode with gcc since these optimizations may be disabled.
 *
 * @return an instance of class T created from the default constructor.
 */
template<typename T>
shared_ptr<vector<T>> parse(const string & source) {
	LocalParser<vector<T>> parser = buildParser<vector,T>(source);
	parser.setSource(source);
	parser.parse();
	return parser.getData();
}

} /* namespace fire */

#endif /* PARSERS_PARSE_H_ */

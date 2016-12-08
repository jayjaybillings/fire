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
#ifndef PARSERS_LOCALPARSER_H_
#define PARSERS_LOCALPARSER_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ILocalParser.h>
#include <memory>
#include <build.h>

namespace fire {

/**
 * The LocalParser<T> is a templated parser that provides support for parsing
 * local files of various types based only on the templated type.
 *
 * This class works through <em>Explicit Specialization</em>, where subclasses
 * are created using concrete types as template arguments instead of through
 * direct inheritance. One important benefit of this approach to the author is
 * that parsing with local files comes uniform and type-independent. For
 * example,
 *
 * @code
 * LocalParser<UTKAstroNetwork> parser1;
 * LocalParser<ReacLib> parser2;
 *
 * // Other work...
 *
 * network = parser1.parse();
 * rates = parser2.parse();
 * @endcode
 *
 * Confer <em>C++ Templates: The Complete Guide</em> by Vandervoorde and
 * Josuttis for full details on explicit specialization.
 *
 * This class always assumes that its source is a file.
 *
 * FIXME! - Specialization
 *
 * Subclasses must always be sure that they implement parse() and setSource()
 * because default implementations are not provided.
 */
template<typename T>
class LocalParser : public virtual ILocalParser<T> {

protected:

	/**
	 * The source file name used if setSource(string) is called.
	 */
	std::string sourceFile;

	/**
	 * The shared pointer to the data set loaded by the parse() operation.
	 */
	std::shared_ptr<T> data;

public:
	LocalParser() {
		data = std::make_shared<T>();
	};

	virtual ~LocalParser() {};

	virtual void setSource(const std::string & source) {
		sourceFile = source;
	};

	virtual void parse() {
		throw "Method not implemented for all types.";
	};

	virtual void setSource(const std::istream & source) {
		throw "Method not yet implemented.";
	};

	virtual const std::string & getSource() {return sourceFile;};

	virtual const std::istream & getSourceStream() {
		throw "Method not yet implemented.";
	};

	virtual std::shared_ptr<T> getData() {
		return data;
	}
};

}
/* namespace fire */

#endif /* PARSERS_LOCALPARSER_H_ */

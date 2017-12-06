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
#ifndef PARSERS_ILOCALPARSER_H_
#define PARSERS_ILOCALPARSER_H_

#include <memory>
#include <IParser.h>

namespace fire {

/**
 * This is a sub-interface of IParser that represents a parser for a local,
 * serially executed parser.
 *
 * isLocal() always returns true.
 * isParallel() always returns false.
 *
 * Implementations should set isFile in their setSource(std::string) if their
 * source is a file. Subclasses must always be sure that they implement parse()
 * and setSource() because default implementations are not provided.
 *
 * The getData() operation always returns a shared_ptr instead of a raw type
 * (copy) or raw pointer to efficiently share the dynamically allocated data.
 *
 */
template <typename T>
class ILocalParser: public virtual IParser {

protected:

	/**
	 * This value is true if the source is a file and false if it is a stream.
	 * This value should be set in the setSource(std::string) and
     * setSource(std::istream) to true and false respectively. It is set to
     * true by default.
	 */
	bool isAFile = true;

public:

	virtual bool isFile() {return isAFile;};

	virtual bool isLocal() {return true;};

	virtual bool isParallel() {return false;};

	/**
	 * This operation returns a shared pointer to an instance of type T.
	 * @return a shared pointer holding an instance of type T that was parsed
	 * from the file.
	 */
	virtual std::shared_ptr<T> getData() = 0;

};

}

#endif /* PARSERS_ILOCALPARSER_H_ */

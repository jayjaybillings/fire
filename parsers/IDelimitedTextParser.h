/**----------------------------------------------------------------------------
 Copyright (c) 2015-, Jay Jay Billings
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
#ifndef PARSERS_IDELIMITEDTEXTPARSER_H_
#define PARSERS_IDELIMITEDTEXTPARSER_H_

#include<vector>
#include<string>
#include "ILocalParser.h"

namespace fire {

/**
 * This is an extension of the parser interface that focuses on parsing
 * delimited text. Delimited text is text with entries that are separated by a
 * common pattern, such as a common or space.
 *
 * The Comma-Separated Variables format is a delimited text format of the form
 *
 * v1, v2, v3, v4
 *
 * where each comma is the delimiter between the text values v1 through v4. A
 * line of text with only spaces is also a delimited text format:
 *
 * v1 v2 v3 v4
 *
 * Delimited text parsers may assume that lines such as those above are
 * terminated by \n.
 */
template<class T>
class IDelimitedTextParser: public virtual ILocalParser {

protected:

	/**
	 * The delimiter used when parsing the file.
	 */
	std::string delimiter;

public:

	/**
	 * Constructor
	 * @param delim the delimiter in the source
	 */
	IDelimitedTextParser(std::string delim) : delimiter(delim) {};

	/**
	 * This operation sets the data source for the parser using a stream
	 * instead of a string.
	 * @param source the stream of delimited text data
	 */
	virtual void setSource(const std::istream & source) = 0;

	/**
	 * This operation returns all of the data in the source as a vector of
	 * vectors of the template type T.
	 * @return all the data in the source with one vector for each line
	 */
	virtual std::vector<std::vector<T>> & getData() = 0;

};

} /* namespace fire */

#endif /* PARSERS_IDELIMITEDTEXTPARSER_H_ */

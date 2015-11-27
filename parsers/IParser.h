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
#ifndef IPARSER_H_
#define IPARSER_H_

#include <string>

namespace fire {

/**
 * This is the base interface for parsers in Fire and it defines the contract
 * that can be expected of all parsers.
 *
 * This source string is only a reference to the source or some sort of handle
 * that points to it, such as a file or stream name. The exact type of the
 * source - file, stream, socket, etc. - is determined by the implementing
 * class.
 *
 * IParsers should always be used by setting the source and then parsing the
 * source:
 *
 * IParser parser = ...;// somehow create your parser
 * parser.setSource(sourceName);
 * parser.parser();
 *
 */
class IParser {
public:

	/**
	 * This operation sets the data source for the parser.
	 * @param source the name of the source that the parser should parse.
	 */
	virtual void setSource(const std::string & source) = 0;

	/**
	 * This operation gets the data source for the parser.
	 * @return the name of the source
	 */
	virtual const std::string & getSource() = 0;

	/**
	 * This operation directs the parser to parse its source.
	 */
	virtual const void parse() = 0;

	/**
	 * This operation indicates whether or not the parser's source is a file.
	 * @return true if this parser is working with a file, false otherwise.
	 */
	virtual bool isFile() = 0;

	/**
	 * This operation indicates whether or not the parser is using a local
	 * source.
	 * @return true if this parser is working with a local source, false
	 * otherwise.
	 */
	virtual bool isLocal() = 0;

	/**
	 * This operation indicates whether or not the parser reads in parallel.
	 * @return true if this parser reads in parallel, false otherwise.
	 */
	virtual bool isParallel() = 0;
};

} // end namespace fire
#endif // IPARSER_H_

/**----------------------------------------------------------------------------
 Copyright (c) 2015-, UT-Battelle, LLC.
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

 Author(s): Jay Jay Billings
 -----------------------------------------------------------------------------*/
#ifndef IPROPERTYPARSER_H_
#define IPROPERTYPARSER_H_

#include <string>
#include <vector>
#include "IParser.h"
#include "IPropertyBlock.h"

namespace fire {

/**
 * This is an extension of the parser interface that focuses on parsing a set
 * of properties. Properties are returned in blocks (see IPropertyBlock).
 */
class IPropertyParser : public IParser {
public:

	/**
	 * This operation returns the names of the property blocks parsed from the
	 * source.
	 * @return the block names
	 */
	const std::vector<std::string> & getPropertyBlockNames();

	/**
	 * This operation returns the property block with the given name.
	 * @param name the block name
	 * @return the property block with the given name
	 */
	const IPropertyBlock & getPropertyBlock(const std::string & name);

	/**
	 * This is a convenience operation that returns all of the property blocks
	 * in the source.
	 * @return the full set of property blocks for the source
	 */
	const std::vector<IPropertyBlock> & getPropertyBlocks();

};

} // end namespace fire
#endif // IPROPERTYPARSER_H_

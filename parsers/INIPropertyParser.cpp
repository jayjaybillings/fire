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
#include "INIPropertyParser.h"
#include <iostream>

namespace fire {

void INIPropertyParser::setSource(const std::string & source) {
	this->source = source;
}

const std::string & INIPropertyParser::getSource() {
	return source;
}

void INIPropertyParser::parse() {

	// Load the parameters file
	iniReader.SetUnicode();
	SI_Error status = iniReader.LoadFile(source.c_str());
	// Exit with a failure if the file won't load.
	if (status < 0) {
		std::cout << "Unable to open source " << source << std::endl;
		exit(EXIT_FAILURE);
	}

	// Get all the sections
	CSimpleIniA::TNamesDepend sections;
	iniReader.GetAllSections(sections);
	// Load them into the vector
	for (auto i = sections.begin(); i != sections.end(); ++i) {
		// Store the block names
		blockNames.push_back(std::string(i->pItem));
		// Grab the keys
		CSimpleIniA::TNamesDepend keys;
		iniReader.GetAllKeys(i->pItem, keys);
		// Initialize the block in the map. Make sure to use 'auto &'!
		auto & map = blockMap[i->pItem];
		// And then fill
		for (auto j = keys.begin(); j != keys.end(); ++j) {
			auto value = iniReader.GetValue(i->pItem, j->pItem,
					NULL, NULL);
			map[std::string(j->pItem)] = std::string(value);
		}
	}

	// FOR some reason the map is empty here.

	std::cout << blockMap["block1"]["prop1"] << std::endl;

	return;
}

const std::vector<std::string> & INIPropertyParser::getPropertyBlockNames() {
	return blockNames;
}

/**
 * @see IPropertyParser.getPropertyBlock()
 */
const std::map<std::string, std::string> & INIPropertyParser::getPropertyBlock(
		const std::string & name) {
	std::cout << blockMap["block1"]["prop1"] << std::endl;
	return blockMap[name.c_str()];
}

} /* namespace fire */

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

#ifndef INIPROPERTYPARSER_H_
#define INIPROPERTYPARSER_H_

#include <IPropertyParser.h>
#include <string>
#include <build.h>
#include <SimpleIni.h>
#include <iostream>
#include <vector>
#include <map>

namespace fire {

/**
 * This class implements IPropertyParser to provide a local, file-based,
 * serially executed INI parser.
 *
 * isFile() always returns true.
 * isLocal() always returns true.
 * isParallel() always returns false.
 *
 * This implementation is backed by SimpleINI, one of Fire's dependencies, so
 * it supports whatever SimpleINI supports.
 *
 * The source must be a file on the local filesystem.
 */
class INIPropertyParser: public IPropertyParser {

private:

	/**
	 * The name of the source that will be parsed
	 */
	std::string source;

	/**
	 * The names of all the property blocks
	 */
	std::vector<std::string> blockNames;

	/**
	 * The master map of blocks from the INI file
	 */
	std::map<std::string,std::map<std::string,std::string>> blockMap;

public:
	INIPropertyParser() {};
	virtual ~INIPropertyParser() {};

	virtual void setSource(const std::string & source) {
		this->source = source;
	};

	virtual const std::string & getSource() {
		return source;
	};

	virtual void parse() {
		// The actual INI reader
		CSimpleIniA iniReader;

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
	};

	virtual const std::vector<std::string> & getPropertyBlockNames() {
		return blockNames;
	};

	virtual const std::map<std::string, std::string> & getPropertyBlock(
			const std::string & name) {
		return blockMap[name.c_str()];
	};
};

/**
 * This is a builder for constructing INIPropertyParsers from a string. It
 * constructs, initializes, and parses the properties. Property files are
 * expected to be relatively small, so any copying on return is acceptable.
 * @param The filename that should be parsed for the properties.
 * @return the fully initialized property parser.
 */
template<>
INIPropertyParser build(const std::string & source) {
	INIPropertyParser parser;
	parser.setSource(source);
	parser.parse();
	return parser;
}

} /* namespace fire */

#endif /* INIPROPERTYPARSER_H_ */

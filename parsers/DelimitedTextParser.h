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
#ifndef PARSERS_DELIMITEDTEXTPARSER_H_
#define PARSERS_DELIMITEDTEXTPARSER_H_

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include "LocalParser.h"
#include <vector>

using namespace std;

namespace fire {

/**
 * Default template for casting strings properly.
 */
template<typename T>
struct StringCaster{
	static T cast(const string & value) {return static_cast<T>(value);};
};

/**
 * Implementation of StringCaster for doubles
 */
template<>
struct StringCaster<double>{
	static double cast(const string & value) {
		return strtod(value.c_str(),NULL);};
};

/**
 * Implementation of StringCaster for floats
 */
template<>
struct StringCaster<float>{
	static double cast(const string & value) {
		return strtof(value.c_str(),NULL);};
};

/**
 * Implementation of StringCaster for ints
 */
template<>
struct StringCaster<int>{
	static double cast(const string & value) {
		return atoi(value.c_str());};
};


/**
 * This class implements ILocalParser to provide a local, file-based,
 * serially executed delimited text parser.
 *
 * isFile() will return true if setSource(string) is used
 * isStream() will return true if setSource(istream) is used
 * isLocal() always returns true.
 * isParallel() always returns false.
 *
 * Because delimited text is most often primitives, this class requires two
 * template arguments. The first argument T is how the data should be returned
 * and the second argument K is the root type of the delimited text, most
 * likely a primitive. This would look something like
 *
 * @code
 * DelimitedTextParser<vector<vector<double>>,double>
 * @endcode
 *
 * for a dense body of text like in a standard CSV file or DAT file where each
 * column is composed of dense primitives.
 *
 * The source may either be a file on the local filesystem or an input stream.
 *
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
 * terminated by \n. Like most other text formats, delimited text files may
 * contain comments, which may be skipped by IDelimitedTextParsers. The
 * characters that denote comments - very commonly a "#" or "//" - may be
 * passed to the constructor.
 */
template<typename T, typename K>
class DelimitedTextParser : public virtual LocalParser<T> {

protected:

	/**
	 * The delimiter used when parsing the file.
	 */
	string delimiter;

	/**
	 * The character that represents a comment and should be skipped.
	 */
	string commentChar;

public:
	DelimitedTextParser(string delim, string comment)
		 : delimiter(delim), commentChar(comment) {};

	virtual ~DelimitedTextParser() {};

	virtual void parse() {
		throw "Method not implemented for all types.";
	};

};

/**
 * This specialization is for dense data of primitive types.
 */
template<>
void DelimitedTextParser<vector<vector<double>>,double>::parse() {
	// Create the initial container
	data = make_shared<vector<vector<double>>>();
	// Load the contents of the file
	string value, line;
	ifstream fileStream(sourceFile);
	// Pull each line and push it into the list
	if (fileStream.is_open()) {
		while (getline(fileStream,line)) {
			if (!line.empty() && !line.find("#") == 0) {
			   istringstream ss(line);
			   vector<double> lineVec;
			   // Push each line into the container
			   while (getline(ss,value,*delimiter.c_str())) {
			       lineVec.push_back(StringCaster<double>::cast(value));
			   }
			   data->push_back(lineVec);
			}
		}
		fileStream.close();
	}
};


} /* namespace fire */

#endif /* PARSERS_DELIMITEDTEXTPARSER_H_ */

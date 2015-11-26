/*
 * INIPropertyParser.h
 *
 *  Created on: Nov 25, 2015
 *      Author: bkj
 */

#ifndef INIPROPERTYPARSER_H_
#define INIPROPERTYPARSER_H_

#include "IPropertyParser.h"

namespace fire {

class INIPropertyParser: public IPropertyParser {
public:
	INIPropertyParser();
	virtual ~INIPropertyParser();
};

} /* namespace fire */

#endif /* INIPROPERTYPARSER_H_ */

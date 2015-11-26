/*
 * BasicPropertyBlock.h
 *
 *  Created on: Nov 25, 2015
 *      Author: bkj
 */

#ifndef PARSERS_BASICPROPERTYBLOCK_H_
#define PARSERS_BASICPROPERTYBLOCK_H_

#include "IPropertyBlock.h"

namespace fire {

class BasicPropertyBlock: public IPropertyBlock {
public:
	BasicPropertyBlock();
	virtual ~BasicPropertyBlock();
};

} /* namespace fire */

#endif /* PARSERS_BASICPROPERTYBLOCK_H_ */

/***********************************************************************************************************************
 * BinaryNodeUnit.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNodeUnit.h"

namespace Model {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNodeUnit, BinaryNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNodeUnit)

void BinaryNodeUnit::init()
{
	BinaryNodeUnit::setParentMeta<BinaryNodeUnit, BinaryNode>();
	registerNodeConstructors();
}

NodeReadWriteLock* BinaryNodeUnit::getAccessLock() const
{
	return & (const_cast<BinaryNodeUnit*> (this) )->accessLock; // TODO const cast madness. What is the best way to use these?
}

}

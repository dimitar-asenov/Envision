/***********************************************************************************************************************
 * BinaryNodeAccessUnit.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/BinaryNodeAccessUnit.h"

namespace TestNodes {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNodeAccessUnit, BinaryNode)
NODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNodeAccessUnit)

void BinaryNodeAccessUnit::init()
{
	BinaryNodeAccessUnit::setParentMeta<BinaryNodeAccessUnit, BinaryNode>();
	registerNodeType();
}

Model::NodeReadWriteLock* BinaryNodeAccessUnit::getAccessLock() const
{
	return & (const_cast<BinaryNodeAccessUnit*> (this) )->accessLock; // TODO const cast madness. What is the best way to use these?
}

}

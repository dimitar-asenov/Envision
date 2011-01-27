/***********************************************************************************************************************
 * BinaryNodeAccessUnit.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/BinaryNodeAccessUnit.h"

namespace TestNodes {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNodeAccessUnit, BinaryNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNodeAccessUnit, BinaryNode)

void BinaryNodeAccessUnit::init()
{
	registerNodeType();
}

Model::NodeReadWriteLock* BinaryNodeAccessUnit::accessLock() const
{
	return & (const_cast<BinaryNodeAccessUnit*> (this) )->accessLock_; // TODO const cast madness. What is the best way to use these?
}

}

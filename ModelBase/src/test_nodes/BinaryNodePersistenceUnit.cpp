/***********************************************************************************************************************
 * BinaryNodePersistenceUnit.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/BinaryNodePersistenceUnit.h"

namespace TestNodes {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNodePersistenceUnit, BinaryNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNodePersistenceUnit, BinaryNode)

bool BinaryNodePersistenceUnit::isNewPersistenceUnit() const
{
	return true;
}

}

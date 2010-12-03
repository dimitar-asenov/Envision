/***********************************************************************************************************************
 * BinaryNodeUnit.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNodeUnit.h"

namespace FilePersistence {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNodeUnit, BinaryNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNodeUnit)

void BinaryNodeUnit::init()
{
	BinaryNodeUnit::setParentMeta<BinaryNodeUnit, BinaryNode>();
	registerNodeConstructors();
}

bool BinaryNodeUnit::isNewPersistenceUnit() const
{
	return true;
}

}

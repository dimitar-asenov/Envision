/***********************************************************************************************************************
 * Block.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/Block.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Block, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Block, Statement)

REGISTER_ATTRIBUTE(Block, items, TypedListOfStatementItem, false, false, true)

}

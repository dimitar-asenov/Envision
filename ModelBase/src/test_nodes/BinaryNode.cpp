/***********************************************************************************************************************
 * BinaryNode.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/BinaryNode.h"

namespace TestNodes {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNode, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNode, Model::ExtendableNode)

REGISTER_ATTRIBUTE(BinaryNode, name, Text, false, false, true)
REGISTER_ATTRIBUTE(BinaryNode, left, BinaryNode, false, true, true)
REGISTER_ATTRIBUTE(BinaryNode, right, BinaryNode, false, true, true)

}

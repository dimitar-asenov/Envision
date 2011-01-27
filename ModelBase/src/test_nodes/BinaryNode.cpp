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

Model::ExtendableIndex BinaryNode::nameIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::leftIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::rightIndex = Model::ExtendableIndex();

void BinaryNode::init()
{
	registerNodeType();
	nameIndex = registerNewAttribute("name", "Text", false, false, true);
	leftIndex = registerNewAttribute("left", "BinaryNode", false, true, true);
	rightIndex = registerNewAttribute("right", "BinaryNode", false, true, true);
}

}

/***********************************************************************************************************************
 * BinaryNode.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNode.h"

namespace Model {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNode, ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNode)

ExtendableIndex BinaryNode::textIndex = ExtendableIndex();
ExtendableIndex BinaryNode::leftIndex = ExtendableIndex();
ExtendableIndex BinaryNode::rightIndex = ExtendableIndex();

void BinaryNode::init()
{
	registerNodeConstructors();
	textIndex = registerNewAttribute<BinaryNode> ("text", "Text", false, false);
	leftIndex = registerNewAttribute<BinaryNode> ("left", "BinaryNode", false, true);
	rightIndex = registerNewAttribute<BinaryNode> ("right", "BinaryNode", false, true);
}

Text* BinaryNode::text()
{
	return static_cast<Text*> (get(textIndex));
}

BinaryNode* BinaryNode::left()
{
	return static_cast<BinaryNode*> (get(leftIndex));
}

BinaryNode* BinaryNode::right()
{
	return static_cast<BinaryNode*> (get(rightIndex));
}

BinaryNode* BinaryNode::makeLeftNode()
{
	return static_cast<BinaryNode*> (createOptional(leftIndex));
}

BinaryNode* BinaryNode::makeRightNode()
{
	return static_cast<BinaryNode*> (createOptional(rightIndex));
}

void BinaryNode::removeLeftNode()
{
	removeOptional(leftIndex);
}

void BinaryNode::removeRightNode()
{
	removeOptional(rightIndex);
}

}

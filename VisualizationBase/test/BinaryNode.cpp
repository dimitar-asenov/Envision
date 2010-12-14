/***********************************************************************************************************************
 * BinaryNode.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNode.h"

namespace Visualization {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNode, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNode)

Model::ExtendableIndex BinaryNode::nameIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::leftIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::rightIndex = Model::ExtendableIndex();

void BinaryNode::init()
{
	registerNodeType();
	nameIndex = registerNewAttribute<BinaryNode> ("name", "Text", false, false, true);
	leftIndex = registerNewAttribute<BinaryNode> ("left", "BinaryNode", false, true, true);
	rightIndex = registerNewAttribute<BinaryNode> ("right", "BinaryNode", false, true, true);
}

Model::Text* BinaryNode::name()
{
	return static_cast<Model::Text*> (get(nameIndex));
}

BinaryNode* BinaryNode::left()
{
	return static_cast<BinaryNode*> (get(leftIndex));
}

BinaryNode* BinaryNode::right()
{
	return static_cast<BinaryNode*> (get(rightIndex));
}

BinaryNode* BinaryNode::makeLeftNode(const QString &type)
{
	return createOptional<BinaryNode>(leftIndex, type);
}

BinaryNode* BinaryNode::makeRightNode(const QString &type)
{
	return createOptional<BinaryNode>(rightIndex, type);
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

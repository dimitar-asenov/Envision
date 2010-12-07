/***********************************************************************************************************************
 * BinaryNode.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNode.h"

namespace FilePersistence {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNode, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNode)

Model::ExtendableIndex BinaryNode::textIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::leftIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryNode::rightIndex = Model::ExtendableIndex();

void BinaryNode::init()
{
	registerNodeType();
	textIndex = registerNewAttribute<BinaryNode> ("text", "Text", false, false, true);
	leftIndex = registerNewAttribute<BinaryNode> ("left", "BinaryNode", false, true, true);
	rightIndex = registerNewAttribute<BinaryNode> ("right", "BinaryNode", false, true, true);
}

Model::Text* BinaryNode::text()
{
	return static_cast<Model::Text*> (get(textIndex));
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

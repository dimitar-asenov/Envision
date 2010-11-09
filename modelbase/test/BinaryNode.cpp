/***********************************************************************************************************************
 * BinaryNode.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryNode.h"

namespace Model {

NODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryNode, ExtendableNode<BinaryNode> )
NODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryNode)

int BinaryNode::textIndex = 0;
int BinaryNode::leftIndex = 0;
int BinaryNode::rightIndex = 0;

void BinaryNode::init()
{
	registerNodeConstructors();
	textIndex = registerNewAttribute("text", "Text", false, false);
	leftIndex = registerNewAttribute("left", "BinaryNode", false, true);
	rightIndex = registerNewAttribute("right", "BinaryNode", false, true);
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

}

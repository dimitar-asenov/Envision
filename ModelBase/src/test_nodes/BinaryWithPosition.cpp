/***********************************************************************************************************************
 * BinaryWithPosition.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/BinaryWithPosition.h"
#include "nodes/Integer.h"

namespace TestNodes {

Model::ExtendableIndex BinaryWithPosition::xIndex = Model::ExtendableIndex();
Model::ExtendableIndex BinaryWithPosition::yIndex = Model::ExtendableIndex();

BinaryWithPosition::BinaryWithPosition(BinaryNode* binary) :
	binaryNode(binary)
{
}

BinaryWithPosition::~BinaryWithPosition()
{
}

void BinaryWithPosition::set(int x, int y)
{
	static_cast<Model::Integer*> (binaryNode->get(xIndex))->set(x);
	static_cast<Model::Integer*> (binaryNode->get(yIndex))->set(y);
}

void BinaryWithPosition::init()
{
	xIndex = BinaryNode::registerNewAttribute("x", "Integer", false, false, true);
	yIndex = BinaryNode::registerNewAttribute("y", "Integer", false, false, true);
}

}

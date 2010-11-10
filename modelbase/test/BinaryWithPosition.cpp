/***********************************************************************************************************************
 * BinaryWithPosition.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BinaryWithPosition.h"
#include "BinaryNode.h"
#include "nodes/Integer.h"

namespace Model {

int BinaryWithPosition::xIndex;
int BinaryWithPosition::yIndex;

BinaryWithPosition::BinaryWithPosition(BinaryNode* binary_) :
	binary(binary_)
{
}

BinaryWithPosition::~BinaryWithPosition()
{
}

int BinaryWithPosition::x()
{
	return static_cast<Integer*> (binary->get(xIndex))->get();
}

int BinaryWithPosition::y()
{
	return static_cast<Integer*> (binary->get(yIndex))->get();
}

void BinaryWithPosition::set(int x, int y)
{
	static_cast<Integer*> (binary->get(xIndex))->set(x);
	static_cast<Integer*> (binary->get(yIndex))->set(y);
}

void BinaryWithPosition::init()
{
	xIndex = BinaryNode::registerNewAttribute("x", "Integer");
	yIndex = BinaryNode::registerNewAttribute("y", "Integer");
}

}

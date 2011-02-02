/***********************************************************************************************************************
 * PositionExtension.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/PositionExtension.h"

namespace TestNodes {

int extensionId_ = -1;

void PositionExtension::registerExtension()
{
	extensionId_ = Model::ExtendableNode::registerExtensionId();
};

PositionExtension::PositionExtension(Model::ExtendableNode* self, const QVector<Model::ExtendableIndex>& extensionAttributes) :
	self_(self), xIndex(extensionAttributes[0]), yIndex(extensionAttributes[1])
{
}

void PositionExtension::set(int x, int y)
{
	static_cast<Model::Integer*> (self_->get(xIndex))->set(x);
	static_cast<Model::Integer*> (self_->get(yIndex))->set(y);
}

}
